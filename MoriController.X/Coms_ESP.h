/* 
 * File:   Coms_ESP.h
 * Author: belke
 *
 * Created on April 4, 2019, 5:34 PM
 */

#ifndef COMS_ESP_H
#define	COMS_ESP_H

#include "mcc_generated_files/mcc.h"

void Coms_ESP_Eval (void);
void Coms_ESP_SetMots (void);
void Coms_ESP_Drive (uint8_t, int8_t, uint8_t, uint8_t);
uint8_t Coms_ESP_ReturnID(uint8_t);
void Coms_ESP_Verbose (void);
void Coms_ESP_SetLEDs (uint8_t, uint8_t);

#endif	/* COMS_ESP_H */

