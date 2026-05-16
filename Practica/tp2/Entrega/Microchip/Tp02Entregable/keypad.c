/*
 * keypad.c
 *
 * Created: 5/16/2026 1:44:37 AM
 *  Author: tomas
 */ 
#include "keypad.h"

/********************************************************
FUNCION PARA ESCANEAR UN TECLADO MATRICIAL Y DEVOLVER LA
TECLA PRESIONADA UNA SOLA VEZ. TIENE DOBLE VERIFICACION Y
MEMORIZA LA ULTIMA TECLA PRESIONADA
DEVUELVE:
0 -> NO HAYNUEVA TECLA PRESIONADA
1 -> HAY NUEVA TECLA PRESIONADA Y ES *pkey
********************************************************/
uint8_t KEYPAD_Scan (uint8_t *pkey)
{

	static uint8_t Old_key, Last_valid_key=0xFF; // no hay tecla presionada;
	uint8_t Key,
	Key= KeypadUpdate(); //Falta implementar esta (En la teoria de la clase 8 hay una pero nose bien que hace)
	if(Key==0xFF){
		Old_key=0xFF;
		// no hay tecla presionada
		Last_valid_key=0xFF;
		return 0;
	}
	if(Key==Old_key) { //2da verificación
		if(Key!=Last_valid_key){ //evita múltiple detección
			*pkey=Key;
			Last_valid_key = Key;
			return 1;
		}
	}
	Old_key=Key; //1era verificación
	return 0;
}
