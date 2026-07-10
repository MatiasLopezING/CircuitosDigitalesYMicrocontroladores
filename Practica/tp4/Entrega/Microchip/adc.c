/*
 * adc.c  -  Lectura del LDR por ADC3 (ver adc.h).
 *
 * A implementar:
 *   ADC_Inicializar() -> ADMUX: REFS0 (AVCC), canal 3; ADCSRA: ADEN + prescaler
 *                        (16MHz/128 = 125kHz, dentro de 50-200kHz).
 *   ADC_Leer(canal)   -> selecciona canal, inicia conversion (ADSC), espera fin,
 *                        devuelve ADC (0..1023).
 *   ADC_ObtenerPeriodoParpadeoMs() -> mapear lectura del LDR a T entre 6000 y 3000 ms.
 *
 * Referencia teorica: clase17_ADC.pdf
 */
#include "adc.h"
#include <avr/io.h>
