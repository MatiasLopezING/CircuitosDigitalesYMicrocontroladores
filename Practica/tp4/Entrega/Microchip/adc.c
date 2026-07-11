/*
 * adc.c  -  Lectura del LDR por ADC3 (ver adc.h).
 *
 *
 *   - Vref = AVCC (REFS0=1). Requiere AVCC conectado a VCC en el circuito.
 *   - Prescaler /128 -> ADCLK = 16MHz/128 = 125 kHz (<200 kHz).
 *   - Conversion de 10 bits (ADLAR=0), se lee el registro ADC (0..1023).
 *   - En polling hay que limpiar el flag ADIF escribiendole un 1.
 *
 * Divisor del LDR: VCC - LDR - (PC3) - R1(100k) - GND.
 *   Mas luz  -> baja R_LDR -> sube V(PC3) -> sube ADC -> periodo 3 s.
 *   Menos luz-> sube R_LDR -> baja V(PC3) -> baja ADC -> periodo 6 s.
 */
#include "adc.h"
#include <avr/io.h>

#define ADC_CANAL_LDR   3      /* LDR en ADC3 / PC3 */

#define PERIODO_MIN_MS  3000   /* maxima iluminacion */
#define PERIODO_MAX_MS  6000   /* minima iluminacion (oscuro) */

void ADC_Inicializar(void) {
    /* PC3 como entrada (estado por reset, se hace explicito). */
    DDRC &= ~(1 << PC3);

    /* Opcional: deshabilita el buffer de entrada digital de ADC3. */
    DIDR0 |= (1 << ADC3D);

    /* Vref = AVCC (REFS0=1), ADLAR=0, canal por defecto = LDR. */
    ADMUX = (1 << REFS0) | (ADC_CANAL_LDR & 0x0F);

    /* Habilita el ADC con prescaler /128 (ADPS2:0 = 111). */
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t ADC_Leer(uint8_t canal) {
    /* Selecciona el canal conservando REFS/ADLAR. */
    ADMUX = (ADMUX & 0xF0) | (canal & 0x0F);

    ADCSRA |= (1 << ADSC);                 /* inicia la conversion */
    while ((ADCSRA & (1 << ADIF)) == 0)    /* espera fin (encuesta ADIF) */
        ;
    ADCSRA |= (1 << ADIF);                 /* borra el flag (polling) */

    return ADC;                            /* resultado 10 bits (0..1023) */
}

uint16_t ADC_ObtenerPeriodoParpadeoMs(void) {
    uint16_t muestra = ADC_Leer(ADC_CANAL_LDR);

    /* Mapea 0..1023 -> 6000..3000 ms (mas luz => periodo mas corto). */
    return (uint16_t)(PERIODO_MAX_MS
                      - ((uint32_t)muestra * (PERIODO_MAX_MS - PERIODO_MIN_MS)) / 1023);
}
