/*
 * parser.c
 *
 * Created: 6/25/2026 10:28:00 PM
 *  Author: tomas
 */ 
#include "parser.h"

static type_statusCmd parser_setTime(const char *cmd, type_Data *data);
static type_statusCmd parser_setPeriodo(const char *cmd, type_Data *data);
static uint8_t esDigito(char c);
 //Retorna type_statusCmd segun los enums definidos para cada caso en parser.h.
 //Devuelve en tipo el tipo de comando (type_Cmd) y en data la informacion correspondiente segun el tipo de comando
type_statusCmd parser_parsearCmd(const char *cmd, type_Cmd *tipo, type_Data *data)
{
	// Ignoro espacios al principio 
	
	while (*cmd == ' ')
		cmd++;

	if (strncmp(cmd, "SET_TIME=", 9) == 0)
	{
		*tipo = CMD_SET_TIME;
		return parser_setTime(&cmd[9], data);
	}

	if (strncmp(cmd, "SET_T=", 6) == 0)
	{
		*tipo = CMD_SET_T;
		return parser_setPeriodo(&cmd[6], data);
	}

	return PARSER_CMD_INVALID;
}
	
static type_statusCmd parser_setTime(const char *cmd, type_Data *data)
{
    uint8_t hora;
    uint8_t minutos;
    uint8_t segundos;

    //Longitud exacta: SET_TIME=HH:MM:SS
 
    if (strlen(cmd) != 8) //Strlen no incluye el \0, por eso 8
        return PARSER_FORMAT_INVALID;

    //Verifico formato 

    if (!esDigito(cmd[0]) || !esDigito(cmd[1]))
        return PARSER_FORMAT_INVALID;

    if (cmd[2] != ':')
        return PARSER_FORMAT_INVALID;

    if (!esDigito(cmd[3]) || !esDigito(cmd[4]))
        return PARSER_FORMAT_INVALID;

    if (cmd[5] != ':')
        return PARSER_FORMAT_INVALID;

    if (!esDigito(cmd[6]) || !esDigito(cmd[7]) )
        return PARSER_FORMAT_INVALID;


    // Conversion de datos 

    hora = (cmd[0]-'0')*10 + (cmd[1]-'0');
    minutos = (cmd[3]-'0')*10 + (cmd[4]-'0');
    segundos = (cmd[6]-'0')*10 + (cmd[7]-'0');

    // Verifico rangos 

    if (hora > 23 || minutos > 59 || segundos > 59)
        return PARSER_RANGE_ERROR;

    // Copio a la union

    data->hora.hora = hora;
    data->hora.minutos = minutos;
    data->hora.segundos = segundos;

    return PARSER_OK;
}

static type_statusCmd parser_setPeriodo(const char *cmd, type_Data *data)
{
	uint16_t T = 0;
	uint8_t i, len=strlen(cmd);
	
	if (len == 0 || len > 2)  //Debe existir al menos un digito y no puede haber mas de 2 digitos asi como tampoco otros caracteres -> Decision de modelado, despues de ingresar SET_T=T se debe darle a enter
		return PARSER_FORMAT_INVALID;

	for (i=0 ; cmd[i] != '\0' ; i++) //Proceso hasta el fin de la cadena lo que el usuario haya ingresado como T, podria ser 1 o 2 chars como maximo
	{
		if (!esDigito(cmd[i]))
		return PARSER_FORMAT_INVALID;

		T *= 10;
		T += cmd[i]-'0';
	}

	if (T < 2 || T > 60)
		return PARSER_RANGE_ERROR;

	data->periodo.periodoT = T;

	return PARSER_OK;
}

static uint8_t esDigito(char c)
{
	return (c >= '0' && c <= '9');
}

