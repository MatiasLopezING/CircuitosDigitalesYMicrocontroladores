#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include <stdio.h>
#include <avr/io.h>

/* Inicializa el ADC */
void adc_Init(void);

/* Lee el valor actual del ADC (Bloqueante) */
uint16_t adc_Leer(void);


#endif