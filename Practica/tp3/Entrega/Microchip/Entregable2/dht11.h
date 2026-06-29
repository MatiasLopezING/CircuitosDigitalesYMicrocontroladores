

#ifndef DHT11_H_
#define DHT11_H_

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

/* Pin donde esta conectado el DHT11 (PC0, con pull-up externo de 10k) */
#define DHT11_DDR   DDRC
#define DHT11_PORT  PORTC
#define DHT11_PIN   PINC
#define DHT11_BIT   PC0

/* Codigos de retorno de dht11_read() */
typedef enum {
    DHT11_OK      = 0,   /* Lectura exitosa                        */
    DHT11_TIMEOUT = 1,   /* El sensor no respondio a tiempo        */
    DHT11_CHKSUM  = 2,   /* Checksum incorrecto (dato corrompido)  */
} dht11_status_t;


 
dht11_status_t dht11_read(uint8_t *temp, uint8_t *hum);

#endif /* DHT11_H_ */
