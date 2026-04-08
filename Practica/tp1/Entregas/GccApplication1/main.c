#include <avr/io.h>

void secuenciaA();
void secuenciaB();
void secuenciaC();
void secuenciaD();

uint8_t ledSec1,subiendo;

int main(void)
{
	
	uint8_t contadorSec1, contadorSec2,estAct1,estAct2;
	//Se puede optimizar aun mas el tema de los estados utilizando una sola variable con 1 bit por secuencia!
    while (1) 
    {
		_delay_ms(50); //Cada 50 ms chequeo 
		contadorSec1++; 
		contadorSec2++;
		//Incremento el contador de ambas secuencias que corren en simultaneo
		
		if ( (PINC & (1 << PINC0)) ==1 ) //Chequeo cambio de estado de la secuencia 1. ACA HAY QUE VER EL TEMA DE QUE NO QUEDE PULSADO MUCHO TIEMPO Y SE QUEDE ALTERNANDO
			if (estAct1==1) //Actualizo la secuencia 1 (A o B)
			 estAct1=0;
			else 
			 estAct1=1;
	    if (!(contadorSec1 % 2)) //Si pasaron 100 ms realizo la secuencia correspondiente
			{
				if(estAct1==1)
					secuenciaB(); //Se encarga de la logica de prender el led correspondiente
				else 
					secuenciaA();
			}
		
		if ( PINC1 == 1 )
			if(estAct2==1)
				estAct2=0;
			else 
				estAct2=1	 
		if (!(contadorSec2 % 3))   //Si pasaron 150 ms realizo la secuencia correspondiente
		{ 
			
			if(estAct2==1)
			secuenciaD(); //Se encarga de la logica de prender el led correspondiente
			else
			secuenciaC();
		}
		
    }
}
void secuenciaB () {
	PORTD=(1 << ledSec1);
	if (ledSec1 == 7){
		ledSec1--;
		subiendo=0;}
	else
		if(ledSec1==0) {
			ledSec1++;
			subiendo=1;}
		else 
			if(subiendo)
				ledSec1++;
			else ledSec1--;
	
}

void secuenciaA () {
	PORTD=(1 << ledSec1);	
	if(ledSec1== 7)
		ledSec1=0;
	else 
		ledSec1++;
	
}

void secuenciaC () {
	
}

void secuenciaD () {
	
}



/*-------------------------------------------------------------------------------------------------------*/
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

#define F_CPU 16000000UL // Ajusta esto a la frecuencia de tu simulación en Proteus

#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
    DDRD = 0xFF; // DDRD = 0b11111111; // Configura PORTD como salida
    PORTD = 0x00; // Apagar todos los LEDs al inicio
    
    // Configurar PORTC0 y PORTC1 como entradas para los pulsadores
    DDRC &= ~((1 << PINC0) | (1 << PINC1));  //Anexo1
    PORTC |= (1 << PINC0) | (1 << PINC1); // Anexo2
    
    // Variables del sistema (Usamos variables locales de 8-bits para optimizar SRAM y velocidad)
    uint8_t tick_50ms = 0;
    
    // Variables de estado para el Inciso 1
    uint8_t estado_secuencia_D = 0; // 0 = Secuencia A, 1 = Secuencia B
    uint8_t indice_A = 0;
    uint8_t indice_B = 7;     // Empieza en MSB
    int8_t  direccion_B = -1; // -1 para bajar hacia el LSB, 1 para subir
    
    while (1) 
    {
        // --- GENERADOR DE TICK BASE ---
        _delay_ms(50);
        tick_50ms++;
        
        // --- TAREA 1: LEDs PORTD (Retardo de 100ms = 2 ticks)  ---
        if ((tick_50ms % 2) == 0) 
        {
            if (estado_secuencia_D == 0) 
            {
                // Secuencia A: Enciende un LED a la vez desde LSB hacia MSB 
                PORTD = (1 << indice_A);
                indice_A++;
                if (indice_A > 7) {
                    indice_A = 0; // Reinicia el ciclo
                }
            } 
            else 
            {
                // Secuencia B: Empieza desde MSB y rebota 
                PORTD = (1 << indice_B);
                indice_B += direccion_B;
                
                // Control de rebote en los extremos
                if (indice_B == 0) {
                    direccion_B = 1;  // Cambia dirección hacia arriba
                } else if (indice_B == 7) {
                    direccion_B = -1; // Cambia dirección hacia abajo
                }
            }
        }
        
        // --- MANTENIMIENTO DEL SUPER LOOP ---
        // Reseteamos el contador al mínimo común múltiplo de las tareas (2 y 3 ticks = 6)
        // para evitar que la variable crezca infinitamente.
        if (tick_50ms >= 6) {
            tick_50ms = 0;
        }
    }
}