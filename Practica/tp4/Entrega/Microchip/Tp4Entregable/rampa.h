#ifndef RAMPA_H
#define RAMPA_H

#include <stdint.h>
#include "adc.h"

#define MS_RAMPA     1000
#define MS_MESETA    1000
#define MS_TRAPECIO  3000

typedef enum { SUBIENDO, MANTENIENDO, BAJANDO, APAGADO } estado_efecto_t;

/* Inicializa la maquina de estados de la rampa. */
void rampa_Init(void);

/* Establece el periodo total T de la rampa. */
void rampa_SetPeriodo(uint16_t periodo);

/* Actualiza la maquina de estados . */
void rampa_Update(void);

/* Devuelve el nivel de brillo actual (0 a 255). */
uint8_t rampa_GetNivel(void);

/* Calcula el periodo T a partir de la lectura del ADC. */
uint16_t rampa_CalcularPeriodoMs(void);

#endif