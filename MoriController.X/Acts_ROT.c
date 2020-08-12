#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <libpic30.h>
#include "Defs.h"
#include "Mnge_DAC.h"

float rPID_eOld[3] = {0, 0, 0};
float rPID_I[3] = {0, 0, 0};
float rPID_D[3] = {0, 0, 0};

/* ******************** ROTARY MOTOR OUTPUTS ******************************** */
void Acts_ROT_Out(uint8_t edge, int16_t duty) {
    if (!STAT_MotRot_Active) duty = 0; // linear motors off
    if (!Flg_MotRot_Active) duty = 0;
    switch (edge) {
        case 0:
            if (duty > 0) ROT_DIR_1 = 1; // direction output
            else ROT_DIR_1 = 0;
            // if (not connected) or (connected and synchronised)
            if ((!Flg_EdgeCon[edge]) || Flg_EdgeSyn[edge]) {
                PWM_Set(ROT_PWM_1, abs(duty)); // pwm output
            }
            break;
            
        case 1:
            if (duty > 0) ROT_DIR_2 = 1; // direction output
            else ROT_DIR_2 = 0;
            // if (not connected) or (connected and synchronised)
            if ((!Flg_EdgeCon[edge]) || Flg_EdgeSyn[edge]) {
                PWM_Set(ROT_PWM_2, abs(duty)); // pwm output
            }
            break;
            
        case 2:
            if (duty > 0) ROT_DIR_3 = 1; // direction output
            else ROT_DIR_3 = 0;
            // if (not connected) or (connected and synchronised)
            if ((!Flg_EdgeCon[edge]) || Flg_EdgeSyn[edge]) {
                PWM_Set(ROT_PWM_3, abs(duty)); // pwm output
            }
            break;
            
        default:
            break;
    }
}

/* ******************** ROTARY MOTOR PID ************************************ */
void Acts_ROT_PID(uint8_t edge, float current, float desired) {
    // avoid bad control inputs
    if (desired < -MotRot_AngleRange / 2) desired = -MotRot_AngleRange / 2;
    else if (desired > MotRot_AngleRange / 2) desired = MotRot_AngleRange / 2;

    // calculate error
    static float error;
    error = desired - current;

    // calculate integral component
    rPID_I[edge] += error; // * MotRot_PID_dt;

    // limit integral component
    if (rPID_I[edge] < -MotRot_PID_Imax) rPID_I[edge] = -MotRot_PID_Imax;
    else if (rPID_I[edge] > MotRot_PID_Imax) rPID_I[edge] = MotRot_PID_Imax;

    // calculate derivative component
    rPID_D[edge] = (error - rPID_eOld[edge]); // / MotRot_PID_dt;
    rPID_eOld[edge] = error;

    // calculate PID output
    static float outf;
    outf = MotRot_PID_kP * error + MotRot_PID_kI * rPID_I[edge] + MotRot_PID_kD * rPID_D[edge];

    // limit duty cycle
    if (outf < -MotRot_PID_Max) outf = -MotRot_PID_Max;
    else if (outf > MotRot_PID_Max) outf = MotRot_PID_Max;

    // update motor control output
    Acts_ROT_Out(edge, (int16_t) outf);
}

/* ******************** ROTARY MOTOR CURRENT ******************************** */
void Acts_ROT_Limit(uint8_t edge, uint8_t voltagelevel) {
    Mnge_DAC_Write(edge, voltagelevel);
}
