/*
 * dht11.h
 *
 * Driver para sensor de temperatura y humedad DHT11.
 * Comunicacion por protocolo 1-wire single-bus en PC0.
 *
 * Implementacion bloqueante: dht11_read() deshabilita interrupciones
 * globales durante la lectura (~22ms) para garantizar timing preciso.
 */

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

/*
 * dht11_read - Lee temperatura y humedad del sensor.
 *
 * Parametros:
 *   temp  - puntero donde se almacena la temperatura en grados Celsius
 *   hum   - puntero donde se almacena la humedad relativa en porcentaje
 *
 * Retorna:
 *   DHT11_OK      si la lectura fue exitosa (temp y hum son validos)
 *   DHT11_TIMEOUT si el sensor no respondio (temp y hum no se modifican)
 *   DHT11_CHKSUM  si el checksum fallo    (temp y hum no se modifican)
 *
 * Nota: esta funcion es bloqueante ~22ms con interrupciones deshabilitadas.
 */
dht11_status_t dht11_read(uint8_t *temp, uint8_t *hum);

#endif /* DHT11_H_ */
