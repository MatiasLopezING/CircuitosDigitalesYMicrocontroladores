#undef F_CPU
#define F_CPU 8000000UL

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
    _delay_ms(100);

    // Inicialización de periféricos.
    ACTUADORES_Init();
    KEYPAD_Init();
    TIMER_Init();
    LCD_Init();
    
    // Habilitación de interrupciones globales.
    sei();
    
    // Interfaz inicial.
    LCD_Resetear(); // Imprime estado inicial en pantalla.
    
    uint8_t tecla;

    // Bucle principal de ejecución.
    while(1) {
        
        // Escaneo de teclado matricial.
        if (KEYPAD_Scan(&tecla)) {
            
            // Actualización de pantalla.
            LCDclr();
            LCDGotoXY(0, 0);
            LCDstring((uint8_t*)"Tecla: ", 7);
            LCDsendChar(tecla);
            
            // Control de actuadores.
            switch(tecla) {
                case 'A': // Inicia operación.
                    MAGNETRON_On();
                    LUZ_On();
                    break;
                case 'B': // Detiene operación y resetea.
                    MAGNETRON_Off();
                    LUZ_Off();
                    ALARMA_Off();
                    LCD_Resetear();
                    break;
                case 'C': // Alterna estado de alarma.
                    ALARMA_Toggle();
                    break;
                case 'D': // Apaga magnetrón.
                    MAGNETRON_Off();
                    break;
            }
        }
        
        // Actualización periódica gobernada por Timer0.
        if (flag_actualizar_lcd == 1) {
            flag_actualizar_lcd = 0;
            // Actualización de cronómetro.
        }
        
        _delay_ms(10); // Retardo de estabilización.
    }

    return 0;
}
