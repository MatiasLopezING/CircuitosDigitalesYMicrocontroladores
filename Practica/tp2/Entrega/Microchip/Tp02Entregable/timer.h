/**
 * 
 * Librería para la configuración y manejo del Timer0.
 * 
 * 
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>

extern volatile uint8_t flag_tick_10ms;
extern volatile uint16_t flag_tick_1s ;

/**
 * @brief Inicializacion de Timer0 con tick cada 1ms.
 * 
 * 
 */
void TIMER_Init(void);

/**
 * @brief Reseteo del tick de timer para 1 seg.
 * 
 * 
 */
void TIMER_ResetTimerSeg();

#endif /* TIMER_H_ */
