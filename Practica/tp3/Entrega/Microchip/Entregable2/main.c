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
	timer_init();
	
    while (1) 
    {
		terminal_poll(); //Consumo los caracteres que coloca el usuario como comando
		if (timer_pasoT()){ //Logica cada T segundos

			 
			 //rtc_getTime(&hora);
			 //dht11_read(&temp, &hum);
			 
			// uint8_t estadoSis = evaluar_rangos(hora, temp, hum); // NORMAL o ALERTA
			// enviar_telemetria(hora, temp, hum, estado);
			 
			// if (estadoSis == ALERTA) {
			//	 contador_alertas++;
			//	 if (contador_alertas % 2 == 0)  // cada 2 tramas
			//		enviar_alerta(hora, temp, hum);
			 //}
			 //Me va a conveir hacer en el parser una funcion que le paso hora, temp, humedad y estado y me arma el string para enviar a la terminal
			terminal_enviarMensaje("[HH:MM:SS] T: XX°C | H: XX% | Estado: NORMAL/ALERTA");
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
						 switch (tipoCmd)
						 {
							 case CMD_SET_TIME:
							 terminal_enviarMensaje("ERROR: formato invalido. El formato es SET_TIME=HH:MM:SS");
							 break;

							 case CMD_SET_T:
							 terminal_enviarMensaje("ERROR: formato invalido. El formato es SET_T=T");
							 break;
						 }


				 break;

				 case PARSER_RANGE_ERROR:
						 switch (tipoCmd)
						 {
							 case CMD_SET_TIME:
							 terminal_enviarMensaje("ERROR: Rango invalido. Maximo tiempo posible es 23:59:59");
							 break;

							 case CMD_SET_T:
							 terminal_enviarMensaje("ERROR: Rango invalido. T debe estar entre 2 y 60 seg");
							 break;
						 }
				 break;
			 }
		}
	}
}



