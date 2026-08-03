/*
  led_rgb.c
  Modulo de control del LED RGB.
  Mantiene el color objetivo (R,G,B) deseado por el usuario y aplica el nivel de brillo deseado.
 */

#include "led_rgb.h"

/* Variables privadas que almacenan el color objetivo. */
static uint8_t col_R, col_G, col_B;

/*
  @brief Inicializa el modulo LED RGB.
  Pone el color objetivo en negro (0,0,0) y aplica nivel 0, dejando el LED apagado. No reconfigura los PWMs porque estos ya fueron inicializados en pwm_hw_Init() y pwm_sw_Init() desde el main.
 */
void led_rgb_Init(void) {
    col_R = 0;
    col_G = 0;
    col_B = 0;
    led_rgb_ApplyBrightness(0);
}

/*
  @brief Guarda el color objetivo enviado por el usuario.
  @param r  Valor rojo (0-255).
  @param g  Valor verde (0-255).
  @param b  Valor azul (0-255).
  No modifica el brillo actual; solo almacena los colores para ser usados
  en la proxima llamada a ApplyBrightness.
 */
void led_rgb_SetColor(uint8_t r, uint8_t g, uint8_t b) {
    col_R = r;
    col_G = g;
    col_B = b;
}

/*
  @brief Aplica un nivel de brillo al color objetivo y actualiza los PWMs.
  @param nivel  Nivel de brillo (0-255).
  La escala se realiza mediante la formula:
    valor_final = (color * nivel) / 255.
  Esto asegura que cuando nivel = 255, el LED brille exactamente con el
  color que el usuario pidio; cuando nivel = 0, el LED se apaga.
  La multiplicacion se hace en 16 bits para evitar overflow, y el resultado se trunca a 8 bits porque siempre esta en el rango 0-255.
  Luego se llaman a los drivers de PWM:
    - pwm_hw_SetColor(verde, azul) para el hardware (Timer1).
    - pwm_sw_SetDuty(rojo) para el software (Timer0).
 */
void led_rgb_ApplyBrightness(uint8_t nivel) {
    uint8_t r = (uint8_t)(((uint16_t)col_R * nivel) / 255);
    uint8_t g = (uint8_t)(((uint16_t)col_G * nivel) / 255);
    uint8_t b = (uint8_t)(((uint16_t)col_B * nivel) / 255);

    pwm_hw_SetColor(g, b);
    pwm_sw_SetDuty(r);
}