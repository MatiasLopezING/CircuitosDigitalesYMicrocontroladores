
/**
 * @file    terminal.c
 * @brief   Driver de terminal serie para envio de mensajes y recepcion de comandos.
 */

#include "terminal.h"
 
/* Buffer con el comando en curso (se llena caracter a caracter). */ 
static char comando[SIZE_COMANDO_MAX];
/* Buffer con el ultimo comando completo listo para ser procesado. */
static char comando_listo[SIZE_COMANDO_MAX];
/* Indice de escritura en el buffer de armado. */
static uint8_t pos=0;
/* Flag que indica que hay un comando completo pendiente de procesar. */
static bool comando_Pendiente=false;
/* Flag de descarte: activo cuando el comando supero SIZE_COMANDO_MAX.
 * Mientras este activo se ignoran todos los chars hasta el proximo \r o \n. */
static bool descartando=false;



/*
  @brief   Envia un mensaje de texto por la UART hacia la PC.
 
  Carga el mensaje en el buffer de transmision y habilita la ISR de TX
  para que lo envee de forma no bloqueante. Agrega '\r\n' al final. Si el mensaje supera SIZE_BUFFERTX_MAX - 3 caracteres
  se trunca.
 
  @param   mensaje     Puntero a string terminado en '\0' a transmitir.
 */
void terminal_enviarMensaje(const char * mensaje) {
	
	 while (!uart_txCompleto());  // Esto es para que no se pisen 2 mensajes que envio a la terminal, el tiempo es despreciable frente a T por eso hago el while.
	/* Detener la ISR de TX antes de limpiar el buffer para evitar la condicion
	 * de carrera en la que la ISR lee datos viejos o parciales mientras se carga
	 * el nuevo mensaje. uart_setUDRIE0() al final reinicia la transmision. */
	UCSR0B &= ~(1 << UDRIE0);
	uart_limpiarBuffer();

	uint8_t i=0;
	while (mensaje[i] != '\0' && i < SIZE_BUFFERTX_MAX - 3) {
		uart_cargarByteBuffer(mensaje[i++]);
	}

	uart_cargarByteBuffer('\r'); //Carriage return
	uart_cargarByteBuffer('\n'); //Salto de linea
	uart_cargarByteBuffer('\0'); //Finalizacion de cadena

	uart_setUDRIE0(); //Habilito interrupcion por UDR de Transmision empty

}

/*
  @brief   Consume los bytes disponibles en el buffer RX y arma comandos.
 
  Lee todos los bytes disponibles del buffer de recepcion.
  Acumula caracteres hasta recibir '\n' o '\r', momento en que da
  el comando por completo y setea el flag interno.
 
  Maneja dos condiciones de error:
  - Overflow del buffer RX de la UART: resetea la recepcion e informa.
  - Comando demasiado largo (supera SIZE_COMANDO_MAX): resetea e informa.
 
  En ambos casos envia un mensaje de error por terminal y descarta
  el comando en curso.
 */
void terminal_consumirChars() {
	char c;

	if (uart_huboOV()) {
		uart_resetearRx();
		pos = 0;
		descartando = false;
		terminal_enviarMensaje("ERROR: overflow RX. Vuelva a ingresar el comando.");
		return;
	}

	while (uart_leerByteBuffer(&c))
	{
		if (c == '\n' || c == '\r')
		{
			if (descartando) {
				/* Fin del comando largo que se estaba descartando.
				 * Volver a estado limpio sin generar otro error. */
				descartando = false;
				pos = 0;
			} else if (pos > 0) {
				/* Comando completo: copiar y avisar al main. */
				comando[pos] = '\0';
				strlcpy(comando_listo, comando, SIZE_COMANDO_MAX);
				comando_Pendiente = true;
				pos = 0;
			}
			/* Si pos==0 y no descartando: terminador extra (\r\n doble), ignorar. */
		}
		else if (descartando)
		{
			/* Caracter perteneciente al comando demasiado largo: descartar. */
		}
		else if (pos < SIZE_COMANDO_MAX - 1)
		{
			comando[pos++] = c;
		}
		else
		{
			/* Se supero el largo maximo: activar descarte y notificar una sola vez.
			 * No se llama uart_resetearRx() para no perder chars de otros comandos
			 * que pudieran estar en el buffer; se descartan en el mismo while loop. */
			descartando = true;
			pos = 0;
			terminal_enviarMensaje("ERROR: Comando demasiado largo. Ingrese uno valido.");
		}
	}
}

/*
  @brief   Indica si hay un comando completo pendiente de procesar.
 
  @return  true    Si hay un comando listo en el buffer interno.
  @return  false   Si no hay comandos pendientes.
 */
bool terminal_hayComando() {
	return comando_Pendiente;
}

/*
 @brief   Copia el ultimo comando completo al buffer que se envia por parametro.

 Una vez copiado, limpia el flag interno de comando pendiente.
 Debe llamarse solo si terminal_hayComando() devolvio true.

 @param   com     Buffer destino donde se copia el comando.
                  Debe tener al menos SIZE_COMANDO_MAX bytes.
*/
void terminal_getComando(char * com) {

		strcpy(com, comando_listo);
		comando_Pendiente = false;
	
}


