#include "timer.h"
#include <avr/interrupt.h>

// ==============================================================================
// Variables Globales Volátiles
// ==============================================================================
volatile uint8_t flag_tick_10ms = 0;

volatile uint16_t segundos_transcurridos = 0;

static uint16_t contador_1ms = 0;

// ==============================================================================
// Inicialización del Timer
// ==============================================================================
void TIMER_Init(void) {
	// ---------------------------------------------------------
	// Configuración del Timer0 (8 bits) en Modo CTC
	// Frecuencia del reloj (F_CPU): 16 MHz
	// ---------------------------------------------------------
	// 1. Configuración de Modo CTC (Clear Timer on Compare match).
	// Reinicio automático de contador al alcanzar OCR0A.
	TCCR0A = (1 << WGM01);
	// 2. Configuración de Prescaler (64) e inicialización del timer.
	// Frecuencia de timer: 250,000 Hz (Resolución: 4 us).
	TCCR0B = (1 << CS01) | (1 << CS00);
	// 3. Configuración del valor tope (OCR0A) para 1 ms exacto.
	// Tope = (250,000 Hz / 1000 Hz) - 1 = 249.
	OCR0A = 249;
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
    
	static uint8_t ticks_10ms=0;
	
	ticks_10ms++;
	
	contador_1ms++;
	
	 // Tick principal de scheduler cada 10 ms
	 if(ticks_10ms >= 10)
	 {
		 ticks_10ms = 0;

		 flag_tick_10ms = 1;
	 }
    
	 // 1 segundo para reloj del microondas
	 if(contador_1ms >= 1000)
	 {
		 contador_1ms = 0;

		 segundos_transcurridos++;
	 }
	 
  }

