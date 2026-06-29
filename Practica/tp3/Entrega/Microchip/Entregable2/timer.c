/**
 * @file    timer.c
 * @brief   Driver de temporizacion para el ATmega328P a 16 MHz.
 *
 * Utiliza Timer1 en modo CTC con prescaler 1024 y OCR1A = 15624,
 * generando una interrupcion exactamente cada 1 segundo (1 Hz).
 *
 * F_interrupcion = F_CPU / (Prescaler * (OCR1A + 1))
 *               = 16.000.000 / (1024 * 15625) = 1 Hz
 *
 * Arquitectura foreground/background estricta:
 *   - Foreground (ISR): solo activa flag_1s cada 1 segundo.
 *   - Background (timer_pasoT): cuenta los ticks de 1 s y determina si transcurrio T.
 */

#include "timer.h"

/* Activada por la ISR cada 1 segundo. Consumida y limpiada por timer_pasoT() en background. */
static volatile bool flag_1s = false;

/* Las siguientes variables solo se acceden desde el background (timer_pasoT / timer_setT),
 * por lo tanto no necesitan ser volatile. */
static uint8_t T     = 2;  /* Periodo T en segundos. Rango valido: 2-60. */
static uint8_t ticks = 0;  /* Ticks de 1 s acumulados desde el ultimo evento T. */

/*
 @brief Inicializa el Timer1 en modo CTC con interrupcion cada 10ms.

 */
void timer_init(void)
{
    /* Modo CTC (TOP = OCR1A) */
    TCCR1A = 0;
    TCCR1B = (1 << WGM12);

    /* Prescaler = 1024: CS12=1, CS11=0, CS10=1 */
    TCCR1B |= (1 << CS12) | (1 << CS10);

    /* OCR1A = 15624 → interrupcion exactamente cada 1 segundo
     * 16.000.000 / (1024 * (15624 + 1)) = 1 Hz */
    OCR1A = 15624;

    /* Habilitar interrupcion compare A */
    TIMSK1 = (1 << OCIE1A);
}
/*
 @brief Actualiza el periodo de reporte T y resetea el contador de ticks.

 @param Tnuevo  Nuevo periodo en segundos. Rango valido: 2 a 60.
 */
void timer_setT(uint8_t Tnuevo) {
	T     = Tnuevo;
	ticks = 0;       /* reiniciar conteo al cambiar el periodo */
}

/*
  @brief Indica si transcurrio un periodo T completo.

  Debe llamarse en el background (while principal). Consume el flag_1s
  activado por la ISR y cuenta los segundos transcurridos.
  Retorna true una sola vez por cada periodo T completado.

  @return true   Si transcurrieron T segundos desde la ultima vez que retorno true.
  @return false  Si no hubo tick de 1 s todavia, o aun no se completo el periodo T.
*/
bool timer_pasoT(void) {
	if (!flag_1s) return false;
	flag_1s = false;        /* consumir el tick de 1 segundo */
	if (++ticks >= T) {
		ticks = 0;
		return true;
	}
	return false;
}

/*
  @brief   ISR del Timer1 (TIMER1_COMPA_vect), se ejecuta cada 1 segundo.

  Unicamente activa flag_1s. Todo el conteo de segundos y la logica de
  periodo T se realiza en el background (timer_pasoT), respetando la
  arquitectura foreground/background estricta: el ISR hace el minimo
  trabajo posible.
 */
ISR(TIMER1_COMPA_vect) {
	flag_1s = true;
}