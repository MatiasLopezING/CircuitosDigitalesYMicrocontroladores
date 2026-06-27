
#include "i2c.h"


/* Macro interna: espera a que el hardware TWI termine una operacion */
#define TWI_WAIT()  while (!(TWCR & (1 << TWINT)))
#define TWI_STATUS  (TWSR & 0xF8)   // Mascara que quita los bits de prescaler

void i2c_init(void) {
	TWSR = 0x00;            // prescaler = 1
	TWBR = I2C_TWBR_100K;  // velocidad 100kHz
	TWCR = (1 << TWEN);    // habilita el periférico TWI
}

// Envía condición START + dirección + bit R/W
// Retorna 1 si OK, 0 si error
uint8_t i2c_start(uint8_t addr_rw) {
	// Generar START: limpiar TWINT, setear TWSTA, habilitar TWEN
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	TWI_WAIT();

	if (TWI_STATUS != TWI_START && TWI_STATUS != TWI_REP_START)
	return 0;

	// Enviar dirección + R/W
	TWDR = addr_rw;
	TWCR = (1 << TWINT) | (1 << TWEN);
	TWI_WAIT();

	uint8_t status = TWI_STATUS;
	if (status != TWI_MT_SLA_ACK && status != TWI_MR_SLA_ACK)
	return 0;

	return 1;
}

void i2c_stop(void) {
	// TWSTO genera la condición STOP, TWINT limpia el flag
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	// No hay TWINT después del STOP, el hardware lo maneja solo
	// Pequeña espera para asegurarse que el bus quede libre
	while (TWCR & (1 << TWSTO));
}

// Escribe un byte, retorna 1 si recibió ACK
uint8_t i2c_write(uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	TWI_WAIT();
	return (TWI_STATUS == TWI_MT_DATA_ACK) ? 1 : 0;
}

// Lee un byte y envía ACK (hay más bytes por venir)
uint8_t i2c_read_ack(void) {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	TWI_WAIT();
	return TWDR;
}

// Lee un byte y envía NACK (es el último byte)
uint8_t i2c_read_nack(void) {
	TWCR = (1 << TWINT) | (1 << TWEN);
	TWI_WAIT();
	return TWDR;
}


// Escribe un registro: START ? addr+W ? reg ? data ? STOP
uint8_t i2c_write_reg(uint8_t dev_addr, uint8_t reg, uint8_t data) {
	if (!i2c_start((dev_addr << 1) | 0)) return 0;  // SLA+W
	if (!i2c_write(reg))                 return 0;
	if (!i2c_write(data))                return 0;
	i2c_stop();
	return 1;
}

// Lee un registro: START ? addr+W ? reg ? RESTART ? addr+R ? data ? STOP
uint8_t i2c_read_reg(uint8_t dev_addr, uint8_t reg, uint8_t *data) {
	if (!i2c_start((dev_addr << 1) | 0)) return 0;  // SLA+W
	if (!i2c_write(reg))                 return 0;
	if (!i2c_start((dev_addr << 1) | 1)) return 0;  // repeated START + SLA+R
	*data = i2c_read_nack();                         // único byte ? NACK
	i2c_stop();
	return 1;
}

// Lee N registros consecutivos a partir de reg
uint8_t i2c_read_burst(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len) {
	if (!i2c_start((dev_addr << 1) | 0)) return 0;
	if (!i2c_write(reg))                 return 0;
	if (!i2c_start((dev_addr << 1) | 1)) return 0;

	for (uint8_t i = 0; i < len - 1; i++)
	buf[i] = i2c_read_ack();   // ACK en todos menos el último

	buf[len - 1] = i2c_read_nack(); // NACK en el último
	i2c_stop();
	return 1;
}