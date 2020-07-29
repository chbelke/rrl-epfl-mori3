/*
 * File:   Coms_123.h
 * Author: belke
 *
 * Created on July 27, 2020, 8:18 AM
 */

#ifndef COMS_123_H
#define	COMS_123_H

#include "mcc_generated_files/mcc.h"

void Coms_123_Eval (uint8_t);
void Coms_123_IdleEval (uint8_t);
void Coms_123_ConHandle (void);
void Coms_123_ActHandle(void);
void Coms_123_Write (uint8_t, uint8_t);
void Coms_123_WriteID (uint8_t);
uint8_t Coms_123_Read(uint8_t);

#endif	/* COMS_123_H */

