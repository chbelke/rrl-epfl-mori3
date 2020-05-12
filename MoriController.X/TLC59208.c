#include "TLC59208.h"
#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <libpic30.h>
#include "define.h"

volatile uint8_t TLC_Values[8] = {255, 255, 255, 0, 0, 0, 0, 0};
uint8_t initReg1[2] = {TLC59208_MODE1Add, TLC59208_MODE1};
uint8_t initReg2[2] = {TLC59208_LEDOUT0Add, TLC59208_LEDOUT0};
uint8_t initReg3[2] = {TLC59208_LEDOUT1Add, TLC59208_LEDOUT1};
volatile uint8_t SMA_Count[3] = {0, 0, 0};

void TLC59208_Setup(void) {
    I2C1_MESSAGE_STATUS status;
    I2C1_TRANSACTION_REQUEST_BLOCK TRB[3];
    uint8_t *pWrite, writeBuffer[2], nCount, iCount;
    uint16_t timeOut, slaveTimeOut;

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

    timeOut = 0;
    slaveTimeOut = 0;

    while (status != I2C1_MESSAGE_FAIL) {
        // now send the transactions
        I2C1_MasterTRBInsert(3, TRB, &status);

        // wait for the message to be sent or status has changed.
        while (status == I2C1_MESSAGE_PENDING) {
            // add some delay here
            __delay_us(1);
            // timeout checking
            if (slaveTimeOut == SLAVE_I2C_GENERIC_DEVICE_TIMEOUT)
                break; //return (0);
            else
                slaveTimeOut++;
        }

        if (status == I2C1_MESSAGE_COMPLETE)
            break;

        // check for max retry and skip this byte
        if (timeOut == SLAVE_I2C_GENERIC_RETRY_MAX)
            break; //return (0);
        else
            timeOut++;

        __delay_us(10);
    }

    // make sure everything set to zero on startup
    TLC59208_Write();
}

void TLC59208_Write(void) {
    static I2C1_MESSAGE_STATUS status;
    static I2C1_TRANSACTION_REQUEST_BLOCK TRB;
    static uint8_t *pWrite, writeBuffer[9], nCount = 9, iCount;
    static uint16_t timeOut, slaveTimeOut;

    // this initial value is important
    status = I2C1_MESSAGE_PENDING;

    // fill write buffer
    writeBuffer[0] = TLC59208_CtrlReg;
    for (iCount = 1; iCount < nCount; iCount++) {
        writeBuffer[iCount] = TLC_Values[iCount - 1];
    }
    pWrite = writeBuffer;

    // build TRB for writing
    I2C1_MasterWriteTRBBuild(&TRB, pWrite, nCount, TLC59208_ADDRESS);

    timeOut = 0;
    slaveTimeOut = 0;

    while (status != I2C1_MESSAGE_FAIL) {
        // now send the transactions
        I2C1_MasterTRBInsert(1, &TRB, &status);

        // wait for the message to be sent or status has changed.
        while (status == I2C1_MESSAGE_PENDING) {
            // add some delay here
            __delay_us(1);
            // timeout checking
            if (slaveTimeOut == SLAVE_I2C_GENERIC_DEVICE_TIMEOUT){
                break; //return (0);
            } else {
                slaveTimeOut++;
            }
        }

        if (status == I2C1_MESSAGE_COMPLETE)
            break;

        // check for max retry and skip this byte
        if (timeOut == SLAVE_I2C_GENERIC_RETRY_MAX)
            break; //return (0);
        else
            timeOut++;

        __delay_us(10);
    }
}

void SMA_Set(uint8_t edge, uint8_t duty) {
    switch (edge) {
        case 0:
            TLC_Values[3] = duty;
            break;
        case 1:
            TLC_Values[4] = duty;
            break;
        case 2:
            TLC_Values[5] = duty;
            break;
        default:
            break;
    }
}

void SMA_On(uint8_t edge) {
    SMA_Count[edge] = SMA_Period;
    SMA_Set(edge, SMA_Duty);
    LED_R = 0;
}

void SMA_Off(uint8_t edge) {
    SMA_Count[edge] = 0;
    SMA_Set(edge, 0);
    if (edge == 0){
        LED_R = 1;
    }
}

// coupling sma controller, called in TMR3_CallBack (20 Hz)
// switches SMA off when counter run out

void SMA_Ctrl(void) {
    uint8_t m;
    for (m = 0; m <= 2; m++) {
        if (SMA_Count[m]) {
            SMA_Count[m] = SMA_Count[m] - 1;
            if (SMA_Count[m] <= 0) {
                SMA_Off(m);
            }
        } else {
            SMA_Off(m);
        }
    }
    TLC59208_Write();
}

void LED_Set(uint8_t RGBcolor, uint8_t duty) {
    switch (RGBcolor) {
        case 0:
            TLC_Values[2] = duty; // red
            break;
        case 1:
            TLC_Values[1] = duty; // green
            break;
        case 2:
            TLC_Values[0] = duty; // blue
            break;
        default:
            break;
    }
    TLC59208_Write();
}

void LED_SetAll(uint8_t R, uint8_t G, uint8_t B) {
    TLC_Values[2] = R; // red
    TLC_Values[1] = G; // green
    TLC_Values[0] = B; // blue
    TLC59208_Write();
}
