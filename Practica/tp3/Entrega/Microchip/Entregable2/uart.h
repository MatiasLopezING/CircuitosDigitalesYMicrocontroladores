

#ifndef UART_H_
#define UART_H_

#include <avr/io.h>
#include <stdbool.h>
#include <string.h>
#include <avr/interrupt.h>

/* Tamano del buffer de transmision (bytes).
 * Dimensionado para la cadena mas larga posible:
 * [ALERTA] [00:00:00] Temp y Humedad fuera de rango nocturno! T: 35 C H: 90%\r\n\0" */
#define SIZE_BUFFERTX_MAX 78

/* Tamano del buffer circular de recepcion (bytes).
 * Basado en el comando mas largo: "SET_TIME=HH:MM:SS\n" */
#define SIZE_BUFFERRX_MAX 19

void    uart_init(void);
void    uart_setUDRIE0(void);
void    uart_limpiarBuffer(void);
void    uart_cargarByteBuffer(const char c);
bool    uart_leerByteBuffer(char *c);
void    uart_resetearRx(void);
bool    uart_huboOV(void);
bool    uart_hayDatosRx(void);

#endif /* UART_H_ */