#include "Defs_GLB.h"
#include "Defs_MOD.h"
#include "Mnge_DAC.h"
#include "Sens_ENC.h"
#include "Acts_LIN.h"
#include "Acts_ROT.h"
#include "Coms_ESP.h"
#include "math.h"
#include "dsp.h"

uint16_t Ang_Desired[3] = {1800, 1800, 1800}; // -180.0 to 180.0 deg = 0 to 3600
uint8_t Trq_Limit[3] = {0, 0, 0}; // save torque limit during wiggle
uint8_t Speed_100[3] = {MotRot_SpeedInit, MotRot_SpeedInit, MotRot_SpeedInit};
float Speed_DEG[3] = {(((float) MotRot_SpeedInit) / 100.0f) * MotRot_SpeedMax * MotRot_PID_period,
    (((float) MotRot_SpeedInit) / 100.0f) * MotRot_SpeedMax * MotRot_PID_period,
    (((float) MotRot_SpeedInit) / 100.0f) * MotRot_SpeedMax * MotRot_PID_period};
uint8_t DrvInterval[3] = {0, 0, 0};
float PID_I[3] = {0, 0, 0}; // integral error variable
//float SPD_I[3] = {0, 0, 0}; // speed control integral

volatile bool SPD_AvgFlag[3] = {false, false, false};
volatile int8_t SPD_AvgOut[3] = {0, 0, 0};
volatile int8_t SPD_AvgIn[3] = {0, 0, 0};

bool OffsetUpdateFlag = false;

volatile int16_t SPD_PID_Monitor[3] = {0, 0, 0};

uint8_t TMP_SpeedReg_GainD = 0;
uint8_t TMP_SpeedReg_GainI = 15;
uint8_t TMP_SpeedReg_GainP = 0;
uint8_t TMP_Switch = 0;

#define WHEEL 68.15f // wheel distance from vertex
#define SxOUT 0.9f // output speed factor for non-primary wheels (tune curve)

/* ******************** ROTARY MOTOR OUTPUTS ******************************** */
void Acts_ROT_Out(uint8_t edge, int16_t duty) {
    if (!MODE_MotRot_Active || !FLG_MotRot_Active) duty = 0; // rotary motors off
    switch (edge) {
        case 0:
            if (duty > 0) ROT_DIR_1 = 1; // direction output
            else ROT_DIR_1 = 0;
            PWM_Set(ROT_PWM_1, abs(duty)); // pwm output
            break;

        case 1:
            if (duty > 0) ROT_DIR_2 = 1; // direction output
            else ROT_DIR_2 = 0;
            PWM_Set(ROT_PWM_2, abs(duty)); // pwm output
            break;

        case 2:
            if (duty > 0) ROT_DIR_3 = 1; // direction output
            else ROT_DIR_3 = 0;
            PWM_Set(ROT_PWM_3, abs(duty)); // pwm output
            break;

        default:
            break;
    }
}

