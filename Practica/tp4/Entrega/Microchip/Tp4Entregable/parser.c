#include "parser.h"
#include "terminal.h"   // Para usar terminal_enviarMensaje()
#include <stdio.h>      // Para usar sscanf()

// Definición de las variables
static uint8_t color_R = 0;
static uint8_t color_G = 0;
static uint8_t color_B = 0;

void parser_procesarComando(char *comando_str) {
	int r, g, b;  // Deben ser int para sscanf
	
	// Intentamos leer el formato exacto: "SET_COLOR=%d,%d,%d"
	if (sscanf(comando_str, "SET_COLOR=%d,%d,%d", &r, &g, &b) == 3) {
		// Validamos que los valores estén en el rango de 8 bits (0-255)
		if (r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255) {
			color_R = (uint8_t)r;  
			color_G = (uint8_t)g;
			color_B = (uint8_t)b;
			
			// Confirmación al usuario
			terminal_enviarMensaje("Color actualizado");
			} else {
			terminal_enviarMensaje("ERROR: Los valores deben estar entre 0 y 255");
		}
		} else {
		// Si el formato no coincide, enviamos error
		terminal_enviarMensaje("ERROR: Formato invalido. Use: SET_COLOR=R,G,B");
	}
}

void parser_getColores(uint8_t *r, uint8_t *g, uint8_t *b) {
	*r = color_R;
	*g = color_G;
	*b = color_B;
}