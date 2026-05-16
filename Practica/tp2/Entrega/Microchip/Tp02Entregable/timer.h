/**
 * @file timer.h
 * @brief Librería para la configuración y manejo del Timer0.
 * 
 * Configura el Timer0 en modo CTC para generar una base de tiempos exacta,
 * utilizada para actualizar la máquina de estados y llevar la cuenta de tiempo.
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>

/**
 * @brief Inicializa el Timer0 en Modo CTC.
 * Genera una interrupción exacta cada 1 milisegundo (asumiendo F_CPU = 8 MHz).
 * Requiere habilitar las interrupciones globales (`sei()`).
 */
void TIMER_Init(void);

#endif /* TIMER_H_ */
