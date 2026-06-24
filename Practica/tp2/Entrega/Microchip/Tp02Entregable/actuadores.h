/**
 * 
 *  Librería de control de actuadores de hardware (Salidas).
 *  Contiene las funciones para inicializar y controlar los actuadores del sistema
 *  (Magnetrón, Luz interior y Alarma sonora).
 */

#ifndef ACTUADORES_H_
#define ACTUADORES_H_

#include <avr/io.h>

/**
 *  @brief Configura los pines correspondientes a los actuadores y los inicializa en estado bajo (apagados).
 */
void ACTUADORES_Init(void);

// ==============================================================================
// --- Magnetrón (LED Rojo en PB5) ---
// ==============================================================================

/**
 * @brief  Enciende el Magnetrón.
 */
void ACTUADORES_MagnetronOn(void);

/**
 *  @brief Apaga el Magnetrón.
 */
void ACTUADORES_MagnetronOff(void);

// ==============================================================================
// --- Luz Interior (LED Amarillo en PC4) ---
// ==============================================================================

/**
 * @brief  Enciende la luz interior del microondas.
 */
void ACTUADORES_LuzOn(void);

/**
 * @brief  Apaga la luz interior del microondas.
 */
void ACTUADORES_LuzOff(void);

// ==============================================================================
// --- Alarma (LED Verde en PC5) ---
// ==============================================================================

/**
 * @brief  Enciende la alarma sonora.
 */
void ACTUADORES_AlarmaOn(void);

/**
 * @brief  Apaga la alarma sonora.
 */
void ACTUADORES_AlarmaOff(void);

/**
 * @brief  Invierte (toggle) el estado actual de la alarma.
 */
void ACTUADORES_AlarmaToggle(void);


#endif /* ACTUADORES_H_ */