/* ******************** ROTARY MOTOR PID ************************************ */
void Acts_ROT_PID(uint8_t edge, float current, uint16_t target) {
    static float errorOld[3] = {0, 0, 0}; // previous error (derivative gain)
//    static float outPOld[3] = {0, 0, 0}; // speed control switch detect variable
//    static float errorSPDOld5[3] = {0, 0, 0};
//    static float errorSPDOld4[3] = {0, 0, 0};
//    static float errorSPDOld3[3] = {0, 0, 0};
//    static float errorSPDOld2[3] = {0, 0, 0};
//    static float errorSPDOld1[3] = {0, 0, 0};

    // avoid bad control inputs
    if (target < MotRot_AngleIntMIN) target = MotRot_AngleIntMIN;
    else if (target > MotRot_AngleIntMAX) target = MotRot_AngleIntMAX;

    float error = ((float) target) * 0.1f - 180.0f - current;

    // avoid integral build up when far away
    if (fabsf(error) < 7.0f) // if error is >6.69, kp results in max (ignoring kd)
        PID_I[edge] += error * MotRot_PID_period;
    else PID_I[edge] = 0;

    // limit integral component
    if (PID_I[edge] < -MotRot_PID_Imax) PID_I[edge] = -MotRot_PID_Imax;
    else if (PID_I[edge] > MotRot_PID_Imax) PID_I[edge] = MotRot_PID_Imax;

    // derivative component
    float PID_D = (error - errorOld[edge]) * MotRot_PID_freq;
    errorOld[edge] = error;

    // limit derivative component
    if (PID_D < -MotRot_PID_Dmax) PID_D = -MotRot_PID_Dmax;
    else if (PID_D > MotRot_PID_Dmax) PID_D = MotRot_PID_Dmax;

    // PID output
    float outP = MotRot_PID_kP * error + MotRot_PID_kI * PID_I[edge] + MotRot_PID_kD * PID_D;

    // limit duty cycle
    if (outP < -MotRot_PID_Max) outP = -MotRot_PID_Max;
    else if (outP > MotRot_PID_Max) outP = MotRot_PID_Max;

    float OUT = outP; // position pwm output
    
    static uint16_t loopsSinceLastOffset = 0;
    loopsSinceLastOffset++;
    if (OffsetUpdateFlag){
        SPD_PID_Monitor[1] = loopsSinceLastOffset;
        loopsSinceLastOffset = 0;
        OffsetUpdateFlag = false;
    }

    // speed control
//    if ((Speed_100[edge] != 100) && (fabs(error) > 1.0f)) {
    if ((Speed_100[edge] != 100) && (fabsf(error) >= 1.0f)) {
        static uint16_t targetOld = 0;
        static float SpeedReg_I[3] = {0, 0, 0};
//        static float SpeedReg_D0[3] = {0, 0, 0};
//        static float SpeedReg_D1[3] = {0, 0, 0};
//        static float SpeedReg_D2[3] = {0, 0, 0};
//        static float SpeedReg_Delta[3][5] = {
//            {0, 0, 0, 0, 0},
//            {0, 0, 0, 0, 0},
//            {0, 0, 0, 0, 0}
//        };
        
        // initialise speed regulation with feed forward
//        if (target != targetOld){
//            SpeedReg_I[edge] = sgn(outP) * MotRot_SPD_Max * (((float) Acts_ROT_GetSpeedLimit(edge)) * 0.01f);
////            SpeedReg_D0[edge] = 0;
//            targetOld = target;
//        }
        
        
//        uint8_t i;
//        for (i = 0; i < 4; i++){
//            SpeedReg_Delta[edge][i] = SpeedReg_Delta[edge][i+1];
//        }
//        SpeedReg_Delta[edge][4] = Sens_ENC_GetDelta(edge);
        
//        float angleDelta = Sens_ENC_GetDelta(edge);
//        float angleDelta = 0.2f * (SpeedReg_Delta[edge][0]
//                + SpeedReg_Delta[edge][1] + SpeedReg_Delta[edge][2]
//                + SpeedReg_Delta[edge][3] + SpeedReg_Delta[edge][4]);
        
//        // only update speed control every 5 iterations (= 20Hz))
//        if (SpeedReg_Count[edge] < 4){
//            SpeedReg_Delta[edge][SpeedReg_Count[edge]] = Sens_ENC_GetDelta(edge);
//            SpeedReg_Count[edge]++;
//        } else {
//            SpeedReg_Delta[edge][SpeedReg_Count[edge]] = Sens_ENC_GetDelta(edge);
//            SpeedReg_Count[edge] = 0;
//            
//            float angleDelta = 0.2f * (SpeedReg_Delta[edge][0]
//                    + SpeedReg_Delta[edge][1] + SpeedReg_Delta[edge][2]
//                    + SpeedReg_Delta[edge][3] + SpeedReg_Delta[edge][4]);
        
        
        /**********************************************************************/
        if (target != targetOld){
//            SpeedReg_I[edge] = sgn(outP) * MotRot_SPD_Max * (((float) Acts_ROT_GetSpeedLimit(edge)) * 0.01f);
            SpeedReg_I[edge] = copysgn(MotRot_SPD_Max, outP) * (((float) Acts_ROT_GetSpeedLimit(edge)) * 0.01f);
            
            targetOld = target;
        }
        float angleDelta = Sens_ENC_GetDelta(edge);

        
        float angleDeltaOut = angleDelta * 500.0f;
        SPD_PID_Monitor[0] = (int16_t) angleDeltaOut;

//        float errorSPD = sgn(outP) * Speed_DEG[edge] - angleDelta;        
        float errorSPD = copysgn(Speed_DEG[edge], outP) - angleDelta;
        
            // speed PID components
//            SpeedReg_P0[edge] = 10.0f * ((float)TMP_SpeedReg_GainP) * errorSPD;
        SpeedReg_I[edge] += ((float)TMP_SpeedReg_GainI) * errorSPD;
//            SpeedReg_D0[edge] = ((float)TMP_SpeedReg_GainD) * (errorSPD - errorSPDOld1[edge]) / MotRot_PID_period;
//        errorSPDOld1[edge] = errorSPD;
            

//        // limit proportional
//        if (SpeedReg_P0[edge] < -MotRot_SPD_Pmax) SpeedReg_P0[edge] = -MotRot_SPD_Pmax;
//        else if (SpeedReg_P0[edge] > MotRot_SPD_Pmax) SpeedReg_P0[edge] = MotRot_SPD_Pmax;
        
        // limit integral
        if (SpeedReg_I[edge] < -MotRot_SPD_Imax) SpeedReg_I[edge] = -MotRot_SPD_Imax;
        else if (SpeedReg_I[edge] > MotRot_SPD_Imax) SpeedReg_I[edge] = MotRot_SPD_Imax;
        //ok
        int16_t AverageSpeedOut = (int16_t)SpeedReg_I[edge] / 2;
        Acts_ROT_SetSPDAvgOut(edge, (int8_t)AverageSpeedOut);
//        Acts_ROT_SetSPDAvgOut(edge, 0);
        
        // limit derivative
//        if (SpeedReg_D0[edge] < -MotRot_SPD_Dmax) SpeedReg_D0[edge] = -MotRot_SPD_Dmax;
//        else if (SpeedReg_D0[edge] > MotRot_SPD_Dmax) SpeedReg_D0[edge] = MotRot_SPD_Dmax;
        
        // speed controller output
//        float outS = SpeedReg_P0[edge] + SpeedReg_I[edge] + SpeedReg_D0[edge];
//        float outS = SpeedReg_I[edge] + SpeedReg_D0[edge];
        float outS = SpeedReg_I[edge];
//        
////        if (outS < -MotRot_SPD_Max) outS = -MotRot_SPD_Max;
////        else if (outS > MotRot_SPD_Max) outS = MotRot_SPD_Max;
//                
        // ensure speed never changes target direction from position PID
        if ((OUT > 0) && (outS < 0)) outS = 0;
        else if ((OUT < 0) && (outS > 0)) outS = 0;

        // speed controller output scaled by position controller output
//        OUT = fabs(outP) * MotRot_PID_OneOverMax * outS;
        OUT = fabsf(outP) * MotRot_PID_OneOverMax * outS;
//        
////        SPD_PID_Monitor[0] = (int16_t)(Sens_ENC_GetDelta(edge) * 500);
////        static uint16_t loopsSinceLastOffset = 0;
////        loopsSinceLastOffset++;
////        if (OffsetUpdateFlag){
////            SPD_PID_Monitor[1] = loopsSinceLastOffset;
////            loopsSinceLastOffset = 0;
////            OffsetUpdateFlag = false;
////        }
//        
        SPD_PID_Monitor[2] = (int16_t)SpeedReg_I[edge];
////        SPD_PID_Monitor[2] = (int16_t)SpeedReg_D0[edge];
//
        if (SPD_AvgFlag[edge]){
            SPD_AvgFlag[edge] = false;
            SpeedReg_I[edge] = SpeedReg_I[edge] * 0.5f + Acts_ROT_GetSPDAvgNeighbour(edge);
//            SpeedReg_I[edge] = SpeedReg_I[edge] * 0.5f + SpeedReg_I[edge] * 0.5f;
        }
        /**********************************************************************/
        
//        float delta = Sens_ENC_GetDelta(edge); // change in angle reading
        // Speed_DEG is const, sign dictates direction of outP
//        float errorSPD = ((float)sgn(outP)) * Speed_DEG[edge] - delta;
//        
//        SPD_I[edge]  += ((float)TMP_SpeedReg_GainI) * errorSPD * MotRot_PID_period;
//        float SPD_D = ((float)TMP_SpeedReg_GainD) * (errorSPD - errorSPDOld1[edge]) / MotRot_PID_period;
//        
//        if (SPD_I[edge] < -MotRot_SPD_Imax) SPD_I[edge] = -MotRot_SPD_Imax;
//        else if (SPD_I[edge] > MotRot_SPD_Imax) SPD_I[edge] = MotRot_SPD_Imax;
//        
//        if (SPD_D < -MotRot_SPD_Dmax) SPD_D = -MotRot_SPD_Dmax;
//        else if (SPD_D > MotRot_SPD_Dmax) SPD_D = MotRot_SPD_Dmax;
//        
//        float outS = 10.f * TMP_SpeedReg_GainP * errorSPD + SPD_I[edge] + SPD_D;
//        
//        OUT = (OUT / ((float)MotRot_PID_Max)) * outS;
//        
        /**********************************************************************/
        
//        // first errorSPD will be Speed_DEG
//        // second errorSPD will likely be same direction as Speed_deg
//        // Eventually, errorSPD is negative as initial speed is quite fast
//        // feed forward of output speed (proportional to set speed)
//        // Speed_100 causes outP to shrink to small (80 at 20 spd)
////        OUT = outP * (((float) Speed_100[edge]) / 255.0);
//        
//        float errorSPD = ((float)sgn(outP)) * Speed_DEG[edge] - Sens_ENC_GetDelta(edge);
//
//        // initialise speed regulation to max when new target
////        if ((TMP_Switch == 2) || (TMP_Switch == 3))
////        if (target != targetOld) SpeedReg_I[edge] = sgn(OUT)*MotRot_SPD_Max - OUT;
//        if (target != targetOld){
//            SpeedReg_I[edge] = ((float)sgn(outP)) * MotRot_SPD_Max * (((float) Speed_100[edge]) / 255.0f);
//            SpeedReg_D0[edge] = 0;
//            SpeedReg_D1[edge] = 0;
//            SpeedReg_D2[edge] = 0;
//            SpeedReg_P0[edge] = 0;
//            SpeedReg_P1[edge] = 0;
//            SpeedReg_P2[edge] = 0;
//        }
//        targetOld = target;
//        
//        // update proportional moving average
//        SpeedReg_P4[edge] = SpeedReg_P3[edge];
//        SpeedReg_P3[edge] = SpeedReg_P2[edge];
//        SpeedReg_P2[edge] = SpeedReg_P1[edge];
//        SpeedReg_P1[edge] = SpeedReg_P0[edge];
//        
//        // update derivative moving average
//        SpeedReg_D2[edge] = SpeedReg_D1[edge];
//        SpeedReg_D1[edge] = SpeedReg_D0[edge];
//        
//        // PID components
//        SpeedReg_P0[edge] = 100.0f * ((float)TMP_SpeedReg_GainP) * errorSPD;
//        SpeedReg_I[edge] += ((float)TMP_SpeedReg_GainI) * errorSPD;
//        SpeedReg_D0[edge] = ((float)TMP_SpeedReg_GainD) * (errorSPD - errorSPDOld1[edge]) / MotRot_PID_period;
//        errorSPDOld1[edge] = errorSPD;
//        
//        // limit proportional
//        if (SpeedReg_P0[edge] < -MotRot_SPD_Pmax) SpeedReg_P0[edge] = -MotRot_SPD_Pmax;
//        else if (SpeedReg_P0[edge] > MotRot_SPD_Pmax) SpeedReg_P0[edge] = MotRot_SPD_Pmax;
//        
//        // average proportional component
//        float SpeedReg_P = 0.33f * (SpeedReg_P0[edge] + SpeedReg_P1[edge]
//                + SpeedReg_P2[edge]);
//        
//        // limit integral
//        if (SpeedReg_I[edge] < -MotRot_SPD_Imax) SpeedReg_I[edge] = -MotRot_SPD_Imax;
//        else if (SpeedReg_I[edge] > MotRot_SPD_Imax) SpeedReg_I[edge] = MotRot_SPD_Imax;
//        
//        // limit derivative
//        if (SpeedReg_D0[edge] < -MotRot_SPD_Dmax) SpeedReg_D0[edge] = -MotRot_SPD_Dmax;
//        else if (SpeedReg_D0[edge] > MotRot_SPD_Dmax) SpeedReg_D0[edge] = MotRot_SPD_Dmax;
//        
//        float SpeedReg_D = 0.33f * (SpeedReg_D0[edge] + SpeedReg_D1[edge]
//                + SpeedReg_D2[edge]);
//        
//        // speed controller output
//        float outS = SpeedReg_P + SpeedReg_I[edge] + SpeedReg_D;
//        
//        if (outS < -MotRot_SPD_Max) outS = -MotRot_SPD_Max;
//        else if (outS > MotRot_SPD_Max) outS = MotRot_SPD_Max;
//                
//        // ensure speed never changes target direction from position PID
//        if ((OUT > 0) && (outS < 0)) outS = 0;
//        else if ((OUT < 0) && (outS > 0)) outS = 0;
//
//        // speed controller output scaled by position controller output
//        OUT = (fabs(outP) / ((float)MotRot_PID_Max)) * outS;
//        
////        SPD_PID_Monitor[0] = (int16_t)SpeedReg_P;
//        SPD_PID_Monitor[0] = (int16_t)(Sens_ENC_GetDelta(edge) * 500);
//        SPD_PID_Monitor[1] = (int16_t)SpeedReg_I[edge];
//        SPD_PID_Monitor[2] = (int16_t)SpeedReg_D;
        
        /**********************************************************************/

        /**********************************************************************/
        
//        // first errorSPD will be Speed_DEG
//        // second errorSPD will likely be same direction as Speed_deg
//        // Eventually, errorSPD is negative as initial speed is quite fast
//        // feed forward of output speed (proportional to set speed)
//        // Speed_100 causes outP to shrink to small (80 at 20 spd)
//        OUT = outP * (((float) Speed_100[edge]) / 255.0);
//        
//        // initialise speed regulation to max when new target
////        if ((TMP_Switch == 2) || (TMP_Switch == 3))
////        if (target != targetOld) SpeedReg_I[edge] = sgn(OUT)*MotRot_SPD_Max - OUT;
//        if (target != targetOld) SpeedReg_I[edge] = 0;
//        targetOld = target;
//        float derrr  = 0;
//
//
//        derrr = ((float)TMP_SpeedReg_GainD) * (errorSPD - errorSPDOld1[edge]) / MotRot_PID_period;
//        SpeedReg_I[edge] += ((float)TMP_SpeedReg_GainI) * errorSPD * MotRot_PID_period;
//
//        errorSPDOld1[edge] = errorSPD;
//        
//        if (sgn(SpeedReg_I[edge]) != sgn(OUT)){
//            if (fabs(OUT) - (fabs(SpeedReg_I[edge])) < 0)
//                SpeedReg_I[edge] = -OUT;
//        } else if (fabs(OUT + SpeedReg_I[edge]) > MotRot_SPD_Max){
//            SpeedReg_I[edge] = sgn(OUT)*MotRot_SPD_Max - OUT;
//        }
//        
//        float outS = OUT + SpeedReg_I[edge] + derrr;
//
//        // ensure speed never changes target direction from position PID
//        if ((OUT > 0) && (outS < 0)) outS = 0;
//        else if ((OUT < 0) && (outS > 0)) outS = 0;
//        
        /**********************************************************************/
        
        // if desired direction change, reset integral and derivative
//        if (((outPOld[edge] > 0) && (outP <= 0))
//                || ((outPOld[edge] < 0) && (outP >= 0))){
//            SPD_I[edge] = 0;
//            errorSPDOld5[edge] = 0;
//            errorSPDOld4[edge] = 0;
//            errorSPDOld3[edge] = 0;
//            errorSPDOld2[edge] = 0;
//            errorSPDOld1[edge] = 0;
//        }
//        outPOld[edge] = outP;

        // integral component
//        SPD_I[edge] += errorSPD * MotRot_SPD_kI;
//        SPD_I[edge] += errorSPD * TMP_GainI;
//        float SPD_P = MotRot_SPD_kP * errorSPD;
//        if (SPD_I[edge] < (-MotRot_SPD_Max - OUT - SPD_P))
//            SPD_I[edge] = -MotRot_SPD_Max - OUT - SPD_P;
//        else if (SPD_I[edge] > (MotRot_SPD_Max - OUT - SPD_P))
//            SPD_I[edge] = MotRot_SPD_Max - OUT - SPD_P;
//        if (SPD_I[edge] < (-MotRot_SPD_Max))
//            SPD_I[edge] = -MotRot_SPD_Max;
//        else if (SPD_I[edge] > (MotRot_SPD_Max))
//            SPD_I[edge] = MotRot_SPD_Max;
//        
//        if (TMP_Switch == 1){
//            if ((SPD_I[edge] > 0) && (OUT > 0)){
//                if ((SPD_I[edge] + OUT) > MotRot_SPD_Max)
//                    SPD_I[edge] = MotRot_SPD_Max - OUT;
//            } else if ((SPD_I[edge] < 0) && (OUT < 0)) {
//                if ((SPD_I[edge] + OUT) < -MotRot_SPD_Max)
//                    SPD_I[edge] = -MotRot_SPD_Max - OUT;
//            }
//        }
        
        // derivative reduction of integral term
//        float SPD_D = ((errorSPD - errorSPDOld1[edge]) +
//                (errorSPDOld1[edge] - errorSPDOld2[edge]) +
//                (errorSPDOld2[edge] - errorSPDOld3[edge]) +
//                (errorSPDOld3[edge] - errorSPDOld4[edge]) +
//                (errorSPDOld4[edge] - errorSPDOld5[edge])) / 5;
//        errorSPDOld5[edge] = errorSPDOld4[edge];
//        errorSPDOld4[edge] = errorSPDOld3[edge];
//        errorSPDOld3[edge] = errorSPDOld2[edge];
//        errorSPDOld2[edge] = errorSPDOld1[edge];
//        errorSPDOld1[edge] = errorSPD;
//        SPD_I[edge] += SPD_D * MotRot_SPD_kD / MotRot_PID_period;
        
        // if change in error is less than 10% of desired speed 
        // and speed error bigger than 90% of desired speed - boost to start
//        if ((fabs(SPD_D) < fabs(0.1*Speed_DEG[edge])) 
//                && (fabs(errorSPD) > fabs(0.9*Speed_DEG[edge])))
//            SPD_I[edge] += sgn(errorSPD)*5;
        
//        float outS = OUT + SPD_I[edge] + SPD_P + SPD_D * MotRot_SPD_kD / MotRot_PID_period;;
//        float outS = OUT + SPD_I[edge];
        
//        // limit speed duty cycle
//        if (outS < -MotRot_SPD_Max) OUT = -MotRot_SPD_Max;
//        else if (outS > MotRot_SPD_Max) OUT = MotRot_SPD_Max;
//        else OUT = outS;
//    } else { // reset integral
//        SPD_I[edge] = 0;
    }
    Acts_ROT_Out(edge, (int16_t) OUT); // output pwm value
}

