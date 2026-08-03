

#ifndef LED_RGB_H
#define LED_RGB_H

#include <stdint.h>
#include "pwm_hw.h"
#include "pwm_sw.h"

/* Pone el color objetivo en negro (0,0,0) y el nivel de brillo en 0, dejando el LED apagado al inicio. */
void led_rgb_Init(void);

/* Guarda el color objetivo enviado por el usuario. */
void led_rgb_SetColor(uint8_t r, uint8_t g, uint8_t b);

/* Escala el color objetivo (guardado previamente) segun el nivel de brillo */
void led_rgb_ApplyBrightness(uint8_t nivel);

#endif