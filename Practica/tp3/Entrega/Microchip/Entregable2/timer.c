/*
 * CFile1.c
 *
 * Created: 6/24/2026 12:49:46 AM
 *  Author: tomas
 */ 
#include "main.h"


static volatile ticks=0;

ISR (TIMER0_COMPA_vect) {
	
	if (++ticks==T) {
		flag_T=true;
		ticks=0;
	}
	
}