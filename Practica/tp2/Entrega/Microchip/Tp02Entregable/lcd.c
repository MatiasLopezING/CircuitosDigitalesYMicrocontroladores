#include "lcd.h"

// Definición de caracteres personalizados LCD
const uint8_t LcdCustomChar[] PROGMEM=
{
	0x00, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x1F, 0x00, // 0/5 progreso
	0x00, 0x1F, 0x10, 0x10, 0x10, 0x10, 0x1F, 0x00, // 1/5 progreso
	0x00, 0x1F, 0x18, 0x18, 0x18, 0x18, 0x1F, 0x00, // 2/5 progreso
	0x00, 0x1F, 0x1C, 0x1C, 0x1C, 0x1C, 0x1F, 0x00, // 3/5 progreso
	0x00, 0x1F, 0x1E, 0x1E, 0x1E, 0x1E, 0x1F, 0x00, // 4/5 progreso
	0x00, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00, // 5/5 progreso
	0x03, 0x07, 0x0F, 0x1F, 0x0F, 0x07, 0x03, 0x00, // Flecha retroceso
	0x18, 0x1C, 0x1E, 0x1F, 0x1E, 0x1C, 0x18, 0x00  // Flecha avance
};

// ==============================================================================
// Funciones de Transmisión de Bajo Nivel
// ==============================================================================

/**
 * @brief Envía un caracter ASCII al LCD.
 * @param ch Caracter a enviar.
 */
void LCDsendChar(uint8_t ch){

#ifdef LCD_4bit
    // Habilitación de pin RS para envío de datos.
    LCP |= (1<<LCD_RS); 
    
    // Nibble superior.
	LCD_DATAWR(ch & 0b11110000);
	LCP|=1<<LCD_E;		
	_delay_us(50);
	LCP&=~(1<<LCD_E);	
	_delay_ms(1);
	
	// Nibble inferior.
	LCD_DATAWR((ch & 0b00001111)<<4);
	LCP|=1<<LCD_E;		
	_delay_us(50);
	LCP&=~(1<<LCD_E);	
	_delay_ms(2); // Retardo de espera.
#else
	// Interfaz de 8 bits.
	LDP=ch;
	LCP|=1<<LCD_RS;
	LCP|=1<<LCD_E;		
	_delay_us(2);
	LCP&=~(1<<LCD_E);	
	LCP&=~(1<<LCD_RS);
	_delay_us(100);
#endif
}

/**
 * @brief Envía un comando de control al LCD.
 * @param cmd Código del comando (ej. 0x01 para limpiar).
 */
void LCDsendCommand(uint8_t cmd)	
{
#ifdef LCD_4bit	
    // Deshabilitación de pin RS para envío de comandos.
    LCP &= ~(1<<LCD_RS); 
    
	// Nibble superior.
	LCD_DATAWR(cmd & 0b11110000);
	LCP|=1<<LCD_E;		
	_delay_us(50);
	LCP&=~(1<<LCD_E);
	_delay_ms(1);	
	
	// Nibble inferior.
	LCD_DATAWR((cmd & 0b00001111)<<4);	
	LCP|=1<<LCD_E;		
	_delay_us(50);
	LCP&=~(1<<LCD_E);
	
	if(cmd == 0x01 || cmd == 0x02) {
        _delay_ms(5); // Retardo extendido para comandos Clear/Home.
    } else {
        _delay_ms(2); // Retardo de espera.
    }
#else
	// Interfaz de 8 bits.
	LDP=cmd;
	LCP &= ~(1<<LCD_RS);
	LCP|=1<<LCD_E;		
	_delay_us(2);
	LCP&=~(1<<LCD_E);
	
	if(cmd == 0x01 || cmd == 0x02) {
        _delay_ms(2); 
    } else {
        _delay_us(100);
    }
#endif
}

// ==============================================================================
// Inicialización del Hardware
// ==============================================================================

/**
 * @brief Inicializa el controlador HD44780 respetando los tiempos del datasheet.
 */
