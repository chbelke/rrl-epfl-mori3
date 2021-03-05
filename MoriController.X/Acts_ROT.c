#include "Defs_GLB.h"
#include "Defs_MOD.h"
#include "Mnge_DAC.h"
#include "Sens_ENC.h"
#include "Acts_LIN.h"
#include "Acts_ROT.h"
#include "Coms_ESP.h"
#include "math.h"
#include "dsp.h"
#include "Mnge_ERR.h"

uint16_t ANG_Desired[3] = {1800, 1800, 1800}; // -180.0 to 180.0 deg = 0 to 3600
volatile float ANG_Deadband = MotRot_PID_Deadband;
volatile float ANG_DeadbandInverse = 1.0f / MotRot_PID_Deadband;
uint8_t Trq_Limit[3] = {0, 0, 0}; // save torque limit during wiggle
uint8_t Speed_100[3] = {MotRot_SpeedInit, MotRot_SpeedInit, MotRot_SpeedInit};
float Speed_DEG[3] = {(((float) MotRot_SpeedInit) * 0.01f) * MotRot_SpeedMax * MotRot_PID_period,
    (((float) MotRot_SpeedInit) * 0.01f) * MotRot_SpeedMax * MotRot_PID_period,
    (((float) MotRot_SpeedInit) * 0.01f) * MotRot_SpeedMax * MotRot_PID_period};
uint8_t DrvInterval[3] = {0, 0, 0};
float PID_I[3] = {0, 0, 0}; // integral error variable

float PID_GainP = MotRot_PID_kP;
float PID_GainI = MotRot_PID_kI;
float PID_GainD = MotRot_PID_kD;
float SPD_Gain = MotRot_SPD_k;

uint8_t ANG_OkRange = MotRot_OkRange;

volatile bool SPD_AvgFlag[3] = {false, false, false};
volatile int16_t SPD_AvgOut[3] = {0, 0, 0};
volatile int16_t SPD_AvgIn[3] = {0, 0, 0};

#define WHEEL 68.15f // wheel distance from vertex
#define SxOUT 0.9f // output speed factor for non-primary wheels (tune curve)
#define INV_THREE 0.333333f // division avoidance
#define INV255_x180 7.058824f // factor for 255 to 180pwm conversion


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
    float OUT; // pwm output variable
    static bool SPD_Flag[3] = {false, false, false}; // speed controller on flag
    static bool SPD_Used[3] = {false, false, false}; // speed output used flag
    
    // avoid bad control inputs
    target = clamp_ui16(target, MotRot_AngleIntMIN, MotRot_AngleIntMAX);

    const float error = ((float) target) * 0.1f - 180.0f - current;
    
        // stop speed controller when target reached
    if (SPD_Flag[edge] && (fabsf(error) < 0.1f)){
        SPD_Flag[edge] = false;
        if (SPD_Used[edge]) PID_I[edge] = 0.0f;
        /* if it gets to target without position controller used
         * (ie SPD output was always smaller than position), reset
         * position PID to avoid overshoot */
    }

    // integral component
    if (fabsf(error) < Acts_ROT_GetAngleDeadband()) {
        // only allow wind-down within deadband
        if ((sgn(PID_I[edge]) != sgn(error)) && (fabsf(PID_I[edge]) < epsilon))
            PID_I[edge] += error * MotRot_PID_period;
        // clamp to max output value in deadband
        if (error < 0) PID_I[edge] = clamp_f(PID_I[edge], -MotRot_PID_Imax * fabsf(error) * Acts_ROT_GetAngleDeadbandInverse(), 0.0f);
        else if (error > 0) PID_I[edge] = clamp_f(PID_I[edge], 0.0f, MotRot_PID_Imax * fabsf(error) * Acts_ROT_GetAngleDeadbandInverse());
    } else if (fabsf(error) < 7.0f){
        // avoid integral build up when far away (if error >6.69, kp maxes PWM)
        PID_I[edge] += error * MotRot_PID_period;
        PID_I[edge] = clamp_f(PID_I[edge], -MotRot_PID_Imax, MotRot_PID_Imax);
    } else PID_I[edge] = 0.0f;

    // derivative component
    float PID_D = (error - errorOld[edge]) * MotRot_PID_freq;
    errorOld[edge] = error;

    // limit derivative
    PID_D = clamp_f(PID_D, -MotRot_PID_Dmax, MotRot_PID_Dmax);

    // PID position output
    float outP = PID_GainP * error + PID_GainI * PID_I[edge] + PID_GainD * PID_D;
    outP = clamp_f(outP, -MotRot_PID_Max, MotRot_PID_Max); // limit output
    
    // speed control
    if (Speed_100[edge] != 100) {
        static uint16_t targetOld[3] = {0, 0, 0};
        static uint8_t newTargetCount[3] = {0, 0, 0};
        static int8_t SPD_Dir[3] = {0, 0, 0}; // set with new value
        static float SPD[3] = {0, 0, 0}; // speed control integral term

        // when new target, set speed direction and feed forward
        if (target != targetOld[edge]){
            SPD_Dir[edge] = copysgn(1, outP);
            SPD[edge] = SPD_Dir[edge] * MotRot_SPD_Max * (((float) Acts_ROT_GetSpeedLimit(edge)) * 0.01f);
            targetOld[edge] = target;
            newTargetCount[edge] = 0;
            SPD_Flag[edge] = true;
        }

        // speed control (integral)
        SPD[edge] += SPD_Gain * (copysgn(Speed_DEG[edge], error) - Sens_ENC_GetDelta(edge));
        // ignore other direction (no additional breaking under load)
        if (error > 0.0f) SPD[edge] = clamp_f(SPD[edge], 0.0f, MotRot_SPD_Max);
        else if (error < -0.0f) SPD[edge] = clamp_f(SPD[edge], -MotRot_SPD_Max, -0.0f);
        Acts_ROT_SetSPDAvgOut(edge, (int16_t)(SPD[edge]));

        // average speed integral 
        if (SPD_AvgFlag[edge]){
            if (newTargetCount[edge] >= 16){ // ignore first loops before averaging
                SPD_AvgFlag[edge] = false;
                SPD[edge] = (SPD[edge] + ((float)Acts_ROT_GetSPDAvgNeighbour(edge))) * 0.5f;
            } else newTargetCount[edge]++;
        }
        
        // whichever is smallest in direction of target
        if (SPD_Flag[edge] && // if target has not been reached yet
                (((error > 0.0f) && (SPD[edge] < outP)) ||
                ((error < -0.0f) && (SPD[edge] > outP)))){
            OUT = SPD[edge];
            SPD_Used[edge] = true;
        } else {
            OUT = outP;
            SPD_Used[edge] = false;
        }
    } else {
        OUT = outP; // full speed position output
    }
    // scale output down towards zero within deadband
    if (fabsf(error) < Acts_ROT_GetAngleDeadband())
        OUT *= fabsf(error) * Acts_ROT_GetAngleDeadbandInverse();
    Acts_ROT_Out(edge, (int16_t) OUT); // output pwm
}

