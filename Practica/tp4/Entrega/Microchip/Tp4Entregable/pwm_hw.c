/*
  pwm_hw.c
  Driver del PWM por hardware (Timer1) para los canales Verde (PB2) y Azul (PB1).
  Configura el Timer1 en modo Fast PWM de 8 bits, salida invertida, con prescaler 256. La frecuencia resultante es ~244 Hz, muy por encima de los 30 Hz exigido evitando parpadeo.
  El modo invertido se adapta al LED de Anodo Comun: un valor OCR = 255 mantiene el pin en LOW (encendido maximo), y OCR = 0 lo mantiene en HIGH (apagado). Para el caso OCR = 0 se desconecta el PWM y se fuerza el pin a HIGH, eliminando el pulso residual que produce el hardware.
 */

#include "pwm_hw.h"

/*
  @brief Inicializa el Timer1 para PWM en PB1 (Azul) y PB2 (Verde).
  Configura:
  - Modo Fast PWM de 8 bits (WGM13:0 = 0101).
  - Salida invertida (COM1A1:0 = 11, COM1B1:0 = 11) para Anodo Comun.
  - Prescaler 256 (CS12 = 1), frecuencia de reloj del timer = 62.5 kHz.
  - Frecuencia PWM resultante = 16 MHz / (256 * 256) = 244 Hz (> 30 Hz).
  - Inicializa OCR1A y OCR1B en 0 (apagado).
 */
void pwm_hw_Init(void) {
    /* Configurar pines PB1 y PB2 como salidas. */
    DDRB |= (1 << PORTB1) | (1 << PORTB2);

    /* TCCR1A: Modo Fast PWM 8 bits (WGM10 = 1), salidas invertidas. */
    TCCR1A = (1 << WGM10) | (1 << COM1A1) | (1 << COM1A0) | (1 << COM1B1) | (1 << COM1B0);

    /* TCCR1B: Prescaler 256 (CS12 = 1) y WGM12 = 1 (completa modo 5). */
    TCCR1B = (1 << WGM12) | (1 << CS12);

    /* Inicializar en 0 (apagado). */
    OCR1A = 0;  
    OCR1B = 0;  
}

/*
  @brief Actualiza el brillo de los canales Verde y Azul.
  @param verde  Valor de brillo para el Verde (0-255).
  @param azul   Valor de brillo para el Azul (0-255).
  Si el valor es 0, se desconecta el PWM y se fuerza el pin a HIGH (apagado total). Si el valor es > 0, se reactiva el PWM en modo invertido y se actualiza OCR. Esto evita el pulso residual que el hardware genera cuando OCR = 0 en modo invertido.
 */
void pwm_hw_SetColor(uint8_t verde, uint8_t azul) {
    /* Canal B (Verde) - PB2 */
    if (verde == 0) {
        /* Desconectar PWM y forzar HIGH (apagado). */
        TCCR1A &= ~(1 << COM1B1) & ~(1 << COM1B0);
        PORTB |= (1 << PORTB2);
    } else {
        /* Reactivar PWM en modo invertido y actualizar OCR. */
        TCCR1A |= (1 << COM1B1) | (1 << COM1B0);
        OCR1B = verde;
    }

    /* Canal A (Azul) - PB1 */
    if (azul == 0) {
        /* Desconectar PWM y forzar HIGH (apagado). */
        TCCR1A &= ~(1 << COM1A1) & ~(1 << COM1A0);
        PORTB |= (1 << PORTB1);
    } else {
        /* Reactivar PWM en modo invertido y actualizar OCR. */
        TCCR1A |= (1 << COM1A1) | (1 << COM1A0);
        OCR1A = azul;
    }
}