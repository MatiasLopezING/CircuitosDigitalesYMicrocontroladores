/**
 * @file    ds3231.h
 * @brief   Driver para el RTC DS3231 via I2C (TWI), modo 24 horas.
 *
 * Expone funciones para inicializar el chip, leer y escribir la hora,
 * y consultar si la hora actual corresponde al periodo diurno.
 */

#ifndef DS3231_H
#define DS3231_H

#include <stdint.h>

#define DS3231_ADDR     0x68  /* Direccion I2C fija del DS3231 (7 bits)        */
#define DS3231_REG_SEC  0x00  /* Registro de segundos (BCD, bit 7 = CH)        */
#define DS3231_REG_MIN  0x01  /* Registro de minutos  (BCD)                    */
#define DS3231_REG_HOUR 0x02  /* Registro de horas    (BCD, bit 6 = 0: 24h)   */

/* Estructura que representa la hora leida/escrita en el DS3231. */
typedef struct {
	uint8_t seconds;  /* Segundos: 0-59 */
	uint8_t minutes;  /* Minutos:  0-59 */
	uint8_t hours;    /* Horas:    0-23 */
} type_rtcTime;

void    ds3231_init(void);
uint8_t ds3231_getTime(type_rtcTime *t);
uint8_t ds3231_setTime(const type_rtcTime *t);
uint8_t ds3231_isDaytime(const type_rtcTime *t);

#endif /* DS3231_H */