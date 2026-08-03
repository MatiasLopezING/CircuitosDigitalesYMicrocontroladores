/*
  parser.c
  Modulo encargado de interpretar el comando recibido por UART.
  Reconoce el formato "SET_COLOR=R,G,B" y extrae los valores RGB
  validando que esten en el rango 0-255. Los colores se almacenan
  de forma privada (static) y se devuelven a traves de una funcion getter.
 */

#include "parser.h"   

/* Variables privadas que almacenan el color objetivo actual.*/
static uint8_t color_R = 0;
static uint8_t color_G = 0;
static uint8_t color_B = 0;

/*
  @brief   Procesa un comando y actualiza el color objetivo.
  @param   comando_str  Puntero al string terminado en '\0' que contiene el comando.
 
  El comando debe tener el formato exacto "SET_COLOR=R,G,B" (sin espacios).

 */
void parser_procesarComando(char *comando_str) {
    int r, g, b;  // Variables temporales para sscanf (deben ser int)

    /* Se intenta leer exactamente "SET_COLOR=%d,%d,%d". sscanf devuelve el numero de conversiones exitosas; si es 3, el formato es valido. */
    if (sscanf(comando_str, "SET_COLOR=%d,%d,%d", &r, &g, &b) == 3) {
        /* Validacion de rango: los valores deben estar entre 0 y 255 (8 bits). */
        if (r >= 0 && r <= 255 && g >= 0 && g <= 255 && b >= 0 && b <= 255) {
            color_R = (uint8_t)r;  
            color_G = (uint8_t)g;
            color_B = (uint8_t)b;

            terminal_enviarMensaje("Color actualizado");
        } else {
            terminal_enviarMensaje("ERROR: Los valores deben estar entre 0 y 255");
        }
    } else {
        /* Si el formato no coincide (falta "SET_COLOR=", o no hay tres numeros) se informa al usuario el formato esperado. */
        terminal_enviarMensaje("ERROR: Formato invalido. Use: SET_COLOR=R,G,B");
    }
}

/*
  @brief   Obtiene los valores actuales de R, G y B.
  @param   r  Puntero donde se almacenara el valor rojo.
  @param   g  Puntero donde se almacenara el valor verde.
  @param   b  Puntero donde se almacenara el valor azul.
 
 */
void parser_getColores(uint8_t *r, uint8_t *g, uint8_t *b) {
    *r = color_R;
    *g = color_G;
    *b = color_B;
}