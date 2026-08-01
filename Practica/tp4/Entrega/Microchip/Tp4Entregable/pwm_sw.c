#include "pwm_sw.h"
#include <avr/io.h>
#include <avr/interrupt.h>

static volatile uint8_t duty_R = 0;
static volatile uint8_t phase_R = 0;

// tick de 1 ms: 32 interrupciones a ~31.7 kHz
static volatile uint8_t cuenta_isr = 0;
static volatile bool flag_tick_ms = false;

void pwm_sw_Init(void) {
	DDRB |= (1 << PB5);

	// Modo CTC (WGM01 = 1)
	TCCR0A = (1 << WGM01);

	// Prescaler = 8 (CS01 = 1, CS00 = 0)
	TCCR0B = (1 << CS01);

	// TOP = 62 (la ISR salta cada 63 cuentas)
	OCR0A = 62;

	TIMSK0 |= (1 << OCIE0A);
}

void pwm_sw_SetDuty(uint8_t rojo) {
	duty_R = rojo;
}

bool pwm_sw_hayTickMs(void) {
	if (!flag_tick_ms) return false;
	flag_tick_ms = false;
	return true;
}

ISR(TIMER0_COMPA_vect) {
	phase_R++;

	// Anodo comun: LOW enciende, HIGH apaga
	if (phase_R < duty_R) {
		PORTB &= ~(1 << PB5);
		} else {
		PORTB |= (1 << PB5);
	}

	if (++cuenta_isr >= 32) {
		cuenta_isr = 0;
		flag_tick_ms = true;
	}
}
