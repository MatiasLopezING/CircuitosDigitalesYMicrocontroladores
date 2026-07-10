/*
 * main.h
 * TP4 - Control de intensidad y color de un LED RGB
 * Circuitos Digitales y Microcontroladores (E305) - UNLP
 *
 * MCU: ATmega328P @ 16 MHz
 *
 * Conexiones:
 *   LED RGB (RGBLED-CA, anodo comun):
 *     R -> PB5  (PWM por software)
 *     G -> PB2  (OC1B, Timer1)
 *     B -> PB1  (OC1A, Timer1)
 *   LDR    -> PC3 / ADC3 (divisor con 100k a GND)
 *   UART0  -> 9600 bps, 8N1 (terminal serie)
 *
 * OJO: al ser anodo comun el LED enciende con nivel BAJO,
 *      por lo tanto los ciclos de trabajo van invertidos.
 */
#ifndef MAIN_H
#define MAIN_H

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define F_CPU 16000000UL

/* Includes de los modulos del proyecto */
#include "uart.h"
#include "pwm_rgb.h"
#include "adc.h"
#include "fade.h"

/* Bandera de tick de 1 ms. Se DEFINE en main.c y la setea la ISR de
 * Timer0 (en pwm_rgb.c). Declarada extern aca para el resto de modulos. */
extern volatile uint8_t bandera_ms;

#endif /* MAIN_H */
