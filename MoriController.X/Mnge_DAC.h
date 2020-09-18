#ifndef MNGE_DAC_H
#define	MNGE_DAC_H

#include "mcc_generated_files/system.h"

// Digital to analog converter DAC5574

#define DAC5574_ADDRESS 0x4F // address pins A0-A1 tied to VCC

// void DAC5574_Setup(void);
void Mnge_DAC_Write(uint8_t, uint8_t);
void Mnge_DAC_Ctrl(void);
void Mnge_DAC_Set(uint8_t, uint8_t);

#endif	/* MNGE_DAC_H */