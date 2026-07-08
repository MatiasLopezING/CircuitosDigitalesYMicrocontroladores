#include "main.h"

#define F_CPU 16000000UL
#define BAUD 9600
#define UBRR_VALUE (F_CPU/16/BAUD - 1)

volatile uint8_t flag_ms = 0;

int main(void) {
    PWM_RGB_Init();
    UART_Init(UBRR_VALUE);
    ADC_Init();
    sei();

    while (1) {
        UART_ProcessRX();

        if () {
            Fade_Update();
        }
        // sleep_cpu(); // opcional, según Clase09_MEF (sEOS)
    }
}