void LCDinit(void)
{
#ifdef LCD_4bit	
    // Espera de estabilización de voltaje.
	_delay_ms(50);
	
	// Inicialización de pin RS.
    LCP &= ~(1<<LCD_RS);
	
	LDDR1|=1<<LCD_D7|1<<LCD_D6;
	LDDR2|=1<<LCD_D4|1<<LCD_D5;
	LCDR |= (1<<LCD_E) | (1<<LCD_RS);
	
    // Secuencia de inicialización (pulsos únicos).
	LCD_DATAWR(0b00110000);	 // 0x30
	LCP|=1<<LCD_E;		
	_delay_us(50);
	LCP&=~(1<<LCD_E);
	_delay_ms(5);
	
	LCD_DATAWR(0b00110000);	 // 0x30
	LCP|=1<<LCD_E;		
	_delay_us(50);
	LCP&=~(1<<LCD_E);
	_delay_ms(5);

	LCD_DATAWR(0b00110000);	 // 0x30
	LCP|=1<<LCD_E;		
	_delay_us(50);
	LCP&=~(1<<LCD_E);
	_delay_ms(5);
	
	LCD_DATAWR(0b00100000);	 // 0x20 Configuración a modo 4 bits.
	LCP|=1<<LCD_E;		
	_delay_us(50);
	LCP&=~(1<<LCD_E);
	_delay_ms(5);
	
	// Configuración mediante comandos de 4 bits.
	LCDsendCommand(0x28); // Modo 4 bits, 2 líneas, 5x8 puntos.
	LCDsendCommand(0x08); // Apagar display.
	LCDsendCommand(0x01); // Limpiar pantalla.
	LCDsendCommand(0x06); // Modo de entrada: incremento a la derecha.
	LCDsendCommand(0x0C); // Encender display, apagar cursor.
	
	// Inicialización de caracteres personalizados en CGRAM.
	uint8_t ch=0, chn=0;
	while(ch<64)
	{
		LCDdefinechar((LcdCustomChar+ch),chn++);
		ch=ch+8;
	}

#else
	// Interfaz LCD de 8 bits.
	_delay_ms(40);
	LDP=0x00;
	LCP &= ~((1<<LCD_E) | (1<<LCD_RS));
	LDDR|=1<<LCD_D7|1<<LCD_D6|1<<LCD_D5|1<<LCD_D4|1<<LCD_D3
			|1<<LCD_D2|1<<LCD_D1|1<<LCD_D0;
	LCDR |= (1<<LCD_E) | (1<<LCD_RS);
   //---------one------
	LDP=0<<LCD_D7|0<<LCD_D6|1<<LCD_D5|1<<LCD_D4|0<<LCD_D3
			|0<<LCD_D2|0<<LCD_D1|0<<LCD_D0; //8 it mode
	LCP|=1<<LCD_E|0<<LCD_RW|0<<LCD_RS;		
	_delay_us(1);
	LCP&=~(1<<LCD_E);
	_delay_ms(5); // Retardo según especificaciones.
	//-----------two-----------
	LDP=0<<LCD_D7|0<<LCD_D6|1<<LCD_D5|1<<LCD_D4|0<<LCD_D3
			|0<<LCD_D2|0<<LCD_D1|0<<LCD_D0; //8 it mode
	LCP|=1<<LCD_E|0<<LCD_RW|0<<LCD_RS;		
	_delay_us(1);
	LCP&=~(1<<LCD_E);
	_delay_us(150);
	//-------three-------------
	LDP=0<<LCD_D7|0<<LCD_D6|1<<LCD_D5|1<<LCD_D4|0<<LCD_D3
			|0<<LCD_D2|0<<LCD_D1|0<<LCD_D0; //8 it mode
	LCP|=1<<LCD_E|0<<LCD_RW|0<<LCD_RS;		
	_delay_us(1);
	LCP&=~(1<<LCD_E);
	_delay_us(50);
	// Modo 8 bits, 2 líneas.
	LDP=0<<LCD_D7|0<<LCD_D6|1<<LCD_D5|1<<LCD_D4|1<<LCD_D3
			|0<<LCD_D2|0<<LCD_D1|0<<LCD_D0; //8 it mode
	LCP|=1<<LCD_E|0<<LCD_RW|0<<LCD_RS;		
	_delay_us(1);
	LCP&=~(1<<LCD_E);
	_delay_us(50);
   // Incremento de dirección, desplazamiento invisible de cursor.
	LDP=0<<LCD_D7|0<<LCD_D6|0<<LCD_D5|0<<LCD_D4|1<<LCD_D3
			|1<<LCD_D2|0<<LCD_D1|0<<LCD_D0; //8 it mode
	LCP|=1<<LCD_E|0<<LCD_RW|0<<LCD_RS;		
	_delay_us(1);
	LCP&=~(1<<LCD_E);
	_delay_ms(2);
		// Inicialización de caracteres personalizados.
	uint8_t ch=0, chn=0;
	while(ch<64)
	{
		LCDdefinechar((LcdCustomChar+ch),chn++);
		ch=ch+8;
	}

#endif
}

