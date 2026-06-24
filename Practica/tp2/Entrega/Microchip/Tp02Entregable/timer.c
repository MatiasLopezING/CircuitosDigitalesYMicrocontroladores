#include "timer.h"
#include <avr/interrupt.h>


volatile uint8_t flag_tick_10ms = 0;

volatile uint16_t flag_tick_1s = 0;

static uint16_t ticks_1s=0;
static uint8_t ticks_10ms=0;



void TIMER_Init(void) {

	// Configuración del Timer0 (8 bits) en Modo CTC
	// Frecuencia del reloj (F_CPU): 16 MHz
	
	// Configuración de Modo CTC (Clear Timer on Compare match).
	// Reinicio automático de contador al alcanzar OCR0A.
	
	TCCR0A = (1 << WGM01);
	
	// Configuración de Prescaler (64) e inicialización del timer.
	// Frecuencia de timer: 250,000 Hz 
	
	TCCR0B = (1 << CS01) | (1 << CS00);
	
	// Configuración del valor tope (OCR0A) para 1 ms exacto.
	// Tope = (250,000 Hz / 1000 Hz) - 1 = 249.
	OCR0A = 249;
	
	// Habilitación de interrupción por comparación con OCR0A.
	TIMSK0 = (1 << OCIE0A);
	
	// Habilitación de interrupciones globales.
	sei();
}



void TIMER_ResetTimerSeg() {
	ticks_1s=0;
	flag_tick_1s=0;
}


/**
 * @brief ISR - Timer0 Compare Match A.
 * 
 * Ejecución periódica cada 1 ms. Base de tiempos del sistema.
 */
ISR(TIMER0_COMPA_vect) {
	
	ticks_10ms++;
	
    ticks_1s++;
	
	 // Scheduler cada 10 ms
	 if(ticks_10ms == 10)
	 {
		 ticks_10ms = 0;

		 flag_tick_10ms = 1;
	 }
    
	 // Tick de 1 segundo para reloj del microondas
	 if(ticks_1s == 1000)
	 {
		 ticks_1s = 0;

		 flag_tick_1s=1;
	 }
	 
  }

