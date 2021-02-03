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
uint8_t Speed_255[3] = {MotRot_SpeedInit, MotRot_SpeedInit, MotRot_SpeedInit};
float Speed_DEG[3] = {(((float) MotRot_SpeedInit) / 255) * MotRot_SpeedMax * MotRot_PID_period,
    (((float) MotRot_SpeedInit) / 255) * MotRot_SpeedMax * MotRot_PID_period,
    (((float) MotRot_SpeedInit) / 255) * MotRot_SpeedMax * MotRot_PID_period};
uint8_t DrvInterval[3] = {0, 0, 0};

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
    static float PID_I[3] = {0, 0, 0}; // integral error variable
    static float outPOld[3] = {0, 0, 0}; // speed control switch detect variable
    static float SPD_I[3] = {0, 0, 0}; // speed control integral
    static float errorSPDOld5[3] = {0, 0, 0};
    static float errorSPDOld4[3] = {0, 0, 0};
    static float errorSPDOld3[3] = {0, 0, 0};
    static float errorSPDOld2[3] = {0, 0, 0};
    static float errorSPDOld1[3] = {0, 0, 0};

    float desired = ((float) target) / 10.0 - 180.0;
    // avoid bad control inputs
    if (desired < -MotRot_AngleRange / 2) desired = -MotRot_AngleRange / 2;
    else if (desired > MotRot_AngleRange / 2) desired = MotRot_AngleRange / 2;

    float error = desired - current;

    // avoid integral build up when far away
    if (abs(error) > 7) // if error is >6.69, kp results in max (ignoring kd)
        PID_I[edge] += error * MotRot_PID_period;
    else PID_I[edge] = 0;

    // limit integral component
    if (PID_I[edge] < -MotRot_PID_Imax) PID_I[edge] = -MotRot_PID_Imax;
    else if (PID_I[edge] > MotRot_PID_Imax) PID_I[edge] = MotRot_PID_Imax;

    // derivative component
    float PID_D = (error - errorOld[edge]) / MotRot_PID_period;
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

    // speed control
    if ((Speed_255[edge] != 255) && (fabs(error) > 1)) {
        float delta = Sens_ENC_GetDelta(edge); // change in angle reading
        float errorSPD; // speed error
        if (outP > 0) errorSPD = Speed_DEG[edge] - delta;
        else errorSPD = -Speed_DEG[edge] - delta;

        // feed forward of output speed (proportional to set speed)
        OUT = outP * (((float) Speed_255[edge]) / 255);

        // if desired direction change, reset integral and derivative
        if (((outPOld[edge] > 0) && (outP < 0))
                || ((outPOld[edge] < 0) && (outP > 0))){
            SPD_I[edge] = 0;
            errorSPDOld5[edge] = 0;
            errorSPDOld4[edge] = 0;
            errorSPDOld3[edge] = 0;
            errorSPDOld2[edge] = 0;
            errorSPDOld1[edge] = 0;
        }
        outPOld[edge] = outP;

        // integral component
        SPD_I[edge] += errorSPD * MotRot_SPD_kI;
        float SPD_P = MotRot_SPD_kP * errorSPD;
        if (SPD_I[edge] < (-MotRot_SPD_Max - OUT - SPD_P))
            SPD_I[edge] = -MotRot_SPD_Max - OUT - SPD_P;
        else if (SPD_I[edge] > (MotRot_SPD_Max - OUT - SPD_P))
            SPD_I[edge] = MotRot_SPD_Max - OUT - SPD_P;

        // derivative reduction of integral term
        float SPD_D = ((errorSPD - errorSPDOld1[edge]) +
                (errorSPDOld1[edge] - errorSPDOld2[edge]) +
                (errorSPDOld2[edge] - errorSPDOld3[edge]) +
                (errorSPDOld3[edge] - errorSPDOld4[edge]) +
                (errorSPDOld4[edge] - errorSPDOld5[edge])) / 5;
        errorSPDOld5[edge] = errorSPDOld4[edge];
        errorSPDOld4[edge] = errorSPDOld3[edge];
        errorSPDOld3[edge] = errorSPDOld2[edge];
        errorSPDOld2[edge] = errorSPDOld1[edge];
        errorSPDOld1[edge] = errorSPD;
        SPD_I[edge] += SPD_D * MotRot_SPD_kD / MotRot_PID_period;
        
        // if change in error is less than 20% of desired speed 
        // and speed error bigger than 80% of desired speed - boost to start
        if ((fabs(SPD_D) < fabs(0.1*Speed_DEG[edge])) 
                && (fabs(errorSPD) > fabs(0.9*Speed_DEG[edge])))
            SPD_I[edge] += sgn(errorSPD)*5;
        
        float outS = OUT + SPD_I[edge] + SPD_P;
        
        // limit speed duty cycle
        if (outS < -MotRot_SPD_Max) OUT = -MotRot_SPD_Max;
        else if (outS > MotRot_SPD_Max) OUT = MotRot_SPD_Max;
        else OUT = outS;
    } else { // reset integral
        SPD_I[edge] = 0;
    }
    Acts_ROT_Out(edge, (int16_t) OUT); // output pwm value
}

/* ******************** EXECUTE WIGGLE ************************************** */
void Acts_ROT_Wiggle(uint8_t edge) {
    static uint16_t Wgl_Count[3] = {0, 0, 0};
    Wgl_Count[edge]++;
    if (Wgl_Count[edge] <= (MotRot_WiggleTime * 100)) {
        if (Wgl_Count[edge] <= MotRot_WiggleTime * 60) {
            Acts_ROT_Out(edge, 600);
            if (Flg_EdgeSyn[edge]) {
                Wgl_Count[edge] = MotRot_WiggleTime * 61;
                if (Flg_DriveAndCouple[edge]) {
                    uint8_t i;
                    for (i = 0; i < 3; i++) if (i != edge) Acts_ROT_Out(i, 0);
                }
            }
        } else if (Wgl_Count[edge] <= MotRot_WiggleTime * 90)
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
    Speed_255[edge] = limit;
    Speed_DEG[edge] = (((float) limit) / 255) * MotRot_SpeedMax * MotRot_PID_period;
}

/* ******************** RETURN ROTARY MOTOR SPEED LIMIT ********************* */
uint8_t Acts_ROT_GetSpeedLimit(uint8_t edge) {
    return Speed_255[edge];
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
    return (uint16_t) map((int16_t) rawAngle, -1800, 1800, 0, 3600);
}

/* ******************** RETURN WHETHER ALL IN DESIRED RANGE ***************** */
bool Acts_ROT_InRange(uint8_t edge) {
    uint16_t diff = abs(Acts_ROT_GetAngle(edge) - Acts_ROT_GetTarget(edge));
    if (diff <= MotRot_OkRange) return true;
    else return false;
}

int8_t sgn(float value){
    return (value > 0) - (value < 0);
}