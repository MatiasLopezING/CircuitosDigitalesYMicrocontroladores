#include "mef.h"
#include "keypad.h"
#include "actuadores.h"
#include "lcd.h"
#include "timer.h"
#include <stdbool.h>

#define TICKSERROR 200  // Defino la cantidad de ticks por los que mostrare un error en el LCD. Como FSM_Update() se hace cada 10 ms, mostrare el mensaje de error por 200*10ms= 2seg
#define TICKSDISPLAY 50 // Defino la cantidad de ticks por los que el display estara prendido/apagado al estar parpadeando. Como FSM_Update() se hace cada 10 ms, el display se apagara y prendera cada 50*10ms= 0,5 seg
#define TICKSALARMA 50  // Defino la cantidad de ticks por los que la alarma estara prendido/apagado al estar conmutando entre estos estados. Como FSM_Update() se hace cada 10 ms, la alarma se apagara y prendera cada 50*10ms= 0,5 seg

typedef enum {
	ERRORTIEMPOCERO,
	ERRORTIEMPOINVALIDO,
	ERRORPUERTAABIERTA
	
} Error_t;

//-------------------------Variables privadas de la MEF--------------------------------------//

static state EstadoActual; //Estado actual de la MEF

static uint16_t SegundosRestantes=0; //Segundos restantes de coccion

static bool hayError=false; //Variable para detectar errores
static uint16_t ticksMostrarError; //Cuenta de ticks para la muestra de los errores

static bool primerTermino=true,primerCocinando=true; //Variables para saber si se acaba de transicionar al estado TERMINO o COCINANDO. Con estas sera posible asegurarnos que el primer segundo que pase dure exactamente 1 seg y no tenga una duracion menor.

static bool errorEnPausa; //Variable para saber cuando se origino un error por la puerta abierta en el estado de PAUSA y poder mostrar el tiempo correspondiente en el display

static uint8_t hayTecla; //Variable para saber si se detecto tecla del KEYPAD
static uint8_t key; //Tecla leida del KEYPAD

static uint8_t ticksParpadeoDisplay=0,segTermino=0,estadoDisplay=1; //Variables para lograr el parpadeo del display en estado TERMINO

static uint8_t ticksParpadeoAlarma=0; //Variable para contar ticks para el parpadeo de la alarma en TERMINO

static uint8_t M1 = 0,M0 = 0,S1 = 0,S0 = 0; //Digitos a mostrar en el LCD (M1M0:S1S0)

static bool puertaAbierta = false; //Estado de la puerta

//-------------------------Funciones privadas de la MEF--------------------------------------//

static void Shift_Digitos (uint8_t digito) { //Desplazamiento al ingresar un nuevo digito
	M1=M0;
	M0=S1;
	S1=S0;
	S0=digito;
}

static void Resetear_Digitos () { //Reseteo de los digitos a 00:00
	M1=0;
	M0=0;
	S1=0;
	S0=0;
}

static uint16_t Calculo_Segundos(void) { //Calculo de los segundos en base a los digitos seleccionados por el usuario (M1M0:S1S0)
	
	return (M1*10 + M0)*60 + (S1*10 + S0);
	
}

static bool Tiempo_Valido(void) //Funcion para saber si el tiempo es valido. Ej de tiempo invalido seria 66:66 ya que tanto los minutos como los segundos exceden el valor de 59 -> Decision de modelado del problema no se aceptaran mas de 59 segundos ni mas de 59 minutos.
{
	uint8_t minutos = M1 * 10 + M0;
	uint8_t segundos = S1 * 10 + S0;

	if ((minutos > 59) || (segundos > 59))
	{
		return false;
	}

   
	return true;
}