void Acts_ROT_TempUpdateControl(uint8_t select, uint8_t value){
    if (select == 0) TMP_SpeedReg_GainP = value;
    else if (select == 1) TMP_SpeedReg_GainI = value;
    else if (select == 2) TMP_SpeedReg_GainD = value;
}

uint8_t Acts_ROT_ReturnTempSwitch(){
    return TMP_Switch;
}

int16_t Acts_ROT_TempSPDMonitor(uint8_t i){
    return SPD_PID_Monitor[i];
}

/* ******************** EXECUTE WIGGLE ************************************** */
void Acts_ROT_Wiggle(uint8_t edge) {
    static uint16_t Wgl_Count[3] = {0, 0, 0};
    Wgl_Count[edge]++;
    if (Wgl_Count[edge] <= (MotRot_WiggleTime * 100)) {
        if (Wgl_Count[edge] <= MotRot_WiggleTime * 60) {
            Acts_ROT_Out(edge, 30);
            if (Flg_EdgeSyn[edge]) {
                Wgl_Count[edge] = MotRot_WiggleTime * 61;
                if (Flg_DriveAndCouple[edge]) {
                    uint8_t i;
                    for (i = 0; i < 3; i++) if (i != edge) Acts_ROT_Out(i, 0);
                }
            }
        } else if (Wgl_Count[edge] <= MotRot_WiggleTime * 90)
            Acts_ROT_Out(edge, -30);
        else if (Wgl_Count[edge] <= MotRot_WiggleTime * 98)
            Acts_ROT_Out(edge, 30);
        else
            Acts_ROT_Out(edge, -30);
    } else {
        Acts_ROT_Out(edge, 0);
        Mnge_DAC_Set(edge, Trq_Limit[edge]);
        Flg_EdgeWig[edge] = false;
        Wgl_Count[edge] = 0;
    }
}

