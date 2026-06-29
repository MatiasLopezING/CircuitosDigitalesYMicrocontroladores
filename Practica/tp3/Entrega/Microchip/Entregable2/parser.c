/**
 * @file    parser.c
 * @brief   Implementacion del modulo de parseo de comandos y armado de mensajes.
 */
#include "parser.h"

/* Prototipos de funciones internas. */
static type_statusCmd parser_setTime(const char *cmd, type_Data *data);
static type_statusCmd parser_setPeriodo(const char *cmd, type_Data *data);
static uint8_t esDigito(char c);

/*
  @brief   Parsea un comando recibido por la terminal y extrae tipo y datos.

  Ignora espacios iniciales. Identifica "SET_TIME=" y "SET_T=" y delega
  la validacion de formato y rango a las funciones internas correspondientes.

  @param   cmd   String del comando terminado en '\0'.
  @param   tipo  Puntero donde se escribe el tipo de comando (type_Cmd).
  @param   data  Puntero donde se escriben los datos del comando (type_Data).
  @return  PARSER_OK              Si el comando es valido y los datos estan en rango.
  @return  PARSER_CMD_INVALID     Si el prefijo del comando no existe.
  @return  PARSER_FORMAT_INVALID  Si no cumple el formato establecido.
  @return  PARSER_RANGE_ERROR     Si el valor esta fuera del rango permitido.
 */
type_statusCmd parser_parsearCmd(const char *cmd, type_Cmd *tipo, type_Data *data)
{
	// Ignoro espacios al principio 
	
	while (*cmd == ' ')
		cmd++;

	/* Verificar primero la forma completa (con '=' y valor).
	 * El orden importa: SET_TIME= debe ir antes que SET_T= porque SET_T es prefijo de SET_TIME. */

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

	/* Comando reconocido pero sin '=' o sin valor: retornar FORMAT_INVALID
	 * en lugar de CMD_INVALID para dar un mensaje de error mas descriptivo.
	 * SET_TIME debe verificarse antes que SET_T para el mismo motivo de orden. */

	if (strncmp(cmd, "SET_TIME", 8) == 0)
	{
		*tipo = CMD_SET_TIME;
		return PARSER_FORMAT_INVALID;
	}

	if (strncmp(cmd, "SET_T", 5) == 0)
	{
		*tipo = CMD_SET_T;
		return PARSER_FORMAT_INVALID;
	}

	return PARSER_CMD_INVALID;
}
	
/*
  @brief   Valida y extrae hora, minutos y segundos del comando SET_TIME.

  Espera exactamente 8 caracteres con formato "HH:MM:SS".
  Verifica que los separadores sean ':' y que los valores esten en rango.

  @param   cmd   Puntero al string posterior a "SET_TIME=" (terminado en '\0').
  @param   data  Puntero donde se escriben los datos de hora extraidos.
  @return  PARSER_OK / PARSER_FORMAT_INVALID / PARSER_RANGE_ERROR.
 */