static void Mostrar_Error( Error_t error) { //Permite mostrar los distintos tipos de errores en el LCD
	
		LCDclr();
	    errorEnPausa = (EstadoActual == PAUSA);  //  Guardo contexto para saber que imprimir al terminar Procesar_error
		hayError=true; //Aviso de que hay que mostrar el mensaje de error en el display LCD
		ticksMostrarError=TICKSERROR; //Seteo la cantidad de ticks que mantendre el mensaje de error en el LCD (Se puede cambiar en el define)
		LCDGotoXY(0,0); //Posicionamiento
		switch(error) {
			 case ERRORTIEMPOCERO:
			 LCDstring((uint8_t*)"COLOQUE TIEMPO", 14);
			 LCDGotoXY(0,1);
			 LCDstring((uint8_t*)"MAYOR A CERO", 12);
			 break;
			 case ERRORTIEMPOINVALIDO:
			 LCDstring((uint8_t*)"COLOQUE TIEMPO", 14);
			 LCDGotoXY(0,1);
			 LCDstring((uint8_t*)"VALIDO", 6);
			 break;
			 case ERRORPUERTAABIERTA: 
			 LCDstring((uint8_t*)"CIERRE LA PUERTA", 16);
			 LCDGotoXY(0,1);
			 LCDstring((uint8_t*)"PARA CONTINUAR", 14);
			 break;
		}
	

}

static void Procesar_Error(){
	 if(ticksMostrarError > 0)
		ticksMostrarError--;
	 else
	 {
		 hayError = false;
		 LCDclr();
		 if (errorEnPausa) // Si vengo de un error en PAUSA, es porque la puerta estaba abierta y quise reanudar la coccion, por lo que debo imprimir los segundos que se hayan establecido y no los digitos que habia ingresado inicialmente el usuario
		   LCD_PrintTime(SegundosRestantes);
		 else
		   LCD_PrintDigits(M1,M0,S1,S0);
		
	 }
}

//-------------------------Funciones publicas de la MEF--------------------------------------//

void FSM_Init(void) { 
	EstadoActual=REPOSO;
	LCD_PrintTime(0); 
	
}

