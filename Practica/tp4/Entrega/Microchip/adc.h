/*
 * adc.h
 * Modulo ADC - lectura del sensor de luz (LDR) en el canal ADC3 (PC3).
 *
 * Responsabilidades:
 *   - Configurar el ADC (Vref = AVCC, prescaler para fclk <= 200 kHz).
 *   - Leer el canal 3 en 10 bits.
 *   - El valor del LDR se usa para mapear el periodo de parpadeo T
 *     entre 6 s (minima luz) y 3 s (maxima luz).
 */
#ifndef ADC_H
#define ADC_H

#include <stdint.h>

/* Inicializa el ADC (referencia AVCC, canal ADC3, prescaler adecuado). */
void ADC_Inicializar(void);

/* Realiza una conversion del canal indicado y devuelve el valor 0..1023. */
uint16_t ADC_Leer(uint8_t canal);

/* Lee el LDR (ADC3) y devuelve el periodo de parpadeo T en ms,
 * mapeado linealmente entre 6000 ms (oscuro) y 3000 ms (iluminado). */
uint16_t ADC_ObtenerPeriodoParpadeoMs(void);

#endif /* ADC_H */
