#ifndef ACTS_LIN_H
#define	ACTS_LIN_H

void Acts_LIN_Out(uint8_t, int16_t);
void Acts_LIN_PID(uint8_t, int16_t, int16_t);
uint16_t Acts_LIN_GetTarget(uint8_t);
void Acts_LIN_SetTarget(uint8_t , uint16_t );

#endif	/* ACTS_LIN_H */

