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

typedef enum {PARSER_OK,PARSER_CMD_INVALID,PARSER_FORMAT_INVALID,PARSER_RANGE_ERROR,} type_statusCmd;

type_statusCmd parser_parsearCmd(const char *cmd, type_Cmd *tipo, type_Data *data);

#endif /* PARSER_H_ */