/*
 * pwm_rgb.h
 * Modulo de generacion de las tres senales PWM del LED RGB (8 bits, >30 Hz).
 *
 * Canales:
 *   G (PB2) y B (PB1) -> hardware, Timer1 en Fast PWM 8 bits (TOP=0xFF).
 *                        Salidas OC1B (PB2) y OC1A (PB1).
 *   R (PB5)           -> PWM por software, generado en la ISR de un timer
 *                        base (Timer0/Timer2) comparando un contador 0..255.
 *
 * Recordar: LED anodo comun -> el brillo se controla con el valor
 * INVERTIDO del duty (0 = apagado, 255 = maximo brillo hacia el usuario,
 * pero el pin conduce al reves).
 *
 * El "color base" (r,g,b) fijado por UART se combina con la "intensidad"
 * del efecto de fade (ver fade.h) para obtener el duty final de cada canal.
 */
#ifndef PWM_RGB_H
#define PWM_RGB_H

#include <stdint.h>

/* Configura pines, Timer1 (HW) y el timer base para el PWM por software. */
void PWM_RGB_Inicializar(void);

/* Fija el color base seleccionado por el usuario (0..255 cada componente). */
void PWM_RGB_FijarColor(uint8_t r, uint8_t g, uint8_t b);

/* Aplica un factor de intensidad global 0..255 (efecto de desvanecimiento)
 * sobre el color base y actualiza los tres duty (HW y variable del PWM sw). */
void PWM_RGB_FijarIntensidad(uint8_t nivel);

/* Rutina del PWM por software para el canal R (PB5).
 * Llamar desde la ISR del timer base (frecuencia ~ 30*256 Hz o mayor). */
void PWM_RGB_PasoSoftware(void);

/* Configura la base de tiempo (Timer0 CTC a 16 kHz) que genera el PWM
 * por software de PB5 y, dividida por 16, el tick de 1 ms (bandera_ms).
 * Llamar desde PWM_RGB_Inicializar(). */
void PWM_RGB_BaseTiempoInit(void);

#endif /* PWM_RGB_H */
