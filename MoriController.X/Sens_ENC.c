#include <string.h>
#include "Defs_GLB.h"
#include "Defs_MOD.h"
#include "Defs_OFF.h"
#include "Sens_ENC.h"
#include "Coms_123.h"
#include "mcc_generated_files/i2c1.h"

// Encoder AS5048B
float ENC_Data[3] = {1, 2, 3};
float ENC_DataOld1[3] = {1, 2, 3};
int8_t ENC_Offset[3] = {0, 0, 0};

const int8_t ModuleOffsetData[3][NUM_MODS*3] = {
    {Off1_A1, Off1_A2, Off1_A3, Off1_B1, Off1_B2, Off1_B3, Off1_C1, Off1_C2, Off1_C3, Off1_D1, Off1_D2, Off1_D3, Off1_E1, Off1_E2, Off1_E3, Off1_F1, Off1_F2, Off1_F3, Off1_G1, Off1_G2, Off1_G3, Off1_H1, Off1_H2, Off1_H3, Off1_I1, Off1_I2, Off1_I3, Off1_J1, Off1_J2, Off1_J3, Off1_K1, Off1_K2, Off1_K3, Off1_L1, Off1_L2, Off1_L3, Off1_M1, Off1_M2, Off1_M3, Off1_N1, Off1_N2, Off1_N3},
    {Off2_A1, Off2_A2, Off2_A3, Off2_B1, Off2_B2, Off2_B3, Off2_C1, Off2_C2, Off2_C3, Off2_D1, Off2_D2, Off2_D3, Off2_E1, Off2_E2, Off2_E3, Off2_F1, Off2_F2, Off2_F3, Off2_G1, Off2_G2, Off2_G3, Off2_H1, Off2_H2, Off2_H3, Off2_I1, Off2_I2, Off2_I3, Off2_J1, Off2_J2, Off2_J3, Off2_K1, Off2_K2, Off2_K3, Off2_L1, Off2_L2, Off2_L3, Off2_M1, Off2_M2, Off2_M3, Off2_N1, Off2_N2, Off2_N3},
    {Off3_A1, Off3_A2, Off3_A3, Off3_B1, Off3_B2, Off3_B3, Off3_C1, Off3_C2, Off3_C3, Off3_D1, Off3_D2, Off3_D3, Off3_E1, Off3_E2, Off3_E3, Off3_F1, Off3_F2, Off3_F3, Off3_G1, Off3_G2, Off3_G3, Off3_H1, Off3_H2, Off3_H3, Off3_I1, Off3_I2, Off3_I3, Off3_J1, Off3_J2, Off3_J3, Off3_K1, Off3_K2, Off3_K3, Off3_L1, Off3_L2, Off3_L3, Off3_M1, Off3_M2, Off3_M3, Off3_N1, Off3_N2, Off3_N3}
};
const uint8_t ModuleIDs[NUM_MODS][6] = {
    {ID_A1, ID_A2, ID_A3, ID_A4, ID_A5, ID_A6},
    {ID_B1, ID_B2, ID_B3, ID_B4, ID_B5, ID_B6},
    {ID_C1, ID_C2, ID_C3, ID_C4, ID_C5, ID_C6},
    {ID_D1, ID_D2, ID_D3, ID_D4, ID_D5, ID_D6},
    {ID_E1, ID_E2, ID_E3, ID_E4, ID_E5, ID_E6},
    {ID_F1, ID_F2, ID_F3, ID_F4, ID_F5, ID_F6},
    {ID_G1, ID_G2, ID_G3, ID_G4, ID_G5, ID_G6},
    {ID_H1, ID_H2, ID_H3, ID_H4, ID_H5, ID_H6},
    {ID_I1, ID_I2, ID_I3, ID_I4, ID_I5, ID_I6},
    {ID_J1, ID_J2, ID_J3, ID_J4, ID_J5, ID_J6},
    {ID_K1, ID_K2, ID_K3, ID_K4, ID_K5, ID_K6},
    {ID_L1, ID_L2, ID_L3, ID_L4, ID_L5, ID_L6},
    {ID_M1, ID_M2, ID_M3, ID_M4, ID_M5, ID_M6},
    {ID_N1, ID_N2, ID_N3, ID_N4, ID_N5, ID_N6}
};

/* ******************** READ ENCODER **************************************** */
void Sens_ENC_Read(uint8_t edge) {
    I2C1_MESSAGE_STATUS status;
    I2C1_TRANSACTION_REQUEST_BLOCK TRB[2];
    uint8_t writeBuffer, readBuffer[2], *pWrite, *pRead;
    uint16_t timeOut, slaveTimeOut, angleINT = 0;
    float angleFLT = 0;
    
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

        __delay_us(1);
    }
    
    // combine 14 bit result
    angleINT = (((uint16_t) readBuffer[0]) << 6);
    angleINT += (uint16_t)(readBuffer[1] & 0x3F);
    
    // convert to float
    angleFLT = (float)(0b11111111111111 - angleINT);
    angleFLT = (angleFLT / AS5048B_Res) * 360.0 - 180;

    // output
    ENC_DataOld1[edge] = ENC_Data[edge];
    ENC_Data[edge] = angleFLT;
}

/* ******************** GET CURRENT ANGLE *********************************** */
float Sens_ENC_Get(uint8_t edge) {
    return ENC_Data[edge] - ((float)ENC_Offset[edge])*0.1;
}

/* ******************** GET ANGLE DELTA ************************************* */
float Sens_ENC_GetDelta(uint8_t edge) {
    return (ENC_Data[edge] - ENC_DataOld1[edge]);
}

/* ******************** UPDATE ANGLE OFFSET BY NEIGHBOUR ******************** */
void Sens_ENC_NbrOffset(uint8_t edge){
    uint8_t *neighbours;
    uint8_t i;
    neighbours = Coms_123_GetNeighbourIDs();
    for (i = 0; i < NUM_MODS; i++){
        if (!memcmp(neighbours+edge*7, ModuleIDs[i], 6)){
            ENC_Offset[edge] = ModuleOffsetData[edge][i*3+(*(neighbours+6+7*edge)-48)];
            // 48 is ascii protection against 0 in Coms_123
            break;
        }
    }
}