/* ******************** EXECUTE WIGGLE ************************************** */
void Acts_ROT_Wiggle(uint8_t edge) {
    static uint16_t Wgl_Count[3] = {0, 0, 0};
    Wgl_Count[edge]++;
    if (Wgl_Count[edge] <= (MotRot_WiggleTime * 100)) {
        if (Wgl_Count[edge] <= MotRot_WiggleTime * 60) {
            Acts_ROT_Out(edge, 300);
            if (Flg_EdgeSyn[edge]) {
                Wgl_Count[edge] = MotRot_WiggleTime * 61;
                if (Flg_DriveAndCouple[edge]) {
                    uint8_t i;
                    for (i = 0; i < 3; i++) if (i != edge) Acts_ROT_Out(i, 0);
                }
            }
        } else if (Wgl_Count[edge] <= MotRot_WiggleTime * 90)
            Acts_ROT_Out(edge, -300);
        else if (Wgl_Count[edge] <= MotRot_WiggleTime * 98)
            Acts_ROT_Out(edge, 300);
        else
            Acts_ROT_Out(edge, -300);
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
    const float Mo = curve * 137.9f;
    float Sa = speed * INV255_x180; // 255 to 180 max
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
    const float beta = acosf((a * a + c * c - b * b) / (2 * a * c));
    const float gamm = acosf((a * a + b * b - c * c) / (2 * a * b));

    // wheel coordinates (for a: [WHEEL, 0])
    const float Wb[2] = {(b - WHEEL) * cosf(gamm), (b - WHEEL) * sinf(gamm)};
    const float Wc[2] = {a - WHEEL * cosf(beta), WHEEL * sinf(beta)};

    // second point in wheel direction
    const float Wb2[2] = {Wb[0] - cosf(PI * 0.5f - gamm), Wb[1] + sinf(PI * 0.5f - gamm)};
    const float Wc2[2] = {Wc[0] + cosf(PI * 0.5f - beta), Wc[1] + sinf(PI * 0.5f  - beta)};

    // centroid coordinates
    const float D[2] = {(b * cosf(gamm) + a) * INV_THREE, b * sinf(gamm) * INV_THREE};

    // moment arm of wheel force to centroid
    const float Da = fabsf(D[0] - WHEEL);
    const float Db = fabsf((Wb2[1] - Wb[1]) * D[0]
            - (Wb2[0] - Wb[0]) * D[1] + Wb2[0] * Wb[1] - Wb2[1] * Wb[0])
            / sqrtf(powf(Wb2[1] - Wb[1], 2) + powf(Wb2[0] - Wb[0], 2));
    const float Dc = fabsf((Wc2[1] - Wc[1]) * D[0]
            - (Wc2[0] - Wc[0]) * D[1] + Wc2[0] * Wc[1] - Wc2[1] * Wc[0])
            / sqrtf(powf(Wc2[1] - Wc[1], 2) + powf(Wc2[0] - Wc[0], 2));

    // wheel speeds
    float Sc = SxOUT*((Mo - Sa * Da) / (Db * cosf(PI * 0.5f - beta) / cosf(PI * 0.5f - gamm) + Dc));
    float Sb = SxOUT*(Sc * cosf(PI * 0.5f - beta) / cosf(PI * 0.5f - gamm));

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

/* ******************** SPEED LIMIT ***************************************** */
void Acts_ROT_SetSpeedLimit(uint8_t edge, uint8_t limit) {
    if (limit > 100) limit = 100;
    else if (limit < 1) limit = 1;
    Speed_DEG[edge] = ((float) limit) * 0.01f * MotRot_SpeedMax * MotRot_PID_period;
    Speed_100[edge] = limit;
}

uint8_t Acts_ROT_GetSpeedLimit(uint8_t edge) {
    return Speed_100[edge];
}

/* ******************** DESIRED ANGLE *************************************** */
void Acts_ROT_SetTarget(uint8_t edge, uint16_t desired) {
    ANG_Desired[edge] = desired;
    Flg_EdgeAct[edge] = false; // reset act flag until cmd verified with neighbour
    Flg_EdgeReq_Ang[edge] = true;
    PID_I[edge] = 0;
}

uint16_t Acts_ROT_GetTarget(uint8_t edge) {
    return ANG_Desired[edge];
}

/* ******************** RETURN FORMATTED ANGLE ****************************** */
uint16_t Acts_ROT_GetAngle(uint8_t edge, bool withLiveOffset) {
    return (uint16_t)(10.0f * Sens_ENC_Get(edge, withLiveOffset) + 1800.0f);
}

/* ******************** RETURN WHETHER ALL IN DESIRED RANGE ***************** */
bool Acts_ROT_InRange(uint8_t edge) {
    const uint16_t diff = abs(Acts_ROT_GetAngle(edge, true) - Acts_ROT_GetTarget(edge));
    if (diff <= ANG_OkRange) return true;
    return false;
}

/* ******************** MATH FUNCTIONS ************************************** */
int8_t sgn(float value){
    return (value > 0) - (value < 0);
}

float copysgn(float value, float check){
    if (check > 0.0f) return value;
    if (check < 0.0f) return -value;
    return 0.0f;
}

/* https://stackoverflow.com/questions/427477/fastest-way-to-clamp-a-real-fixed-floating-point-value */
float clamp_f(float d, float min, float max) {
  const float t = d < min ? min : d;
  return t > max ? max : t;
}

uint16_t clamp_ui16(uint16_t d, uint16_t min, uint16_t max) {
  const uint16_t t = d < min ? min : d;
  return t > max ? max : t;
}

/* ******************** SPEED AVERAGING ************************************* */
void Acts_ROT_SetSPDAvgOut(uint8_t edge, int16_t value){
    SPD_AvgOut[edge] = value;
}

uint16_t Acts_ROT_GetSPDAvgOut(uint8_t edge){
    return (uint16_t)(SPD_AvgOut[edge] + 1800);
}

void Acts_ROT_SetSPDAvgNeighbour(uint8_t edge, uint16_t value){
    SPD_AvgFlag[edge] = true;
    SPD_AvgIn[edge] = ((int16_t)value) - 1800;
}

int16_t Acts_ROT_GetSPDAvgNeighbour(uint8_t edge){
    return SPD_AvgIn[edge];
}

/* ******************** DEADBAND ******************************************** */
// https://deltamotion.com/support/webhelp/rmctools/Registers/Register_Descriptions/Axis_Parameter_Registers/Output/Param_Deadband_Tolerance.htm
void Acts_ROT_SetAngleDeadband(uint8_t value){
    ANG_Deadband = ((float) value) * 0.1f;
    ANG_DeadbandInverse = 1.0f / ANG_Deadband;
}

float Acts_ROT_GetAngleDeadband(){
    return ANG_Deadband;
}

float Acts_ROT_GetAngleDeadbandInverse(){
    return ANG_DeadbandInverse;
}

void Acts_ROT_SetOkRange(uint8_t value){
    ANG_OkRange = value;
}

/* ******************** ADJUST GAINS **************************************** */
void Acts_ROT_SetPIDGains(uint8_t pid, uint8_t value){
    switch (pid){
        case 0:
            PID_GainP = (float)value;
            break;
        case 1:
            PID_GainI = (float)value;
            break;
        case 2:
            PID_GainD = (float)value;
            break;
        default:
            break;
    }
}

void Acts_ROT_SetSPDGain(uint8_t value){
    SPD_Gain = (float)value;
}