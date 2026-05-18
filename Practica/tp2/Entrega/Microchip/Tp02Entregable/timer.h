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

extern volatile uint8_t flag_tick_10ms;
extern volatile uint16_t flag_tick_1s ;


void TIMER_Init(void);

void TIMER_ResetTimerSeg();

#endif /* TIMER_H_ */
