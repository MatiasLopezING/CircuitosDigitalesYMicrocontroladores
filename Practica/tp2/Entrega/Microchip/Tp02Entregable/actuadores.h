/**
 * @file actuadores.h
 * @brief Librería de control de actuadores de hardware.
 * 
 * Contiene las funciones para inicializar y controlar los actuadores del sistema
 * (Magnetrón, Luz interior y Alarma sonora).
 */

#ifndef ACTUADORES_H_
#define ACTUADORES_H_

#include <avr/io.h>

/**
 * @brief Inicializa los pines correspondientes a los actuadores.
 * Configura los pines de salida del microcontrolador y los establece en estado bajo (apagados).
 */
void ACTUADORES_Init(void);

// ==============================================================================
// --- Magnetrón (LED Rojo en PB5) ---
// ==============================================================================

/**
 * @brief Enciende el Magnetrón.
 */
void MAGNETRON_On(void);

/**
 * @brief Apaga el Magnetrón.
 */
void MAGNETRON_Off(void);

// ==============================================================================
// --- Luz Interior (LED Amarillo en PC4) ---
// ==============================================================================

/**
 * @brief Enciende la luz interior del microondas.
 */
void LUZ_On(void);

/**
 * @brief Apaga la luz interior del microondas.
 */
void LUZ_Off(void);

// ==============================================================================
// --- Alarma (LED Verde en PC5) ---
// ==============================================================================

/**
 * @brief Enciende la alarma sonora.
 */
void ALARMA_On(void);

/**
 * @brief Apaga la alarma sonora.
 */
void ALARMA_Off(void);

/**
 * @brief Invierte (toggle) el estado actual de la alarma.
 */
void ALARMA_Toggle(void);

#endif /* ACTUADORES_H_ */
