#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <libpic30.h>
#include <stdbool.h>
#include "Defs.h"
#include "mcc_generated_files/pwm.h"

uint16_t MotLin_Desired[3] = {457, 457, 457};
float lPID_eOld[3] = {0, 0, 0};
float lPID_I[3] = {0, 0, 0};
uint8_t Stbl_Count[3] = {0, 0, 0};
int16_t Stbl_dOld[3] = {0, 0, 0};
bool Stbl_Flag[3] = {false, false, false};

/* ******************** LINEAR MOTOR OUTPUTS ******************************** */
void Acts_LIN_Out(uint8_t edge, int16_t duty) {
    if (!STAT_MotLin_Active) duty = 0; // linear motors off
    switch (edge) {
        case 0:
            if (duty > 0) LIN_DIR_1 = 0; // direction output
            else LIN_DIR_1 = 1;
            // if (not connected) or (connected and synchronised)
            if ((!Flg_EdgeCon[edge]) || Flg_EdgeSyn[edge]) {
                PWM_Set(LIN_PWM_1, abs(duty)); // pwm output
            }
            break;

        case 1:
            if (duty > 0) LIN_DIR_2 = 0; // direction output
            else LIN_DIR_2 = 1;
            // if (not connected) or (connected and synchronised)
            if ((!Flg_EdgeCon[edge]) || Flg_EdgeSyn[edge]) {
                PWM_Set(LIN_PWM_2, abs(duty)); // pwm output
            }
            break;

        case 2:
            if (duty > 0) LIN_DIR_3 = 0; // direction output
            else LIN_DIR_3 = 1;
            // if (not connected) or (connected and synchronised)
            if ((!Flg_EdgeCon[edge]) || Flg_EdgeSyn[edge]) {
                PWM_Set(LIN_PWM_3, abs(duty)); // pwm output
            }
            break;

        default:
            break;
    }
}

/* ******************** LINEAR MOTOR PID ************************************ */
void Acts_LIN_PID(uint8_t edge, uint16_t current, uint16_t desired) {
    // avoid bad control inputs
    switch (edge) {
        case 0:
            if (desired < MotLin_MIN_1) desired = MotLin_MIN_1;
            else if (desired > MotLin_MAX_1) desired = MotLin_MAX_1;
            break;

        case 1:
            if (desired < MotLin_MIN_2) desired = MotLin_MIN_2;
            else if (desired > MotLin_MAX_2) desired = MotLin_MAX_2;
            break;

        case 2:
            if (desired < MotLin_MIN_3) desired = MotLin_MIN_3;
            else if (desired > MotLin_MAX_3) desired = MotLin_MAX_3;
            break;

        default:
            break;
    }

    // calculate error
    float error = (float) (desired) - (float) (current);

    // if desired value changed, reset stable flag
    if (desired != Stbl_dOld[edge]) Stbl_Flag[edge] = false;
    Stbl_dOld[edge] = desired;

    // acceptable error band -> switch off motor
    if ((error > -0.5 * MotLin_PID_de) && (error < 0.5 * MotLin_PID_de)) {
        Stbl_Count[edge]++;
        if (Stbl_Count[edge] >= 100) Stbl_Flag[edge] = true;
    } else {
        Stbl_Count[edge] = 0;
    }

    // calculate integral component
    lPID_I[edge] += error;

    // limit integral component
    if (lPID_I[edge] < -MotLin_PID_Imax) lPID_I[edge] = -MotLin_PID_Imax;
    else if (lPID_I[edge] > MotLin_PID_Imax) lPID_I[edge] = MotLin_PID_Imax;

    // calculate PID output
    float outf = MotLin_PID_kP * error + MotLin_PID_kI * lPID_I[edge];

    // limit duty cycle
    if (outf < -MotLin_PID_Max) outf = -MotLin_PID_Max;
    else if (outf > MotLin_PID_Max) outf = MotLin_PID_Max;

    // slow down motors near min/max
    switch (edge) {
        case 0:
            if ((current < (MotLin_MIN_1 + MotLin_SlowRegion))
                    || (current > (MotLin_MAX_1 - MotLin_SlowRegion)))
                outf = outf / MotLin_SlowFactor;
            break;

        case 1:
            if ((current < (MotLin_MIN_2 + MotLin_SlowRegion))
                    || (current > (MotLin_MAX_2 - MotLin_SlowRegion)))
                outf = outf / MotLin_SlowFactor;
            break;

        case 2:
            if ((current < (MotLin_MIN_3 + MotLin_SlowRegion))
                    || (current > (MotLin_MAX_3 - MotLin_SlowRegion)))
                outf = outf / MotLin_SlowFactor;
            break;

        default:
            break;
    }

    // update motor control output
    if (Stbl_Flag[edge]) outf = 0; // extension value stable
    Acts_LIN_Out(edge, (int16_t) outf);
}

/* ******************** GET DESIRED EXTENSION ******************************* */
uint16_t Acts_LIN_GetTarget(uint8_t edge) {
    return MotLin_Desired[edge];
}

/* ******************** SET DESIRED EXTENSION ******************************* */
void Acts_LIN_SetTarget(uint8_t edge, uint16_t desired) {
    MotLin_Desired[edge] = desired;
}