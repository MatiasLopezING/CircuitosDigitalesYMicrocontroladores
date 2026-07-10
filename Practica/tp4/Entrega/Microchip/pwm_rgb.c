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

/* Bandera de tick de 1 ms. Se DEFINE en main.c; aca solo la usamos. */
extern volatile uint8_t bandera_ms;

/* Brillo del canal R (0..255). Lo cargara PWM_RGB_FijarIntensidad(). */
static volatile uint8_t duty_r = 0;

/* ------------------------------------------------------------------
 * Base de tiempo: Timer0 en CTC, prescaler 8, OCR0A=124
 *   f_ISR = 16MHz / (8 * (124+1)) = 16 kHz  (cada 62.5 us)
 *   -> PWM por software de PB5 = 16000/256 = 62.5 Hz (>30 Hz, 8 bits)
 *   -> dividida por 16 (16 * 62.5us = 1 ms) genera bandera_ms
 * ------------------------------------------------------------------ */
void PWM_RGB_BaseTiempoInit(void) {
    DDRB  |= (1 << PB5);         // PB5 (R) como salida (PWM por software)
    TCCR0A = (1 << WGM01);       // modo CTC
    TCCR0B = (1 << CS01);        // prescaler 8
    OCR0A  = 124;                // 16MHz/(8*125) = 16 kHz
    TIMSK0 = (1 << OCIE0A);      // habilita interrupcion por comparacion A
}

/* Paso del PWM por software del canal R (PB5).
 * LED anodo comun -> enciende con nivel BAJO. */
void PWM_RGB_PasoSoftware(void) {
    static uint8_t cont = 0;
    if (cont < duty_r)  PORTB &= ~(1 << PB5);   // enciende (nivel bajo)
    else                PORTB |=  (1 << PB5);   // apaga
    cont++;                                     // 255 -> 0 : periodo de 256 pasos
}

/* ISR de la base de tiempo (16 kHz). */
ISR(TIMER0_COMPA_vect) {
    PWM_RGB_PasoSoftware();      // PWM por software del canal R (PB5)

    static uint8_t div = 0;
    if (++div >= 16) {          // 16 * 62.5us = 1 ms
        div = 0;
        bandera_ms = 1;         // tick de 1 ms para el lazo principal (main.c)
    }
}
