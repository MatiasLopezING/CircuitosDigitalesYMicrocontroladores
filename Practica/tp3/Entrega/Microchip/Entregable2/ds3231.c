#include "ds3231.h"
#include "i2c.h"

// ---------------------------------------------------------------------------
// BCD <-> decimal
// ---------------------------------------------------------------------------
static uint8_t dec2bcd(uint8_t val) { return ((val / 10) << 4) | (val % 10); }
static uint8_t bcd2dec(uint8_t val) { return ((val >> 4) * 10) + (val & 0x0F); }

void ds3231_init(void) {
	i2c_init();
	// Nos aseguramos que el bit 6 del registro de horas esté en 0 (modo 24h)
	// Leemos primero para no pisar otros bits
	uint8_t hour_reg;
	if (i2c_read_reg(DS3231_ADDR, DS3231_REG_HOUR, &hour_reg)) {
		hour_reg &= ~(1 << 6);  // forzar modo 24h
		i2c_write_reg(DS3231_ADDR, DS3231_REG_HOUR, hour_reg);
	}
}

// Lee segundos, minutos y horas en una sola ráfaga de 3 bytes
uint8_t ds3231_get_time(rtc_time_t *t) {
	uint8_t buf[3];
	if (!i2c_read_burst(DS3231_ADDR, DS3231_REG_SEC, buf, 3))
	return 0;

	t->seconds = bcd2dec(buf[0] & 0x7F);  // bit 7 es CH (clock halt), ignorar
	t->minutes = bcd2dec(buf[1] & 0x7F);
	t->hours   = bcd2dec(buf[2] & 0x3F);  // bits 5:0 en modo 24h
	return 1;
}

uint8_t ds3231_set_time(const rtc_time_t *t) {
	// Validación básica
	if (t->hours > 23 || t->minutes > 59 || t->seconds > 59)
	return 0;

	// Para escribir múltiples registros consecutivos: START ? addr ? reg_base ? d0 ? d1 ? d2 ? STOP
	if (!i2c_start((DS3231_ADDR << 1) | 0)) return 0;
	if (!i2c_write(DS3231_REG_SEC))         return 0;
	if (!i2c_write(dec2bcd(t->seconds)))    return 0;
	if (!i2c_write(dec2bcd(t->minutes)))    return 0;
	if (!i2c_write(dec2bcd(t->hours)))      return 0;  // bit 6 = 0 ? 24h
	i2c_stop();
	return 1;
}

uint8_t ds3231_is_daytime(const rtc_time_t *t) {
	return (t->hours >= 7 && t->hours <= 18);
}