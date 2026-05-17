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



int main(void) {
    // Desactivación del Watchdog Timer.
    MCUSR = 0;
    wdt_disable();
    
    // Retardo inicial de estabilización.
    //_delay_ms(100);

    // Inicialización de periféricos.
    ACTUADORES_Init();
    KEYPAD_Init();
    TIMER_Init();
    LCD_Init();
    
    // Interfaz inicial.
    LCD_PrintTime(0); // Imprime estado inicial en pantalla.
    
    uint8_t tecla;

    // Bucle principal de ejecución.
    while(1) {
        
        // Escaneo de teclado matricial.
        if (flag_tick_10ms) {
			
			flag_tick_10ms=0;
			
			ACTUADORES_AlarmaUpdate();
			
           if( KEYPAD_Scan(&tecla)) {
			
            // Actualización de pantalla.
 
            LCDGotoXY(0, 0);
            LCDstring((uint8_t*)"Tecla: ", 7);
            LCDsendChar(tecla);
            
            // Control de actuadores.
            switch(tecla) {
                case 'A': // Inicia operación.
                    ACTUADORES_MagnetronOn();
                    ACTUADORES_LuzOn();
                    break;
                case 'B': // Detiene operación y resetea.
                    ACTUADORES_MagnetronOff();
                    ACTUADORES_LuzOff();
                    ACTUADORES_AlarmaOff();
                    LCD_PrintTime(0);
                    break;
                case 'C': // Alterna estado de alarma.
                    ACTUADORES_AlarmaToggle();
                    break;
                case 'D': // Apaga magnetrón.
                   ACTUADORES_MagnetronOff();
                    break;
            }
		   }
        }
        

        
  
    }

    return 0;
}
