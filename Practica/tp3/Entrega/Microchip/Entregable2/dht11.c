/**
 * @file    dht11.c
 * @brief   Driver bloqueante para DHT11 sobre PC0.
 *
 * Protocolo (single-bus):
 *   1. MCU tira PC0 a LOW por 18ms (start signal)
 *   2. MCU libera la linea (pull-up externo de 10k la lleva a HIGH)
 *   3. DHT11 responde: LOW ~80us, luego HIGH ~80us
 *   4. DHT11 transmite 40 bits (5 bytes):
 *        [0] Humedad entera   [1] Humedad decimal
 *        [2] Temp entera      [3] Temp decimal
 *        [4] Checksum = suma de bytes 0..3
 *      Cada bit: LOW ~50us de preambulo, luego:
 *        - bit 0: HIGH ~26us
 *        - bit 1: HIGH ~70us
 *
 * Tecnica de muestreo:
 *   Se detecta el flanco ascendente de cada bit y se espera 30us.
 *   Si el pin sigue en HIGH -> bit 1 (HIGH duraria 70us).
 *   Si el pin ya bajo a LOW -> bit 0 (HIGH solo duraba 26us).
 *   Esto evita contar iteraciones de loop y es independiente del nivel
 *   de optimizacion del compilador.
 *
 * Manejo de interrupciones:
 *   cli() al inicio, sei() antes de cada return.
 *   El bloqueo total es ~22ms (18ms start + handshake + 40 bits).
 */

#include "dht11.h"

/*
 * Maximo de iteraciones de espera por transicion de pin.
 * A 16MHz con ~4 ciclos por iteracion: 500 iter ~ 125us.
 * Suficiente margen sobre el pulso mas largo del protocolo (80us).
 */
#define DHT11_TIMEOUT_ITER  500u

/*
  @brief   Espera a que PC0 alcance el nivel logico indicado.

  Itera con un contador de guarda para evitar cuelgues si el sensor no responde.

  @param   esperado  0 para esperar LOW, cualquier otro valor para esperar HIGH.
  @return  1 si el pin alcanzo el nivel esperado antes del timeout.
  @return  0 si se supero DHT11_TIMEOUT_ITER iteraciones sin cambio.
 */
static uint8_t wait_pin(uint8_t esperado)
{
    uint16_t count = 0;

    if (esperado) {
        /* Esperar hasta que el pin suba a HIGH */
        while (!(DHT11_PIN & (1 << DHT11_BIT))) {
            if (++count > DHT11_TIMEOUT_ITER) return 0;
        }
    } else {
        /* Esperar hasta que el pin baje a LOW */
        while (DHT11_PIN & (1 << DHT11_BIT)) {
            if (++count > DHT11_TIMEOUT_ITER) return 0;
        }
    }
    return 1;
}

/*
  @brief   Ejecuta el protocolo DHT11 completo y retorna temperatura y humedad.

  Secuencia: start signal (18 ms LOW) -> handshake (80 us + 80 us) -> 40 bits.
  Tecnica de muestreo: se espera el flanco ascendente de cada bit y se muestrea
  a 30 us; si el pin sigue HIGH es bit 1 (~70 us), si ya bajo es bit 0 (~26 us).
  Las interrupciones permanecen deshabilitadas durante toda la funcion (~22 ms).

  @param   temp  Puntero donde se escribe la temperatura entera en grados Celsius.
  @param   hum   Puntero donde se escribe la humedad relativa entera en porcentaje.
  @return  DHT11_OK      Lectura y checksum correctos; *temp y *hum son validos.
  @return  DHT11_TIMEOUT El sensor no respondio en algun punto del protocolo.
  @return  DHT11_CHKSUM  El checksum no coincide; *temp y *hum no se modifican.
 */
dht11_status_t dht11_read(uint8_t *temp, uint8_t *hum)
{
    uint8_t data[5] = {0, 0, 0, 0, 0};
    uint8_t byte_idx, bit_idx;

    cli(); /* Deshabilitar interrupciones durante toda la lectura */

    /* -------------------------------------------------------
     * 1. Señal de start: MCU tira LOW por 18ms, luego suelta
     * ------------------------------------------------------- */
    DHT11_DDR  |=  (1 << DHT11_BIT);  /* PC0 como salida                          */
    DHT11_PORT &= ~(1 << DHT11_BIT);  /* Nivel LOW                                */
    _delay_ms(18);                     /* Minimo 18ms                              */
    DHT11_DDR  &= ~(1 << DHT11_BIT);  /* Soltar linea: PC0 como entrada           */
    DHT11_PORT &= ~(1 << DHT11_BIT);  /* Sin pull-up interno (hay externo de 10k) */
    /* La resistencia de 10k sube la linea pasivamente a HIGH.
     * El DHT11 detecta el flanco ascendente y responde 20-40us despues. */

    /* -------------------------------------------------------
     * 2. Handshake del DHT11: LOW ~80us -> HIGH ~80us
     * ------------------------------------------------------- */
    if (!wait_pin(0)) { sei(); return DHT11_TIMEOUT; } /* Esperar que DHT11 tire LOW  */
    if (!wait_pin(1)) { sei(); return DHT11_TIMEOUT; } /* Esperar que DHT11 suelte    */
    if (!wait_pin(0)) { sei(); return DHT11_TIMEOUT; } /* Esperar fin del HIGH de 80us */

    /* -------------------------------------------------------
     * 3. Lectura de 40 bits (5 bytes)
     * ------------------------------------------------------- */
    for (byte_idx = 0; byte_idx < 5; byte_idx++) {
        for (bit_idx = 0; bit_idx < 8; bit_idx++) {

            /* Cada bit comienza con un LOW de ~50us; esperar flanco ascendente */
            if (!wait_pin(1)) { sei(); return DHT11_TIMEOUT; }

            /*
             * Muestrear a 30us del flanco ascendente:
             *   - Bit 0: HIGH dura ~26us -> el pin ya bajo  -> leemos LOW
             *   - Bit 1: HIGH dura ~70us -> el pin sigue HIGH -> leemos HIGH
             */
            _delay_us(30);

            data[byte_idx] <<= 1;
            if (DHT11_PIN & (1 << DHT11_BIT)) {
                data[byte_idx] |= 0x01; /* Bit 1 */
            }

            /* Sincronizar al LOW del siguiente bit (o fin de trama) */
            if (!wait_pin(0)) { sei(); return DHT11_TIMEOUT; }
        }
    }

    sei(); /* Rehabilitar interrupciones */

    /* -------------------------------------------------------
     * 4. Verificacion de checksum
     * ------------------------------------------------------- */
    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    if (checksum != data[4]) return DHT11_CHKSUM;

    /* -------------------------------------------------------
     * 5. Asignacion de resultados (solo si todo fue correcto)
     * ------------------------------------------------------- */
    *hum  = data[0]; /* Humedad relativa entera (%) */
    *temp = data[2]; /* Temperatura entera (°C)     */

    return DHT11_OK;
}
