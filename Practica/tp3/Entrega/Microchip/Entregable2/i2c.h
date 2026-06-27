/**
 * @file    i2c.h
 * @brief   Driver I2C (TWI) master bloqueante para ATmega328P a 100 kHz.
 *
 * Expone primitivas de bus (start/stop/write/read) y funciones de alto nivel
 * para leer y escribir registros de dispositivos esclavos.
 */

#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <avr/io.h>
#include <util/twi.h>

/* Valor de TWBR para 100 kHz con F_CPU = 16 MHz y prescaler = 1.
 * Formula: TWBR = ((F_CPU / F_SCL) - 16) / (2 * prescaler) = 72 */
#define I2C_TWBR_100K   72

/* Codigos de estado TWI relevantes (TWSR & 0xF8). */
#define TWI_START        0x08  /* Condicion START transmitida              */
#define TWI_REP_START    0x10  /* Condicion START repetida transmitida     */
#define TWI_MT_SLA_ACK   0x18  /* Master TX: SLA+W enviado, ACK recibido  */
#define TWI_MT_DATA_ACK  0x28  /* Master TX: dato enviado, ACK recibido   */
#define TWI_MR_SLA_ACK   0x40  /* Master RX: SLA+R enviado, ACK recibido  */
#define TWI_MR_DATA_ACK  0x50  /* Master RX: dato recibido, ACK enviado   */
#define TWI_MR_DATA_NACK 0x58  /* Master RX: dato recibido, NACK enviado  */

void    i2c_init(void);
uint8_t i2c_start(uint8_t addr_rw);        /* addr_rw = (addr << 1) | R/W  */
void    i2c_stop(void);
uint8_t i2c_write(uint8_t data);
uint8_t i2c_read_ack(void);
uint8_t i2c_read_nack(void);

uint8_t i2c_write_reg(uint8_t dev_addr, uint8_t reg, uint8_t data);
uint8_t i2c_read_reg(uint8_t dev_addr, uint8_t reg, uint8_t *data);
uint8_t i2c_read_burst(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len);

#endif /* I2C_H */