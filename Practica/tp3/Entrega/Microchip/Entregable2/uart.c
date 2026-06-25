/*
 * uart.c
 *
 * Created: 6/24/2026 12:48:40 AM
 *  Author: tomas
 */ 
#include "uart.h"


static char bufferTx [SIZE_BUFFERTX_MAX];
static volatile char bufferRx [SIZE_BUFFERRX_MAX];
static volatile uint8_t headTx=0, headRx=0;
static bool flag_hayComando=false;

void uart_init() {
	//BaudRate=16Mhz/(UBRR+1)16 con BaudRate=9600 -> UBRR=103
	UBRR0=103;
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0)  ; //Habilito recepcion, transmision, interrupcion por recepcion de byte
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //Formato de trama 8N1
	
}

void uart_enviarTelemetria(const char * telemetria) {
	strlcpy(bufferTx, telemetria, SIZE_BUFFERTX_MAX + 1); //No acepto telemetrias de mas de SIZE_BUFFERTX_MAX , el + 1 es para el caracter de terminacion \0s
	headTx=0;
	UCSR0B |= ( 1<<UDRIE0); //Habilito interrupcion por UDR0 libre
}


bool uart_hayComando () {
	return flag_hayComando;
}



ISR(USART_RX_vect) { //Handler de interrupcion al recibir 1 char
	
	char c = UDR0;
	
	if (c == '\n'){
		bufferRx[headRx] = '\0'; //Caracter de fin 
		flag_hayComando=true; //Comando recibido desde la terminal
		headRx=0; //Esto habria que ver si no genera problemas al intentar ingresar 2 comandos muy seguidos, entiendo que como la toma del comando es del orden de ms no habria problema
	}else {
		bufferRx[headRx++]=c; //Almaceno char 
	}
	
}

ISR(USART_UDRE_vect) { //Handler de interrupcion cuando UDR0 esta vacio
	
	if (  bufferTx[headTx] != '\0'){
		UDR0=bufferTx[headTx++];
	}else 
	{
		UCSR0B &= ~( 1<<UDRIE0); //Deshabilito interrupcion por UDR0 libre
	}
	
}