#ifndef TIMER_TICK_H
#define TIMER_TICK_H

#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/* Inicializa el Timer2 para generar un tick de 1 ms. */
void timer_tick_Init(void);

/* Devuelve true si ocurrio un tick de 1 ms desde la ultima consulta. */
bool timer_tick_HayTickMs(void);

#endif