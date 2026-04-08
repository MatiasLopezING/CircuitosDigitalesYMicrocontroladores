#include <avr/io.h>

void secuenciaA();
void secuenciaB();
void secuenciaC();
void secuenciaD();

uint8_t ledSec1,subiendo;

int main(void)
{
	
	uint8_t contadorSec1, contadorSec2,estAct1,estAct2;
	//Se puede optimizar aun mas el tema de los estados utilizando una sola variable con 1 bit por secuencia!
    while (1) 
    {
		_delay_ms(50); //Cada 50 ms chequeo 
		contadorSec1++; 
		contadorSec2++;
		//Incremento el contador de ambas secuencias que corren en simultaneo
		
		if ( (PINC & (1 << PINC0)) ==1 ) //Chequeo cambio de estado de la secuencia 1. ACA HAY QUE VER EL TEMA DE QUE NO QUEDE PULSADO MUCHO TIEMPO Y SE QUEDE ALTERNANDO
			if (estAct1==1) //Actualizo la secuencia 1 (A o B)
			 estAct1=0;
			else 
			 estAct1=1;
	    if (!(contadorSec1 % 2)) //Si pasaron 100 ms realizo la secuencia correspondiente
			{
				if(estAct1==1)
					secuenciaB(); //Se encarga de la logica de prender el led correspondiente
				else 
					secuenciaA();
			}
		
		if ( PINC1 == 1 )
			if(estAct2==1)
				estAct2=0;
			else 
				estAct2=1	 
		if (!(contadorSec2 % 3))   //Si pasaron 150 ms realizo la secuencia correspondiente
		{ 
			
			if(estAct2==1)
			secuenciaD(); //Se encarga de la logica de prender el led correspondiente
			else
			secuenciaC();
		}
		
    }
}
void secuenciaB () {
	PORTD=(1 << ledSec1);
	if (ledSec1 == 7){
		ledSec1--;
		subiendo=0;}
	else
		if(ledSec1==0) {
			ledSec1++;
			subiendo=1;}
		else 
			if(subiendo)
				ledSec1++;
			else ledSec1--;
	
}

void secuenciaA () {
	PORTD=(1 << ledSec1);	
	if(ledSec1== 7)
		ledSec1=0;
	else 
		ledSec1++;
	
}

void secuenciaC () {
	
}

void secuenciaD () {
	
}


