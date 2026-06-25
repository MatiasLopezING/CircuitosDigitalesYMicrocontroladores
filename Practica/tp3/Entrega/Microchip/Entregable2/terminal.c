/*
 * CFile1.c
 *
 * Created: 6/24/2026 12:53:49 AM
 *  Author: tomas
 */ 

#include "terminal.h"


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

