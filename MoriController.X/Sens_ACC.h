#ifndef Sens_ACC_H
#define	Sens_ACC_H

#include "mcc_generated_files/mcc.h"
#include "Defs.h"

// Accelerometer MMA8452Q

void Sens_ACC_Setup(void);
void Sens_ACC_Read(void);
int16_t Sens_ACC_Get(uint8_t);

#endif	/* Sens_ACC_H */

