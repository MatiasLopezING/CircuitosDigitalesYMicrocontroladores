
#ifndef TERMINAL_H_
#define TERMINAL_H_

#include "uart.h"

#define SIZE_COMANDO_MAX 19  //Tamaño del comando mas largo -> SET_TIME=HH:MM:SS\n\0

void terminal_enviarMensaje(const char * mensaje);
void terminal_limpiar();
void terminal_getComando(char * com);
bool terminal_hayComando();
void terminal_consumirChars();


#endif /* TERMINAL_H_ */