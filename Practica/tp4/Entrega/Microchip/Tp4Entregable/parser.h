#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include <stdbool.h>


// Función que procesa el comando 
void parser_procesarComando(char *comando_str);

// Devuelve los tres colores actuales
void parser_getColores(uint8_t *r, uint8_t *g, uint8_t *b);

#endif