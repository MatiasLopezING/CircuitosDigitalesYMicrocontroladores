/**
 * @file    ds3231.c
 * @brief   Implementacion del driver para el RTC DS3231 via I2C.
 */
#include "ds3231.h"
#include "i2c.h"

/* Conversion decimal -> BCD (empacado). */
static uint8_t dec2bcd(uint8_t val) { return ((val / 10) << 4) | (val % 10); }

/* Conversion BCD (empacado) -> decimal. */
static uint8_t bcd2dec(uint8_t val) { return ((val >> 4) * 10) + (val & 0x0F); }

/*
  @brief   Inicializa el DS3231 forzando el modo de 24 horas.

  Lee el registro de horas, limpia el bit 6 (modo 12h/24h) y lo reescribe
  para garantizar modo 24 h sin alterar los demas bits del registro.
 */
void ds3231_init(void) {
	uint8_t hour_reg;
	if (i2c_read_reg(DS3231_ADDR, DS3231_REG_HOUR, &hour_reg)) {
		hour_reg &= ~(1 << 6);  /* bit 6 = 0 -> modo 24 h */
		i2c_write_reg(DS3231_ADDR, DS3231_REG_HOUR, hour_reg);
	}
}

/*
  @brief   Lee la hora actual del DS3231 en una sola rafaga de 3 bytes.

  Los valores BCD del chip se convierten a decimal antes de almacenarse.

  @param   t  Puntero a la estructura donde se escriben segundos, minutos y horas.
  @return  1 si la lectura fue exitosa, 0 si hubo error de bus.
 */
uint8_t ds3231_getTime(type_rtcTime *t) {
	uint8_t buf[3];
	if (!i2c_read_burst(DS3231_ADDR, DS3231_REG_SEC, buf, 3))
	return 0;

	t->seconds = bcd2dec(buf[0] & 0x7F);  // bit 7 es CH (clock halt), ignorar
	t->minutes = bcd2dec(buf[1] & 0x7F);
	t->hours   = bcd2dec(buf[2] & 0x3F);  // bits 5:0 en modo 24h
	return 1;
}

/*
  @brief   Escribe la hora en el DS3231 mediante una rafaga de 3 bytes.

  Valida que los valores esten en rango antes de escribir.
  Secuencia: START -> SLA+W -> REG_SEC -> seg -> min -> horas -> STOP.

  @param   t  Puntero a la estructura con la hora a configurar.
  @return  1 si la escritura fue exitosa, 0 si los valores estan fuera de rango o hubo error.
 */
uint8_t ds3231_setTime(const type_rtcTime *t) {
	/* Validacion de rangos */
	if (t->hours > 23 || t->minutes > 59 || t->seconds > 59)
	return 0;

	/* Escritura en rafaga: START -> addr -> reg_base -> seg -> min -> horas -> STOP */
	if (!i2c_start((DS3231_ADDR << 1) | 0)) return 0;
	if (!i2c_write(DS3231_REG_SEC))         return 0;
	if (!i2c_write(dec2bcd(t->seconds)))    return 0;
	if (!i2c_write(dec2bcd(t->minutes)))    return 0;
	if (!i2c_write(dec2bcd(t->hours)))      return 0;  // bit 6 = 0 ? 24h
	i2c_stop();
	return 1;
}

/*
  @brief   Indica si la hora actual corresponde al periodo diurno (07:00 - 18:59).

  @param   t  Puntero a la estructura con la hora actual.
  @return  1 si es periodo diurno, 0 si es nocturno.
 */
uint8_t ds3231_isDaytime(const type_rtcTime *t) {
	return (t->hours >= 7 && t->hours <= 18);
}