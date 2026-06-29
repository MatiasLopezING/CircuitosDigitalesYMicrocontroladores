

#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdbool.h>

void    timer_init(void);
void    timer_setT(uint8_t Tnuevo);
bool    timer_pasoT(void);

#endif /* TIMER_H_ */