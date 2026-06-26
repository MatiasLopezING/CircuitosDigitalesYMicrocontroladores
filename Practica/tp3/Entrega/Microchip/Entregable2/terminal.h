/*
 * terminal.h
 *
 * Created: 6/24/2026 3:08:49 AM
 *  Author: tomas
 */ 


#ifndef TERMINAL_H_
#define TERMINAL_H_

#include "uart.h"
#define SIZE_BUFFERTX_MAX 67
#define SIZE_BUFFERRX_MAX 19
#define SIZE_COMANDO_MAX 19 

void terminal_enviarMensaje(const char * mensaje);
void terminal_limpiar();
void terminal_getComando(char * com);
bool terminal_hayComando();
void terminal_poll();


#endif /* TERMINAL_H_ */