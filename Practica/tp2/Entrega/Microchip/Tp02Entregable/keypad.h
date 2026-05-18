/**
 * @file keypad.h
 * @brief Librería para el escaneo matricial de un teclado 4x4.
 * 
 * Configura los puertos necesarios y proporciona funciones para
 * detectar qué tecla fue presionada devolviendo su valor en código ASCII.
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#include <stdint.h>
#include <avr/io.h>

/**
 * @brief Inicializa los pines del microcontrolador conectados al teclado.
 * Configura las filas como salidas y las columnas como entradas con
 * resistencias pull-up activadas.
 */
void KEYPAD_Init(void);

/**
 * @brief Escanea la matriz del teclado para detectar pulsaciones.
 * @param pkey Puntero donde se almacenará el código ASCII de la tecla detectada.
 * @return 1 si se detectó una nueva tecla presionada, 0 en caso contrario.
 */
uint8_t KEYPAD_Scan(uint8_t *pkey);

/**
 * @brief Función auxiliar que escanea el teclado y devuelve el caracter directamente.
 * @return El caracter ASCII de la tecla presionada (ej: '1', 'A', '#'), o 0 si no hay pulsación.
 */
//..uint8_t KEYPAD_GetKey(void);

#endif /* KEYPAD_H_ */