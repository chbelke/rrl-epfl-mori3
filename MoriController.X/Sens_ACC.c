#include <string.h>
#include "Sens_ACC.h"
#include "Defs_GLB.h"
#include "Mnge_RGB.h"
#include "Mnge_ERR.h"
#include "math.h"
#include "dsp.h"

// Accelerometer MMA8452Q
volatile int16_t ACC_Data[3] = {0, 0, 0};
const float smol_PI = 3.141592;
const float inv_Pi_times_180 = 57.2957795;
const float epsilon = 1e-10;

void Sens_ACC_Setup(void) {
    uint8_t MMAinitReg1[2] = {MMA8452Q_CTRL_REG1_ADDR, MMA8452Q_CTRL_REG1_STBY};
    uint8_t MMAinitReg2[2] = {MMA8452Q_CTRL_REG1_ADDR, MMA8452Q_CTRL_REG1_ACTV};
    uint8_t MMAinitReg3[2] = {MMA8452Q_CTRL_REG2_ADDR, MMA8452Q_CTRL_REG2_RNGE};
    
    static I2C1_MESSAGE_STATUS status;
    static I2C1_TRANSACTION_REQUEST_BLOCK TRB[3];
    static uint8_t *pWrite, writeBuffer[2], nCount, iCount;
    uint8_t timeOut = 0, slaveTimeOut = 0;

    // this initial value is important
    status = I2C1_MESSAGE_PENDING;

    // fill write buffer with first set of instructions
    nCount = 2;
    pWrite = MMAinitReg1;
    for (iCount = 0; iCount < nCount; iCount++) writeBuffer[iCount] = *pWrite++;
    // build first TRB for writing
    I2C1_MasterWriteTRBBuild(&TRB[0], pWrite, nCount, MMA8452Q_Address);

    // fill write buffer with second set of instructions
    nCount = 2;
    pWrite = MMAinitReg2;
    for (iCount = 0; iCount < nCount; iCount++) writeBuffer[iCount] = *pWrite++;
    // build second TRB for writing
    I2C1_MasterWriteTRBBuild(&TRB[1], pWrite, nCount, MMA8452Q_Address);

    // fill write buffer with second set of instructions
    nCount = 2;
    pWrite = MMAinitReg3;
    for (iCount = 0; iCount < nCount; iCount++) writeBuffer[iCount] = *pWrite++;
    // build second TRB for writing
    I2C1_MasterWriteTRBBuild(&TRB[2], pWrite, nCount, MMA8452Q_Address);

    while (status != I2C1_MESSAGE_FAIL) {
        // now send the transactions
        I2C1_MasterTRBInsert(3, TRB, &status);

        // wait for the message to be sent or status has changed.
        while (status == I2C1_MESSAGE_PENDING) {
            __delay_us(1); // add some delay here
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

        __delay_us(1);
    }
    
    if (status != I2C1_MESSAGE_COMPLETE)
        Mnge_ERR_ActivateStop(0, ERR_I2CAccelerometerFailed);
}

void Sens_ACC_Read(void) {
    static I2C1_MESSAGE_STATUS status;
    static I2C1_TRANSACTION_REQUEST_BLOCK TRB[2];
    static uint8_t writeBuffer, readBuffer[6], *pWrite, *pRead;
    uint8_t timeOut = 0, slaveTimeOut = 0;

    // this initial value is important
    status = I2C1_MESSAGE_PENDING;

    // fill write buffer with register and declare pointers
    writeBuffer = MMA8452Q_OUT_X_MSB_ADDR;
    pWrite = &writeBuffer;
    pRead = readBuffer;

    // build TRB for writing and reading
    I2C1_MasterWriteTRBBuild(&TRB[0], pWrite, 1, MMA8452Q_Address);
    I2C1_MasterReadTRBBuild(&TRB[1], pRead, 6, MMA8452Q_Address);

    while (status != I2C1_MESSAGE_FAIL) {
        // now send the transactions
        I2C1_MasterTRBInsert(2, TRB, &status);

        // wait for the message to be sent or status has changed.
        while (status == I2C1_MESSAGE_PENDING) {
            __delay_us(1); // add some delay here
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

        __delay_us(1);
    }
    
    if (status != I2C1_MESSAGE_COMPLETE)
        Mnge_ERR_ActivateStop(0, ERR_I2CAccelerometerFailed);

    uint8_t i;
    for (i = 0; i < 5; i += 2) {
        ACC_Data[i / 2] = ((readBuffer[i] * 256) + readBuffer[i + 1]) / 16;
    }
}

// acceleration values between -2047 and 2047 (2g)
uint16_t Sens_ACC_GetRaw(uint8_t axis) { //axis 0 corresponds to x, 1 to y, 2 to z
    return (uint16_t)(ACC_Data[axis] + 2047);
}

uint16_t Sens_ACC_GetAngle(uint8_t angle){ 
    float accX = (float)ACC_Data[0];
    float accY = (float)ACC_Data[1];
    float accZ = (float)ACC_Data[2];
         
    // 0 = alpha (about x), 1 = beta (about y), 2 = gamma (about z); right hand rule
    if (angle == 0){
        float sqrt_denom = fast_sqrt(accX*accX + accZ*accZ);
//        float sqrt_denom = sqrtf(accX*accX + accZ*accZ);
        if(sqrt_denom < epsilon)
            return (uint16_t) 0;
        return (uint16_t) 10.f*((atan2_approximation1(accY, sqrt_denom)*inv_Pi_times_180) + 180.f);
    } else if (angle == 1) {
        float sqrt_denom = fast_sqrt(accY*accY + accZ*accZ);
//        float sqrt_denom = sqrtf(accY*accY + accZ*accZ);
        if(sqrt_denom < epsilon)
            return (uint16_t) 0;
        return (uint16_t) 10.f*(-(atan2_approximation1(accX, sqrt_denom)*inv_Pi_times_180) + 180.f);
    } else if (angle == 2) {
        if(accZ < epsilon)
            return (uint16_t) 0;
        return (uint16_t) 10.f*(-(atan2_approximation1(-accY, accZ)*inv_Pi_times_180) + 180.f);
    } else {
        return 999;
    }
}

// From below:
// https://gist.github.com/volkansalma/2972237
float atan2_approximation1(float y, float x)
{
    //http://pubs.opengroup.org/onlinepubs/009695399/functions/atan2.html
    //Volkan SALMA

    const float ONEQTR_PI = smol_PI / 4.0;
	const float THRQTR_PI = 3.0 * smol_PI / 4.0;
	float r, angle;
	float abs_y = fabs(y) + 1e-10f;      // kludge to prevent 0/0 condition
	if ( x < 0.0f )
	{
		r = (x + abs_y) / (abs_y - x);
		angle = THRQTR_PI;
	}
	else
	{
		r = (x - abs_y) / (x + abs_y);
		angle = ONEQTR_PI;
	}
	angle += (0.1963f * r * r - 0.9817f) * r;
	if ( y < 0.0f )
		return( -angle );     // negate if in quad III or IV
	else
		return( angle );
}

// From below:
// https://codegolf.stackexchange.com/questions/85555/the-fastest-square-root-calculator
float fast_sqrt(float n) {
    n = 1.0f / n;
    long i;
    float x, y;

    x = n * 0.5f;
    y = n;
//    i = *(long *)&y;
    memcpy(&i, &y, sizeof y);
    i = 0x5f3759df - (i >> 1);
    memcpy(&y, &i, sizeof i);
//    y = *(float *)&i;
    y = y * (1.5f - (x * y * y));

    return y;
}