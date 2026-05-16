#include "timer.h"
#include <avr/interrupt.h>

// ==============================================================================
// Variables Globales Volátiles (Compartidas con el flujo principal y otras rutinas)
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

    // 1. Configurar Modo CTC (Clear Timer on Compare match)
    // El contador vuelve a 0 automáticamente al alcanzar OCR0A.
    TCCR0A = (1 << WGM01);

    // 2. Configurar Prescaler en 64 y encender el timer
    // F_timer = 8,000,000 / 64 = 125,000 Hz (1 tick cada 8 us)
    TCCR0B = (1 << CS01) | (1 << CS00);

    // 3. Configurar el tope (OCR0A) para obtener 1 ms exacto
    // 1 ms = 1000 Hz. Por ende: 125,000 / 1000 = 125 ticks.
    // Como el contador cuenta desde 0, el tope es 125 - 1 = 124.
    OCR0A = 124;

    // 4. Habilitar la interrupción por comparación con OCR0A
    TIMSK0 = (1 << OCIE0A);

    // 5. Habilitar interrupciones globales
    sei();
}

// ==============================================================================
// Rutina de Servicio de Interrupción (ISR)
// ==============================================================================

/**
 * @brief ISR - Timer0 Compare Match A
 * 
 * Se ejecuta automáticamente mediante interrupción de hardware cada 1 ms.
 * Funciona como la base de tiempos (Tick) para el sistema.
 */
ISR(TIMER0_COMPA_vect) {
    
    // Incremento de la base de tiempos local (milisegundos)
    timer_milisegundos++;
    
    // Bloque que se ejecuta exactamente cada 1 segundo (1000 ms)
    if (timer_milisegundos >= 1000) { 
        timer_milisegundos = 0;
        segundos_transcurridos++;
        
        // Prevención de overflow si el microondas pasa los 99 segundos
        if (segundos_transcurridos > 99) {
            segundos_transcurridos = 0;
        }
        
        // Levantar bandera para notificar al flujo principal que pasó un segundo
        flag_actualizar_lcd = 1; 
    }
}
