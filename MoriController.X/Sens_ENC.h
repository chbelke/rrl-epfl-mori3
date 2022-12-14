#ifndef Sens_ENC_H
#define	Sens_ENC_H

// Encoder AS5048B
    
void Sens_ENC_Read(uint8_t);
float Sens_ENC_Get(uint8_t, bool);
float Sens_ENC_GetDelta(uint8_t);
void Sens_ENC_SetGlobalOffset(uint8_t);
void Sens_ENC_SetLiveOffset(uint8_t, uint16_t);
float Sens_ENC_GetLiveOffset(uint8_t);

void Sens_ENC_UpdateOld(uint8_t);
void Sens_ENC_UpdateNew(uint8_t, float);

float Sens_ENC_GetLrgOffsetMult(uint8_t);

#endif	/* Sens_ENC_H */

