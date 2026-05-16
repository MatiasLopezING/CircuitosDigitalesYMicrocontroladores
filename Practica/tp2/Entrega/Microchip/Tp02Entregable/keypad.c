/*
 * keypad.c
 *
 * Created: 5/16/2026 1:44:37 AM
 *  Author: tomas
 */ 
#include "keypad.h"
#include <util/delay.h>

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
    
    // Habilitar las resistencias de pull-up en las columnas
    PORTD |= (1 << PD3) | (1 << PD5) | (1 << PD4) | (1 << PD2);
}

// Función auxiliar para poner todas las filas en HIGH
static void setAllRowsHigh(void) {
    PORTB |= (1 << PB4) | (1 << PB3) | (1 << PB0);
    PORTD |= (1 << PD7);
}

// Función auxiliar para poner una sola fila en LOW
static void setRowLow(uint8_t row) {
    setAllRowsHigh(); // Primero todas en HIGH
    if(row == 0) PORTB &= ~(1 << PB4);
    else if(row == 1) PORTB &= ~(1 << PB3);
    else if(row == 2) PORTB &= ~(1 << PB0);
    else if(row == 3) PORTD &= ~(1 << PD7);
}

// Función auxiliar para leer una columna
static uint8_t readCol(uint8_t col) {
    if(col == 0) return (PIND & (1 << PD3)) == 0;
    if(col == 1) return (PIND & (1 << PD5)) == 0;
    if(col == 2) return (PIND & (1 << PD4)) == 0;
    if(col == 3) return (PIND & (1 << PD2)) == 0;
    return 0;
}

static uint8_t KeypadUpdate(void) {
    for (uint8_t row = 0; row < 4; row++) {
        setRowLow(row); // Poner en bajo (0) la fila a escanear
        _delay_us(10);  // Delay para estabilización eléctrica
        
        for (uint8_t col = 0; col < 4; col++) {
            if (readCol(col)) { // Si la columna está en 0 (tecla presionada)
                setAllRowsHigh();
                return keymap[row][col];
            }
        }
    }
    setAllRowsHigh();
    return 0xFF; // Ninguna tecla presionada
}

uint8_t KEYPAD_Scan(uint8_t *pkey) {
static uint8_t Old_key = 0xFF, Last_valid_key = 0xFF; 
uint8_t Key = KeypadUpdate(); 

if(Key == 0xFF){
Old_key = 0xFF;
Last_valid_key = 0xFF;
return 0;
}
if(Key == Old_key) { 
if(Key != Last_valid_key){
*pkey = Key;
Last_valid_key = Key;
return 1;
}
}
Old_key = Key;
return 0;
}
