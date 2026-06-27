/**
 * @file    timer.h
 * @brief   Interfaz del driver de temporizacion basado en Timer1 del ATmega328P.
 *
 * Genera ticks de 10 ms (CTC, prescaler 64, OCR1A = 2499) y cuenta
 * el periodo T configurable entre 2 y 60 segundos.
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

void    timer_init(void);
void    timer_setT(uint8_t Tnuevo);
bool    timer_pasoT(void);

#endif /* TIMER_H_ */