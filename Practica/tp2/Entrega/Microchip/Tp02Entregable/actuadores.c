#include "actuadores.h"

void ACTUADORES_Init(void) {
    // Configurar pines como salidas
    DDRB |= (1 << PB5);              // Magnetrón
    DDRC |= (1 << PC4) | (1 << PC5); // Luz y Alarma

    // Apagar todos por defecto (estado lógico bajo)
    PORTB &= ~(1 << PB5);
    PORTC &= ~((1 << PC4) | (1 << PC5));
}

// ==============================================================================
// --- MAGNETRÓN (PB5) ---
// ==============================================================================

void MAGNETRON_On(void) {
    PORTB |= (1 << PB5);
}

void MAGNETRON_Off(void) {
    PORTB &= ~(1 << PB5);
}

// ==============================================================================
// --- LUZ INTERIOR (PC4) ---
// ==============================================================================

void LUZ_On(void) {
    PORTC |= (1 << PC4);
}

void LUZ_Off(void) {
    PORTC &= ~(1 << PC4);
}

// ==============================================================================
// --- ALARMA SONORA (PC5) ---
// ==============================================================================

void ALARMA_On(void) {
    PORTC |= (1 << PC5);
}

void ALARMA_Off(void) {
    PORTC &= ~(1 << PC5);
}

void ALARMA_Toggle(void) {
    PORTC ^= (1 << PC5);
}
