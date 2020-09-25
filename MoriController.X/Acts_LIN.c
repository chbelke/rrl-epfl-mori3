#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <libpic30.h>
#include <stdbool.h>
#include "Defs.h"
#include "Defs_Mod.h"
#include "mcc_generated_files/pwm.h"
#include "mcc_generated_files/adc1.h"

uint8_t Ext_Desired[3] = {60, 60, 60};

/* ******************** ARDUINO MAP FUNCTION ******************************** */
long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/* ******************** LINEAR MOTOR OUTPUTS ******************************** */
void Acts_LIN_Out(uint8_t edge, int16_t duty) {
    if (!STAT_MotLin_Active) duty = 0; // linear motors off
    if (!Flg_MotLin_Active) duty = 0; 
    switch (edge) {
        case 0:
            if (duty > 0) LIN_DIR_1 = 0; // direction output
            else LIN_DIR_1 = 1;
            PWM_Set(LIN_PWM_1, abs(duty)); // pwm output
            break;

        case 1:
            if (duty > 0) LIN_DIR_2 = 0; // direction output
            else LIN_DIR_2 = 1;
            PWM_Set(LIN_PWM_2, abs(duty)); // pwm output
            break;

        case 2:
            if (duty > 0) LIN_DIR_3 = 0; // direction output
            else LIN_DIR_3 = 1;
            PWM_Set(LIN_PWM_3, abs(duty)); // pwm output
            break;

        default:
            break;
    }
}

/* ******************** LINEAR MOTOR PID ************************************ */
void Acts_LIN_PID(uint8_t edge, uint16_t current, uint8_t target) {
    static float lPID_I[3] = {0, 0, 0};
    static uint8_t Stbl_Count[3] = {0, 0, 0};
    static int16_t Stbl_dOld[3] = {0, 0, 0};
    static bool Stbl_Flag[3] = {false, false, false};

    // avoid bad control inputs
    if (target < MotLin_MinInput) target = MotLin_MinInput;
    else if (target > MotLin_MaxInput) target = MotLin_MaxInput;
    
    // map input to max and min of potentiometer
    int16_t OUT_min, OUT_max;
    switch (edge) {
        case 0:
            OUT_min = MotLin_MIN_1;
            OUT_max = MotLin_MAX_1;
            break;
        case 1:
            OUT_min = MotLin_MIN_2;
            OUT_max = MotLin_MAX_2;
            break;
        case 2:
            OUT_min = MotLin_MIN_3;
            OUT_max = MotLin_MAX_3;
            break;
        default:
            OUT_min = 200;
            OUT_max = 800;
            break;
    }
    uint16_t desired = (uint16_t) map(target, 0, 120, OUT_max, OUT_min);

    // calculate error
    float error = (float) (desired) - (float) (current);

    // if desired value changed, reset stable flag
    if (desired != Stbl_dOld[edge]) Stbl_Flag[edge] = false;
    Stbl_dOld[edge] = desired;

    // acceptable error band -> switch off motor
    if ((error > -0.5 * MotLin_PID_de) && (error < 0.5 * MotLin_PID_de)) {
        Stbl_Count[edge]++;
        if (Stbl_Count[edge] >= 100){
            Stbl_Flag[edge] = true;
            Flg_EdgeRequest_Ext[edge] = false;
        }
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
uint8_t Acts_LIN_GetTarget(uint8_t edge) {
    return Ext_Desired[edge];
}

/* ******************** SET DESIRED EXTENSION ******************************* */
void Acts_LIN_SetTarget(uint8_t edge, uint8_t desired) {
    Ext_Desired[edge] = desired;
    Flg_EdgeRequest_Ext[edge] = true; //  relevant when coupled
}

/* ******************** RETURN FORMATTED EXTENTION ************************** */
uint8_t Acts_LIN_GetCurrent(uint8_t edge) {
    int16_t IN_min, IN_max;
    switch (edge) {
        case 0:
            IN_min = MotLin_MIN_1;
            IN_max = MotLin_MAX_1;
            break;
        case 1:
            IN_min = MotLin_MIN_2;
            IN_max = MotLin_MAX_2;
            break;
        case 2:
            IN_min = MotLin_MIN_3;
            IN_max = MotLin_MAX_3;
            break;
        default:
            IN_min = 200;
            IN_max = 800;
            break;
    }
    return (uint8_t) map(ADC1_Return(edge), IN_min, IN_max, 120, 0);
}