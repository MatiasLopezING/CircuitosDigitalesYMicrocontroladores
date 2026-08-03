#ifndef PWM_HW_H
#define PWM_HW_H

#include <stdint.h>
#include <avr/io.h>

/* Inicializa el TIMER1 en modo Fast PWM de 8 bits (Modo 5).*/
void pwm_hw_Init(void);

/* Actualiza los valores de brillo para el Verde y el Azul. */
void pwm_hw_SetColor(uint8_t verde, uint8_t azul);

#endif