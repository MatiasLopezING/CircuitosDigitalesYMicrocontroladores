#ifndef TIMER_H_
#define TIMER_H_

#include <avr/io.h>

// Inicializa el Timer0 para generar una interrupción exacta cada 1 milisegundo
void TIMER_Init(void);

#endif /* TIMER_H_ */
