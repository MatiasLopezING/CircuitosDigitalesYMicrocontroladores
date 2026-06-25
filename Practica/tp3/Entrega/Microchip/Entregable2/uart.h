/*
 * uart.h
 *
 * Created: 6/24/2026 1:21:01 AM
 *  Author: tomas
 */ 


#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <stdbool.h>
#include <string.h>
#include <avr/interrupt.h>
#define SIZE_BUFFERTX_MAX 64
#define SIZE_BUFFERRX_MAX 19

void uart_init();
void uart_enviarTelemetria(const char * telemetria);

#endif /* UART_H_ */