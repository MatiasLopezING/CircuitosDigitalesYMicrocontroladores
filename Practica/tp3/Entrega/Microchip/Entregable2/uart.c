/**
 * @file    uart.c
 * @brief   Driver UART para el ATmega328P a 16 MHz, 9600 bps 8N1.
 *
 * Implementa comunicacion serie asincronica mediante ISR de recepcion
 * (USART_RX_vect) y transmision (USART_UDRE_vect), con buffers independientes
 * para cada canal.
 */
#include "uart.h"

/* Buffer lineal de transmision. Se llena desde el foreground y se vacia en la ISR. */
static char bufferTx[SIZE_BUFFERTX_MAX];

/*
  Buffer circular de recepcion.
  volatile: escrito por la ISR USART_RX_vect, leido desde el foreground.
  Se deja una posicion libre para distinguir lleno de vacio:
    headRx == tailRx               -> buffer vacio
    (headRx + 1) % SIZE == tailRx  -> buffer lleno
 */
static volatile char bufferRx[SIZE_BUFFERRX_MAX];

/* Variable para saber el caso inicial en que no se haya transmitido nada y no quedar bloqueado esperando TX */
static bool huboTx = false;


/*
  Indices de los buffers:
    headTx: proxima posicion libre para escribir en TX (foreground).
    tailTx: proximo byte a transmitir en TX (ISR USART_UDRE_vect).
    headRx: proxima posicion libre para escribir en RX (ISR USART_RX_vect).
    tailRx: proximo byte disponible para leer en RX (foreground).
 */
static volatile uint8_t headTx = 0, tailTx = 0, headRx = 0, tailRx = 0;

/* Flag de overflow del buffer RX. Se activa cuando la ISR no puede almacenar un byte. */
static volatile bool flag_OF = false;

/* Flag que indica que la ISR de RX recibio al menos un byte nuevo.
 * Activada por USART_RX_vect (foreground). Consumida por uart_hayDatosRx()
 * en el background para evitar polling  del main. */
static volatile bool flag_RX = false;

/*
  @brief   Inicializa el periferico USART0 a 9600 bps, trama 8N1.

  Configura UBRR0 para 9600 bps a 16 MHz (UBRR = 103).
  Habilita TX, RX y la interrupcion de recepcion por byte (RXCIE0).
 */
void uart_init(void) {
	//BaudRate=16Mhz/(UBRR+1)16 con BaudRate=9600 -> UBRR=103
	UBRR0=103;
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0)  ; 
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); 
	UCSR0A |= (1 << TXC0); // Limpiar TXC0 para que uart_txCompleto() retorne true la primera vez
}

/*
  @brief Habilita la interrupcion UDRIE0 para iniciar la transmision.
 */
void uart_setUDRIE0() {
	huboTx=true; //Para la primera TX
	UCSR0B |= ( 1<<UDRIE0); 
}

/*
  @brief Resetea el buffer de transmision descartando cualquier dato pendiente.
 */
void uart_limpiarBuffer() { 
	headTx=0;
	tailTx=0;
}

/*
  @brief Carga un byte en el buffer de transmision.
  Si el buffer esta lleno el byte se descarta.
 
  @param c  Byte a cargar.
 */
void uart_cargarByteBuffer(const char c) {
	if (headTx < SIZE_BUFFERTX_MAX)
		bufferTx[headTx++]=c;
}

/*
  Lee un byte del buffer circular de recepcion.
  No bloqueante: retorna false inmediatamente si no hay datos disponibles.
 
  @param c   Puntero donde se almacena el byte leido.
  @return   true  Si se leyo un byte correctamente.
  @return   false Si el buffer estaba vacio.
 */
bool uart_leerByteBuffer( char * c) { 
	if (tailRx == headRx)
		return false;
	*c=bufferRx[tailRx++];
	if (tailRx == SIZE_BUFFERRX_MAX){
		tailRx=0;
	}
	return true;
}


