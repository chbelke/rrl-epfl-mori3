#include "math.h"
#include "dsp.h"
#include "Defs_GLB.h"
#include "Defs_MOD.h"
#include "Coms_123.h"
#include "mcc_generated_files/pwm.h"
#include "mcc_generated_files/adc1.h"

uint8_t Ext_Desired[3] = {60, 60, 60};
uint16_t MotLin_PWM_Max[3] = {MotLin_PID_Max, MotLin_PID_Max, MotLin_PID_Max};
volatile bool Stbl_Flag[3] = {true, true, true};
volatile bool Flg_InRange_L[3] = {true, true, true};
#define RampUp 128

/* ******************** ARDUINO MAP FUNCTION ******************************** */
long map(long x, long in_min, long in_max, long out_min, long out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/* ******************** LINEAR MOTOR OUTPUTS ******************************** */
void Acts_LIN_Out(uint8_t edge, int16_t duty) {
    static int16_t oldDuty[3] = {0, 0, 0};
    if (!MODE_MotLin_Active || !FLG_MotLin_Active) duty = 0; // linear motors off
    int16_t out = 0;
    
//    // limit max speed
//    if (duty < -((int16_t)MotLin_PWM_Max[edge])) duty = -((int16_t)MotLin_PWM_Max[edge]);
//    else if (duty > ((int16_t)MotLin_PWM_Max[edge])) duty = ((int16_t)MotLin_PWM_Max[edge]);
    
    // ramp up
    if (duty > 0){
        if ((duty - oldDuty[edge]) > RampUp) out = oldDuty[edge] + RampUp;
        else out = duty;
    } else if (duty < 0){
        if ((duty - oldDuty[edge]) < -RampUp) out = oldDuty[edge] - RampUp;
        else out = duty;
    }
    oldDuty[edge] = out;
    
    // limit max speed
    if (out < -((int16_t)MotLin_PWM_Max[edge])) out = -((int16_t)MotLin_PWM_Max[edge]);
    else if (out > ((int16_t)MotLin_PWM_Max[edge])) out = ((int16_t)MotLin_PWM_Max[edge]);
    
    switch (edge) {
        case 0:
            if (out > 0) LIN_DIR_1 = 0; // direction output
            else LIN_DIR_1 = 1;
            PWM_Set(LIN_PWM_1, abs(out)); // pwm output
            break;

        case 1:
            if (out > 0) LIN_DIR_2 = 0; // direction output
            else LIN_DIR_2 = 1;
            PWM_Set(LIN_PWM_2, abs(out)); // pwm output
            break;

        case 2:
            if (out > 0) LIN_DIR_3 = 0; // direction output
            else LIN_DIR_3 = 1;
            PWM_Set(LIN_PWM_3, abs(out)); // pwm output
            break;

        default:
            break;
    }
}

/* ******************** LINEAR MOTOR PID ************************************ */
void Acts_LIN_PID(uint8_t edge, uint16_t current, uint8_t target) {
    static float errorOld[3] = {0, 0, 0};
    static float PID_I[3] = {0, 0, 0};
    float PID_D = 0;
    static uint8_t Stbl_Count[3] = {0, 0, 0};
    static int16_t Stbl_dOld[3] = {0, 0, 0};
    
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
    if ((error > -0.5 * MotLin_PID_erband) && (error < 0.5 * MotLin_PID_erband)) {
        Stbl_Count[edge]++;
        if (Stbl_Count[edge] >= MotLin_PID_stable * 20) Stbl_Flag[edge] = true;
    } else
        Stbl_Count[edge] = 0;

    // avoid integral build up when far away
    if (abs(error) < 6.5) // if error is >6.32, kp results in max (ignoring kd)
        PID_I[edge] += error * MotLin_PID_period; // calculate integral component
    else
        PID_I[edge] = 0;

    // limit integral component
    if (PID_I[edge] < -MotLin_PID_Imax) PID_I[edge] = -MotLin_PID_Imax;
    else if (PID_I[edge] > MotLin_PID_Imax) PID_I[edge] = MotLin_PID_Imax;
    
    // calculate derivative component
    PID_D = (error - errorOld[edge]) / MotLin_PID_period;  
    errorOld[edge] = error;
    
    // limit derivative component
    if (PID_D < -MotLin_PID_DMax) PID_D = -MotLin_PID_DMax;
    else if (PID_D > MotLin_PID_DMax) PID_D = MotLin_PID_DMax;

    // calculate PID output
    float outf = MotLin_PID_kP * error + MotLin_PID_kI * PID_I[edge] + MotLin_PID_kD * PID_D;

    // limit duty cycle
    if (outf < -MotLin_PID_Max) outf = -MotLin_PID_Max;
    else if (outf > MotLin_PID_Max) outf = MotLin_PID_Max;

    // slow down motors near min/max
    switch (edge) {
        case 0:
            if (((current < (MotLin_MIN_1 + MotLin_SlowRegion)) && (outf < 0))
                    || ((current > (MotLin_MAX_1 - MotLin_SlowRegion)) && (outf > 0)))
                outf = outf / MotLin_SlowFactor;
            break;

        case 1:
            if (((current < (MotLin_MIN_2 + MotLin_SlowRegion)) && (outf < 0))
                    || ((current > (MotLin_MAX_2 - MotLin_SlowRegion)) && (outf > 0)))
                outf = outf / MotLin_SlowFactor;
            break;

        case 2:
            if (((current < (MotLin_MIN_3 + MotLin_SlowRegion)) && (outf < 0))
                    || ((current > (MotLin_MAX_3 - MotLin_SlowRegion)) && (outf > 0)))
                outf = outf / MotLin_SlowFactor;
            break;

        default:
            break;
    }

    // update motor control output
    if (Stbl_Flag[edge]) outf = 0; // extension value stable
    Acts_LIN_Out(edge, (int16_t) outf);
    
//    // in desired range?
//    if ((error > -MotLin_OkRange) && (error < MotLin_OkRange))
//        Flg_InRange_L[edge] = true;
//    else
//        Flg_InRange_L[edge] = false;
}

/* ******************** GET DESIRED EXTENSION ******************************* */
uint8_t Acts_LIN_GetTarget(uint8_t edge) {
    return Ext_Desired[edge];
}

/* ******************** SET DESIRED EXTENSION ******************************* */
void Acts_LIN_SetTarget(uint8_t edge, uint8_t desired) {
    Ext_Desired[edge] = desired;
    Flg_EdgeAct[edge] = false; // reset act flag until cmd verified with neighbour
    Flg_EdgeReq_Ext[edge] = true; //  relevant when coupled
    Flg_AllEdgRdy[edge] = false;
    Flg_NbrEdgRdy[edge] = false;
    Coms_123_ResetCmdMatch(edge);
}

/* ******************** SET MAX PWM VALUE *********************************** */
void Acts_LIN_SetMaxPWM(uint8_t edge, uint16_t MaxPWM){
    MotLin_PWM_Max[edge] = MaxPWM;
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
        default: //  safe values
            IN_min = 200;
            IN_max = 800;
            break;
    }
    return (uint8_t) map(ADC1_Return(edge), IN_min, IN_max, 120, 0);
}

/* ******************** RETURN WHETHER ALL STABLE *************************** */
bool Act_LIN_IsStable(){
    bool out = true;
    uint8_t edge;
    for (edge = 0; edge < 3; edge++)
        if (!Stbl_Flag[edge]) out = false;
    return out;
}

/* ******************** RETURN WHETHER IN DESIRED RANGE ********************* */
bool Act_LIN_InRange(uint8_t edge){
    uint8_t diff = abs(Acts_LIN_GetCurrent(edge) - Acts_LIN_GetTarget(edge));
    if (diff <= MotLin_OkRange) return true;
    return false;
}