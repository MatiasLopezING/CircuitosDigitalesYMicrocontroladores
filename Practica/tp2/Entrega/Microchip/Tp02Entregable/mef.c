#include "mef.h"
#include "keypad.h"
#include "actuadores.h"
#include <stdbool.h>
#define TICKSERROR 10
#define ERRORDIGITOS "ERROR DIGITOS"
#define ERRORTIEMPOINVALIDO "ERROR TIEMPO INVALIDO"
#define ERRORTIEMPOCERO "ERROR TIEMPO CERO"

#if 0 // Bloque comentado temporalmente por errores de sintaxis a resolver

static state EstadoActual; //Guardare el estado actual de la MEF
static uint16_t Ticks1s = 0; //Ticks para saber si paso 1 seg (Dependera de la frec del clock del timer), por ahi tambien puedo hacer que me devuelvan un true cuando paso 1 seg
static uint16_t SegundosRestantes=0;
static bool 1erTermino=true;
static uint8_t key;
static uint8_t M1 = 0,M0 = 0,S1 = 0,S0 = 0; //Variables para los digitos a mostrar en el LCD
static uint8_t cantDigitos=0;


static Shift_Digitos (uint8_t digito) {
	M1=M0;
	M0=S1;
	S1=S0;
	S0=digito;
}

static uint16_t Calculo_Segundos(void) {
	
	return (M1*10 + M0)*60 + (S1*10 + S0);
	
}

static bool TiempoValido(void)
{
	uint8_t minutos = M1 * 10 + M0;
	uint8_t segundos = S1 * 10 + S0;

	if ((minutos > 59) || (segundos > 59))
	{
		return false;
	}


	return true;
}

static void Mostrar_Error(string mensaje) {
	
		mostrarError=true; //Aviso de que hay que mostrar el mensaje de error en el display LCD
		tickMostrarError=TICKSERROR; //Seteo la cantidad de ticks que mantendre el mensaje de error en el LCD (Se puede cambiar en el define)
		LCD_PrintMessage(mensaje); //Aviso de error en el display
	
}

void FSM_Init(void) { 
	EstadoActual=REPOSO;
	LCD_Resetear(); //Resetea el contador a 00:00, Esto podria hacerse por ahi desde el main directamente
	
}

void FSM_Update(void)
{
	key = KEYPAD_GetKey();

	switch(EstadoActual)
	{
		case REPOSO:
		
		if(mostrarError) {
		     if(tickMostrarError)
			     tickMostrarError--;
		     else
		     {
			     mostrarError = false;
			     LCD_PrintTime(SegundosRestantes);
		     }

		    
		     break;  //Ignorar input mientras hay error -> Decision de modelado del problema
	     }

	    

		if (key) { //Se presiono tecla	
			if ((key >= '0') AND (key <= '9'))  {
				if( cantDigitos < 4) { //Si no complete los 4 digitos
					cantDigitos++; //Sumo cantidad de digitos ingresados
					Shift_Digitos(key-'0'); //Actualizo digitos luego del ingresado
					SegundosRestantes=Calculo_Segundos(); //Actualizo tiempo
				}
				else { //Aviso de que para modificar el tiempo debe presionar tecla 'B' = STOP/CLEAR
					Mostrar_Error(ERRORDIGITOS)
				}
			}
		   else {
				if(key == 'B') //Se presiono STOP/CLEAR
					SegundosRestantes=0; //Resetea el contador a 00:00
				else 
					if(key == 'A') { //Se presiono START
						if(TiempoValido()) {
							if(SegundosRestantes)
								EstadoActual=COCINANDO;
								cantDigitos=0; //Reseteo para cuando vuelvan a REPOSO o si modifican en PAUSA
							else  {
								Mostrar_Error(ERRORTIEMPOCERO);
							}
						else {
							Mostrar_Error(ERRORTIEMPOINVALIDO);
						}
					}
					else 
						if(key == 'C') { //Se presiono +30 seg
							SegundosRestantes+=30;
							EstadoActual=COCINANDO;
						}
		if (!mostrarError) LCD_PrintTime(SegundosRestantes); //Actualizo el tiempo del display solo si se presiono una tecla nueva y no hay error
		 }
		   
		}
		break;
		
		case COCINANDO:
		
		if (key) {
			if (key == 'C') { // Se presiono +30 seg
				SegundosRestantes+=30;
				LCD_PrintTime(SegundosRestantes);

			}
			else
				if (key == 'D') { // Se abrio la puerta
				EstadoActual=PUERTA_ABIERTA;
				}
				else
					if (key == 'B') //Se presiono STOP/CLEAR
					{
						EstadoActual=PAUSA;
					}
	

		}
		
		
		if (++Ticks1Seg == TICKS1SEG) { //Paso 1 seg
			Ticks1Seg=0;
			SegundosRestantes--;
			if (SegundosRestantes == 0) //Se termino el tiempo de coccion
				EstadoActual=TERMINO;
			LCD_PrintTime(SegundosRestantes); //Solo actualizo cuando pasa 1 seg
		}
		
		
		
		case PUERTA_ABIERTA:
		
		if (key == 'D') { //Se cerro la puerta
			EstadoActual=COCINANDO;
			
		}
		
		case PAUSA:
		
		if (key) {
			if (key == 'B') //Se presiono STOP/CLEAR
			{
				EstadoActual=REPOSO;
				SegundosRestantes=0;
			}
			else
				if ((key >= '0') AND (key <= '9'))  {
					if( cantDigitos < 4) { //Si no complete los 4 digitos
						cantDigitos++; //Sumo cantidad de digitos ingresados
						Shift_Digitos(key-'0'); //Actualizo digitos luego del ingresado
						SegundosRestantes=Calculo_Segundos(); //Actualizo tiempo
					}
					else { //Aviso de que para modificar el tiempo debe presionar tecla 'B' = STOP/CLEAR
						Mostrar_Error(ERRORDIGITOS);
					}
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
	
	FSM_UpdateOutputs(); //Segun el estado actualizo las salidas
}


void FSM_UpdateOutputs(void)
{
	switch(EstadoActual)
	{
		case REPOSO:
		ACTUADORES_MagnetronOff();
		ACTUADORES_LuzOff();
		ACTUADORES_AlarmaOff();
		break;

		case COCINANDO:
		ACTUADORES_MagnetronOn();
		ACTUADORES_LuzOn();
		ACTUADORES_AlarmaOff();
		break;

		case PAUSA:
		ACTUADORES_MagnetronOff();
		ACTUADORES_LuzOff();
		ACTUADORES_AlarmaOff();
		break;
		
		case PUERTA_ABIERTA:
		ACTUADORES_MagnetronOff();
		ACTUADORES_LuzOn();
		ACTUADORES_AlarmaOff();
		break;

		case TERMINO:
		ACTUADORES_MagnetronOff();
		ACTUADORES_LuzOff();
		ACTUADORES_AlarmaOn();
		break;
	}
}
#endif // Fin bloque comentado

// Implementaciones temporales (stubs) para permitir la compilación del módulo principal
void FSM_Init(void) { 
}

void FSM_Update(void) {
}