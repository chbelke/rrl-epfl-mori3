#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <libpic30.h>
#include "Defs.h"
#include "Mnge_DAC.h"
#include "Sens_ENC.h"
#include "Acts_LIN.h"
#include "Acts_ROT.h"
#include "Coms_ESP.h"

uint16_t Ang_Desired[3] = {1800, 1800, 1800}; // -180.0 to 180.0 deg = 0 to 3600
uint8_t Trq_Limit[3] = {0, 0, 0}; // save torque limit during wiggle

/* ******************** ROTARY MOTOR OUTPUTS ******************************** */
void Acts_ROT_Out(uint8_t edge, int16_t duty) {
    if (!STAT_MotRot_Active) duty = 0; // rotary motors off
    if (!Flg_MotRot_Active) duty = 0;
    switch (edge) {
        case 0:
            if (duty > 0) ROT_DIR_1 = 1; // direction output
            else ROT_DIR_1 = 0;
            PWM_Set(ROT_PWM_1, abs(duty)); // pwm output
            break;
            
        case 1:
            if (duty > 0) ROT_DIR_2 = 1;
            else ROT_DIR_2 = 0; // direction output
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
    static float rPID_eOld[3] = {0, 0, 0};
    static float rPID_I[3] = {0, 0, 0};
    static float rPID_D[3] = {0, 0, 0};
    
    float desired = ((float)target) / 10 - 180;
    
    // avoid bad control inputs
    if (desired < -MotRot_AngleRange / 2) desired = -MotRot_AngleRange / 2;
    else if (desired > MotRot_AngleRange / 2) desired = MotRot_AngleRange / 2;

    // calculate error
    float error = desired - current;
    
    // avoid integral build up when far away
    if (abs(error) > 7) // if error is >6.69, kp results in max (ignoring kd)
        rPID_I[edge] += error * MotRot_PID_dt; // calculate integral component
    else
        rPID_I[edge] = 0;
    
    // limit integral component
    if (rPID_I[edge] < -MotRot_PID_Imax) rPID_I[edge] = -MotRot_PID_Imax;
    else if (rPID_I[edge] > MotRot_PID_Imax) rPID_I[edge] = MotRot_PID_Imax;

    // calculate derivative component
    rPID_D[edge] = (error - rPID_eOld[edge]) / MotRot_PID_dt;  
    rPID_eOld[edge] = error;
    
    // limit derivative component
    if (rPID_D[edge] < -MotRot_PID_Dmax) rPID_D[edge] = -MotRot_PID_Dmax;
    else if (rPID_D[edge] > MotRot_PID_Dmax) rPID_D[edge] = MotRot_PID_Dmax;

    // calculate PID output
    float outf = MotRot_PID_kP * error + MotRot_PID_kI * rPID_I[edge] + MotRot_PID_kD * rPID_D[edge];

    // limit duty cycle
    if (outf < -MotRot_PID_Max) outf = -MotRot_PID_Max;
    else if (outf > MotRot_PID_Max) outf = MotRot_PID_Max;
    
    // update motor control output
    Acts_ROT_Out(edge, (int16_t) outf);
}

/* ******************** EXECUTE WIGGLE ************************************** */
void Acts_ROT_Wiggle(uint8_t edge){
    static uint16_t Wgl_Count[3] = {0, 0, 0};
    Wgl_Count[edge]++;
    if (Wgl_Count[edge] <= (MotRot_WiggleTime * 100)){
        if (Wgl_Count[edge] <= MotRot_WiggleTime * 67){
            Acts_ROT_Out(edge, 1024);
            if (Flg_EdgeSyn[edge]) Wgl_Count[edge] = 67;
        }
        else if (Wgl_Count[edge] <= MotRot_WiggleTime * 85)
            Acts_ROT_Out(edge, -1024);
        else if (Wgl_Count[edge] <= MotRot_WiggleTime * 94)
            Acts_ROT_Out(edge, 1024);
        else 
            Acts_ROT_Out(edge, -1024);
    } else {
        Acts_ROT_Out(edge, 0);
        Mnge_DAC_Set(edge, Trq_Limit[edge]);
        Flg_EdgeWig[edge] = false;
        Wgl_Count[edge] = 0;
    }
}

/* ******************** ACTIVATE WIGGLE ************************************* */
void Acts_ROT_SetWiggle(uint8_t edge){
    Flg_EdgeWig[edge] = true;
    Mnge_DAC_Set(edge, MotRot_WiggleTroque);
}

/* ******************** ROTARY MOTOR CURRENT LIMIT ************************** */
void Acts_ROT_Limit(uint8_t edge, uint8_t limit) {
    Trq_Limit[edge] = limit; // save limit so wiggle remembers
    Mnge_DAC_Set(edge, limit);
}

/* ******************** GET DESIRED ANGLE *********************************** */
uint16_t Acts_ROT_GetTarget(uint8_t edge) {
    return Ang_Desired[edge];
}


/* ******************** SET DESIRED ANGLE *********************************** */
void Acts_ROT_SetTarget(uint8_t edge, uint16_t desired) {
    Ang_Desired[edge] = desired;
    Flg_EdgeRequest_Ang[edge] = true;
}

/* ******************** RETURN FORMATTED ANGLE ****************************** */
uint16_t Acts_ROT_GetCurrent(uint8_t edge) {
    float rawAngle = 10 * Sens_ENC_Get(edge);
    return (uint16_t) map((int16_t)rawAngle, -1800, 1800, 0, 3600);
}