#undef F_CPU
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

#include "lcd.h"
#include "keypad.h"
#include "actuadores.h"
#include "timer.h"
#include "mef.h"



int main(void) {
 
    // Inicialización 
    ACTUADORES_Init();
    KEYPAD_Init();
    TIMER_Init();
    LCD_Init();
	FSM_Init();

  
    while(1) {

  
        if (flag_tick_10ms) {
			
			flag_tick_10ms=0;
			
           FSM_Update();
        }

  
    }

    return 0;
}
