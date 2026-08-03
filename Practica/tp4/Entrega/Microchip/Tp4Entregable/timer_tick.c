/*
  timer_tick.c
  Driver del Timer2 para generar un tick de 1 ms exacto.

 */

#include "timer_tick.h"

/* Bandera volatile porque es escrita por la ISR y leida por el foreground. */
static volatile bool flag_tick = false;

/*
  @brief Inicializa el Timer2 para generar un tick de 1 ms.

 */
void timer_tick_Init(void) {
    TCCR2A = (1 << WGM21);             /* Modo CTC. */
    TCCR2B = (1 << CS22);              /* Prescaler 64. */
    OCR2A = 249;                       /* TOP = 249 -> 1 ms. */
    TIMSK2 |= (1 << OCIE2A);           /* Habilitar interrupción. */
}

/*
  @brief Consulta si ocurrio un tick de 1 ms.
  @return true si hubo un tick desde la ultima consulta, false en caso contrario.
 
 */
bool timer_tick_HayTickMs(void) {
    if (!flag_tick) return false;
    flag_tick = false;
    return true;
}

/*
  @brief ISR del Timer2 para el tick de 1 ms.
  Simplemente activa la bandera flag_tick.
 */
ISR(TIMER2_COMPA_vect) {
    flag_tick = true;
}