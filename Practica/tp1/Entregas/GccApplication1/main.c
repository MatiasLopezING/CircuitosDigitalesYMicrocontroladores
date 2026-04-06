/*
 * GccApplication1.c
 *
 * Created: 6/4/2026 15:38:47
 * Author : ml328
 */ 

#include <avr/io.h>


int main(void)
{
	
	uint8_t contador1, contador2;
	
	
    /* Replace with your application code */
    while (1) 
    {
		_delay_ms(50);
		contador1++;
		contador2++;
		if (!(contador1 mod 2))
			if (PINC0==0) //Secuencia a)
			{
			} 
			else //Secuencia b)
			{
			}
		if(!(contador2 mod 3))
			if(PINC1 == 0) 
    }
}

