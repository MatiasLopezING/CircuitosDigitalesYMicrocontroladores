/*
  adc.c
  Driver del ADC para el ATmega328P.
  Configura el ADC en el canal PC3 (ADC3) con referencia AVCC (5V)
  y prescaler 128 para obtener una frecuencia de reloj de 125 kHz,
  dentro del rango optimo de precision (50-200 kHz) segun el datasheet.
 */

#include "adc.h"
#include <avr/io.h>

/*
  @brief Inicializa el periferico ADC.
  Configura:
  - Canal: ADC3 (PC3) mediante MUX3..0 = 0011.
  - Referencia: AVCC (5V) mediante REFS1=0, REFS0=1.
  - Prescaler: 128 (ADPS2=1, ADPS1=1, ADPS0=1), lo que da 125 kHz a partir de 16 MHz. Esta frecuencia esta dentro del rango recomendado para maximizar la precision de 10 bits.
  - Habilita el ADC (ADEN=1).
 */
void adc_Init(void) {
    /* Seleccion de referencia y canal */
    ADMUX = (1 << REFS0) | (1 << MUX1) | (1 << MUX0);

    /* Habilitacion del ADC y prescaler 128 */
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

/*
  @brief Realiza una conversion analogica-digital y devuelve el resultado.
  @return Valor de 10 bits (0 a 1023) correspondiente al voltaje en PC3.
 */
uint16_t adc_Leer(void) {
    ADCSRA |= (1 << ADSC);                   /* Iniciar conversion. */

    while ((ADCSRA & (1 << ADIF)) == 0);     /* Esperar flag de fin. */

    ADCSRA |= (1 << ADIF);                   /* Borrar flag (escribiendo 1). */

    return ADC;                              /* Devolver resultado de 10 bits. */
}

