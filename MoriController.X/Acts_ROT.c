//#include <xc.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <libpic30.h>
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
int16_t Spd_Limit[3] = {MotRot_PID_Max, MotRot_PID_Max, MotRot_PID_Max};
uint8_t DrvInterval[3] = {0, 0, 0};

#define WHEEL 68.15f // wheel distance from vertex
#define SxOUT 0.9f // output speed factor for non-primary wheels

/* ******************** ROTARY MOTOR OUTPUTS ******************************** */
void Acts_ROT_Out(uint8_t edge, int16_t duty) {
    if (!MODE_MotRot_Active || !FLG_MotRot_Active) duty = 0; // rotary motors off
    else if (duty > Spd_Limit[edge]) duty = Spd_Limit[edge];
    else if (duty < -Spd_Limit[edge]) duty = -Spd_Limit[edge];
    
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
        rPID_I[edge] += error * MotRot_PID_period; // calculate integral component
    else
        rPID_I[edge] = 0;
    
    // limit integral component
    if (rPID_I[edge] < -MotRot_PID_Imax) rPID_I[edge] = -MotRot_PID_Imax;
    else if (rPID_I[edge] > MotRot_PID_Imax) rPID_I[edge] = MotRot_PID_Imax;

    // calculate derivative component
    rPID_D[edge] = (error - rPID_eOld[edge]) / MotRot_PID_period;  
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
        if (Wgl_Count[edge] <= MotRot_WiggleTime * 60){
            Acts_ROT_Out(edge, 600);
            if (Flg_EdgeSyn[edge]){
                Wgl_Count[edge] = MotRot_WiggleTime * 61;
                if (Flg_DriveAndCouple[edge]){
                    uint8_t i;
                    for (i = 0; i < 3; i++) if (i != edge) Acts_ROT_Out(i, 0);
                }
            }
        }
        else if (Wgl_Count[edge] <= MotRot_WiggleTime * 90)
            Acts_ROT_Out(edge, -600);
        else if (Wgl_Count[edge] <= MotRot_WiggleTime * 98)
            Acts_ROT_Out(edge, 600);
        else 
            Acts_ROT_Out(edge, -600);
    } else {
        Acts_ROT_Out(edge, 0);
        Mnge_DAC_Set(edge, Trq_Limit[edge]);
        Flg_EdgeWig[edge] = false;
        Wgl_Count[edge] = 0;
    }
}

/* ******************** DRIVE FUNCTION ************************************** */
void Acts_ROT_Drive(uint8_t speed, int8_t curve, uint8_t edge, uint8_t direc){
    float Mo = curve * 137.9;
    float Sa = speed * 4;
    if (!direc) { // inwards or outwards
        Sa = -1 * Sa;
    }

    float a, b, c; // extension values from 180
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
void Acts_ROT_DrvHandle(){
    uint8_t edge;
    for (edge = 0; edge < 3; edge++){
        if (DrvInterval[edge]){
            DrvInterval[edge]--;
        } else {
            Flg_Drive[edge] = false;
        }
    }
}

void Acts_ROT_SetDrvInterval(uint8_t edge, uint8_t interval){
    DrvInterval[edge] = interval;
    Flg_Drive[edge] = true;
}

/* ******************** ACTIVATE WIGGLE ************************************* */
void Acts_ROT_SetWiggle(uint8_t edge){
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
    Spd_Limit[edge] = ((uint16_t) limit)*4;
}

/* ******************** RETURN ROTARY MOTOR SPEED LIMIT ********************* */
uint8_t Acts_ROT_GetSpeedLimit(uint8_t edge) {
    return Spd_Limit[edge]/4;
}

/* ******************** GET DESIRED ANGLE *********************************** */
uint16_t Acts_ROT_GetTarget(uint8_t edge) {
    return Ang_Desired[edge];
}

/* ******************** SET DESIRED ANGLE *********************************** */
void Acts_ROT_SetTarget(uint8_t edge, uint16_t desired) {
    Ang_Desired[edge] = desired;
    Flg_EdgeReq_Ang[edge] = true;
}

/* ******************** RETURN FORMATTED ANGLE ****************************** */
uint16_t Acts_ROT_GetAngle(uint8_t edge) {
    float rawAngle = 10 * Sens_ENC_Get(edge);
    return (uint16_t) map((int16_t)rawAngle, -1800, 1800, 0, 3600);
}