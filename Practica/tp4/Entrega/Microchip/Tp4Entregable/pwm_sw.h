#ifndef PWM_SW_H
#define PWM_SW_H

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/* Inicializa el TIMER0 para el PWM por software del Rojo (PB5). */
void pwm_sw_Init(void);

/*Establece el brillo del Rojo (PB5).*/
void pwm_sw_SetDuty(uint8_t rojo);


#endif
