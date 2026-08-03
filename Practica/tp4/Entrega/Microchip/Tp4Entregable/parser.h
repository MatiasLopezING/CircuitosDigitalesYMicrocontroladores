#ifndef PARSER_H
#define PARSER_H

#include "terminal.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

// Funcion que procesa el comando SET_COLOR=R,G,B
void parser_procesarComando(char *comando_str);

// Devuelve los tres colores actuales
void parser_getColores(uint8_t *r, uint8_t *g, uint8_t *b);

#endif