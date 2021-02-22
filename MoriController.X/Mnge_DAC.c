#include "Mnge_DAC.h"
#include "Defs_GLB.h"

volatile bool DAC_Flag[3] = {false, false, false};
volatile uint8_t DAC_Value[3] = {0, 0, 0};

void Mnge_DAC_Write(uint8_t channel, uint8_t value) {
    static I2C1_MESSAGE_STATUS status;
    static I2C1_TRANSACTION_REQUEST_BLOCK TRB;
    static uint8_t *pWrite, writeBuffer[3], nCount = 3;
    uint8_t timeOut = 0, slaveTimeOut = 0;
    
    // this initial value is important
    status = I2C1_MESSAGE_PENDING;

    writeBuffer[0] = 0b00010000 | ((channel & 0x03) << 1); //Set Sel1 and Sel0 bits of control register
    writeBuffer[1] = value;
    writeBuffer[2] = 0; // 8 don't care bits but needed (see DS)

    pWrite = writeBuffer;
    
    // build TRB for writing
    I2C1_MasterWriteTRBBuild(&TRB, pWrite, nCount, DAC5574_ADDRESS);

    while (status != I2C1_MESSAGE_FAIL) {
        // now send the transactions
        I2C1_MasterTRBInsert(1, &TRB, &status);

        // wait for the message to be sent or status has changed.
        while(status == I2C1_MESSAGE_PENDING) {
            __delay_us(100); // add some delay here
            // timeout checking
            if (slaveTimeOut >= SLAVE_I2C_GENERIC_DEVICE_TIMEOUT){
                slaveTimeOut = 0;
                break;
            } else slaveTimeOut++;
        }

        if (status == I2C1_MESSAGE_COMPLETE) break;

        // check for max retry and skip this byte
        if (timeOut >= SLAVE_I2C_GENERIC_RETRY_MAX) break;
        else timeOut++;
        
        __delay_us(10);
    }
}

void Mnge_DAC_Ctrl(void) {
    uint8_t edge;
    for (edge = 0; edge < 3; edge++) {
        if (DAC_Flag[edge]){
            Mnge_DAC_Write(edge,DAC_Value[edge]);
            DAC_Flag[edge] = false;
        }
    }
}

void Mnge_DAC_Set(uint8_t edge, uint8_t voltagelevel){
    DAC_Value[edge] = voltagelevel;
    DAC_Flag[edge] = true;
    Flg_i2c_DAC = true;
}