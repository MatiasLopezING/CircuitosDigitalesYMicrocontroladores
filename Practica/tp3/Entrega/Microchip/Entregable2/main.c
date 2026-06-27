/**
 * @file    main.c
 * @brief   Monitor de invernadero TP3 - Punto de entrada y bucle principal.
 *
 * Arquitectura foreground/background:
 *   - Background (main while): consume comandos UART, lee sensores cada T segundos
 *     y envia telemetria o alertas segun el estado del invernadero.
 *   - Foreground (ISRs): Timer1 (tick 10 ms), USART RX (recepcion), USART UDRE (TX).
 */
#include "main.h"

static volatile bool flag_T, flag_comando;  /* Flags de sincronizacion ISR -> main      */
static volatile uint8_t T;                  /* Periodo de reporte en segundos (2-60)    */
static char           comando[SIZE_COMANDO_MAX]; /* Buffer del comando en curso          */
static type_Cmd       tipoCmd;              /* Tipo del ultimo comando parseado          */
static type_Data      dataCmd;              /* Datos del ultimo comando parseado         */
static type_statusCmd estado;               /* Resultado del parseo del comando          */
static type_rtcTime   hora;                 /* Hora actual leida del DS3231             */
static char           telemetria[TELEMETRIA_LEN]; /* Buffer de mensaje de telemetria    */
static char           alerta[ALERTA_LEN];         /* Buffer de mensaje de alerta        */
static uint8_t        contador_alertas = 0; /* Cuenta alertas consecutivas (alerta cada 3) */
static type_rtcTime   horaNueva;            /* Nueva hora a escribir en el DS3231        */

int main(void)
{
	//Inicializacion
	sei(); 
    uart_init();
	timer_init();
	i2c_init();
	ds3231_init();
	
	uint8_t temp = 0, hum = 0;
	terminal_enviarMensaje("Sistema Iniciado. Esperando lecturas del DHT11...\r\n");
    while (1) 
    {

		terminal_consumirChars(); //Consumo los caracteres que coloca el usuario como comando
		
		//Logica cada T segundos
		if (timer_pasoT()){ 

			 if(ds3231_getTime(&hora)) {
				 
				 // Leemos el DHT11
				 if (dht11_read(&temp, &hum) != DHT11_OK) {
					 terminal_enviarMensaje("ERROR: Fallo lectura del DHT11\r\n");
				 }

				 type_VentanaHor ventana= parser_getVentana(&hora);
				 type_Estado estado=parser_getEstado(ventana, temp, hum); //NORMAL o ALERTA
				
		
				 if (estado != ESTADO_NORMAL) {
					 contador_alertas++;
					 
					 if (contador_alertas % 3 == 0) {
						parser_getAlerta(alerta, &hora, ventana, estado, temp, hum);
						terminal_enviarMensaje(alerta);
					 }
					 else {
						 parser_getTelemetria(telemetria,&hora, temp, hum, estado);
						 terminal_enviarMensaje(telemetria);
					 }
				} else {
					 contador_alertas = 0;  // reset al volver a normal
					 parser_getTelemetria(telemetria,&hora, temp, hum, estado);
					 terminal_enviarMensaje(telemetria);
				 }
			 } else {
				 terminal_enviarMensaje("ERROR: DS3231 no responde");
			  } 
			
			
		}
		
		//Procesar comando de entrada en caso de que haya
		if (terminal_hayComando()) { 
			
			
			terminal_getComando(comando);

			estado = parser_parsearCmd(comando, &tipoCmd, &dataCmd); //Recibo el estado del comando que el usuario ingreso , el tipo (Dependiendo si fue SET_T o SET_TIME) y la informacion necesaria segun el tipo de comando

			switch (estado) //Segun el estado del comando realizo acciones 
			 {
				 case PARSER_OK: //Comando valido

				 switch (tipoCmd)
				 {
					 case CMD_SET_TIME:
					 
					//Extraigo la informacion de la hora a actualizar en el DS3231
					 horaNueva.hours=dataCmd.hora.hora;
					 horaNueva.minutes=dataCmd.hora.minutos;
					 horaNueva.seconds=dataCmd.hora.segundos;
					
					//Seteo nuevo tiempo
					 ds3231_setTime(&horaNueva);

					 terminal_enviarMensaje("Hora actualizada.");
					 break;

					 case CMD_SET_T:

					//Seteo nuevo periodo T
					 timer_setT(dataCmd.periodo.periodoT);

					 terminal_enviarMensaje("Periodo actualizado.");
					 break;
				 }

				 break;

				 case PARSER_CMD_INVALID: //Comando inexistente, por lo tanto invalido

				 terminal_enviarMensaje("ERROR: comando inexistente.");
				 break;

				 case PARSER_FORMAT_INVALID: //Dentro de los comandos posibles, pero no con el formato deseado -> Decision de modelado del problema
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

				 case PARSER_RANGE_ERROR: //Comando con formato valido pero con variables que exceden los rangos establecidos 
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