/* ******************** DRIVE FUNCTION ************************************** */
void Acts_ROT_Drive(uint8_t speed, int8_t curve, uint8_t edge, uint8_t direc) {
    float a, b, c; // extension values from 180
    float Mo = curve * 137.9;
    float Sa = speed * 4;
    if (!direc) Sa = -1 * Sa; // inwards or outwards

    switch (edge) {
        case 0:
            a = 180 + (MotLin_MAX_1 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_1 - MotLin_MIN_1);
            b = 180 + (MotLin_MAX_2 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_2 - MotLin_MIN_2);
            c = 180 + (MotLin_MAX_3 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_3 - MotLin_MIN_3);
            break;
        case 1:
            a = 180 + (MotLin_MAX_2 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_2 - MotLin_MIN_2);
            b = 180 + (MotLin_MAX_3 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_3 - MotLin_MIN_3);
            c = 180 + (MotLin_MAX_1 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_1 - MotLin_MIN_1);
            break;
        case 2:
            a = 180 + (MotLin_MAX_3 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_3 - MotLin_MIN_3);
            b = 180 + (MotLin_MAX_1 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_1 - MotLin_MIN_1);
            c = 180 + (MotLin_MAX_2 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_2 - MotLin_MIN_2);
            break;
        default:
            a = 180 + (MotLin_MAX_1 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_1 - MotLin_MIN_1);
            b = 180 + (MotLin_MAX_2 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_2 - MotLin_MIN_2);
            c = 180 + (MotLin_MAX_3 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_3 - MotLin_MIN_3);
            break;
    }

    // vertex angles (float alpha = acosf((b*b + c*c - a*a)/(2*b*c));)
    float beta = acosf((a * a + c * c - b * b) / (2 * a * c));
    float gamm = acosf((a * a + b * b - c * c) / (2 * a * b));

    // wheel coordinates (for a: [WHEEL, 0])
    float Wb[2] = {(b - WHEEL) * cosf(gamm), (b - WHEEL) * sinf(gamm)};
    float Wc[2] = {a - WHEEL * cosf(beta), WHEEL * sinf(beta)};

    // second point in wheel direction
    float Wb2[2] = {Wb[0] - cosf(PI / 2 - gamm), Wb[1] + sinf(PI / 2 - gamm)};
    float Wc2[2] = {Wc[0] + cosf(PI / 2 - beta), Wc[1] + sinf(PI / 2 - beta)};

    // centroid coordinates
    float D[2] = {(b * cosf(gamm) + a) / 3, b * sinf(gamm) / 3};

    // moment arm of wheel force to centroid
    float Da = fabsf(D[0] - WHEEL);
    float Db = fabsf((Wb2[1] - Wb[1]) * D[0]
            - (Wb2[0] - Wb[0]) * D[1] + Wb2[0] * Wb[1] - Wb2[1] * Wb[0])
            / sqrtf(powf(Wb2[1] - Wb[1], 2) + powf(Wb2[0] - Wb[0], 2));
    float Dc = fabsf((Wc2[1] - Wc[1]) * D[0]
            - (Wc2[0] - Wc[0]) * D[1] + Wc2[0] * Wc[1] - Wc2[1] * Wc[0])
            / sqrtf(powf(Wc2[1] - Wc[1], 2) + powf(Wc2[0] - Wc[0], 2));

    // wheel speeds
    float Sc = (Mo - Sa * Da) / (Db * cosf(PI / 2 - beta) / cosf(PI / 2 - gamm) + Dc);
    float Sb = Sc * cosf(PI / 2 - beta) / cosf(PI / 2 - gamm);

    Sc = SxOUT*Sc;
    Sb = SxOUT*Sb;

    uint8_t i;
    for (i = 0; i < 3; i++) Acts_ROT_SetDrvInterval(i, 5); // lasts for 1 sec

    // output depending on driving edge
    switch (edge) {
        case 0:
            Acts_ROT_Out(0, Sa);
            Acts_ROT_Out(1, Sb);
            Acts_ROT_Out(2, Sc);
            break;
        case 1:
            Acts_ROT_Out(1, Sa);
            Acts_ROT_Out(2, Sb);
            Acts_ROT_Out(0, Sc);
            break;
        case 2:
            Acts_ROT_Out(2, Sa);
            Acts_ROT_Out(0, Sb);
            Acts_ROT_Out(1, Sc);
            break;
        default:
            break;
    }
}

