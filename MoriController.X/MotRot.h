/* 
 * File:   MotRot.h
 * Author: belke
 *
 * Created on October 9, 2018, 9:56 AM
 */

#ifndef MOTROT_H
#define	MOTROT_H

void MotRot_OUT(uint8_t, int16_t);
    
void MotRot_PID(uint8_t, float, float);

void MotRot_LIM(uint8_t, uint8_t);


#ifdef	__cplusplus
extern "C" {
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* MOTROT_H */

