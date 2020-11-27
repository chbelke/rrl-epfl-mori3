#ifndef ACTS_LIN_H
#define	ACTS_LIN_H

long map(long,long,long,long,long);

void Acts_LIN_Out(uint8_t, int16_t);
void Acts_LIN_PID(uint8_t, int16_t, int16_t);
uint8_t Acts_LIN_GetTarget(uint8_t);
void Acts_LIN_SetTarget(uint8_t, uint8_t);
void Acts_LIN_SetMaxPWM(uint8_t, uint16_t);
uint8_t Acts_LIN_GetCurrent(uint8_t);
bool Act_LIN_IsStable();
bool Act_LIN_InRange(uint8_t);

#endif	/* ACTS_LIN_H */
