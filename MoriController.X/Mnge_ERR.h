/* 
 * File:   Mnge_ERR.h
 * Author: belke
 *
 * Created on 16 November 2020, 14:44
 */

#ifndef MNGE_ERR_H
#define	MNGE_ERR_H

#include "mcc_generated_files/system.h"

void Mnge_ERR_ActivateStop(uint8_t, uint8_t);
uint8_t Mnge_ERR_GetErrorCode();
uint8_t Mnge_ERR_GetErrorEdge();
void Mnge_ERR_checkReal(float, uint8_t);

#endif	/* MNGE_ERR_H */