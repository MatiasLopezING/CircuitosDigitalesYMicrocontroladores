/**
 * @file    i2c.c
 * @brief   Implementacion del driver I2C (TWI) master bloqueante para ATmega328P.
 */
#include "i2c.h"

/* Espera a que el hardware TWI complete la operacion en curso (TWINT = 1). */
#define TWI_WAIT()  while (!(TWCR & (1 << TWINT)))

/* Lee el codigo de estado TWI descartando los bits de prescaler. */
#define TWI_STATUS  (TWSR & 0xF8)

/*
  @brief   Inicializa el periferico TWI a 100 kHz con prescaler = 1.
 */
void i2c_init(void) {
	TWSR = 0x00;            /* prescaler = 1                */
	TWBR = I2C_TWBR_100K;  /* velocidad 100 kHz            */
	TWCR = (1 << TWEN);    /* habilita el periferico TWI   */
}

/*
  @brief   Genera la condicion START y envia la direccion del esclavo con el bit R/W.

  @param   addr_rw  Direccion del esclavo desplazada 1 bit a la izquierda con el bit R/W
                    en el LSB: (addr << 1) | 0 para escritura, | 1 para lectura.
  @return  1 si el esclavo respondio con ACK, 0 si ocurrio un error de bus.
 */
uint8_t i2c_start(uint8_t addr_rw) {
	// Generar START: limpiar TWINT, setear TWSTA, habilitar TWEN
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	TWI_WAIT();

	if (TWI_STATUS != TWI_START && TWI_STATUS != TWI_REP_START)
	return 0;

	// Enviar direcci�n + R/W
	TWDR = addr_rw;
	TWCR = (1 << TWINT) | (1 << TWEN);
	TWI_WAIT();

	uint8_t status = TWI_STATUS;
	if (status != TWI_MT_SLA_ACK && status != TWI_MR_SLA_ACK)
	return 0;

	return 1;
}

/*
  @brief   Genera la condicion STOP y libera el bus I2C.
 */
void i2c_stop(void) {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	/* No se genera TWINT despues del STOP; esperar hasta que el hardware limpie TWSTO. */
	while (TWCR & (1 << TWSTO));
}

/*
  @brief   Envia un byte de datos y espera la confirmacion del esclavo.

  @param   data  Byte a transmitir.
  @return  1 si el esclavo respondio con ACK, 0 si respondio con NACK o hubo error.
 */
uint8_t i2c_write(uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	TWI_WAIT();
	return (TWI_STATUS == TWI_MT_DATA_ACK) ? 1 : 0;
}

/*
  @brief   Lee un byte del esclavo y envia ACK (hay mas bytes por recibir).

  @return  Byte leido del bus.
 */
uint8_t i2c_read_ack(void) {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	TWI_WAIT();
	return TWDR;
}

/*
  @brief   Lee el ultimo byte del esclavo y envia NACK para finalizar la lectura.

  @return  Byte leido del bus.
 */
uint8_t i2c_read_nack(void) {
	TWCR = (1 << TWINT) | (1 << TWEN);
	TWI_WAIT();
	return TWDR;
}


/*
  @brief   Escribe un byte en un registro del esclavo (START -> SLA+W -> reg -> data -> STOP).

  @param   dev_addr  Direccion I2C del dispositivo (7 bits sin desplazar).
  @param   reg       Direccion del registro destino.
  @param   data      Valor a escribir.
  @return  1 si la operacion fue exitosa, 0 si hubo error de bus.
 */
uint8_t i2c_write_reg(uint8_t dev_addr, uint8_t reg, uint8_t data) {
	if (!i2c_start((dev_addr << 1) | 0)) return 0;  // SLA+W
	if (!i2c_write(reg))                 return 0;
	if (!i2c_write(data))                return 0;
	i2c_stop();
	return 1;
}

/*
  @brief   Lee un byte de un registro del esclavo (START -> SLA+W -> reg -> RESTART -> SLA+R -> data -> STOP).

  @param   dev_addr  Direccion I2C del dispositivo (7 bits sin desplazar).
  @param   reg       Direccion del registro a leer.
  @param   data      Puntero donde se almacena el byte leido.
  @return  1 si la operacion fue exitosa, 0 si hubo error de bus.
 */
uint8_t i2c_read_reg(uint8_t dev_addr, uint8_t reg, uint8_t *data) {
	if (!i2c_start((dev_addr << 1) | 0)) return 0;  // SLA+W
	if (!i2c_write(reg))                 return 0;
	if (!i2c_start((dev_addr << 1) | 1)) return 0;  // repeated START + SLA+R
	*data = i2c_read_nack();                         // �nico byte ? NACK
	i2c_stop();
	return 1;
}

/*
  @brief   Lee N registros consecutivos del esclavo a partir de una direccion base.

  Envia ACK en todos los bytes intermedios y NACK en el ultimo para cerrar la lectura.

  @param   dev_addr  Direccion I2C del dispositivo (7 bits sin desplazar).
  @param   reg       Registro inicial de la rafaga.
  @param   buf       Buffer destino de al menos len bytes.
  @param   len       Cantidad de bytes a leer.
  @return  1 si la operacion fue exitosa, 0 si hubo error de bus.
 */
uint8_t i2c_read_burst(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len) {
	if (!i2c_start((dev_addr << 1) | 0)) return 0;
	if (!i2c_write(reg))                 return 0;
	if (!i2c_start((dev_addr << 1) | 1)) return 0;

	for (uint8_t i = 0; i < len - 1; i++)
	buf[i] = i2c_read_ack();   // ACK en todos menos el �ltimo

	buf[len - 1] = i2c_read_nack(); // NACK en el �ltimo
	i2c_stop();
	return 1;
}