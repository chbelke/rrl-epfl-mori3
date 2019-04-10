#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include <libpic30.h>
#include "mcc_generated_files/mcc.h"
#include "define.h"
#include "DAC5574.h"

float rPID_eOld[3] = {0, 0, 0};
float rPID_I[3] = {0, 0, 0};
float rPID_D[3] = {0, 0, 0};

/* ******************** ROTARY MOTOR OUTPUTS ******************************** */
void MotRot_OUT(uint8_t edge, int16_t duty){
    switch (edge) {
        case 0:
            if (duty > 0) ROT_DIR_A = 1;        // direction output
            else ROT_DIR_A = 0;
            PWM_Set(ROT_PWM_A, abs(duty));      // pwm output
        case 2:
            if (duty > 0) ROT_DIR_B = 1;        // direction output
            else ROT_DIR_B = 0;
            PWM_Set(ROT_PWM_B, abs(duty));      // pwm output
        case 3:
            if (duty > 0) ROT_DIR_C = 1;        // direction output
            else ROT_DIR_C = 0;
            PWM_Set(ROT_PWM_C, abs(duty));      // pwm output
    }
}

/* ******************** ROTARY MOTOR PID ************************************ */
void MotRot_PID(uint8_t edge, float current, float desired){
    // avoid bad control inputs
    if (desired < -MotRot_AngleRange/2) desired = -MotRot_AngleRange/2;
    else if (desired > MotRot_AngleRange/2) desired = MotRot_AngleRange/2;
    
    // calculate error
    static float error;
    error = desired - current;
    
    // calculate integral component
    rPID_I[edge] += error;// * MotRot_PID_dt;
    
    // limit integral component
    if (rPID_I[edge] < -MotRot_PID_Imax) rPID_I[edge] = -MotRot_PID_Imax;
    else if (rPID_I[edge] > MotRot_PID_Imax) rPID_I[edge] = MotRot_PID_Imax;
    
    // calculate derivative component
    rPID_D[edge] = (error - rPID_eOld[edge]);// / MotRot_PID_dt;
    rPID_eOld[edge] = error;
    
    // calculate PID output
    static float outf;
    outf = MotRot_PID_kP * error + MotRot_PID_kI * rPID_I[edge] + MotRot_PID_kD * rPID_D[edge];
    
    // limit duty cycle
    if (outf < -MotRot_PID_Max) outf = -MotRot_PID_Max;
    else if (outf > MotRot_PID_Max) outf = MotRot_PID_Max;
    
    // update motor control output
    MotRot_OUT(edge, (int16_t)outf);
    
    UART4_Write((int8_t)(outf));
}

/* ******************** ROTARY MOTOR CURRENT ******************************** */
void MotRot_LIM(uint8_t edge, uint8_t voltagelevel){
    DAC5574_Write(edge, voltagelevel);
}
