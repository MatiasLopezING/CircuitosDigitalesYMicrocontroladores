
#ifndef MEF_H_
#define MEF_H_

typedef enum {REPOSO,COCINANDO,TERMINO,PAUSA} state;


void FSM_Init ();
void FSM_Update();
void FSM_UpdateOutputs();



#endif /* MEF_H_ */