#ifndef EFECTO_H
#define EFECTO_H

#include <stdint.h>

/**
 * @brief Inicializa la maquina de estados del efecto de desvanecimiento.
 */
void efecto_init(void);

/**
 * @brief Fija el color base sobre el que se aplica la rampa.
 * @param r,g,b  Componentes 0 a 255.
 */
void efecto_setColor(uint8_t r, uint8_t g, uint8_t b);

/**
 * @brief Fija el periodo total T del parpadeo, en ms (3000 a 6000).
 */
void efecto_setPeriodo(uint16_t periodo_ms);

/**
 * @brief Avanza la maquina de estados un paso. Llamar cada 1 ms.
 */
void efecto_actualizar(void);

#endif
