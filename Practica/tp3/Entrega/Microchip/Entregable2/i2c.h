
#ifndef I2C_H
#define I2C_H

#include <stdint.h>
#include <avr/io.h>
#include <util/twi.h>


/* Codigos de estado TWI relevantes (TWSR & 0xF8). */
#define TWI_START        0x08  /* Condicion START transmitida              */
#define TWI_REP_START    0x10  /* Condicion START repetida transmitida     */
#define TWI_MT_SLA_ACK   0x18  /* Master TX: SLA+W enviado, ACK recibido  */
#define TWI_MT_DATA_ACK  0x28  /* Master TX: dato enviado, ACK recibido   */
#define TWI_MR_SLA_ACK   0x40  /* Master RX: SLA+R enviado, ACK recibido  */
#define TWI_MR_DATA_ACK  0x50  /* Master RX: dato recibido, ACK enviado   */
#define TWI_MR_DATA_NACK 0x58  /* Master RX: dato recibido, NACK enviado  */

void    i2c_init(void);
uint8_t i2c_start(uint8_t addr_rw);       
void    i2c_stop(void);
uint8_t i2c_write(uint8_t data);
uint8_t i2c_readAck(void);
uint8_t i2c_readNack(void);

uint8_t i2c_writeReg(uint8_t dev_addr, uint8_t reg, uint8_t data);
uint8_t i2c_readReg(uint8_t dev_addr, uint8_t reg, uint8_t *data);
uint8_t i2c_readBurst(uint8_t dev_addr, uint8_t reg, uint8_t *buf, uint8_t len);

#endif /* I2C_H */