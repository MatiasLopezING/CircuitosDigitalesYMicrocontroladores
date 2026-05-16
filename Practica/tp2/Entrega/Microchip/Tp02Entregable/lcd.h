/**
 * @file lcd.h
 * @brief Librería para el control de un display LCD HD44780.
 * 
 * Configurado para interfaz de 4 bits. Contiene las definiciones de pines,
 * comandos básicos de bajo nivel y funciones de alto nivel para escritura.
 */

#ifndef LCD_H
#define LCD_H

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>
#include <avr/pgmspace.h>

#ifndef F_CPU
#define F_CPU 8000000UL // Frecuencia de reloj: 8 MHz
#endif

// Activar interfaz de 4 bits
#define LCD_4bit

// ==============================================================================
// Definición de Pines y Puertos
// ==============================================================================

#define LCD_RS 1    // Pin MCU conectado a LCD RS (PD1)
#define LCD_RW 2    // Reservado (En Proteus va a GND)
#define LCD_E  0    // Pin MCU conectado a LCD E (PD0)

#define LCD_D4 1    // Pin MCU conectado a LCD D4 (PC1)
#define LCD_D5 2    // Pin MCU conectado a LCD D5 (PC2)
#define LCD_D6 2    // Pin MCU conectado a LCD D6 (PB2)
#define LCD_D7 1    // Pin MCU conectado a LCD D7 (PB1)

// Definición de Puertos
#define LDP1  PORTB // Puerto MCU conectado a datos altos
#define LDP2  PORTC // Puerto MCU conectado a datos bajos
#define LCP   PORTD // Puerto MCU conectado a pines de control
#define LDDR1 DDRB  // Registro de dirección (datos altos)
#define LDDR2 DDRC  // Registro de dirección (datos bajos)
#define LCDR  DDRD  // Registro de dirección (control)

/**
 * @brief Macro para escribir en los puertos conectados al LCD.
 * Asigna los bits 4,5,6 y 7 del dato a los pines físicos configurados.
 */
#define LCD_DATAWR(Data) \
    PORTB = (PORTB & 0xF9) | (((Data) & (1<<6)) >> 4) | (((Data) & (1<<7)) >> 6); \
    PORTC = (PORTC & 0xF9) | (((Data) & (1<<4)) >> 3) | (((Data) & (1<<5)) >> 3);

// ==============================================================================
// Comandos estándar del LCD HD44780
// ==============================================================================

#define LCD_CLR             0
#define LCD_HOME            1
#define LCD_ENTRY_MODE      2
#define LCD_ENTRY_INC       1
#define LCD_ENTRY_SHIFT     0
#define LCD_ON_CTRL         3
#define LCD_ON_DISPLAY      2
#define LCD_ON_CURSOR       1
#define LCD_ON_BLINK        0
#define LCD_MOVE            4
#define LCD_MOVE_DISP       3
#define LCD_MOVE_RIGHT      2
#define LCD_FUNCTION        5
#define LCD_FUNCTION_8BIT   4
#define LCD_FUNCTION_2LINES 3
#define LCD_FUNCTION_10DOTS 2
#define LCD_CGRAM           6
#define LCD_DDRAM           7

#define LCD_BUSY            7
#define LCD_LINES           2
#define LCD_LINE_LENGTH     16

#define LCD_LINE0_DDRAMADDR 0x00
#define LCD_LINE1_DDRAMADDR 0x40
#define LCD_LINE2_DDRAMADDR 0x14
#define LCD_LINE3_DDRAMADDR 0x54

#define PROGRESSPIXELS_PER_CHAR 6

// ==============================================================================
// Funciones de Bajo Nivel
// ==============================================================================

void LCDsendChar(uint8_t);
void LCDsendCommand(uint8_t);
void LCDinit(void);
void LCDclr(void);
void LCDhome(void);
void LCDstring(uint8_t*, uint8_t);
void LCDGotoXY(uint8_t, uint8_t);
void CopyStringtoLCD(const uint8_t*, uint8_t, uint8_t);
void LCDdefinechar(const uint8_t *, uint8_t);
void LCDshiftRight(uint8_t);
void LCDshiftLeft(uint8_t);
void LCDcursorOn(void);
void LCDcursorOnBlink(void);
void LCDcursorOFF(void);
void LCDblank(void);
void LCDvisible(void);
void LCDcursorLeft(uint8_t);
void LCDcursorRight(uint8_t);
void LCDescribeDato(int val, unsigned int field_length);
void LCDprogressBar(uint8_t progress, uint8_t maxprogress, uint8_t length);

// ==============================================================================
// Funciones de Alto Nivel (Aplicación Principal)
// ==============================================================================

/**
 * @brief Inicializa el hardware del LCD y limpia la pantalla.
 */
void LCD_Init(void);

/**
 * @brief Resetea la cuenta en el display a "00:00".
 */
void LCD_Resetear(void);

/**
 * @brief Actualiza la pantalla mostrando los segundos en formato "MM:SS".
 * @param seg Tiempo total en segundos a mostrar.
 */
void LCD_Actualizar(uint16_t seg);

/**
 * @brief Imprime un tiempo dado en formato "MM:SS" en la primera línea.
 */
void LCD_PrintTime(uint8_t minutos, uint8_t segundos);

/**
 * @brief Enciende o apaga el display completamente sin perder los datos en memoria.
 * @param estado 0 = apagar, 1 = encender.
 */
void LCD_Parpadear(uint8_t estado);

#endif /* LCD_H */

