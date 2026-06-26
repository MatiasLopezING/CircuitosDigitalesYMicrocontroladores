/*
 * Entregable2.c
 *
 * Created: 6/24/2026 12:37:30 AM
 * Author : tomas
 */ 

#include "main.h"

static volatile bool flag_T,flag_comando;
static volatile uint8_t T;
static char comando[SIZE_COMANDO_MAX];
static type_Cmd tipoCmd;
static type_Data dataCmd;
static type_statusCmd estado;

int main(void)
{
	sei();
    uart_init();
	
    while (1) 
    {
		terminal_poll(); //Consumo los caracteres que coloca el usuario como comando
		if (flag_T){ //Logica cada T segundos
			
			
		}
		
		if (terminal_hayComando()) { //Procesar comando
			terminal_getComando(comando);
			//terminal_enviarMensaje(comando); //Muestro el texto que ingreso el usuario --> Este texto no llega a mostrarse 
			//Opcion 1 -> Hacer que los mensajes se vayan como encolando cosa de enviar un unico mensaje
			//Opcion 2 -> No mostrar el comando
			estado = parser_parsearCmd(comando, &tipoCmd, &dataCmd);

			switch (estado)
			 {
				 case PARSER_OK:

				 switch (tipoCmd)
				 {
					 case CMD_SET_TIME:

					 //rtc_setHora(dataCmd.hora.hora,dataCmd.hora.minutos,dataCmd.hora.segundos);

					 terminal_enviarMensaje("Hora actualizada.");
					 break;

					 case CMD_SET_T:

					 timer_setT(dataCmd.periodo.periodoT);

					 terminal_enviarMensaje("Periodo actualizado.");
					 break;
				 }

				 break;

				 case PARSER_CMD_INVALID:

				 terminal_enviarMensaje("ERROR: comando inexistente.");
				 break;

				 case PARSER_FORMAT_INVALID:

				 terminal_enviarMensaje("ERROR: formato invalido.");
				 break;

				 case PARSER_RANGE_ERROR:

				 terminal_enviarMensaje("ERROR: valor fuera de rango.");
				 break;
			 }
		}
	}
}



