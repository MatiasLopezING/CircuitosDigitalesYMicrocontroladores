#ifndef PWM_SW_H
#define PWM_SW_H

#include <stdint.h>

/**
 * @brief Inicializa el TIMER0 en modo CTC para generar el PWM por software.
 * 
 * Configura el pin PB5 como salida digital.
 * El TIMER0 se configura para generar una interrupción cada ~1.024 ms
 * (prescaler 64, OCR0A = 255), donde se actualizará el estado del pin PB5
 * según el duty cycle configurado. La interrupción debe estar habilitada 
 * globalmente (sei()) desde el main.
 */
void pwm_sw_Init(void);

/**
 * @brief Establece el valor de brillo para el Rojo (PB5).
 * 
 * @param rojo  Valor de brillo deseado. Rango válido: 0 a 255.
 */
void pwm_sw_SetDuty(uint8_t rojo);

#endif