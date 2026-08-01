#ifndef PWM_SW_H
#define PWM_SW_H

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Inicializa el TIMER0 (CTC) para el PWM por software del Rojo (PB5).
 *
 * Prescaler 8 y OCR0A = 62: la ISR corre a ~31.7 kHz y genera un PWM de
 * ~124 Hz. La misma ISR deriva un tick de 1 ms (ver pwm_sw_hayTickMs).
 */
void pwm_sw_Init(void);

/**
 * @brief Establece el brillo del Rojo (PB5).
 * @param rojo  Valor 0 a 255.
 */
void pwm_sw_SetDuty(uint8_t rojo);

/**
 * @brief Devuelve true una vez por cada tick de 1 ms (lo consume al leerlo).
 */
bool pwm_sw_hayTickMs(void);

#endif
