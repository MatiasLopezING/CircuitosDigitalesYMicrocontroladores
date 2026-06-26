/*
 * uart.c
 *
 * Created: 6/24/2026 12:48:40 AM
 *  Author: tomas
 */ 
#include "uart.h"


static char bufferTx [SIZE_BUFFERTX_MAX]; //Buffer para la transmision 
static volatile char bufferRx [SIZE_BUFFERRX_MAX]; //Buffer para la recepcion 
static volatile uint8_t headTx=0,tailTx=0, headRx=0, tailRx=0; //Variables para poder transmitir bufferTx y actualizar bufferRx al estar leyendo
static bool flag_OF=false;

void uart_init() { //Inicializacion del UART
	//BaudRate=16Mhz/(UBRR+1)16 con BaudRate=9600 -> UBRR=103
	UBRR0=103;
	UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0)  ; //Habilito recepcion, transmision, interrupcion por recepcion de byte
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); //Formato de trama 8N1
	
}

void uart_setUDRIE0() { //Habilito interrupcion por UDR0 libre para la transmision
	UCSR0B |= ( 1<<UDRIE0); 
}

void uart_limpiarBuffer() { //Limpo el bufferTx
	headTx=0;
	tailTx=0;
}

void uart_cargarByteBuffer(const char c) { //Cargo un char al bufferTx
	if (headTx < SIZE_BUFFERTX_MAX)
		bufferTx[headTx++]=c;
}

bool uart_leerByteBuffer( char * c) { //Devuelvo en c un caracter leido almacenado en bufferRx. El return sera false cuando no puedo leer siguiente byte porque todavia no fue escrito, true en caso contrario
	if (tailRx == headRx)
		return false;
	*c=bufferRx[tailRx++];
	if (tailRx == SIZE_BUFFERRX_MAX){
		tailRx=0;
	}
	return true;
}

void uart_resetearRx()
{
	uint8_t sreg = SREG;
	cli(); //Controlo concurrencia por si salta una ISR provocada por USART_RX_vect.

	headRx = 0;
	tailRx = 0;
	flag_OF = false;

	SREG = sreg;
}

bool uart_huboOV(){
	return flag_OF;
}

 


ISR(USART_RX_vect) { //Handler de interrupcion al recibir 1 char
	
	//Se deja una posicion del buffer vacia para poder distinguir cuando el buffer esta lleno y cuando esta vacio. Es necesario hacer esto para poder distinguir cuando headRx == tailRx pero esto es porque esta vacio el buffer
	//head == tail  ? Buffer vacio
	// next(head) == tail ? Buffer lleno
	char c = UDR0;
	
	uint8_t siguiente; //Utilizar esta variable me permite distinguir del caso que head==tail en el comienzo
	
	if (!flag_OF) { //No hago ninguna validacion si estoy en estado de overflow
		siguiente=headRx + 1;

		if (siguiente == SIZE_BUFFERRX_MAX) //Ya esta el buffer lleno, debo volver al principio
			siguiente=0;
		if (siguiente != tailRx) { // Si la siguiente posicion a escribir no se leyo aun, entonces no almaceno.
			bufferRx[headRx] = c;  //Almaceno char 	
			headRx=siguiente;
		} else 
			flag_OF=true; //No puedo cargar mas chars
	
	}
}

ISR(USART_UDRE_vect) { //Handler de interrupcion cuando UDR0 esta vacio
	
	if (  bufferTx[tailTx] != '\0'){
		UDR0=bufferTx[tailTx++];
	}else 
	{
		uart_limpiarBuffer();
		UCSR0B &= ~( 1<<UDRIE0); //Deshabilito interrupcion por UDR0 libre
	}
	
}