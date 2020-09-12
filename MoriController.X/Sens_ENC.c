#include "Defs.h"
#include "Sens_ENC.h"
#include "mcc_generated_files/i2c1.h"

float ENC_Data[3] = {1, 2, 3};

// Encoder AS5048B
void Sens_ENC_Read(uint8_t edge) {
    static I2C1_MESSAGE_STATUS status;
    static I2C1_TRANSACTION_REQUEST_BLOCK TRB[2];
    static uint8_t writeBuffer, readBuffer[2], *pWrite, *pRead;
    static uint16_t timeOut, slaveTimeOut, angleINT = 0;
    static float angleFLT = 0;
    
    // this initial value is important
    status = I2C1_MESSAGE_PENDING;

    // fill write buffer with register and declare pointers
    writeBuffer = AS5048B_Reg_AngleMSB;
    pWrite = &writeBuffer;
    pRead = readBuffer;
        
    // build TRB for writing and reading
    I2C1_MasterWriteTRBBuild(&TRB[0], pWrite, 1, AS5048B_Address | edge);
    I2C1_MasterReadTRBBuild(&TRB[1], pRead, 2, AS5048B_Address | edge);

    timeOut = 0;
    slaveTimeOut = 0;

    while(status != I2C1_MESSAGE_FAIL) {
        // now send the transactions
        I2C1_MasterTRBInsert(2, TRB, &status);

        // wait for the message to be sent or status has changed.
        while(status == I2C1_MESSAGE_PENDING) {
            // add some delay here
            __delay_us(1);
            // timeout checking
            if (slaveTimeOut == SLAVE_I2C_GENERIC_DEVICE_TIMEOUT)
                break;//return (0);
            else
                slaveTimeOut++;
        }

        if (status == I2C1_MESSAGE_COMPLETE)
            break;

        // check for max retry and skip this byte
        if (timeOut == SLAVE_I2C_GENERIC_RETRY_MAX)
            break;//return (0);
        else
            timeOut++;
    }
    
    // combine 14 bit result
    angleINT = (((uint16_t) readBuffer[0]) << 6);
    angleINT += (uint16_t)(readBuffer[1] & 0x3F);
    
    // convert to float
    angleFLT = (float)(0b11111111111111 - angleINT);
    angleFLT = (angleFLT / AS5048B_Res) * 360.0 - 180;

    // output
    ENC_Data[edge] = angleFLT;
}

float Sens_ENC_Get(uint8_t edge) {
    return ENC_Data[edge];
}