/*
  rampa.c
  Maquina de estados para el efecto de desvanecimiento.
  Controla la evolucion del nivel de brillo a lo largo del tiempo dividiendo el ciclo en cuatro fases: subiendo, manteniendo, bajando y apagado.
  El periodo total T define la duracion de la fase de apagado.
 */

#include "rampa.h"

/* Variables de estado internas. */
static estado_efecto_t estado;
static uint16_t t_fase = 0;        /* Tiempo transcurrido en la fase actual (ms). */
static uint16_t periodo_ms = 6000; /* Periodo total T (ms). */
static uint8_t  nivel = 0;         /* Nivel de brillo actual (0-255). */

/*
  @brief Inicializa la maquina de estados.
  Pone el estado en SUBIENDO, el contador de fase en 0 y el nivel en 0.
 */
void rampa_Init(void) {
    estado = SUBIENDO;
    t_fase = 0;
    nivel = 0;
}

/*
  @brief Establece el periodo total de la rampa.
  @param periodo  Nuevo periodo en milisegundos.

 */
void rampa_SetPeriodo(uint16_t periodo) {
    if (periodo < MS_TRAPECIO) periodo = MS_TRAPECIO;
    periodo_ms = periodo;
}

/*
  @brief Actualiza la maquina de estados.
  Avanza el contador de fase y actualiza el nivel de brillo segun la fase actual.
  Las fases fijas (subida, meseta, bajada) duran 1000 ms cada una.
  La fase de apagado dura (periodo_ms - MS_TRAPECIO) ms.

  Explicacion de las formulas usadas para el nivel:
  - En la fase SUBIENDO, el nivel debe aumentar linealmente desde 0 hasta 255 en MS_RAMPA milisegundos. nivel = (t_fase * 255) / MS_RAMPA
    Esto da un valor proporcional al tiempo transcurrido dentro de la fase.
  - En la fase BAJANDO, el nivel debe disminuir linealmente desde 255 hasta 0 en MS_RAMPA milisegundos. nivel = 255 - ((t_fase * 255) / MS_RAMPA)
    Esto da un valor que comienza en 255 y decrece a medida que avanza t_fase.
  - En MANTENIENDO, el nivel se fija en 255.
  - En APAGADO, el nivel se fija en 0.

  En ambos casos se usa aritmetica de 32 bits para evitar desbordamiento en la multiplicacion (t_fase * 255 puede superar 65535), y se castea a uint8_t porque el resultado siempre esta en el rango 0-255.
 */
void rampa_Update(void) {
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
}

/*
  @brief Devuelve el nivel de brillo actual.
  @return Nivel de brillo entre 0 y 255.
 */
uint8_t rampa_GetNivel(void) {
    return nivel;
}

/*
  @brief Calcula el periodo T a partir de la lectura del LDR.
  Lee el valor del ADC (0-1023) y aplica la formula:
    T = 6000 - (ADC * 3000 / 1023)
  Se usa aritmetica de 32 bits para evitar desbordamiento en la multiplicacion.
  @return Periodo T en milisegundos, entre 3000 y 6000.
 */
uint16_t rampa_CalcularPeriodoMs(void) {
    uint16_t valor = adc_Leer();
    uint32_t T = 6000 - ((uint32_t)valor * 3000 / 1023);
    return (uint16_t)T;
}