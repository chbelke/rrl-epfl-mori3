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

// speed limit
void Acts_ROT_SetSpeedLimit(uint8_t, uint8_t);
uint8_t Acts_ROT_GetSpeedLimit(uint8_t);

// desired angle
void Acts_ROT_SetTarget(uint8_t , uint16_t);
uint16_t Acts_ROT_GetTarget(uint8_t );

uint16_t Acts_ROT_GetAngle(uint8_t, bool);
bool Acts_ROT_InRange(uint8_t);

// math functions
int8_t sgn(float);
float copysgn(float, float);
float clamp_f(float, float, float);
uint16_t clamp_ui16(uint16_t, uint16_t, uint16_t);

// speed averaging functions
void Acts_ROT_SetSPDAvgOut(uint8_t, int16_t);
uint16_t Acts_ROT_GetSPDAvgOut(uint8_t);
void Acts_ROT_SetSPDAvgNeighbour(uint8_t, uint16_t);
int16_t Acts_ROT_GetSPDAvgNeighbour(uint8_t);

// deadband functions
void Acts_ROT_SetAngleDeadband(uint8_t);
float Acts_ROT_GetAngleDeadband();
float Acts_ROT_GetAngleDeadbandInverse();
void Acts_ROT_SetOkRange(uint8_t);

// gain functions
void Acts_ROT_SetPIDGains(uint8_t, uint8_t);
void Acts_ROT_SetSPDGain(uint8_t);

#endif	/* ACTS_ROT_H */