/*
 @brief Resetea el buffer de recepcion descartando todos los bytes pendientes y limpia el flag de overflow.
  Deshabilita interrupciones durante el reseteo para evitar condicion de carrera con la ISR USART_RX_vect.
 */
void uart_resetearRx()
{
	uint8_t sreg = SREG;
	cli(); //Controlo concurrencia por si salta una ISR provocada por USART_RX_vect.

	headRx = 0;
	tailRx = 0;
	flag_OF = false;

	SREG = sreg;
}

/*
  @brief Indica si ocurrio un overflow en el buffer de recepcion.
  El overflow se produce cuando la ISR no puede almacenar un byte porque el buffer esta lleno.

  @return True Si hubo overflow
  @return False en caso contrario.
 */
bool uart_huboOV(void) {
	return flag_OF;
}

/*
  @brief Indica si la ISR de RX recibio al menos un byte desde la ultima consulta.

  Permite al background evitar el polling incondicional: el main solo llama a
  terminal_consumirChars() cuando esta funcion retorna true, respetando la
  arquitectura foreground/background estricta.

  @return true  Si hay datos nuevos en el buffer RX (o hubo overflow pendiente).
  @return false Si no hubo actividad desde la ultima consulta.
 */
bool uart_hayDatosRx(void) {
	if (!flag_RX) return false;
	flag_RX = false;
	return true;
}

 
/*
  @brief   ISR de recepcion UART (USART_RX_vect).

  Se ejecuta cada vez que llega un byte completo al registro UDR0.
  Almacena el byte en el buffer bufferRx.
  Si el buffer esta lleno activa flag_OF y descarta el byte.
 */
ISR(USART_RX_vect) {

	char c = UDR0;
	uint8_t siguiente;

	flag_RX = true;   /* notificar al background que hay actividad en RX */

	if (!flag_OF) {
		siguiente = headRx + 1;
		if (siguiente == SIZE_BUFFERRX_MAX)
			siguiente = 0;
		if (siguiente != tailRx) {
			bufferRx[headRx] = c;
			headRx = siguiente;
		} else {
			flag_OF = true;   /* buffer lleno -> overflow */
		}
	}
}
/*
  @brief   Indica si la cadena en curso termino de transmitirse completamente.

  Verifica dos condiciones en orden:
    1. UDRIE0 apagado: la ISR USART_UDRE_vect ya no tiene bytes pendientes en bufferTx.
    2. TXC0 seteado:   el shift register termino de enviar el ultimo byte por el cable.
  Si ambas se cumplen, limpia TXC0 (escribiendo 1) y retorna true.

  @return  true  Si toda la cadena salio completamente por el cable.
  @return  false Si la ISR todavia transmite o el shift register no vacio el ultimo byte.
 */

bool uart_txCompleto(void) {
	if (!huboTx) return true; // Nunca se transmitio nada, no hay nada que esperar
	if (UCSR0B & (1 << UDRIE0)) return false; // ISR todavia transmitiendo
	if (!(UCSR0A & (1 << TXC0))) return false; // Shift register todavia vaciando ultimo byte
	UCSR0A = (1 << TXC0);                     // Limpiar flag (se limpia escribiendo 1)
	return true;
}

/*
  @brief   ISR de transmision UART (USART_UDRE_vect).

  Se ejecuta cada vez que el registro UDR0 esta listo para un nuevo byte.
  Transmite el siguiente byte de bufferTx. Cuando encuentra el '\0' finaliza
  la transmision: limpia el buffer y deshabilita UDRIE0.
 */
ISR(USART_UDRE_vect) {
	
	if (  bufferTx[tailTx] != '\0'){
		UDR0=bufferTx[tailTx++];
	}else 
	{
		uart_limpiarBuffer();
		UCSR0B &= ~( 1<<UDRIE0); 
	}
	
}