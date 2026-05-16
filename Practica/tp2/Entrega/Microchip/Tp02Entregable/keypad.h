/*
 * keypad.h
 *
 * Created: 5/16/2026 1:45:08 AM
 *  Author: tomas
 */ 


#ifndef KEYPAD_H_
#define KEYPAD_H_

#include <stdint.h>
#include <avr/io.h>

void KEYPAD_Init(void);
uint8_t KEYPAD_Scan(uint8_t *pkey);

#endif /* KEYPAD_H_ */