#include "mef.h"
#include "keypad.h"
#include <stdbool.h>

static state EstadoActual;
static uint16_t Ticks1s = 0;
static uint8_t SegundosRestantes=0;
static bool MAGNETRON_ON=false; //Nose si esto es booleano
static bool LUZINTERIOR_ON=false; //Nose si esto es booleano
static bool ALARMA_ON=false; //Nose si esto es booleano
static bool 1erTermino-true;

void FSM_Init(void) { 
	CurrentState=REPOSO;
	LCD_Resetear(); //Resetea el contador a 00:00
	
}

void FSM_Update(void)
{
	key = KEYPAD_GetKey();

	switch(EstadoActual)
	{
		case REPOSO:

		if(key == 'B') //Se presiono STOP/CLEAR
		{
			LCD_Resetear(); //Resetea el contador a 00:00
		}
		
		if(key == 'A') { //Se presiono START
			SegundosRestantes=LCD_LeerTiempo();
			EstadoActual=COCINANDO;
			MAGNETRON_ON=true; //Esto en realidad lo voy a pasar a hacer en Actualizar Salida
		}
		
		if(key == 'C') { //Se presiono +30 seg
			SegundosRestantes+=30;
			EstadoActual=COCINANDO;
		}
		break;
		
		case COCINANDO:
		
		if (key == 'C') { // Se presiono +30 seg
			SegundosRestantes+=30;
			LCD_Actualizar(SegundosRestantes);
		}
		
		if (key == 'D') { // Se abrio la puerta
			EstadoActual=PUERTA_ABIERTA;
		}
		
		if (key == 'B') //Se presiono STOP/CLEAR
		{
			EstadoActual=PAUSA;
		}
		
		if (TerminoTimerCoccion) //Se termino el tiempo de coccion
		{
			EstadoActual=TERMINO;
		}
		
		case PUERTA_ABIERTA:
		
		if (key == 'D') { //Se cerro la puerta
			EstadoActual=COCINANDO;
			
		}
		
		case PAUSA:
		
		if (key == 'B') //Se presiono STOP/CLEAR
		{
			EstadoActual=REPOSO;
			LCD_Resetear(); //Nose si es necesario hacerlo aca 
		}
		
		case TERMINO:
		
		if (1erTermino) {
			1erTermino=false;
			IniciarTimer5Seg();
		}
		
		LCD_Parpadear(); //Habria que ver como hacemos esto
		
		if (TerminoTimer5Seg) {
			EstadoActual=REPOSO;
			1erTermino=true;
		}
		
		
		
	}
	
	ActualizarSalidas(EstadoActual); //Segun el estado actualizo las salidas
}