static type_statusCmd parser_setTime(const char *cmd, type_Data *data)
{
    uint8_t hora;
    uint8_t minutos;
    uint8_t segundos;

  
 
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

/*
  @brief   Valida y extrae el periodo T del subcomando SET_T.

  Acepta cualquier cantidad de digitos decimales. Da FORMAT_INVALID si no hay
  digitos o si algun caracter no es numerico. Da RANGE_ERROR si el valor
  esta fuera del rango permitido [2, 60], independientemente de cuantos digitos tenga.

  @param   cmd   Puntero al string posterior a "SET_T=" (terminado en '\0').
  @param   data  Puntero donde se escribe el periodo extraido.
  @return  PARSER_OK              Valor valido y en rango.
  @return  PARSER_FORMAT_INVALID  Cadena vacia o contiene caracteres no numericos.
  @return  PARSER_RANGE_ERROR     Valor numerico fuera del rango [2, 60].
 */
static type_statusCmd parser_setPeriodo(const char *cmd, type_Data *data)
{
	uint16_t T = 0;
	uint8_t i;

	/* Debe haber al menos un digito */
	if (cmd[0] == '\0')
		return PARSER_FORMAT_INVALID;

	for (i = 0; cmd[i] != '\0'; i++)
	{
		if (!esDigito(cmd[i]))
			return PARSER_FORMAT_INVALID;

		T = T * 10 + (uint16_t)(cmd[i] - '0');
	}

	/* Verificar rango: 2 a 60 segundos */
	if (T < 2 || T > 60)
		return PARSER_RANGE_ERROR;

	data->periodo.periodoT = T;
	return PARSER_OK;
}

/*
  @brief   Indica si un caracter es un digito ASCII ('0'..'9').

  @param   c  Caracter a evaluar.
  @return  1 si es digito, 0 en caso contrario.
 */
static uint8_t esDigito(char c)
{
	return (c >= '0' && c <= '9');
}

/*
  @brief   Arma el string de telemetria periodica en buf.

  Formato: "[HH:MM:SS] T: XX C | H: XX% | Estado: NORMAL/ALERTA"
  El buffer debe tener al menos TELEMETRIA_LEN bytes.

  @param   buf     Buffer destino.
  @param   hora    Hora actual leida del RTC.
  @param   temp    Temperatura en grados Celsius.
  @param   hum     Humedad relativa en porcentaje.
  @param   estado  Estado actual del invernadero.
 */
void parser_getTelemetria(char *buf, const type_rtcTime *hora,
uint8_t temp, uint8_t hum, type_Estado estado) {
	sprintf(buf, "[%02u:%02u:%02u] T: %u C | H: %u%% | Estado: %s",hora->hours,hora->minutes,hora->seconds,temp,hum,(estado == ESTADO_NORMAL) ? "NORMAL" : "ALERTA");
}

/*
  @brief   Determina el estado del invernadero segun la ventana horaria y los valores de sensor.
  Umbrales diurnos:  temperatura 20-30 C, humedad 50-70%.
  Umbrales nocturnos: temperatura 15-22 C, humedad 60-80%.

  @param   ventana  Ventana horaria activa (VENTANA_DIA o VENTANA_NOCHE).
  @param   temp     Temperatura medida en grados Celsius.
  @param   hum      Humedad relativa medida en porcentaje.
  @return  ESTADO_NORMAL, ESTADO_ALERTA_TEMP, ESTADO_ALERTA_HUM o ESTADO_ALERTA_TEMP_HUM.
 */
type_Estado parser_getEstado(type_VentanaHor ventana, uint8_t temp, uint8_t hum) {
    bool temp_fuera, hum_fuera;

    if (ventana == VENTANA_DIA) {
        temp_fuera = (temp < 20 || temp > 30);
        hum_fuera  = (hum  < 50 || hum  > 70);
    } else {
        temp_fuera = (temp < 15 || temp > 22);
        hum_fuera  = (hum  < 60 || hum  > 80);
    }

    if (temp_fuera && hum_fuera) return ESTADO_ALERTA_TEMP_HUM;
    if (temp_fuera)              return ESTADO_ALERTA_TEMP;
    if (hum_fuera)               return ESTADO_ALERTA_HUM;
    return ESTADO_NORMAL;
}

/*
  @brief   Arma el string de alerta en buf cuando el estado no es NORMAL.
  Para ESTADO_ALERTA_TEMP_HUM imprime ambos valores en el mismo mensaje.
  El buffer debe tener al menos ALERTA_LEN bytes.

  @param   buf      Buffer destino.
  @param   hora     Hora actual leida del RTC.
  @param   ventana  Ventana horaria activa.
  @param   estado   Estado de alerta (ESTADO_ALERTA_TEMP, ESTADO_ALERTA_HUM o ESTADO_ALERTA_TEMP_HUM).
  @param   temp     Temperatura medida en grados Celsius.
  @param   hum      Humedad relativa medida en porcentaje.
 */
void parser_getAlerta(char *buf, const type_rtcTime *hora, type_VentanaHor ventana, type_Estado estado, uint8_t temp, uint8_t hum) {
    const char *ventana_str = (ventana == VENTANA_DIA) ? "diurno" : "nocturno";

    if (estado == ESTADO_ALERTA_TEMP) {
        sprintf(buf, "[ALERTA] [%02u:%02u:%02u] Temperatura fuera de rango %s! Valor: %u C",
                hora->hours, hora->minutes, hora->seconds,
                ventana_str, (unsigned int)temp);

    } else if (estado == ESTADO_ALERTA_HUM) {
        sprintf(buf, "[ALERTA] [%02u:%02u:%02u] Humedad fuera de rango %s! Valor: %u%%",
                hora->hours, hora->minutes, hora->seconds,
                ventana_str, (unsigned int)hum);

    } else if (estado == ESTADO_ALERTA_TEMP_HUM) {
        sprintf(buf, "[ALERTA] [%02u:%02u:%02u] Temp y Humedad fuera de rango %s! T: %u C H: %u%%",
                hora->hours, hora->minutes, hora->seconds,
                ventana_str, (unsigned int)temp, (unsigned int)hum);
    }
}


/*
  @brief   Determina la ventana horaria activa a partir de la hora del RTC.

  Considera dia entre las 07:00 y las 18:59 (inclusive).

  @param   t  Hora actual leida del RTC.
  @return  VENTANA_DIA si 7 <= horas <= 18, VENTANA_NOCHE en caso contrario.
 */
type_VentanaHor parser_getVentana(const type_rtcTime *t) {
	return (t->hours >= 7 && t->hours <= 18) ? VENTANA_DIA : VENTANA_NOCHE;
}

