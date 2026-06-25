/*
 * timer.h
 *
 * Created: 6/24/2026 3:09:36 AM
 *  Author: tomas
 */ 


#ifndef TIMER_H_
#define TIMER_H_
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

void timer_init(void);
void timer_setT(uint8_t);
bool timer_pasoT();



#endif /* TIMER_H_ */