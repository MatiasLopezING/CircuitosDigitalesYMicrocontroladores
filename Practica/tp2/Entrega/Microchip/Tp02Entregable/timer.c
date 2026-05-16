#include "timer.h"
#include <avr/interrupt.h>

// ==============================================================================
// Variables Globales Volátiles
// ==============================================================================
volatile uint16_t timer_milisegundos = 0;
volatile uint16_t segundos_transcurridos = 0;
volatile uint8_t flag_actualizar_lcd = 0;

// ==============================================================================
// Inicialización del Timer
// ==============================================================================
void TIMER_Init(void) {
    // ---------------------------------------------------------
    // Configuración del Timer0 (8 bits) en Modo CTC
    // Frecuencia del reloj (F_CPU): 8 MHz
    // ---------------------------------------------------------

    // 1. Configuración de Modo CTC (Clear Timer on Compare match).
    // Reinicio automático de contador al alcanzar OCR0A.
    TCCR0A = (1 << WGM01);

    // 2. Configuración de Prescaler (64) e inicialización del timer.
    // Frecuencia de timer: 125,000 Hz (Resolución: 8 us).
    TCCR0B = (1 << CS01) | (1 << CS00);

    // 3. Configuración del valor tope (OCR0A) para 1 ms exacto.
    // Tope = (125,000 Hz / 1000 Hz) - 1 = 124.
    OCR0A = 124;

    // 4. Habilitación de interrupción por comparación con OCR0A.
    TIMSK0 = (1 << OCIE0A);

    // 5. Habilitación de interrupciones globales.
    sei();
}

// ==============================================================================
// Rutina de Servicio de Interrupción (ISR)
// ==============================================================================

/**
 * @brief ISR - Timer0 Compare Match A.
 * 
 * Ejecución periódica cada 1 ms. Base de tiempos del sistema.
 */
ISR(TIMER0_COMPA_vect) {
    
    // Actualización de base de tiempos local.
    timer_milisegundos++;
    
    // Ejecución periódica cada 1 segundo (1000 ms).
    if (timer_milisegundos >= 1000) { 
        timer_milisegundos = 0;
        segundos_transcurridos++;
        
        // Manejo de desbordamiento (límite 99 segundos).
        if (segundos_transcurridos > 99) {
            segundos_transcurridos = 0;
        }
        
        // Actualización de bandera de estado.
        flag_actualizar_lcd = 1; 
    }
}
