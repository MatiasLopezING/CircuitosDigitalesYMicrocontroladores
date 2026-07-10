/*
 * fade.c  -  MEF del efecto de desvanecimiento (ver fade.h).
 *
 * A implementar (MEF actualizada cada 1 ms):
 *   DESV_SUBIDA : nivel 0 -> 255 en 1 s (paso ~ cada 1000/255 ms).
 *   DESV_MESETA : nivel maximo durante (T - 2s) o el tramo de meseta elegido.
 *   DESV_BAJADA : nivel 255 -> 0 en 1 s.
 *   DESV_APAGADO: nivel 0; al cumplirse T reinicia el ciclo.
 *   En cada paso llamar a PWM_RGB_FijarIntensidad(nivel).
 *   Desvanecimiento_FijarPeriodo() ajusta T (viene del LDR, 3s..6s).
 *
 * Referencia teorica: clase09_MEF.pdf, clase10_EjemplosMEF.pdf
 */
#include "fade.h"
#include "pwm_rgb.h"
