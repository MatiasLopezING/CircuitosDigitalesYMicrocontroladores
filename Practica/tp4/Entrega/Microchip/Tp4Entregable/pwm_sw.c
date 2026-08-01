#include "pwm_sw.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint8_t duty_R = 0;
static volatile uint8_t phase_R = 0;

void pwm_sw_Init(void) {
	DDRB |= (1 << PB5);
	
	// Modo CTC (WGM01 = 1)
	TCCR0A = (1 << WGM01);
	
	// Prescaler = 8 (CS01 = 1, CS00 = 0)
	TCCR0B = (1 << CS01);
	
	// TOP = 62 (la ISR salta cada 63 cuentas)
	OCR0A = 62;
	
	// Habilitar interrupción por comparación
	TIMSK0 |= (1 << OCIE0A);
}

void pwm_sw_SetDuty(uint8_t rojo) {
	duty_R = rojo;
}

ISR(TIMER0_COMPA_vect) {
	phase_R++; // Se desborda de 0 a 255 automáticamente
	
	// Ánodo Común: LOW enciende, HIGH apaga
	if (phase_R < duty_R) {
		PORTB &= ~(1 << PB5); // Enciende Rojo (LOW)
		} else {
		PORTB |= (1 << PB5);  // Apaga Rojo (HIGH)
	}
}