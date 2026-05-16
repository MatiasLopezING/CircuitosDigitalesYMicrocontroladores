
#ifndef MEF_H_
#define MEF_H_

typedef enum {REPOSO,COCINANDO,TERMINO,PAUSA,PUERTA_ABIERTA} state;
typedef enum {EV_START,EV_STOPCLEAR,EV_ADD30,EV_DOOROPEN,EV_DOORCLOSED,EV_TIMEOUT,EV_5SEG} event;

void FSM_Init ();
void FSM_Update();
void FSM_UpdateOutputs();



#endif /* MEF_H_ */