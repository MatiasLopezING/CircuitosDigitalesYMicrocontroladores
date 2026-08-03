/*
  pwm_sw.c
  PWM por software para el canal Rojo (PB5) utilizando el Timer0.
  Genera una señal PWM con frecuencia ~122 Hz (Mayor a los 30Hz exigidos) a partir de una ISR que se ejecuta cada ~32 us. 
  El duty cycle se controla mediante la comparacion entre phase_R y duty_R (resolucion de 8 bits).

 */

#include "pwm_sw.h"


static volatile uint8_t duty_R = 0;   /* Duty cycle actual (0-255). */
static volatile uint8_t phase_R = 0;  /* Contador de fase (0-255). */

/*
  @brief Inicializa el Timer0 para PWM por software en PB5.
  Configura el Timer0 en modo CTC con prescaler 8 y TOP = 62.
  La ISR se ejecuta cada ~32 us, generando un ciclo de fase de 256 pasos, lo que da una frecuencia PWM de ~122 Hz (> 30 Hz).
 */
void pwm_sw_Init(void) {
    DDRB |= (1 << PORTB5);             /* PB5 como salida. */
    TCCR0A = (1 << WGM01);             /* Modo CTC. */
    TCCR0B = (1 << CS01);              /* Prescaler 8. */
    OCR0A = 62;                        /* TOP = 62 -> ISR cada ~32 us. */
    TIMSK0 |= (1 << OCIE0A);           /* Habilitar interrupción. */
}

/*
  @brief Establece el duty cycle del Rojo.
  @param rojo  Valor de brillo (0-255).
  Si el valor es 0, se fuerza el pin a HIGH (apagado total).
  Si el valor es 255, se fuerza el pin a LOW (encendido máximo).
  Para valores intermedios, se actualiza duty_R y la ISR se encarga del PWM.
 */
void pwm_sw_SetDuty(uint8_t rojo) {
    duty_R = rojo;
    if (rojo == 0) {
        PORTB |= (1 << PORTB5);   /* Forzar HIGH (apagado). */
    } else if (rojo == 255) {
        PORTB &= ~(1 << PORTB5);  /* Forzar LOW (encendido máximo). */
    }
}

/*
  @brief ISR del Timer0 para generar el PWM por software.
  Solo modifica el pin PB5 si duty_R está entre 1 y 254.
  Para valores extremos (0 o 255), el pin ya fue fijado por SetDuty.
 */
ISR(TIMER0_COMPA_vect) {
    phase_R++;  /* Avanza la fase (se desborda de 255 a 0 automáticamente). */

    /* Solo actuar si el duty no es extremo. */
    if (duty_R > 0 && duty_R < 255) {
        if (phase_R < duty_R) {
            PORTB &= ~(1 << PORTB5);  /* LOW enciende (Anodo Comun). */
        } else {
            PORTB |= (1 << PORTB5);   /* HIGH apaga. */
        }
    }
}