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
    // Desactivar Watchdog Timer por seguridad
    MCUSR = 0;
    wdt_disable();
    
    // Retardo inicial de estabilizacion
    _delay_ms(100);

    // Inicializacion de perifericos
    ACTUADORES_Init();
    KEYPAD_Init();
    TIMER_Init();
    LCD_Init();
    
    // Habilitar interrupciones globales
    sei();
    
    // Interfaz inicial
    LCD_Resetear(); // Imprime "00:00" en pantalla
    
    uint8_t tecla;

    // Bucle principal: Prueba de integracion de hardware (sin MEF)
    while(1) {
        
        // 1. Escaneo no bloqueante del teclado matricial
        if (KEYPAD_Scan(&tecla)) {
            
            // Feedback visual en el LCD
            LCDclr();
            LCDGotoXY(0, 0);
            LCDstring((uint8_t*)"Tecla: ", 7);
            LCDsendChar(tecla);
            
            // Prueba de actuadores segun requerimientos del TP
            switch(tecla) {
                case 'A': // START: Enciende magnetron y luz
                    MAGNETRON_On();
                    LUZ_On();
                    break;
                case 'B': // STOP/CLEAR: Apaga todo y resetea
                    MAGNETRON_Off();
                    LUZ_Off();
                    ALARMA_Off();
                    LCD_Resetear();
                    break;
                case 'C': // +30 SEG: Toggle de alarma para probar
                    ALARMA_Toggle();
                    break;
                case 'D': // PUERTA: Apaga magnetron por seguridad
                    MAGNETRON_Off();
                    break;
            }
        }
        
        // 2. Base de tiempos de 1 segundo (gobernada por el Timer0)
        if (flag_actualizar_lcd == 1) {
            flag_actualizar_lcd = 0;
            // (Reservado para actualizar el cronometro del microondas)
        }
        
        _delay_ms(10); // Retardo para estabilizar el barrido
    }

    return 0;
}
