/* 
 * File:   MMA8452Q.h
 * Author: Alex Sigrist
 *
 * Created on August 16, 2018, 10:21 AM
 */

#ifndef MMA8452Q_H
#define	MMA8452Q_H

#include "mcc_generated_files/mcc.h"
#include "define.h"

void MMA8452Q_Setup(void);
void MMA8452Q_Read(void);

uint16_t ACC_Get(uint8_t);

#endif	/* MMA8452Q_H */

