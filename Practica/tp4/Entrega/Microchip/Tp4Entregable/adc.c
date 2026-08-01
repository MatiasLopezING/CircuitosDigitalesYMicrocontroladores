#include "adc.h"
#include <avr/io.h>

void adc_init(void) {
	// Configuramos el pin PC3 como entrada (ADMUX)
	// Referencia: AVCC (5V) -> REFS0 = 1, REFS1 = 0
	// Canal: ADC3 (PC3) -> MUX2..0 = 011 (MUX3=0, MUX2=0, MUX1=1, MUX0=1)
	ADMUX = (1 << REFS0) | (1 << MUX1) | (1 << MUX0);

	// Habilitar ADC (ADEN = 1)
	// Prescaler = 128 (para 16MHz -> 125kHz de frecuencia ADC)
	// ADPS2=1, ADPS1=1, ADPS0=1
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t adc_leer(void) {
	ADCSRA |= (1 << ADSC);                   // Iniciar conversión
	
	while ((ADCSRA & (1 << ADIF)) == 0);     // Esperar a que el flag ADIF se ponga en 1 (fin de conversión)
	
	ADCSRA |= (1 << ADIF);                   // Borrar el flag ADIF escribiendo un 1 
	
	return ADC;                              // Leer resultado (el compilador ya ordena ADCL y ADCH)
}

uint16_t adc_calcularPeriodoMs(void) {
	uint16_t valor = adc_leer();
	// Fórmula: T = 6000 - (ADC * 3000 / 1023)
	// Usamos 32 bits para evitar desbordamiento en la multiplicación (3000 * 1023 = ~3 millones, cabe en uint32_t)
	uint32_t tiempo = 6000 - ((uint32_t)valor * 3000 / 1023);
	return (uint16_t)tiempo;
}