/* ******************** DRIVE HANDLE ************************************* */
void Acts_ROT_DrvHandle() {
    uint8_t edge;
    for (edge = 0; edge < 3; edge++) {
        if (DrvInterval[edge]) {
            DrvInterval[edge]--;
        } else {
            Flg_Drive[edge] = false;
        }
    }
}

void Acts_ROT_SetDrvInterval(uint8_t edge, uint8_t interval) {
    DrvInterval[edge] = interval;
    Flg_Drive[edge] = true;
}

/* ******************** ACTIVATE WIGGLE ************************************* */
void Acts_ROT_SetWiggle(uint8_t edge) {
    Mnge_DAC_Set(edge, MotRot_WiggleTorque);
    Flg_EdgeWig[edge] = true;
}

/* ******************** ROTARY MOTOR CURRENT LIMIT ************************** */
void Acts_ROT_SetCurrentLimit(uint8_t edge, uint8_t limit) {
    Trq_Limit[edge] = limit; // save limit so wiggle remembers
    Mnge_DAC_Set(edge, limit);
}

/* ******************** SET ROTARY MOTOR SPEED LIMIT ************************ */
void Acts_ROT_SetSpeedLimit(uint8_t edge, uint8_t limit) {
    if (limit > 100) limit = 100;
    Speed_DEG[edge] = ((float) limit) * 0.01f * MotRot_SpeedMax * MotRot_PID_period;
    Speed_100[edge] = limit;
}

