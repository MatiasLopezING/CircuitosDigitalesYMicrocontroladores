/*
 * i2c.h
 *
 * Created: 6/24/2026 3:09:13 AM
 *  Author: tomas
 */ 

#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <avr/io.h>
#include <util/twi.h>
// Frecuencia I2C: 100kHz con F_CPU = 16MHz
// TWBR = ((F_CPU / F_SCL) - 16) / (2 * prescaler) = ((16000000/100000) - 16) / 2 = 72
#define I2C_TWBR_100K   72

void    i2c_init(void);
uint8_t i2c_start(uint8_t addr_rw);   // addr_rw = (addr << 1) | R/W
void    i2c_stop(void);
uint8_t i2c_write(uint8_t data);
uint8_t i2c_read_ack(void);
uint8_t i2c_read_nack(void);


uint8_t i2c_write_reg(uint8_t dev_addr, uint8_t reg, uint8_t data);
uint8_t i2c_read_reg(uint8_t dev_addr, uint8_t reg, uint8_t *data);
uint8_t i2c_read_burst(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len);

// Códigos de estado TWI relevantes
#define TWI_START           0x08
#define TWI_REP_START       0x10
#define TWI_MT_SLA_ACK      0x18  // Master Transmit: SLA+W, ACK recibido
#define TWI_MT_DATA_ACK     0x28  // Master Transmit: data, ACK recibido
#define TWI_MR_SLA_ACK      0x40  // Master Receive: SLA+R, ACK recibido
#define TWI_MR_DATA_ACK     0x50  // Master Receive: data, ACK recibido
#define TWI_MR_DATA_NACK    0x58  // Master Receive: data, NACK enviado

#endif