void FSM_Update(void)
{
	hayTecla=KEYPAD_Scan(&key);

	if (hayTecla && key == 'D') { //Me guardo el estado de la puerta para tomar decisiones en cada estado
		puertaAbierta ^= 1;
		hayTecla = 0;  // Consumir la tecla para que no la procese el switch
	}
	switch(EstadoActual) {
	
		case REPOSO:
		
		
		//Logica para muestra de errores una cantidad dada de tiempo
		if(hayError) {
		    Procesar_Error();
	     }

	    else {  //Ignorar input mientras hay error -> Decision de modelado del problema

			if (hayTecla) { //Se presiono tecla	
				if ( (key >= '0') && (key <= '9') )  {
						Shift_Digitos(key-'0'); //Actualizo digitos luego del ingresado
					}
				 else 
					if(key == 'B') { //Se presiono STOP/CLEAR
						Resetear_Digitos();
					}
					else 
						if(key == 'A') { //Se presiono START
							if (puertaAbierta) {
								Mostrar_Error(ERRORPUERTAABIERTA);
							}
							else
							if(Tiempo_Valido()) { //Tiempo valido
								SegundosRestantes=Calculo_Segundos();
								if(SegundosRestantes) {
									EstadoActual=COCINANDO;
								}
								else  {
									Mostrar_Error(ERRORTIEMPOCERO);
								}
							}
							else {
								Resetear_Digitos(); //Si puso tiempo invalido directamente reseteo los digitos -> Decision de modelado del problema
								Mostrar_Error(ERRORTIEMPOINVALIDO);
							}
						}
						else 
							if(key == 'C') { //Al presionarse la tecla C de +30 seg se le da inicio rapido al microondas con un tiempo de 30 segundos -> Decision de modelado del problema
								if (puertaAbierta) {
									Mostrar_Error(ERRORPUERTAABIERTA);
								}
								else{
								SegundosRestantes=30;
								Resetear_Digitos();
								S1=3;
								primerCocinando=true;
								EstadoActual=COCINANDO;
								}
							}
			
			if (!hayError) LCD_PrintDigits(M1,M0,S1,S0); //Actualizo el tiempo del display solo si se presiono una tecla nueva y no se genero error en el medio			
			
			}
			
		}
		
		break;
		
		case COCINANDO:
		
		if (puertaAbierta) {
			EstadoActual = PAUSA;
			primerCocinando = true;
		}
		else {
		if(primerCocinando) { //Nos aseguramos de que el 1er seg del tiempo de coccion dure 1 seg entero, ya que podria pasar que el flag de 1seg se active muy rapido
			primerCocinando=false;
			TIMER_ResetTimerSeg(); //Para que el 1er seg no sea corto 
			LCD_PrintTime(SegundosRestantes); //Para imprimir el primer valor de la cuenta
		}
		
		if (hayTecla) {
			if (key == 'C') { // Se presiono +30 seg
				SegundosRestantes+=30;
				LCD_PrintTime(SegundosRestantes);

			}
			else
				if ( key == 'B') { // Se presiono tecla B STOP/CLEAR
					EstadoActual=PAUSA;
					primerCocinando=true; //Vuelvo a poner en true para cuando se vuelva a este estado
				}

	

		}
		
		
		if (flag_tick_1s) { //Paso 1 seg
			flag_tick_1s=0;
			SegundosRestantes--;
			if (SegundosRestantes == 0) { //Se termino el tiempo de coccion
				EstadoActual=TERMINO;
				primerCocinando=true;//Vuelvo a poner en true para cuando se vuelva a este estado
				}
			LCD_PrintTime(SegundosRestantes); //Solo actualizo cuando pasa 1 seg
		}
		
		}
		
		
		break;
		
		case PAUSA:
		
		if(hayError) {
			Procesar_Error();
		}
		else {
			if (hayTecla) {
				 if (key == 'C') { //Agregar 30 segundos al tiempo de coccion
					 SegundosRestantes += 30;
					 LCD_PrintTime(SegundosRestantes);
				 }
				 else if (key == 'A') { // Reanudar con el tiempo que quedaba
					 if (puertaAbierta) {
						 Mostrar_Error(ERRORPUERTAABIERTA);
					 } else {  
					 primerCocinando=true;
					 EstadoActual = COCINANDO;
					   }
					 }
					else if (key == 'B') {  // Cancelar y volver a REPOSO
						Resetear_Digitos();
						LCD_PrintDigits(M1,M0,S1,S0);
						SegundosRestantes = 0;
						EstadoActual = REPOSO;
				 }
		
			}
		}
		break;
		
		case TERMINO:
		
		if (primerTermino) {
			primerTermino=false;
			TIMER_ResetTimerSeg(); //Para que el 1er seg no sea corto 
			ACTUADORES_AlarmaOn();
		}
		
		// Toggle de la alarma cada 500ms
		if (++ticksParpadeoAlarma >= TICKSALARMA) {
			ticksParpadeoAlarma = 0;
			ACTUADORES_AlarmaToggle();
		}

		
		 // Parpadeo del display cada 500ms (mismo criterio que alarma)
		 if (++ticksParpadeoDisplay >= TICKSDISPLAY) {
			 ticksParpadeoDisplay = 0;
			 estadoDisplay ^= 1;
			 LCD_SetearEstado(estadoDisplay);
		 }

		 if (flag_tick_1s) {
			 flag_tick_1s = 0;
			 segTermino++;
			 if (segTermino == 5) { //Pasaron los 5 seg de alarma, vuelvo a estado de REPOSO
				 //Reseteo de variables y estados
				 primerTermino = true;
				 segTermino = 0; 
				 ticksParpadeoDisplay = 0; 
				 estadoDisplay = 1;
				 LCD_SetearEstado(1);  // Asegurar display encendido al salir
				 ACTUADORES_AlarmaOff(); //Asegurar alarma apagada al salir
				 Resetear_Digitos();
				 LCD_PrintDigits(M1,M0,S1,S0);
				 EstadoActual = REPOSO;
			 }
		 }
		
		break;
		
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

		case TERMINO:
		ACTUADORES_MagnetronOff();
		ACTUADORES_LuzOff();
		break;
	}
}
