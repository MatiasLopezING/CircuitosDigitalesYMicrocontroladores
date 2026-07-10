/*
 * main.c
 * Orquestacion del TP4 (ver main.h para conexiones y periféricos).
 *
 * Estructura sugerida (arquitectura tipo sEOS / super-loop con banderas):
 *   - Un systick de 1 ms (Timer0 o Timer2) marca flag_ms.
 *   - El lazo principal atiende: comandos UART y actualizacion del fade.
 *   - Periodicamente (p.ej. cada 100 ms) se relee el LDR para actualizar T.
 */
#include "main.h"

#define BAUD 9600
#define UBRR_VALUE (F_CPU/16/BAUD - 1)

/* Bandera de tick de 1 ms, seteada en la ISR del systick. */
volatile uint8_t bandera_ms = 0;

int main(void) {
    PWM_RGB_Inicializar();   /* Timer1 (G,B) + timer base para PWM sw (R) */
    UART_Inicializar(UBRR_VALUE);
    ADC_Inicializar();
    Desvanecimiento_Inicializar();
    sei();

    while (1) {
        UART_ProcesarRX();               /* parsea SET_COLOR = R,G,B */

        if (bandera_ms) {                /* cada 1 ms */
            bandera_ms = 0;
            Desvanecimiento_Actualizar(); /* avanza la MEF del desvanecimiento */
            /* TODO: cada N ms leer LDR ->
             *   Desvanecimiento_FijarPeriodo(ADC_ObtenerPeriodoParpadeoMs()) */
        }
        // sleep_cpu(); // opcional, según Clase09_MEF (sEOS)
    }
}