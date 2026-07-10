/*
 * uart.h
 * Modulo UART0 - comunicacion con la terminal serie (9600 8N1).
 *
 * Responsabilidades:
 *   - Inicializar UART0.
 *   - Recibir por interrupcion los caracteres del comando.
 *   - Parsear el comando "SET_COLOR = R,G,B\r\n" y entregar los
 *     valores R,G,B (0..255) al modulo pwm_rgb.
 *
 * Sugerencia: acumular los caracteres en un buffer en la ISR RX
 * hasta recibir '\n', luego marcar una bandera para que el lazo
 * principal (UART_ProcessRX) haga el parseo.
 */
#ifndef UART_H
#define UART_H

#include <stdint.h>

/* Inicializa UART0 con el valor de UBRR ya calculado (F_CPU/16/BAUD - 1). */
void UART_Inicializar(uint16_t ubrr);

/* Envia un byte por UART (bloqueante). Util para debug/eco. */
void UART_EnviarByte(uint8_t dato);

/* Envia una cadena terminada en '\0'. */
void UART_EnviarCadena(const char *cadena);

/* Se llama desde el lazo principal: si hay una linea completa
 * recibida, la parsea. Si es un "SET_COLOR" valido, actualiza el color. */
void UART_ProcesarRX(void);

#endif /* UART_H */