/* ******************** RETURN ROTARY MOTOR SPEED LIMIT ********************* */
uint8_t Acts_ROT_GetSpeedLimit(uint8_t edge) {
    return Speed_100[edge];
}

/* ******************** GET DESIRED ANGLE *********************************** */
uint16_t Acts_ROT_GetTarget(uint8_t edge) {
    return Ang_Desired[edge];
}

/* ******************** SET DESIRED ANGLE *********************************** */
void Acts_ROT_SetTarget(uint8_t edge, uint16_t desired) {
    Ang_Desired[edge] = desired;
    Flg_EdgeAct[edge] = false; // reset act flag until cmd verified with neighbour
    Flg_EdgeReq_Ang[edge] = true;
    PID_I[edge] = 0;
//    SPD_I[edge] = 0;
}

/* ******************** RETURN FORMATTED ANGLE ****************************** */
uint16_t Acts_ROT_GetAngle(uint8_t edge, bool WithLiveOffset) {
    int16_t rawAngle = (int16_t)(10*Sens_ENC_Get(edge, WithLiveOffset));
    return (uint16_t) map(rawAngle, -1800, 1800, 0, 3600);
}

/* ******************** RETURN WHETHER ALL IN DESIRED RANGE ***************** */
bool Acts_ROT_InRange(uint8_t edge) {
    uint16_t diff = abs(Acts_ROT_GetAngle(edge, true) - Acts_ROT_GetTarget(edge));
    if (diff <= MotRot_OkRange) return true;
    return false;
}

