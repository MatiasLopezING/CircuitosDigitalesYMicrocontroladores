/*
 * CFile1.c
 *
 * Created: 6/24/2026 12:53:49 AM
 *  Author: tomas
 */ 

#include "terminal.h"
 
static char comando[SIZE_COMANDO_MAX];
static char comando_listo[SIZE_COMANDO_MAX];
static uint8_t pos=0;
static bool comando_Pendiente=false;

void terminal_enviarMensaje(const char * mensaje) {
	uart_limpiarBuffer();
	uint8_t i=0;
	while (mensaje[i] != '\0' && i < SIZE_BUFFERTX_MAX - 3) {
		uart_cargarByteBuffer(mensaje[i++]);
	}
	uart_cargarByteBuffer('\r'); //Carriage return
	uart_cargarByteBuffer('\n'); //Salto de linea
	uart_cargarByteBuffer('\0'); //Finalizacion de cadena
	uart_setUDRIE0(); //Habilito interrupcion por UDR de Transmision empty

}

void terminal_poll() {
	char c;

	if (uart_huboOV())
	{
		uart_resetearRx();
		pos = 0;
		terminal_enviarMensaje("ERROR: overflow. Vuelva a ingresar comandos");
		return;
	}

	while (uart_leerByteBuffer(&c))
	{
		

		if (c == '\n' || c == '\r')
		{
				comando[pos] = '\0';
				strlcpy(comando_listo, comando,SIZE_COMANDO_MAX);
				comando_Pendiente = true;
				pos = 0;
		}
		else 
			if (pos < SIZE_COMANDO_MAX - 1) {
				comando[pos++] = c;
				}
			else
			{
				uart_resetearRx();
				pos = 0;
				terminal_enviarMensaje("ERROR: Comando demasiado largo. Ingrese uno valido.");
			}
		
	}
}

bool terminal_hayComando() {
	return comando_Pendiente;
}

void terminal_getComando(char * com) {

		strcpy(com, comando_listo);
		comando_Pendiente = false;
	
}

void terminal_limpiar() { //Todavia tengo que decidir como hacer esto y a su vez como manejar que el usuario vea el comando que estaba escribiendo
	//Opcion 1: Que no vea nada y al choto -->> POR AHORA VAMOS CON ESTA PARA MAYOR SIMPLICIDAD
	//Opcion 2: Que lo vea unicamente cuando presione enter (Jere y rasa)
	//Opcion 3: Que lo vea todo el tiempo hasta que aparezca una telemetria y luego dejar la telemetria un tiempo dado y volver a mostrar el comando (Dificilismo I guess)
	//Para todas estas opciones creo que tendria que implementar o un polling para saber que se termino de enviar un mensaje (para no perder envios ni pisarlos) pero esto tiene la desventaja que si me interrumpen por ejemplo para mostrar una telemetria hasta que no se terminen de enviar los msj pendientes no se vera la telemetria (pero creo que los tiempos son despreciables en comparacion con T)
	/* Telemetría lista ? telemetriaPendiente = true
	 tick() ? uart libre ? envía telemetría
	 tick() ? uart ocupado ? espera
	 tick() ? uart libre ? nada pendiente ? idle
	 Usuario presiona Enter ? encolar "[CMD] xxxxx\r\n"
	 tick() ? uart libre ? envía confirmación
	 Sin bloqueos, sin condiciones de carrera, y la telemetría siempre tiene prioridad porque la chequeás primero en el tick.
	 Ademas podria en vez de borrar la linea escribir en lineas separadas*/
	 terminal_enviarMensaje("\b");
}

