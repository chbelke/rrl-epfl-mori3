/* 
 * File:   MotLin.h
 * Author: belke
 *
 * Created on October 9, 2018, 9:55 AM
 */

#ifndef MOTLIN_H
#define	MOTLIN_H

void MotLin_OUT(uint8_t, int16_t);
void MotLin_PID(uint8_t, int16_t, int16_t);
uint16_t MotLin_Get(uint8_t);
void MotLin_Set(uint8_t , uint16_t );

#endif	/* MOTLIN_H */

