#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#define MSB 7
#define LSB 0
#define NOP asm volatile("nop\n\t")

static inline void secuenciaA();
static inline void secuenciaB();
static inline void secuenciaC();
static inline void secuenciaD();
static inline void configurarPuertos();
static inline void neopixel_enviarByte(uint8_t byte);
static inline void neopixel_enviarColor(uint8_t rojo, uint8_t verde, uint8_t azul);

uint8_t ledSec1=LSB; //La secuencia 1 comienza con el LSB
uint8_t subiendo=0; //La secuencia 2 comienza desde el MSB hacia el LSB

int main(void)
{
	uint8_t contadorSec1 = 0;
	uint8_t contadorSec2 = 0;
	uint8_t estAct1=0;
	uint8_t estAct2=0;
	uint8_t botonAnt1;
	uint8_t botonAnt2;
	//Se puede optimizar aun mas el tema de los estados utilizando una sola variable con 1 bit por secuencia!
	configurarPuertos();
	
    while (1) 
    {
		/*botonAnt1= PINC & (1 << PINC0); //Guardo estado del boton de la secuencia 1
		botonAnt2= PINC & (1 << PINC1); //Guardo estado del boton de la secuencia 2
		*/
		
		botonAnt1= (PINC & (1 << PINC0)) ? 1 : 0; 
        botonAnt2= (PINC & (1 << PINC1)) ? 1 : 0;

		//Ventana de tiempo 
		_delay_ms(50); 
		
		//Incremento el contador de ambas secuencias que corren en simultaneo
		contadorSec1++; 
		contadorSec2++;
		
		//Chequeo si se presiono el boton para cambiar la secuencia 1
		
		if ( ((PINC & (1 << PINC0)) == 0) && (botonAnt1 == 1) ) { //Si se presiono el boton 1 y previamente no estaba presionado
			contadorSec1=0; //Reseteo el contador de la secuencia para asegurarme 100ms entre cada led prendido
			//Actualizo segun la secuencia a realizar
			if (estAct1==1){ 
			 estAct1=0;
			 ledSec1=LSB;
			}
			else {
			 estAct1=1;
			 ledSec1=MSB;
			 subiendo=0; //Reinicio la secuencia 2 para que arranque desde el MSB hacia el LSB
			}
		}
		//Si pasaron 100 ms realizo la secuencia 1 correspondiente	
	    if (!(contadorSec1 % 2)) {
		    contadorSec1=0;
			if(estAct1==0)
				secuenciaA(); 
			else 
				secuenciaB();
		}
		
		if ( ((PINC & (1 << PINC1)) == 0) && (botonAnt2 == 1) ) //Si se presiono el boton 2 y previamente no estaba presionado
			if(estAct2==1)
				estAct2=0;
			else 
				estAct2=1;	
				 
		//Si pasaron 150 ms realizo la secuencia 2 correspondiente
		if (!(contadorSec2 % 3)) {
			contadorSec2=0;
			if(estAct2==1)
			secuenciaC(); //Se encarga de la logica de prender el led correspondiente
			else
			secuenciaD();
		}
		
    }
}

static inline void configurarPuertos() {
	 DDRD = 0xFF; // Configuro PORTD como salida
	 PORTD = 0x00; // Apagar todos los LEDs al inicio
	 
	 // Configurar PORTC0 y PORTC1 como entradas para los pulsadores
	 DDRC &= ~((1 << PINC0) | (1 << PINC1));  //Seteo PORTC0 y PORTC1 como entrada
	 PORTC |= (1 << PINC0) | (1 << PINC1); //Activo las resistencias de pull-up internas

     // Neopixel (Pin PB0 como salida, arrancando en bajo)
     DDRB |= (1 << PINB0);
     PORTB &= ~(1 << PINB0);
}


static inline void secuenciaA () {
	PORTD=(1 << ledSec1);	
	if(ledSec1 == MSB)
		ledSec1=LSB;
	else 
		ledSec1++;
	
}

static inline void secuenciaB () {
	PORTD = (1 << ledSec1);
	if (ledSec1 == MSB) //Si llegue al MSB bajo hacia el LSB
		subiendo=0;
	else
		if(ledSec1 == LSB)  //Si llegue al LSB, reboto hacia el MSB
		   subiendo=1;
	if(subiendo) //Sigo subiendo/bajando segun corresponda
	  ledSec1++;
	else 
	  ledSec1--;
	
}

	

