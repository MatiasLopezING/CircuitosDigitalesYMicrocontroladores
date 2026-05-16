#include "timer.h"
#include <avr/interrupt.h>

void TIMER_Init(void) {
    // ---------------------------------------------------------
    // Configuración del Timer0 (8 bits) en Modo CTC
    // Frecuencia del reloj (F_CPU): 8 MHz
    // ---------------------------------------------------------

    // 1. Configurar Modo CTC (Clear Timer on Compare match)
    // El timer cuenta de 0 hasta el valor de OCR0A y luego vuelve a 0
    TCCR0A = (1 << WGM01);

    // 2. Configurar Prescaler en 64 y encender el timer
    // F_timer = 8,000,000 / 64 = 125,000 Hz (1 tick cada 8 us)
    TCCR0B = (1 << CS01) | (1 << CS00);

    // 3. Configurar el tope (OCR0A) para obtener 1 ms exacto
    // 1 ms = 1000 Hz. 125,000 / 1000 = 125 ticks.
    // Como el contador empieza en 0, el tope es 125 - 1 = 124.
    OCR0A = 124;

    // 4. Habilitar la interrupción por comparación con OCR0A
    TIMSK0 = (1 << OCIE0A);

    // 5. Habilitar interrupciones globales
    sei();
}

// Variables compartidas para la prueba desde el main
volatile uint16_t timer_milisegundos = 0;
volatile uint16_t segundos_transcurridos = 0;
volatile uint8_t flag_actualizar_lcd = 0;

// ------------------------------------------------------------------------- //
// Rutina de Servicio de Interrupción (ISR) - Timer0 Compare Match A         //
// ------------------------------------------------------------------------- //
// Se ejecuta de manera automática y periódica cada 1 ms.
// Base de tiempos sugerida para la Máquina de Estados Finitos (MEF)
// y para la actualización del tiempo de cocción en el LCD.
// ------------------------------------------------------------------------- //
ISR(TIMER0_COMPA_vect) {
    
    // Llamada a la actualización de la FSM
    // Descomentar para ejecutar periódicamente:
    // FSM_Update();
    
    // ------------------------------------------
    // Lógica temporal para prueba del hardware:
    // ------------------------------------------
    timer_milisegundos++;
    
    if (timer_milisegundos >= 1000) { 
        timer_milisegundos = 0;
        segundos_transcurridos++;
        
        if (segundos_transcurridos > 99) {
            segundos_transcurridos = 0;
        }
        
        flag_actualizar_lcd = 1; // Avisa al main que pasó 1 segundo
    }
}
