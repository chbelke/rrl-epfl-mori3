#ifndef ACTS_ROT_H
#define	ACTS_ROT_H

void Acts_ROT_Out(uint8_t, int16_t);
void Acts_ROT_PID(uint8_t, float, float);
void Acts_ROT_Limit(uint8_t, uint8_t);
uint16_t Acts_ROT_GetTarget(uint8_t );
void Acts_ROT_SetTarget(uint8_t , uint16_t);
uint16_t Acts_ROT_GetCurrent(uint8_t);

#endif	/* ACTS_ROT_H */