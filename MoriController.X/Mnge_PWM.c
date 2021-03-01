#include "Mnge_PWM.h"
#include "Mnge_ERR.h"
#include "Defs_GLB.h"

volatile uint8_t PWM_Values[8] =
        {RGB_Default_Blue, RGB_Default_Green, RGB_Default_Red,
        0, 0, 0, 0, 0};

void Mnge_PWM_Setup(void) {
    uint8_t initReg1[2] = {TLC59208_MODE1Add, TLC59208_MODE1};
    uint8_t initReg2[2] = {TLC59208_LEDOUT0Add, TLC59208_LEDOUT0};
    uint8_t initReg3[2] = {TLC59208_LEDOUT1Add, TLC59208_LEDOUT1};
    
    // does not work wihout static
    static I2C1_MESSAGE_STATUS status;
    static I2C1_TRANSACTION_REQUEST_BLOCK TRB[3];
    static uint8_t *pWrite, writeBuffer[2], nCount, iCount;
    uint8_t timeOut = 0, slaveTimeOut = 0; 

    // this initial value is important
    status = I2C1_MESSAGE_PENDING;

    // fill write buffer with first set of instructions
    nCount = 2;
    pWrite = initReg1;
    for (iCount = 0; iCount < nCount; iCount++) writeBuffer[iCount] = *pWrite++;
    // build first TRB for writing
    I2C1_MasterWriteTRBBuild(&TRB[0], pWrite, nCount, TLC59208_ADDRESS);

    // fill write buffer with second set of instructions
    nCount = 2;
    pWrite = initReg2;
    for (iCount = 0; iCount < nCount; iCount++) writeBuffer[iCount] = *pWrite++;
    // build second TRB for writing
    I2C1_MasterWriteTRBBuild(&TRB[1], pWrite, nCount, TLC59208_ADDRESS);

    // fill write buffer with second set of instructions
    nCount = 2;
    pWrite = initReg3;
    for (iCount = 0; iCount < nCount; iCount++) writeBuffer[iCount] = *pWrite++;
    // build second TRB for writing
    I2C1_MasterWriteTRBBuild(&TRB[2], pWrite, nCount, TLC59208_ADDRESS);

    while (status != I2C1_MESSAGE_FAIL) {
        // now send the transactions
        I2C1_MasterTRBInsert(3, TRB, &status);

        // wait for the message to be sent or status has changed.
        while (status == I2C1_MESSAGE_PENDING) {
            __delay_us(5); // add some delay here
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
    }

    // make sure everything set to zero on startup
    Mnge_PWM_Write();
}

void Mnge_PWM_Write(void) {
    static I2C1_MESSAGE_STATUS status;
    static I2C1_TRANSACTION_REQUEST_BLOCK TRB;
    static uint8_t *pWrite, writeBuffer[9], nCount = 9, iCount;
    uint8_t timeOut = 0, slaveTimeOut = 0;

    // this initial value is important
    status = I2C1_MESSAGE_PENDING;

    // fill write buffer
    writeBuffer[0] = TLC59208_CtrlReg;
    for (iCount = 1; iCount < nCount; iCount++) {
        writeBuffer[iCount] = PWM_Values[iCount - 1];
    }
    pWrite = writeBuffer;

    // build TRB for writing
    I2C1_MasterWriteTRBBuild(&TRB, pWrite, nCount, TLC59208_ADDRESS);

    while (status != I2C1_MESSAGE_FAIL) {
        // now send the transactions
        I2C1_MasterTRBInsert(1, &TRB, &status);

        // wait for the message to be sent or status has changed.
        while (status == I2C1_MESSAGE_PENDING) {
            __delay_us(5); // add some delay here
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
    }
    
    if (status != I2C1_MESSAGE_COMPLETE)
        Mnge_ERR_ActivateStop(0, ERR_I2CLedDriverFailed);
}

/* ******************** SET PWM VALUES ************************************* */
void Mnge_PWM_SetValues(uint8_t channel, uint8_t duty) {
    PWM_Values[channel] = duty;
}
