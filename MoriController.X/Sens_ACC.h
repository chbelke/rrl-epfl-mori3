#ifndef Sens_ACC_H
#define	Sens_ACC_H

#include "mcc_generated_files/system.h"
#include "Defs_GLB.h"

// Accelerometer MMA8452Q

void Sens_ACC_Setup(void);
void Sens_ACC_Read(void);
uint16_t Sens_ACC_GetRaw(uint8_t);
uint16_t Sens_ACC_GetAngle(uint8_t);

#endif	/* Sens_ACC_H */

