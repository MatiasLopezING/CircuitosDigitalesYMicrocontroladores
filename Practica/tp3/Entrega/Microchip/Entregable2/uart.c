/*
 * CFile1.c
 *
 * Created: 6/24/2026 12:48:40 AM
 *  Author: tomas
 */ 
#include "uart.h"
#include "main.h"

static char bufferTx [63];
static char bufferRx [40]
static uint8_t tailRx=0, headTx=0, headRx=0;
char * aux;
void uart_init() {
	//BaudRate=16Mhz/(UBRR+1)16 con BaudRate=9600 -> UBRR=103
	UBRR0=103;
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0)  ; //Habilito recepcion, transmision, interrupcion por recepcion de byte
	
}

void uart_enviarTelemetria(char * telemetria) {
	strcpy(bufferTx, telemetria);
	headTx=0;
	UCSR0B |= ( 1<<UDRIE0); //Habilito interrupcion por UDR0 libre
}

bool uart_isEmpty() {
	return headRx==tailRx;
}

char uart_getCommand () { // Esto por ahi no lo hago y directamente que acceda al buffer sabiendo que el comando ya se recibio
	return ;
}
ISR(USART_RX_vect) { //Handler de interrupcion al recibir 1 char
	
	char c = UDR0;
	
	if (c == '\n'){
		bufferRx[headRx] = '\0'; //Caracter de fin 
		flag_comando=true; //Comando recibido desde la terminal
		headRx=0;
	}else {
		bufferRx[headRx++]=c; //Almaceno char 
	}
	
}

ISR(USART_UDRE_vect) { //Handler de interrupcion cuando UDR0 esta vacio
	
	if (  bufferTx[headTx] != '\n'){
		UDR0=bufferTx[headTx++];
	}else 
	{
		UCSR0B &= ~( 1<<UDRIE0) //Deshabilito interrupcion por UDR0 libre
	}
	
}