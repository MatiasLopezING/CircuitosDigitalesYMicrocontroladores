#ifndef PWM_HW_H
#define PWM_HW_H

#include <stdint.h>

/**
 * @brief Inicializa el TIMER1 en modo Fast PWM de 8 bits (Modo 5).
 * 
 * Configura los pines PB1 (OC1A) y PB2 (OC1B) como salidas PWM.
 * Utiliza el modo INVERTIDO para adaptarse al LED RGB de Ánodo Común
 * (un valor de 255 en OCR genera un 100% de tiempo en LOW, encendiendo el LED).
 * Frecuencia configurada: ~244 Hz (prescaler 256), cumpliendo > 30 Hz.
 */
void pwm_hw_Init(void);

/**
 * @brief Actualiza los valores de brillo para el Verde y el Azul.
 * 
 * @param verde  Valor de brillo para el Verde (PB2). Rango válido: 0 a 255.
 * @param azul   Valor de brillo para el Azul (PB1). Rango válido: 0 a 255.
 */
void pwm_hw_SetColor(uint8_t verde, uint8_t azul);

#endif