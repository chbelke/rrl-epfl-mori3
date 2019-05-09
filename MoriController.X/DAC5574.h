/* 
 * File:   DAC5574.h
 * Author: Alex Sigrist
 *
 * Created on August 22, 2018, 12:51 PM
 */

#ifndef DAC5574_H
#define	DAC5574_H

#include "mcc_generated_files/mcc.h"


// i2c addresses
#define DAC5574_ADDRESS 0x4F //Address pins A0-A1 tied to VCC

// void DAC5574_Setup(void);
void DAC5574_Write(uint8_t Channel, uint8_t  Value);

#endif	/* DAC5574_H */