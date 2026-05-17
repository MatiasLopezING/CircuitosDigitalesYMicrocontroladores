#include "actuadores.h"

static uint8_t alarma_activa = 0;

void ACTUADORES_Init(void) {
    // Configuración de pines como salidas.
    DDRB |= (1 << PB5);              // Magnetrón
    DDRC |= (1 << PC4) | (1 << PC5); // Luz y Alarma

    // Inicialización en estado apagado.
    PORTB &= ~(1 << PB5);
    PORTC &= ~((1 << PC4) | (1 << PC5));
}

// ==============================================================================
// --- MAGNETRÓN (PB5) ---
// ==============================================================================

void ACTUADORES_MagnetronOn(void) {
    PORTB |= (1 << PB5);
}

void ACTUADORES_MagnetronOff(void) {
    PORTB &= ~(1 << PB5);
}

// ==============================================================================
// --- LUZ INTERIOR (PC4) ---
// ==============================================================================

void ACTUADORES_LuzOn(void) {
    PORTC |= (1 << PC4);
}

void ACTUADORES_LuzOff(void) {
    PORTC &= ~(1 << PC4);
}

// ==============================================================================
// --- ALARMA SONORA (PC5) ---
// ==============================================================================

void ACTUADORES_AlarmaOn(void) {
	alarma_activa=1;
    PORTC |= (1 << PC5);
}

void ACTUADORES_AlarmaOff(void) {
	alarma_activa = 0;
    PORTC &= ~(1 << PC5);
}

void ACTUADORES_AlarmaToggle(void) {
	alarma_activa ^= 1;
	if (!alarma_activa) {
		PORTC &= ~(1 << PC5);  // apaga el pin al desactivar
	}
}
void ACTUADORES_AlarmaUpdate(void) {
	static uint8_t count = 0;
	if (alarma_activa) {
		if (++count >= 50) {  // 50 * 10ms = 500ms
			count = 0;
			PORTC ^= (1 << PC5);
		}
	}
}