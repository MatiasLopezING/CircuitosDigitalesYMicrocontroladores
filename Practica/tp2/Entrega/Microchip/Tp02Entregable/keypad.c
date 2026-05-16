#include "keypad.h"
#include <util/delay.h>

// ==============================================================================
// Mapa de caracteres del teclado matricial 4x4
// ==============================================================================
static const uint8_t keymap[4][4] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}
};

void KEYPAD_Init(void) {
    // Configurar Filas como salidas: PB4, PB3, PB0 y PD7
    DDRB |= (1 << PB4) | (1 << PB3) | (1 << PB0);
    DDRD |= (1 << PD7);
    
    // Poner las filas en alto por defecto (inactivas)
    PORTB |= (1 << PB4) | (1 << PB3) | (1 << PB0);
    PORTD |= (1 << PD7);
    
    // Configurar Columnas como entradas: PD3, PD5, PD4 y PD2
    DDRD &= ~((1 << PD3) | (1 << PD5) | (1 << PD4) | (1 << PD2));
    
    // Habilitar las resistencias de pull-up internas en las columnas
    PORTD |= (1 << PD3) | (1 << PD5) | (1 << PD4) | (1 << PD2);
}

// ==============================================================================
// Funciones Auxiliares Privadas
// ==============================================================================

/**
 * @brief Fuerza todas las filas al estado alto (inactivas).
 */
static void setAllRowsHigh(void) {
    PORTB |= (1 << PB4) | (1 << PB3) | (1 << PB0);
    PORTD |= (1 << PD7);
}

/**
 * @brief Pone una única fila en estado bajo para su escaneo.
 * @param row Índice de la fila (0 a 3).
 */
static void setRowLow(uint8_t row) {
    setAllRowsHigh(); // Primero asegurar que todas estén inactivas (HIGH)
    
    switch(row) {
        case 0: PORTB &= ~(1 << PB4); break;
        case 1: PORTB &= ~(1 << PB3); break;
        case 2: PORTB &= ~(1 << PB0); break;
        case 3: PORTD &= ~(1 << PD7); break;
    }
}

/**
 * @brief Lee el estado lógico de una columna específica.
 * @param col Índice de la columna (0 a 3).
 * @return 1 si la columna está en estado bajo (tecla presionada), 0 si está en alto.
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
 * @brief Realiza un escaneo completo de la matriz 4x4.
 * @return El caracter de la tecla detectada, o 0xFF si ninguna está presionada.
 */
static uint8_t KeypadUpdate(void) {
    for (uint8_t row = 0; row < 4; row++) {
        setRowLow(row); // Activar la fila actual (LOW)
        _delay_us(10);  // Breve retardo para estabilización eléctrica de los pines
        
        for (uint8_t col = 0; col < 4; col++) {
            if (readCol(col)) { // Si la columna lee 0, hay contacto físico
                setAllRowsHigh(); // Restaurar filas antes de salir
                return keymap[row][col];
            }
        }
    }
    
    setAllRowsHigh();
    return 0xFF; // Ninguna tecla detectada en toda la matriz
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
    
    // Lógica anti-rebote básica por muestreo consecutivo
    if (Key == Old_key) { 
        if (Key != Last_valid_key) {
            *pkey = Key;
            Last_valid_key = Key;
            return 1; // Nueva tecla validada
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