// Envía un solo byte (8 bits) al Neopixel
static inline void neopixel_enviarByte(uint8_t byte) {
    for(uint8_t i = 0; i < 8; i++) {
        if (byte & 0x80) { // BIT '1'
            PORTB |= (1 << PINB0);
            NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;  // 812.5ns (10 ciclos de NOP + 3 de asignacion posterior)
            PORTB &= ~(1 << PINB0);
            NOP; NOP; NOP; NOP; NOP; NOP;  // ~437.5 ns bajo (7 NOPs + Overhead de hacer byte <<= 1 (1 ciclo=62.5ns) + Overhead for)
        } else { // BIT '0'
            PORTB |= (1 << PINB0);
            NOP; NOP; NOP;  // ~375ns en alto ( 3 Ciclos de NOP's + 3 ciclos de la asignacion posterior)
            PORTB &= ~(1 << PINB0);
            NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP; NOP;   // ~750ns bajo + Overhead de hacer byte <<= 1 (1 ciclo=62.5ns) + Overhead for
        }
        byte <<= 1;
    }
}
/*-------------------------------------------------------------------------------------------------------*/
/*
Un NOP consume 1 ciclo de reloj.
Tiempo de 1 ciclo = 1 / 16,000,000 = 62.5 ns

El datasheet del Neopxiel dice que para enviar un codigo de '0', debemos mantener el pin en ALTO por 400 ns y luego en BAJO por 850 ns.

400ns/62.5ns = 6.4 ciclos 
850ns/62.5ns = 13.6 ciclos 

El datasheet del Neopixel dice que para enviar un codigo de '1', debemos mantener el pin en ALTO por 800 ns y luego en BAJO por 450 ns.

800ns/62.5ns = 12.8 ciclos 
450ns/62.5ns = 7.2 ciclos 

Encender/Apagar el pin: Al hacer instrucciones del tipo PORTB &= ~(1 << PINB0) o PORTB |= (1 << PINB0) se verifico que al compilar el codigo
el codigo en Assembly generado ocupaba 3 ciclos de instruccion para realizar la actualizacion del pin 0 del puerto B. Por lo que se tomo esto en consideracion 
a la hora de elegir la cantidad de NOP's a ejecutar.

A su vez se tuvo en cuenta que la tolerancia de los tiempos mencionados es de +- 150 ns.
*/

// Envía un color completo (24 bits) a un LED
static inline void neopixel_enviarColor(uint8_t rojo, uint8_t verde, uint8_t azul) {
    // El protocolo WS2812 requiere que el orden sea Verde, Rojo, Azul (GRB)
    neopixel_enviarByte(verde); 
    neopixel_enviarByte(rojo);
    neopixel_enviarByte(azul);
}

static inline void secuenciaC() {
    static uint8_t faseColor = 0; 
    faseColor = !faseColor; 
    
    for(uint8_t i = 0; i < 8; i++) {
        if (i % 2 == 0) {
            // LEDs Pares
            if (faseColor) neopixel_enviarColor(255, 0, 0); // Rojo
            else neopixel_enviarColor(0, 0, 0);             // Apagado
        } else {
            // LEDs Impares
            if (!faseColor) neopixel_enviarColor(0, 0, 255); // Azul
            else neopixel_enviarColor(0, 0, 0);              // Apagado
        }
    }
	_delay_us(60); // Tiempo de reset para los Neopixels (más de 50us)
}

static inline void secuenciaD() {
    static uint8_t posicionLedVerde = 7; // Arranca en el extremo derecho
    
    for(uint8_t i = 0; i < 8; i++) {
        if (i == posicionLedVerde) {
            neopixel_enviarColor(0, 255, 0); // Verde
        } else {
            neopixel_enviarColor(0, 0, 0);   // Apagado
        }
    }
    
    // Mover hacia la izquierda y reiniciar al llegar al extremo
    if (posicionLedVerde == 0) posicionLedVerde = 7;
    else posicionLedVerde--;
	
	_delay_us(60); // Tiempo de reset para los Neopixels (más de 50us)

}






/*	version a revisar del codigo para inciso 2	*/
/* // DDRC &= ~((1 << PINC0) | (1 << PINC1));

/*  	Anexo 1: 
EXPLICACIÓN DETALLADA:
 * 1. (1 << PINC0) | (1 << PINC1): Desplaza un "1" a las posiciones de PINC0 (bit 0) y PINC1 (bit 1) 
 *    y los suma (OR). Esto crea una máscara binaria: 00000011.
 * 2. El operador '~' (NOT) invierte todos los bits de esa máscara. Nos queda: 11111100.
 * 3. El operador '&=' (AND a nivel de bits) compara el valor actual de DDRC con 11111100. 
 *    Cualquier cosa operada con '1' se queda igual, y cualquier cosa operada con '0' se vuelve '0'.
 * RESUMEN: Esta línea fuerza a que los pines C0 y C1 tengan un valor de '0' en su registro 
 * de dirección (DDRC) sin afectar a los demás pines del puerto. En arquitectura AVR, 
 * escribir un '0' en el DDR configura el pin como ENTRADA (ideal para leer botones o sensores).
 */

/*			Anexo 2        

// PORTC |= (1 << PINC0) | (1 << PINC1);

 EXPLICACIÓN DETALLADA:
 * 1. Al igual que antes, crea la máscara binaria con unos en las posiciones 0 y 1: 00000011.
 * 2. El operador '|=' (OR a nivel de bits) compara el valor actual de PORTC con 00000011.
 *    Cualquier cosa operada con '0' se queda igual, y cualquier cosa operada con '1' se vuelve '1'.
 * RESUMEN: Esta línea fuerza a que los pines C0 y C1 tengan un valor de '1' en el registro PORTC 
 * sin modificar el resto de los pines.
 * COMPORTAMIENTO AVR: Cuando un pin ya está configurado como ENTRADA (gracias a la línea anterior), 
 * si le escribes un '1' en su registro PORT, el microcontrolador conecta físicamente una "resistencia 
 * pull-up interna". Esto hace que el pin lea siempre un "1" lógico estable por defecto, evitando 
 * lecturas erróneas por ruido estático, hasta que presiones un botón y lo conectes a tierra (leyendo un "0").
*/