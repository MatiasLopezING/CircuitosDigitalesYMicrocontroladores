/*
 * pwm_rgb.c  -  Generacion de las 3 senales PWM del LED RGB (ver pwm_rgb.h).
 *
 * A implementar:
 *   PWM_RGB_Inicializar()  -> DDRB salidas PB1/PB2/PB5;
 *                            Timer1 Fast PWM 8 bits (WGM=0101, TOP=0xFF),
 *                            COM1A/COM1B, prescaler 1 (f=62.5kHz>30Hz);
 *                            Timer0/2 como base del PWM sw de PB5.
 *   PWM_RGB_FijarColor()   -> guarda color base r,g,b.
 *   PWM_RGB_FijarIntensidad(nivel) -> duty = color*nivel/255; recordar que el
 *                            LED es ANODO COMUN (invertir: OCR = 255 - duty).
 *                            Cargar OCR1A (B), OCR1B (G) y variable del PWM sw (R).
 *   PWM_RGB_PasoSoftware() -> contador 0..255; PB5 en 1 mientras cont < duty_R
 *                            (invertido por anodo comun). Llamar desde ISR.
 *
 * Referencia teorica: clase16_PWM.pdf, clase12_Timer1a/b.pdf
 */
#include "pwm_rgb.h"
#include <avr/io.h>
#include <avr/interrupt.h>
