#include "DAC5574.h"

void DAC5574_Write(uint8_t channel, uint8_t value) {
    static I2C1_MESSAGE_STATUS status;
    static I2C1_TRANSACTION_REQUEST_BLOCK TRB;
    static uint8_t *pWrite, writeBuffer[3], nCount = 3;
    static uint16_t timeOut, slaveTimeOut;
    
    // this initial value is important
    status = I2C1_MESSAGE_PENDING;

    writeBuffer[0] = 0b00010000 | ((channel & 0x03) << 1); //Set Sel1 and Sel0 bits of control register
    writeBuffer[1] = value;
    writeBuffer[2] = 0; // 8 don't care bits but needed (see DS)

    pWrite = writeBuffer;
    
    // build TRB for writing
    I2C1_MasterWriteTRBBuild(&TRB, pWrite, nCount, DAC5574_ADDRESS);
    
    timeOut = 0;
    slaveTimeOut = 0;

    while (status != I2C1_MESSAGE_FAIL) {
        // now send the transactions
        I2C1_MasterTRBInsert(1, &TRB, &status);

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
        
        __delay_us(10);
    }
}