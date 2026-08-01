#include "uart.h"
#include "terminal.h"
#include "parser.h"
#include "adc.h"
#include "pwm_hw.h"
#include "pwm_sw.h"
#include "efecto.h"

int main(void) {
	char cmd_buffer[SIZE_COMANDO_MAX];
	uint8_t R, G, B;
	uint16_t cuenta_ms = 0;

	uart_init();
	adc_init();
	pwm_hw_Init();
	pwm_sw_Init();
	efecto_init();
	sei();

	while(1) {
		// UART y parseo del comando
		if (uart_hayDatosRx()) {
			terminal_consumirChars();
		}
		if (terminal_hayComando()) {
			terminal_getComando(cmd_buffer);
			parser_procesarComando(cmd_buffer);
			parser_getColores(&R, &G, &B);
			efecto_setColor(R, G, B);
		}

		// Base de tiempo de 1 ms
		if (pwm_sw_hayTickMs()) {
			efecto_actualizar();
			if (++cuenta_ms >= 100) {
				cuenta_ms = 0;
				efecto_setPeriodo(adc_calcularPeriodoMs());
			}
		}
	}
}
