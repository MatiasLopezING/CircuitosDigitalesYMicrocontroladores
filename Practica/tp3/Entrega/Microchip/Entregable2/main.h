/**
 * @file    main.h
 * @brief   Cabecera principal del proyecto monitor de invernadero (TP3).
 *
 * Centraliza todas las inclusiones de drivers y modulos del sistema:
 * UART, terminal, parser, timer, DHT11, DS3231 e I2C.
 */

#ifndef MAIN_H_
#define MAIN_H_

#define F_CPU 16000000UL  /* Frecuencia de CPU: 16 MHz */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"
#include "terminal.h"
#include "parser.h"
#include "timer.h"
#include "dht11.h"
#include "ds3231.h"
#include "i2c.h"

#endif /* MAIN_H_ */