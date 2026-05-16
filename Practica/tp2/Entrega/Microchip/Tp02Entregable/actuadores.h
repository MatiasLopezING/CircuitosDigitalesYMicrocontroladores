#ifndef ACTUADORES_H_
#define ACTUADORES_H_

#include <avr/io.h>

// Inicializa los pines de los actuadores como salidas
void ACTUADORES_Init(void);

// --- Magnetrón (LED Rojo en PB5) ---
void MAGNETRON_On(void);
void MAGNETRON_Off(void);

// --- Luz Interior (LED Amarillo en PC4) ---
void LUZ_On(void);
void LUZ_Off(void);

// --- Alarma (LED Verde en PC5) ---
void ALARMA_On(void);
void ALARMA_Off(void);
void ALARMA_Toggle(void);

#endif /* ACTUADORES_H_ */
