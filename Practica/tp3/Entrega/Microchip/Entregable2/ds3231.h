#ifndef DS3231_H
#define DS3231_H

#include <stdint.h>

#define DS3231_ADDR     0x68    // dirección I2C fija del DS3231

// Registros
#define DS3231_REG_SEC  0x00
#define DS3231_REG_MIN  0x01
#define DS3231_REG_HOUR 0x02    // bit 6 = 0 ? modo 24h

typedef struct {
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
} type_rtcTime;

void    ds3231_init(void);
uint8_t ds3231_getTime(type_rtcTime *t);
uint8_t ds3231_setTime(const type_rtcTime *t);

// Devuelve 1 si la hora es diurna (07:00 - 18:59)
uint8_t ds3231_isDaytime(const type_rtcTime *t);

#endif