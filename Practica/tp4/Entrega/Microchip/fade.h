/*
 * fade.h
 * Modulo del efecto de encendido/apagado con desvanecimiento (fade).
 *
 * Perfil de intensidad (segun figura del enunciado):
 *   - Subida (fade-in)  : 1 s de 0 a Max.
 *   - Meseta / recorrido : segun el periodo total T.
 *   - Bajada (fade-out) : 1 s de Max a 0.
 *   - Se repite con periodo T controlado por el LDR (3 s .. 6 s).
 *
 * Implementar como una MEF (maquina de estados) actualizada
 * periodicamente desde una base de tiempo de 1 ms (systick).
 * En cada paso calcula el nivel 0..255 y lo entrega al PWM.
 */
#ifndef FADE_H
#define FADE_H

#include <stdint.h>

/* Estados sugeridos de la MEF del efecto de desvanecimiento. */
typedef enum {
    DESV_SUBIDA,   /* subiendo intensidad  */
    DESV_MESETA,   /* intensidad maxima    */
    DESV_BAJADA,   /* bajando intensidad   */
    DESV_APAGADO   /* apagado (entre ciclos) */
} estado_desvanecimiento_t;

/* Inicializa la MEF del efecto de desvanecimiento. */
void Desvanecimiento_Inicializar(void);

/* Fija el periodo total T del parpadeo (en ms), tomado del LDR. */
void Desvanecimiento_FijarPeriodo(uint16_t periodo_ms);

/* Avanza la MEF un paso (llamar cada 1 ms desde el lazo/systick).
 * Internamente calcula el nivel y llama a PWM_RGB_FijarIntensidad(). */
void Desvanecimiento_Actualizar(void);

#endif /* FADE_H */
