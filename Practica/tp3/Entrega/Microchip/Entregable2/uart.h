/**
 * @file    uart.h
 * @brief   Interfaz del driver UART para ATmega328P a 16 MHz, 9600 bps 8N1.
 *
 * Expone buffers de TX (lineal) y RX (circular) gestionados por ISR.
 * El modulo superior (terminal) usa estas primitivas para armar y enviar mensajes.
 */

#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <stdbool.h>
#include <string.h>
#include <avr/interrupt.h>

/* Tamano del buffer de transmision (bytes).
 * Dimensionado para la cadena mas larga posible:
 * "[ALERTA] [HH:MM:SS] Temperatura fuera de rango nocturno! Valor: XX C\r\n\0" */
#define SIZE_BUFFERTX_MAX 72

/* Tamano del buffer circular de recepcion (bytes).
 * Basado en el comando mas largo: "SET_TIME=HH:MM:SS\n" + 1 posicion libre para deteccion de OF. */
#define SIZE_BUFFERRX_MAX 19

void    uart_init(void);
void    uart_setUDRIE0(void);
void    uart_limpiarBuffer(void);
void    uart_cargarByteBuffer(const char c);
bool    uart_leerByteBuffer(char *c);
void    uart_resetearRx(void);
bool    uart_huboOV(void);

#endif /* UART_H_ */