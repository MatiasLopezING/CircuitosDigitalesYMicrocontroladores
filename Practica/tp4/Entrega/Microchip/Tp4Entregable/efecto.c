#include "efecto.h"
#include "pwm_hw.h"
#include "pwm_sw.h"

/*
 * Maquina de estados del efecto de desvanecimiento del LED RGB.
 * Se actualiza cada 1 ms desde el main.
 *
 *   SUBIENDO    : 0 -> 255 en 1000 ms
 *   MANTENIENDO : 255 durante 1000 ms
 *   BAJANDO     : 255 -> 0 en 1000 ms
 *   APAGADO     : 0 durante (T - 3000) ms
 *
 * T es el periodo total, entre 3000 y 6000 ms segun el LDR.
 */

#define MS_RAMPA     1000
#define MS_MESETA    1000
#define MS_TRAPECIO  3000

typedef enum {
	SUBIENDO,
	MANTENIENDO,
	BAJANDO,
	APAGADO
} estado_efecto_t;

static estado_efecto_t estado;
static uint16_t t_fase;
static uint16_t periodo_ms;
static uint8_t  color_R, color_G, color_B;
static uint8_t  nivel;

static void aplicarNivel(void) {
	uint8_t r = (uint8_t)(((uint16_t)color_R * nivel) / 255);
	uint8_t g = (uint8_t)(((uint16_t)color_G * nivel) / 255);
	uint8_t b = (uint8_t)(((uint16_t)color_B * nivel) / 255);

	pwm_hw_SetColor(g, b);
	pwm_sw_SetDuty(r);
}

void efecto_init(void) {
	estado = SUBIENDO;
	t_fase = 0;
	periodo_ms = 6000;
	color_R = color_G = color_B = 0;
	nivel = 0;
	aplicarNivel();
}

void efecto_setColor(uint8_t r, uint8_t g, uint8_t b) {
	color_R = r;
	color_G = g;
	color_B = b;
}

void efecto_setPeriodo(uint16_t periodo) {
	if (periodo < MS_TRAPECIO) periodo = MS_TRAPECIO;
	periodo_ms = periodo;
}

void efecto_actualizar(void) {
	t_fase++;

	switch (estado) {
		case SUBIENDO:
			nivel = (uint8_t)(((uint32_t)t_fase * 255) / MS_RAMPA);
			if (t_fase >= MS_RAMPA) {
				nivel = 255;
				estado = MANTENIENDO;
				t_fase = 0;
			}
			break;

		case MANTENIENDO:
			nivel = 255;
			if (t_fase >= MS_MESETA) {
				estado = BAJANDO;
				t_fase = 0;
			}
			break;

		case BAJANDO:
			nivel = (uint8_t)(255 - ((uint32_t)t_fase * 255) / MS_RAMPA);
			if (t_fase >= MS_RAMPA) {
				nivel = 0;
				estado = APAGADO;
				t_fase = 0;
			}
			break;

		case APAGADO:
			nivel = 0;
			if (t_fase >= (periodo_ms - MS_TRAPECIO)) {
				estado = SUBIENDO;
				t_fase = 0;
			}
			break;
	}

	aplicarNivel();
}
