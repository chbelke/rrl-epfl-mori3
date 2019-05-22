/* 
 * File:   TLC59208.h
 * Author: Alex Sigrist
 *
 * Created on August 15, 2018, 12:51 PM
 */

#ifndef TLC59208_H
#define	TLC59208_H

#include "mcc_generated_files/mcc.h"

void TLC59208_Setup(void);
void TLC59208_Write(void);

void SMA_Set(uint8_t, uint8_t);
void SMA_On(uint8_t);
void SMA_Off(uint8_t);
void SMA_Ctrl(void);
void LED_Set(uint8_t, uint8_t);

#endif	/* TLC59208_H */