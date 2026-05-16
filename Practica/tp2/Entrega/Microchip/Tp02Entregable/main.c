#include <avr/io.h>
#include <util/delay.h>
#include "keypad.h"
#include "lcd.h"

int main(void)
{
    // Inicializar periféricos
    KEYPAD_Init();
    LCD_Init();
    
    LCDGotoXY(0, 0);
    LCDstring((uint8_t*)"Esperando...", 12);
    
    uint8_t tecla;

    while (1) 
    {
        // 1. Escanear teclado
        if (KEYPAD_Scan(&tecla)) {
            // 2. Si hay tecla detectada, mostrar en display
            LCDclr();
            LCDGotoXY(0, 0);
            LCDstring((uint8_t*)"Tecla pulsada:", 16);
            LCDGotoXY(0, 1);
            LCDsendChar(tecla);
        }
        
        _delay_ms(10); // Pequeño delay de lazo
    }
}
