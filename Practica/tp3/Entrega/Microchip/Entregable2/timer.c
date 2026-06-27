/**
 * @file    timer.c
 * @brief   Driver de temporizacion para el ATmega328P a 16 MHz.
 *
 * Utiliza Timer1 en modo CTC para generar una interrupcion cada 10 ms (100 Hz).
 * A partir de los ticks acumulados se detecta el vencimiento del periodo T,
 * configurable entre 2 y 60 segundos.
 */

#include "timer.h"
/* Periodo T en segundos. Rango valido: 2 a 60. */
static uint8_t T=2;
/* Contador de ticks de 10ms transcurridos desde el ultimo evento T. */
static uint16_t ticks=0;
/* Cantidad de ticks necesarios para completar el periodo T (T * 100). */
static uint16_t ticks_T=200;
/* Flag que indica que transcurrio un periodo T completo. */
static bool flag_T=false;

/*
 @brief Inicializa el Timer1 en modo CTC con interrupcion cada 10ms.

 */
void timer_init(void) //Utilizamos timer1
{
	//F_interrupcion = F_CPU / (Prescaler * (OCR1A + 1))
	//Buscamos interrupcion cada 10 ms, es decir F_interrupcion=100Hz por lo que con Prescaler=64 y OCR1A=2499 obtenemos 10 ms
    // Modo CTC (TOP = OCR1A)
    TCCR1A = 0;
	
    TCCR1B = (1 << WGM12);
    // Prescaler = 64
	
    TCCR1B |= (1 << CS11) | (1 << CS10);
    // 10 ms exactos
	
    OCR1A = 2499;
    // Habilitar interrupcion compare A
    TIMSK1 = (1 << OCIE1A);
}
/*
 @brief Actualiza el periodo de reporte T y resetea el contador de ticks.

 @param Tnuevo  Nuevo periodo en segundos. Rango valido: 2 a 60.
 */
void timer_setT(uint8_t Tnuevo) {
	T=Tnuevo;
	ticks_T=T*100;
	ticks=0;
}

/*
@brief Indica si transcurrio un periodo T completo.

@return true   Si transcurrio el periodo T desde la ultima consulta.
@return false  Si todavia no transcurrio el periodo T.
*/

bool timer_pasoT() {

	if (!flag_T) return false;

	flag_T = false;
	return true;
}

/*
  @brief   ISR del Timer1 (TIMER1_COMPA_vect), se ejecuta cada 10 ms.

  Incrementa el contador de ticks y activa flag_T cuando se completa
  el periodo T configurado.
 */
ISR(TIMER1_COMPA_vect) {
	
	if (++ticks==ticks_T) { 
		flag_T=true;
		ticks=0;
	}
	
}