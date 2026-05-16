#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include "keypad.h"
#include <util/delay.h>

// ==============================================================================
// Mapa de caracteres de teclado matricial
// ==============================================================================
static const uint8_t keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

void KEYPAD_Init(void) {
    // Configuración de filas como salidas.
    DDRB |= (1 << PB4) | (1 << PB3) | (1 << PB0);
    DDRD |= (1 << PD7);
    
    // Inicialización de filas en estado lógico alto (inactivo).
    PORTB |= (1 << PB4) | (1 << PB3) | (1 << PB0);
    PORTD |= (1 << PD7);
    
    // Configuración de columnas como entradas.
    DDRD &= ~((1 << PD3) | (1 << PD5) | (1 << PD4) | (1 << PD2));
    
    // Habilitación de resistencias pull-up en columnas.
    PORTD |= (1 << PD3) | (1 << PD5) | (1 << PD4) | (1 << PD2);
}

// ==============================================================================
// Funciones Auxiliares Privadas
// ==============================================================================

/**
 * @brief Fuerza filas a estado lógico alto (inactivo).
 */
static void setAllRowsHigh(void) {
    PORTB |= (1 << PB4) | (1 << PB3) | (1 << PB0);
    PORTD |= (1 << PD7);
}

/**
 * @brief Establece una fila en estado bajo para escaneo.
 * @param row Índice de la fila (0 a 3).
 */
static void setRowLow(uint8_t row) {
    setAllRowsHigh(); // Establece estado inactivo en todas las filas.
    
    switch(row) {
        case 0: PORTB &= ~(1 << PB4); break;
        case 1: PORTB &= ~(1 << PB3); break;
        case 2: PORTB &= ~(1 << PB0); break;
        case 3: PORTD &= ~(1 << PD7); break;
    }
}

/**
 * @brief Lee estado lógico de columna.
 * @param col Índice de la columna (0 a 3).
 * @return 1 si la columna está en estado bajo, 0 en caso contrario.
 */
static uint8_t readCol(uint8_t col) {
    switch(col) {
        case 0: return (PIND & (1 << PD3)) == 0;
        case 1: return (PIND & (1 << PD5)) == 0;
        case 2: return (PIND & (1 << PD4)) == 0;
        case 3: return (PIND & (1 << PD2)) == 0;
    }
    return 0;
}

/**
 * @brief Escaneo matricial completo.
 * @return Caracter de tecla detectada o 0xFF si no hay detección.
 */
static uint8_t KeypadUpdate(void) {
    for (uint8_t row = 0; row < 4; row++) {
        setRowLow(row); // Activa fila.
        _delay_us(10);  // Retardo de estabilización eléctrica.
        
        for (uint8_t col = 0; col < 4; col++) {
            if (readCol(col)) { // Detección de contacto.
                setAllRowsHigh(); // Restaura estado de filas.
                return keymap[row][col];
            }
        }
    }
    
    setAllRowsHigh();
    return 0xFF; // Sin detección de tecla.
}

// ==============================================================================
// Funciones Públicas
// ==============================================================================

uint8_t KEYPAD_Scan(uint8_t *pkey) {
    static uint8_t Old_key = 0xFF;
    static uint8_t Last_valid_key = 0xFF; 
    
    uint8_t Key = KeypadUpdate(); 
    
    if (Key == 0xFF) {
        Old_key = 0xFF;
        Last_valid_key = 0xFF;
        return 0;
    }
    
    // Lógica anti-rebote por muestreo consecutivo.
    if (Key == Old_key) { 
        if (Key != Last_valid_key) {
            *pkey = Key;
            Last_valid_key = Key;
            return 1; // Validación de tecla.
        }
    }
    
    Old_key = Key;
    return 0;
}

uint8_t KEYPAD_GetKey(void) {
    uint8_t key;
    if (KEYPAD_Scan(&key)) {
        return key; 
    }
    return 0; 
}