// ==============================================================================
// Funciones de Formato y Posición
// ==============================================================================

/**
 * @brief Limpia toda la pantalla.
 */
void LCDclr(void)				
{
	LCDsendCommand(1<<LCD_CLR);
	_delay_ms(5); 
}

// Retorno de cursor a inicio.
void LCDhome(void)			
{
	LCDsendCommand(1<<LCD_HOME);
	_delay_ms(5); 
}

// Transmisión de cadena de texto a LCD.
void LCDstring(uint8_t* data, uint8_t nBytes)	
{
register uint8_t i;

	// check to make sure we have a good pointer
	if (!data) return;
	// print data
	for(i=0; i<nBytes; i++)
	{
		LCDsendChar(data[i]);
	}
}

// Posicionamiento de cursor en coordenadas (X, Y).
void LCDGotoXY(uint8_t x, uint8_t y)	
{
	register uint8_t DDRAMAddr;
	// remap lines into proper order
	switch(y)
	{
	case 0: DDRAMAddr = LCD_LINE0_DDRAMADDR+x; break;
	case 1: DDRAMAddr = LCD_LINE1_DDRAMADDR+x; break;
	case 2: DDRAMAddr = LCD_LINE2_DDRAMADDR+x; break;
	case 3: DDRAMAddr = LCD_LINE3_DDRAMADDR+x; break;
	default: DDRAMAddr = LCD_LINE0_DDRAMADDR+x;
	}
	// set data address
	LCDsendCommand(1<<LCD_DDRAM | DDRAMAddr);
	
}

// Copia cadena desde memoria flash a LCD en posición (x, y).
void CopyStringtoLCD(const uint8_t *FlashLoc, uint8_t x, uint8_t y)
{
	uint8_t i;
	LCDGotoXY(x,y);
	for(i=0;(uint8_t)pgm_read_byte(&FlashLoc[i]);i++)
	{
		LCDsendChar((uint8_t)pgm_read_byte(&FlashLoc[i]));
	}
}

// Define símbolo en CGRAM.
void LCDdefinechar(const uint8_t *pc,uint8_t char_code){
	uint8_t a, pcc;
	uint16_t i;
	a=(char_code<<3)|0x40;
	for (i=0; i<8; i++){
		pcc=pgm_read_byte(&pc[i]);
		LCDsendCommand(a++);
		LCDsendChar(pcc);
		}
}

// Desplazamiento de n caracteres a la izquierda.
void LCDshiftLeft(uint8_t n)	
{
	for (uint8_t i=0;i<n;i++)
	{
		LCDsendCommand(0x1E);
	}
}

// Desplazamiento de n caracteres a la derecha.
void LCDshiftRight(uint8_t n)	
{
	for (uint8_t i=0;i<n;i++)
	{
		LCDsendCommand(0x18);
	}
}

// Muestra cursor de LCD.
void LCDcursorOn(void) 
{
	LCDsendCommand(0x0E);
}

// Muestra cursor parpadeante en LCD.
void LCDcursorOnBlink(void)	
{
	LCDsendCommand(0x0F);
}

// Apaga cursor.
void LCDcursorOFF(void)	
{
	LCDsendCommand(0x0C);
}

// ==============================================================================
// Funciones Adicionales
// ==============================================================================

// Apaga visualización en LCD.
void LCDblank(void)		
{
	LCDsendCommand(0x08);
}

// Enciende visualización en LCD.
void LCDvisible(void)		
{
	LCDsendCommand(0x0C);
}

