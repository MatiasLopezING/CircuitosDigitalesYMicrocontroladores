/*
 * timer.c
 *
 * Created: 6/24/2026 12:49:46 AM
 *  Author: tomas
 */ 
#include "timer.h"

static uint8_t T=2;
static uint16_t ticks=0,ticks_T=200;
static bool flag_T=false;


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

    // Habilitar interrupción compare A
    TIMSK1 = (1 << OCIE1A);
}

void timer_setT(uint8_t Tnuevo) {
	T=Tnuevo;
	ticks_T=T*100;
	ticks=0;
}

bool timer_pasoT() {

	if (!flag_T) return false;

	flag_T = false;
	return true;
}

ISR (TIMER1_COMPA_vect) { //Interrupcion cada 10 ms
	
	if (++ticks==ticks_T) { //Paso periodo T
		flag_T=true;
		ticks=0;
	}
	
}