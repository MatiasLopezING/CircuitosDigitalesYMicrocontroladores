#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#define MSB 7
#define LSB 0
#define NOP asm volatile("nop\n\t")

void secuenciaA();
void secuenciaB();
void neopixel_AlternarRojoAzul();
void neopixel_DesplazamientoVerde();
void configurarPuertos();

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
		botonAnt1= PINC & (1 << PINC0); //Guardo estado del boton de la secuencia 1
		botonAnt2= PINC & (1 << PINC1); //Guardo estado del boton de la secuencia 2
		
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
			neopixel_DesplazamientoVerde(); //Se encarga de la logica de prender el led correspondiente
			else
			neopixel_AlternarRojoAzul();
		}
		
    }
}

void configurarPuertos() {
	 DDRD = 0xFF; // Configuro PORTD como salida
	 PORTD = 0x00; // Apagar todos los LEDs al inicio
	 
	 // Configurar PORTC0 y PORTC1 como entradas para los pulsadores
	 DDRC &= ~((1 << PINC0) | (1 << PINC1));  //Seteo PORTC0 y PORTC1 como entrada
	 PORTC |= (1 << PINC0) | (1 << PINC1); //Activo las resistencias de pull-up internas

     // Neopixel (Pin PB0 como salida, arrancando en bajo)
     DDRB |= (1 << PINB0);
     PORTB &= ~(1 << PINB0);
}


void secuenciaA () {
	PORTD=(1 << ledSec1);	
	if(ledSec1 == MSB)
		ledSec1=LSB;
	else 
		ledSec1++;
	
}

void secuenciaB () {
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

void neopixel_AlternarRojoAzul() {
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
}

void neopixel_DesplazamientoVerde() {
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
}



/*-------------------------------------------------------------------------------------------------------*/
/*
Un nop (No Operation) consume exactamente 1 ciclo de reloj.
Dado que tu microcontrolador corre a 16 MHz:

protocolo dice que para enviar un 0, debemos mantener el pin en ALTO por 400 ns y luego en BAJO por 850 ns.

Tiempo de 1 ciclo = 1 / 16,000,000 = 62.5 ns

Enviar un 0:

400ns/62.5ns = 6.4 ciclos (Aproximadamente 6 NOPs para el pulso en ALTO)
850ns/62.5ns = 13.6 ciclos (Aproximadamente 14 NOPs para el pulso en BAJO)

Emvoar un 1:

protocolo dice que para enviar un 1, debemos mantener el pin en ALTO por 800 ns y luego en BAJO por 450 ns.

800ns/62.5ns = 12.8 ciclos (Aproximadamente 13 NOPs para el pulso en ALTO)
450ns/62.5ns = 7.2 ciclos (Aproximadamente 7 NOPs para el pulso en BAJO)

Encender/Apagar el pin: La instrucción en lenguaje ensamblador que usa el compilador para poner un pin en alto
o bajo (sbi o cbi al puerto) consume 2 ciclos 125ns


*/


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