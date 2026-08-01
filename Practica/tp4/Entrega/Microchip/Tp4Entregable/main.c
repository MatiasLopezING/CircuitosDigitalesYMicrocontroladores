#include "uart.h"
#include "terminal.h"
#include "parser.h"
#include "adc.h"
#include "pwm_hw.h"
#include "pwm_sw.h"

int main(void) {
	char cmd_buffer[SIZE_COMANDO_MAX];
	uint16_t T_periodo_ms = 0;
	uint8_t R, G, B;

	uart_init();
	adc_init();
	pwm_hw_Init();
	pwm_sw_Init();
	sei(); // ¡CRUCIAL para el PWM por software!

	while(1) {
		// 1. UART y Parseo
		if (uart_hayDatosRx()) {
			terminal_consumirChars();
		}
		if (terminal_hayComando()) {
			terminal_getComando(cmd_buffer);
			parser_procesarComando(cmd_buffer);
			// Aplicamos el color fijo inmediatamente
			parser_getColores(&R, &G, &B);
			pwm_hw_SetColor(G, B); // Verde y Azul
			pwm_sw_SetDuty(R);     // Rojo
		}

		// 2. ADC (Lectura cada 500 ms aprox)
		static uint16_t contador_prueba = 0;
		contador_prueba++;
		if (contador_prueba >= 50000) {
			contador_prueba = 0;
			T_periodo_ms = adc_calcularPeriodoMs();
			// Enviar por UART para testear
			char msg[30];
			sprintf(msg, "T = %d ms", T_periodo_ms);
			terminal_enviarMensaje(msg);
		}
	}
}