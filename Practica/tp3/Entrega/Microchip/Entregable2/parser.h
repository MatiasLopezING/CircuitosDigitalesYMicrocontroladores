/*
 * parser.h
 *
 * Created: 6/25/2026 10:28:11 PM
 *  Author: tomas
 */ 


#ifndef PARSER_H_
#define PARSER_H_

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "ds3231.h"
#define TELEMETRIA_LEN 50 // "[HH:MM:SS] T: XX°C | H: XX% | Estado: NORMAL\r\n"
#define ALERTA_LEN 80 //[ALERTA] [HH:MM:SS] Temperatura fuera de rango nocturno! Valor: XX°C\r\n

typedef enum {
	 CMD_SET_TIME,
	 CMD_SET_T,
	 //CMD_INVALID,
} type_Cmd;

typedef struct {
	uint8_t hora;
	uint8_t minutos;
	uint8_t segundos;
} type_DataHora;

typedef struct {
	uint16_t periodoT;
} type_DataPeriodo;

typedef union {
	type_DataHora hora;
	type_DataPeriodo periodo;
} type_Data;

typedef enum {
	ESTADO_NORMAL = 0,
	ESTADO_ALERTA_TEMP,
	ESTADO_ALERTA_HUM,
} type_Estado;

typedef enum {
	VENTANA_DIA = 0,
	VENTANA_NOCHE
} type_VentanaHor;

typedef enum {PARSER_OK,PARSER_CMD_INVALID,PARSER_FORMAT_INVALID,PARSER_RANGE_ERROR,} type_statusCmd;

type_statusCmd parser_parsearCmd(const char *cmd, type_Cmd *tipo, type_Data *data);
void parser_getTelemetria(char *buf, const type_rtcTime *hora,uint8_t temp, uint8_t hum, type_Estado estado);
type_Estado parser_getEstado(type_VentanaHor ventana, uint8_t temp, uint8_t hum);
void parser_getAlerta(char *buf, const type_rtcTime *hora,type_VentanaHor ventana, type_Estado estado, uint8_t temp, uint8_t hum);
type_VentanaHor parser_getVentana(const type_rtcTime *t);
#endif /* PARSER_H_ */