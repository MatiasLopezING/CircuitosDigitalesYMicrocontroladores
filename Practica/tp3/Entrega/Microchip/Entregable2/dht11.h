/**
 * @file    dht11.h
 * @brief   Driver bloqueante para el sensor DHT11 (temperatura y humedad) sobre PC0.
 *
 * Protocolo single-bus de 1 hilo con pull-up externo de 10 k.
 * dht11_read() deshabilita interrupciones globales (~22 ms) para garantizar
 * el timing preciso del protocolo.
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
  @brief   Lee temperatura y humedad del sensor DHT11 (bloqueante ~22 ms).

  Ejecuta el protocolo completo: start signal, handshake y recepcion de 40 bits.
  Deshabilita interrupciones globales durante toda la lectura con cli()/sei().

  @param   temp  Puntero donde se almacena la temperatura entera en grados Celsius.
  @param   hum   Puntero donde se almacena la humedad relativa entera en porcentaje.
  @return  DHT11_OK      Lectura exitosa; *temp y *hum son validos.
  @return  DHT11_TIMEOUT El sensor no respondio en el tiempo esperado.
  @return  DHT11_CHKSUM  Checksum incorrecto; dato descartado.
 */
dht11_status_t dht11_read(uint8_t *temp, uint8_t *hum);

#endif /* DHT11_H_ */
