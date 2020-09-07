#include "Sens_ACC.h"
#include "Defs.h"

// Accelerometer MMA8452Q

int16_t ACC_Data[3] = {200, 200, 200};

void Sens_ACC_Setup(void) {
    I2C1_MESSAGE_STATUS status;
    I2C1_TRANSACTION_REQUEST_BLOCK TRB;
    uint8_t *pWrite, writeBuffer[2] = {MMA8452Q_CTRL_REG1_ADDR, MMA8452Q_CTRL_REG1};
    uint16_t timeOut, slaveTimeOut;

    // this initial value is important
    status = I2C1_MESSAGE_PENDING;

    // declare buffer pointer
    pWrite = writeBuffer;

    // build TRB for writing
    I2C1_MasterWriteTRBBuild(&TRB, pWrite, 2, MMA8452Q_Address);

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

}

void Sens_ACC_Read(void) {
    static I2C1_MESSAGE_STATUS status;
    static I2C1_TRANSACTION_REQUEST_BLOCK TRB[2];
    static uint8_t writeBuffer, readBuffer[6], *pWrite, *pRead;
    static uint16_t timeOut, slaveTimeOut;

    // this initial value is important
    status = I2C1_MESSAGE_PENDING;

    // fill write buffer with register and declare pointers
    writeBuffer = MMA8452Q_OUT_X_MSB_ADDR;
    pWrite = &writeBuffer;
    pRead = readBuffer;

    // build TRB for writing and reading
    I2C1_MasterWriteTRBBuild(&TRB[0], pWrite, 1, MMA8452Q_Address);
    I2C1_MasterReadTRBBuild(&TRB[1], pRead, 6, MMA8452Q_Address);

    timeOut = 0;
    slaveTimeOut = 0;

    while (status != I2C1_MESSAGE_FAIL) {
        // now send the transactions
        I2C1_MasterTRBInsert(2, TRB, &status);

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

    uint8_t i;
    for (i = 0; i < 5; i += 2) {
        ACC_Data[i / 2] = ((readBuffer[i] << 8) | readBuffer[i + 1]) >> 4;
        if (readBuffer[i] > 0x80){
            ACC_Data[i / 2] = (ACC_Data[i / 2] & 0x7FF) - 0x7FF;
        }
    }
}

// acceleration values between -2047 and 2047
int16_t Sens_ACC_Get(uint8_t axis) { //axis 0 corresponds to x, 1 to y, 2 to z
    return ACC_Data[axis];
}