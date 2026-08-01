#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stdio.h>

// Inicializa el ADC (canal 3, referencia AVCC, prescaler 128)
void adc_init(void);

// Lee el valor actual del ADC (0 a 1023) - Bloqueante
uint16_t adc_leer(void);

// Calcula el período T en milisegundos basado en la lectura actual
uint16_t adc_calcularPeriodoMs(void);

#endif