/*
 * Entregable2.c
 *
 * Created: 6/24/2026 12:37:30 AM
 * Author : tomas
 */ 

#include "main.h"
#define F_CPU 16000000UL
#include <util/delay.h>
static volatile bool flag_T,flag_comando;
static volatile uint8_t T;

int main(void)
{
	sei();
    uart_init();
	terminal_enviarMensaje("[ALERTA] Temperatura fuera de rango diurno! Valor: 34 C");
	_delay_ms(1000);
	terminal_enviarMensaje("PEPEPE");
    while (1) 
    {
		
		
		
		
		//if (flag_T){ //Logica cada T segundos
			
			
		//}
		
		//if (flag_comando) { //Procesar comando
			
		}
  //  }
}



