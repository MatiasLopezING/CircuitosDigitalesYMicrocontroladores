
#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <stdbool.h>
#include <string.h>
#include <avr/interrupt.h>
#define SIZE_BUFFERTX_MAX 72 //Tamaño del buffer de transmision, en base a la cadena mas larga enviada -> [ALERTA] [HH:MM:SS] Temperatura fuera de rango nocturno! Valor: XX C\r\n\0
#define SIZE_BUFFERRX_MAX 19 //Tamaño del buffer de recepcion de comandos en base al comando mas largo posible -> SET_TIME=HH:MM:SS\n + 1 posicion libre para caso de OF

void uart_init();
void uart_setUDRIE0();
void uart_limpiarBuffer();
void uart_cargarByteBuffer(const char c);
bool uart_leerByteBuffer( char * c);
bool uart_hayComando();
void uart_resetearRx();
bool uart_huboOV();


#endif /* UART_H_ */