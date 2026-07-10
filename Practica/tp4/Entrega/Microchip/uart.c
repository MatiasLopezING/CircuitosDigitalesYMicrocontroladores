/*
 * uart.c  -  Implementacion del modulo UART0 (ver uart.h).
 *
 * A implementar:
 *   UART_Inicializar(ubrr) -> UBRR0, habilitar RX/TX, RXCIE0, formato 8N1.
 *   ISR(USART_RX_vect)     -> guardar caracter en buffer; al llegar '\n'
 *                             marcar bandera de "linea lista".
 *   UART_ProcesarRX()      -> si hay linea lista: parsear "SET_COLOR = R,G,B"
 *                             (p.ej. con sscanf o parseo manual) y llamar a
 *                             PWM_RGB_FijarColor(r,g,b).
 *   UART_EnviarByte/Cadena -> polling sobre UDRE0.
 *
 * Referencia teorica: clase13_UART.pdf
 */
#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
