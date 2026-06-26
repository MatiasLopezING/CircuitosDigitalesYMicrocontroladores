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
static type_rtcTime hora;
static char telemetria [TELEMETRIA_LEN], alerta [ALERTA_LEN];
static uint8_t contador_alertas=0;
static type_rtcTime horaNueva;

int main(void)
{
	sei();
    uart_init();
	timer_init();
	i2c_init();
	ds3231_init();
    while (1) 
    {
		
		
		terminal_poll(); //Consumo los caracteres que coloca el usuario como comando
		if (timer_pasoT()){ //Logica cada T segundos

			 if(ds3231_getTime(&hora)) {
				 //dht11_readTempHum(&temp, &hum) Falta implementar
				 type_VentanaHor ventana= parser_getVentana(&hora);
				 type_Estado estado=parser_getEstado(ventana,17,70); //NORMAL o ALERTA
				
		
				 if (estado != ESTADO_NORMAL) {
					 contador_alertas++;
					 
					 if (contador_alertas % 3 == 0) {
						parser_getAlerta(alerta, &hora, ventana, estado,17,70);
						terminal_enviarMensaje(alerta);
					 }
					 else {
						 parser_getTelemetria(telemetria,&hora,17,70,estado);
						 terminal_enviarMensaje(telemetria);
					 }
				} else {
					 contador_alertas = 0;  // reset al volver a normal
					 parser_getTelemetria(telemetria,&hora,17,70,estado);
					 terminal_enviarMensaje(telemetria);
				 }
			 } else {
				 terminal_enviarMensaje("ERROR: DS3231 no responde");
			  } 
			
			
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
					 
				
					 horaNueva.hours=dataCmd.hora.hora;
					 horaNueva.minutes=dataCmd.hora.minutos;
					 horaNueva.seconds=dataCmd.hora.segundos;
					 ds3231_setTime(&horaNueva);

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



