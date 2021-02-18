#ifndef ACTS_ROT_H
#define	ACTS_ROT_H

void Acts_ROT_Out(uint8_t, int16_t);
void Acts_ROT_PID(uint8_t, float, uint16_t);
void Acts_ROT_Wiggle(uint8_t);
void Acts_ROT_Drive(uint8_t, int8_t, uint8_t, uint8_t);
void Acts_ROT_DrvHandle();
void Acts_ROT_SetDrvInterval(uint8_t, uint8_t);
void Acts_ROT_SetWiggle(uint8_t);
void Acts_ROT_SetCurrentLimit(uint8_t, uint8_t);
void Acts_ROT_SetSpeedLimit(uint8_t, uint8_t);
uint8_t Acts_ROT_GetSpeedLimit(uint8_t);
uint16_t Acts_ROT_GetTarget(uint8_t );
void Acts_ROT_SetTarget(uint8_t , uint16_t);
uint16_t Acts_ROT_GetAngle(uint8_t, bool);
bool Acts_ROT_InRange(uint8_t);
float sgn(float);
float copysgn(float, float);

void Acts_ROT_TempUpdateControl(uint8_t, uint8_t);
uint8_t Acts_ROT_ReturnTempSwitch();
int16_t Acts_ROT_TempSPDMonitor(uint8_t);

void Acts_ROT_SetSPDAvgOut(uint8_t, int8_t);
uint8_t Acts_ROT_GetSPDAvgOut(uint8_t);
void Acts_ROT_SetSPDAvgNeighbour(uint8_t, uint8_t);
int8_t Acts_ROT_GetSPDAvgNeighbour(uint8_t);

void Acts_ROT_ResetOffsetInterval();
#endif	/* ACTS_ROT_H */