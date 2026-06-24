
#ifndef MEF_H_
#define MEF_H_

typedef enum {REPOSO,COCINANDO,TERMINO,PAUSA} state;

/**
 * @brief Inicializacion de la MEF con estado inicial de REPOSO.
 * 
 * 
 */
void FSM_Init ();

/**
 * @brief Actualizacion del estado de la MEF en base a las entradas del sistema.
 * 
 * 
 */
void FSM_Update();

/**
 * @brief Actualizacion de la logica de las variables de salida de la MEF
 * 
 * 
 */
void FSM_UpdateOutputs();



#endif /* MEF_H_ */