float sgn(float value){
    if (value > 0) return 1.0f;
    if (value < 0) return -1.0f;
    return 0.0f;
}


float copysgn(float value, float check){
    if (check > 0.0f) return value;
    if (check < 0.0f) return -value;
    return 0.0f;
}
//
//uint8_t Acts_ROT_GetSPDIntegral(uint8_t edge){
//    return (uint8_t)((int16_t)(SpeedReg_I[edge]/2) + 127);
//}
//
//void Acts_ROT_AvgSPDIntegral(uint8_t edge, uint8_t value){
//    SpeedReg_I[edge] = (SpeedReg_I[edge] + ((float)(((int16_t)value) - 127)*2)) / 2;
//}


//int8_t Acts_ROT_GetSPDIntegral(uint8_t edge){
//    return (int8_t)(SpeedReg_I[edge] / 2.0f);
//}
//
//void Acts_ROT_AvgSPDIntegral(uint8_t edge, int8_t value){
//    SpeedReg_I[edge] = (SpeedReg_I[edge] + (((float)(value)) * 2.0f)) / 2.0f;
//}


void Acts_ROT_SetSPDAvgOut(uint8_t edge, int8_t value){
    SPD_AvgOut[edge] = value;
}

uint8_t Acts_ROT_GetSPDAvgOut(uint8_t edge){
    return (uint8_t)SPD_AvgOut[edge];
}

void Acts_ROT_SetSPDAvgNeighbour(uint8_t edge, uint8_t value){
    SPD_AvgFlag[edge] = true;
    SPD_AvgIn[edge] = (int8_t)value;
}

int8_t Acts_ROT_GetSPDAvgNeighbour(uint8_t edge){
    return SPD_AvgIn[edge];
}

void Acts_ROT_ResetOffsetInterval(){
    OffsetUpdateFlag = true;
}