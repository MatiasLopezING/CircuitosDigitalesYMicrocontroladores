#include "pwm_hw.h"
#include <avr/io.h>

/*
 * Inicializa el TIMER1 para generar PWM en PB1 (Azul) y PB2 (Verde).
 *
 * CONFIGURACIÓN TÉCNICA:
 * 1. Modo: PWM Rápido de 8 bits (WGM13:0 = 0101).
 *    - La resolución de 8 bits (0-255) coincide con el formato del comando del usuario.
 *    - El modo "rápido" actualiza el valor de OCR inmediatamente, sin desfases.
 * 
 * 2. Salida Invertida (COM1A/B = 11):
 *    - El LED es Ánodo Común (cátodo conectado al MCU).
 *    - Para encender el LED, el pin debe estar a LOW.
 *    - En modo invertido: OCR = 255 ? LOW 100% del tiempo ? Máximo brillo.
 *    - Si usáramos modo normal, OCR = 255 daría HIGH ? LED apagado (contra intuitivo).
 * 
 * 3. Prescaler = 256 (CS12 = 1):
 *    - Frecuencia = 16 MHz / (Prescaler * (OCR_MAX + 1)) = 16M / (256 * 256) = 244 Hz.
 *    - La consigna pide > 30 Hz para evitar parpadeo visible. 244 Hz es seguro y suave.
 *    - Usamos prescaler alto para ahorrar corriente y reducir ruido en la simulación.
 */
void pwm_hw_Init(void) {
    // --- Configurar pines PB1 y PB2 como salidas ---
    DDRB |= (1 << PB1) | (1 << PB2); 
    
    // --- Configurar TCCR1A (Modo y salidas) ---
    // WGM10 = 1 (parte del modo PWM rápido 8 bits)
    // COM1A1 = 1, COM1A0 = 1 (Salida invertida en OC1A - PB1 - Azul)
    // COM1B1 = 1, COM1B0 = 1 (Salida invertida en OC1B - PB2 - Verde)
    TCCR1A = (1 << WGM10) | 
             (1 << COM1A1) | (1 << COM1A0) |
             (1 << COM1B1) | (1 << COM1B0);
    
    // --- Configurar TCCR1B (Prescaler y modo) ---
    // WGM12 = 1 (completa el modo PWM rápido 8 bits WGM13:0 = 0101)
    // CS12 = 1 (Prescaler = 256: 16M / 256 = 62.5 kHz de base)
    TCCR1B = (1 << WGM12) | (1 << CS12);
    
    // Inicializar en 0 (apagado)
    OCR1A = 0; // Canal A -> Azul
    OCR1B = 0; // Canal B -> Verde
}

/*
 * Actualiza el brillo del Verde y Azul.
 * @param verde: 0 (apagado) a 255 (máximo)
 * @param azul:  0 (apagado) a 255 (máximo)
 */
void pwm_hw_SetColor(uint8_t verde, uint8_t azul) {
    OCR1B = verde; // Actualiza el registro hardware del Verde
    OCR1A = azul;  // Actualiza el registro hardware del Azul
}