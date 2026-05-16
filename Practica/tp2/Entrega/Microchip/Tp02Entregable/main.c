#undef F_CPU
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"
#include "keypad.h"
#include "actuadores.h"

int main(void) {
    _delay_ms(100); // 1. Retardo para que estabilicen las señales de Proteus/Placa al encender
    KEYPAD_Init();
    ACTUADORES_Init();
    LCD_Init(); // Mantenemos la inicializacion del LCD al final

    _delay_ms(50); // 2. Retardo extra para asegurar que el LCD terminó de configurarse internamente

    uint8_t tecla_pulsada;

    // Imprimimos el texto fijo una sola vez (forzamos posición 0,0 por las dudas)
    LCDGotoXY(0, 0);
    LCDstring((uint8_t*)"Tecla: ", 7);

    while(1) {
        // Escaneamos el teclado
        if (KEYPAD_Scan(&tecla_pulsada) == 1) { 
            
            // Nos posicionamos justo despues de "Tecla: " (columna 7, fila 0)
            // para pisar solo la letra en lugar de limpiar toda la pantalla
            LCDGotoXY(7, 0);
            LCDsendChar(tecla_pulsada); // Muestra el caracter ASCII en el LCD

            // Jugamos con los actuadores segun la tecla
            switch(tecla_pulsada) {
                case '1':
                    MAGNETRON_On();
                    break;
                case '2':
                    MAGNETRON_Off();
                    break;
                case '4':
                    LUZ_On();
                    break;
                case '5':
                    LUZ_Off();
                    break;
                case '7':
                    ALARMA_Toggle(); // Cambia de estado
                    break;
                case '0':
                    // Boton de panico: apaga todo
                    MAGNETRON_Off();
                    LUZ_Off();
                    ALARMA_Off(); 
                    break;
            }
        }
        
        // Pequeno delay de lazo para que la lectura no sature
        _delay_ms(10);
    }
    return 0;
}
