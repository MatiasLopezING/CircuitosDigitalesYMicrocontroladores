#include "main.h"

/* Buffer para almacenar el comando recibido por UART. */
char cmd_buffer[SIZE_COMANDO_MAX];

/* Variables temporales para los colores extraidos por el parser. */
uint8_t R, G, B;

/* Contador de ticks de 1 ms para decidir cuando leer el ADC*/
static uint16_t cuenta_adc = 0;

int main(void) {
	
    /* Inicializacion de perifericos y modulos. */
    uart_init();        /* UART a 9600 baudios, 8N1. */
    adc_Init();         /* ADC en PC3, referencia AVCC, prescaler 128. */
	pwm_hw_Init();		/* Inicializa Timer1 para generacion de señales PWM (Verde/Azul)*/
	pwm_sw_Init();		/* Inicializa Timer0 para generacion de señal PWM (Rojo)*/
    led_rgb_Init();     /* Inicializa valores por defecto de led_rgb */
    timer_tick_Init();  /* Timer2 en modo CTC con interrupcion cada 1 ms. Este timer se usa para controlar los tiempos de la respiracion del led y el tick de la MEF de rampa, no se utiliza el mismo timer que para PWM por SW */
    rampa_Init();       /* Maquina de estados del desvanecimiento*/
	
    sei();              /* Habilita interrupciones globales. */

    while (1) {
		
        /* Se reciben bytes por terminal hasta que se arma un comando. Cuando se detecta un terminador (\r\n), se extraen los valores RGB y se actualiza el color objetivo en el modulo led_rgb. */
        if (uart_hayDatosRx()) {
            terminal_consumirChars();
        }
        if (terminal_hayComando()) {
            terminal_getComando(cmd_buffer);
            parser_procesarComando(cmd_buffer);
            parser_getColores(&R, &G, &B);
            led_rgb_SetColor(R, G, B); /* Actualiza el color objetivo*/
        }

        /* Tick de 1 ms 
          El Timer2 genera un flag cada 1 ms. Cuando se detecta, se actualiza la maquina de estados de la rampa (calcula el nivel de brillo) y se aplica ese nivel al LED usando el color guardado. */
        if (timer_tick_HayTickMs()) {
            rampa_Update();
            led_rgb_ApplyBrightness(rampa_GetNivel());

            /* Lectura del ADC 
			Cada 100 ms se lee el valor del LDR y se recalcula el periodo total T de la rampa. El nuevo periodo se usa en la proxima actualizacion de la rampa. */
            if (++cuenta_adc >= 100) {
                cuenta_adc = 0;
                rampa_SetPeriodo(rampa_CalcularPeriodoMs());
            }
        }
    }
}