// Mueve cursor n posiciones a la izquierda.
void LCDcursorLeft(uint8_t n)	
{
	for (uint8_t i=0;i<n;i++)
	{
		LCDsendCommand(0x10);
	}
}

// Mueve cursor n posiciones a la derecha.
void LCDcursorRight(uint8_t n)	
{
	for (uint8_t i=0;i<n;i++)
	{
		LCDsendCommand(0x14);
	}
}

// Escritura de dato entero en el módulo LCD.
void LCDescribeDato(int val,unsigned int field_length)
{
	char str[5]={0,0,0,0,0};
	int i=4,j=0;
	while(val)
	{
	str[i]=val%10;
	val=val/10;
	i--;
	}
	if(field_length==-1)
		while(str[j]==0) j++;
	else
		j=5-field_length;

	if(val<0) LCDsendChar('-');
	for(i=j;i<5;i++)
	{
	LCDsendChar(48+str[i]);
	}
}

void LCDprogressBar(uint8_t progress, uint8_t maxprogress, uint8_t length)
{
	uint8_t i;
	uint16_t pixelprogress;
	uint8_t c;

	// Dibuja barra de progreso desde la posición actual del cursor.

	// Longitud de píxeles de la barra.
	pixelprogress = ((progress*(length*PROGRESSPIXELS_PER_CHAR))/maxprogress);
	
	// Imprime caracteres.
	for(i=0; i<length; i++)
	{
		// Verificación de tipo de bloque (completo, parcial o vacío).
		if( ((i*(uint16_t)PROGRESSPIXELS_PER_CHAR)+5) > pixelprogress )
		{
			// Bloque parcial o vacío.
			if( ((i*(uint16_t)PROGRESSPIXELS_PER_CHAR)) > pixelprogress )
			{
				// Bloque vacío.
				c = 0;
			}
			else
			{
				// Bloque parcial.
				c = pixelprogress % PROGRESSPIXELS_PER_CHAR;
			}
		}
		else
		{
			// Bloque completo.
			c = 5;
		}
		
		// Escritura de caracter.
		LCDsendChar(c);
	}	
}

/*
void LCD_Update(){
	char Temp_string[] = "Temp: 00.0 C"; //Preparo la cadena de string a mostrar en el LCD
	char CHARMAP[10]={'0','1','2','3','4','5','6','7','8','9'};
	Temp_string[6] = CHARMAP[temp/100] ; // Obtengo la decena correspondiente al valor de temperatura
	Temp_string[7] = CHARMAP[(temp/10)%10]; // Obtengo la unidad correspondiente al valor de temperatura
	Temp_string[9] = CHARMAP[temp%10]; // Obtengo el primer decimal correspondiente al valor de temperatura
	LCDstring(Temp_string, 12); //Muestro el mensaje en el LCD con la temperatura indicada 
	LCDhome();
}
*/


// ==============================================================================
// Funciones de Alto Nivel (Lógica de Aplicación)
// ==============================================================================


/**
 * @brief Secuencia principal de inicializacion del LCD.
 */
void LCD_Init() {
	LCDinit();
	LCDclr();
	LCDhome();
	LCDGotoXY(0,0);
}






/**
 * @brief Convierte un tiempo total en segundos al formato "MM:SS" y lo imprime.
 * @param seg Tiempo total en segundos.
 */
void LCD_PrintTime(uint16_t seg) {
    uint8_t minutos = seg / 60;
    uint8_t segundos = seg % 60;
    
    char buffer[6]; // Cadena "MM:SS".
    // Formateo a dos dígitos.
    buffer[0] = (minutos / 10) + '0';
    buffer[1] = (minutos % 10) + '0';
    buffer[2] = ':';
    buffer[3] = (segundos / 10) + '0';
    buffer[4] = (segundos % 10) + '0';
    buffer[5] = '\0';
    
    LCDGotoXY(0, 0); // Posicionamiento en primera fila.
    LCDstring((uint8_t*)buffer, 5);
}

/**
 * @brief Enciende o apaga el display completamente.
 * @param estado 0 = apaga el display, 1 = lo enciende.
 */
void LCD_Parpadear(uint8_t estado) {
    if (estado == 0) {
        LCDblank();     // Comando 0x08
    } else {
        LCDvisible();   // Comando 0x0C
    }
}