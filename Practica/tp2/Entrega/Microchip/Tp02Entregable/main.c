/*
 * Tp02Entregable.c
 *
 * 
 * Author : Tomas Gamarra y Matias Romero
 */ 

#include <avr/io.h>
#include "mef.h"

int main(void)
{
    FMS_Init();
	//Setear timer
    while (1) 
    {
		if( timerFlag ) {
			FSM_Update();
		}

    }
}

