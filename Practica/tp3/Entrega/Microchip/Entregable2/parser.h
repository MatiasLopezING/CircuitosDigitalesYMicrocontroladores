
#ifndef PARSER_H_
#define PARSER_H_

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "ds3231.h"

/* Longitud maxima del buffer de telemetria: "[HH:MM:SS] T: XX C | H: XX% | Estado: NORMAL\r\n\0" */
#define TELEMETRIA_LEN 50

/* Longitud maxima del buffer de alerta: "[ALERTA] [HH:MM:SS] Temp y Humedad fuera de rango nocturno! T: 35 C H: 90%\0" */
#define ALERTA_LEN 80

/* Tipo de comando recibido por la terminal. */
typedef enum {
	CMD_SET_TIME,   /* Actualizar hora del RTC: SET_TIME=HH:MM:SS */
	CMD_SET_T,      /* Actualizar periodo de reporte: SET_T=T      */
} type_Cmd;

/* Datos de hora para CMD_SET_TIME. */
typedef struct {
	uint8_t hora;
	uint8_t minutos;
	uint8_t segundos;
} type_DataHora;

/* Datos de periodo para CMD_SET_T. */
typedef struct {
	uint16_t periodoT;
} type_DataPeriodo;

/* Union que agrupa los datos de cualquier tipo de comando. */
typedef union {
	type_DataHora    hora;
	type_DataPeriodo periodo;
} type_Data;

/* Estado actual del invernadero segun los umbrales de la ventana horaria. */
typedef enum {
	ESTADO_NORMAL= 0,  /* Temperatura y humedad dentro del rango */
	ESTADO_ALERTA_TEMP,      /* Temperatura fuera de rango*/
	ESTADO_ALERTA_HUM,       /* Humedad fuera de rango  */
	ESTADO_ALERTA_TEMP_HUM,  /* Temperatura y humedad fuera del rango */
} type_Estado;

/* Ventana horaria activa segun la hora del RTC. */
typedef enum {
	VENTANA_DIA=0,  /* 07:00 - 18:59 */
	VENTANA_NOCHE,  /* 19:00 - 06:59 */
} type_VentanaHor;

/* Resultado del parseo de un comando. */
typedef enum {
	PARSER_OK,              /* Comando valido y datos en rango         */
	PARSER_CMD_INVALID,     /* Comando desconocido                     */
	PARSER_FORMAT_INVALID,  /* Comando conocido pero formato incorrecto */
	PARSER_RANGE_ERROR,     /* Formato correcto pero valor fuera de rango */
} type_statusCmd;

type_statusCmd  parser_parsearCmd(const char *cmd, type_Cmd *tipo, type_Data *data);
void            parser_getTelemetria(char *buf, const type_rtcTime *hora, uint8_t temp, uint8_t hum, type_Estado estado);
type_Estado     parser_getEstado(type_VentanaHor ventana, uint8_t temp, uint8_t hum);
void            parser_getAlerta(char *buf, const type_rtcTime *hora, type_VentanaHor ventana, type_Estado estado, uint8_t temp, uint8_t hum);
type_VentanaHor parser_getVentana(const type_rtcTime *t);

#endif /* PARSER_H_ */