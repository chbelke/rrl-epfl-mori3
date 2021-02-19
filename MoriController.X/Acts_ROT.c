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
float Speed_DEG[3] = {(((float) MotRot_SpeedInit) * 0.01f) * MotRot_SpeedMax * MotRot_PID_period,
    (((float) MotRot_SpeedInit) * 0.01f) * MotRot_SpeedMax * MotRot_PID_period,
    (((float) MotRot_SpeedInit) * 0.01f) * MotRot_SpeedMax * MotRot_PID_period};
uint8_t DrvInterval[3] = {0, 0, 0};
float PID_I[3] = {0, 0, 0}; // integral error variable

volatile bool SPD_AvgFlag[3] = {false, false, false};
volatile int8_t SPD_AvgOut[3] = {0, 0, 0};
volatile int8_t SPD_AvgIn[3] = {0, 0, 0};

volatile bool OffsetUpdateFlag = false;

volatile int16_t SPD_PID_Monitor[3] = {0, 0, 0};

float TMP_SpeedReg_GainI = 15.0f;

#define WHEEL 68.15f // wheel distance from vertex
#define SxOUT 0.9f // output speed factor for non-primary wheels (tune curve)

/* ******************** ROTARY MOTOR OUTPUTS ******************************** */
void Acts_ROT_Out(const uint8_t edge, int16_t duty) {
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
void Acts_ROT_PID(const uint8_t edge, const float current, uint16_t target) {
    static float errorOld[3] = {0, 0, 0}; // previous error (derivative gain)

    // avoid bad control inputs
    target = clamp_ui16(target, MotRot_AngleIntMIN, MotRot_AngleIntMAX);

    const float error = ((float) target) * 0.1f - 180.0f - current;

    // avoid integral build up when far away
    if (fabsf(error) < 7.0f) // if error is >6.69, kp results in max (ignoring kd)
        PID_I[edge] += error * MotRot_PID_period;
    else PID_I[edge] = 0;

    // limit integral component
    PID_I[edge] = clamp_f(PID_I[edge], -MotRot_PID_Imax, MotRot_PID_Imax);

    // derivative component
    float PID_D = (error - errorOld[edge]) * MotRot_PID_freq;
    errorOld[edge] = error;

    // limit derivative component
    PID_D = clamp_f(PID_D, -MotRot_PID_Dmax, MotRot_PID_Dmax);

    // PID output
    float outP = MotRot_PID_kP * error + MotRot_PID_kI * PID_I[edge] + MotRot_PID_kD * PID_D;

    // limit duty cycle
    outP = clamp_f(outP, -MotRot_PID_Max, MotRot_PID_Max);

    float OUT = outP; // position pwm output
    
    // speed control
    if ((Speed_100[edge] != 100) && (fabsf(error) >= 1.0f)) {
        static uint16_t targetOld = 0;
        static uint8_t newTargetCount[3] = {0, 0, 0};
        static float SpeedReg_I[3] = {0, 0, 0};

        if (target != targetOld){
            SpeedReg_I[edge] = copysgn(MotRot_SPD_Max, outP) * (((float) Acts_ROT_GetSpeedLimit(edge)) * 0.01f);
            targetOld = target;
        }

        // speed PID components
        SpeedReg_I[edge] += TMP_SpeedReg_GainI * (copysgn(Speed_DEG[edge], outP) - Sens_ENC_GetDelta(edge));

        // limit integral
        SpeedReg_I[edge] = clamp_f(SpeedReg_I[edge], -MotRot_SPD_Imax, MotRot_SPD_Imax);

        Acts_ROT_SetSPDAvgOut(edge, (int8_t)(SpeedReg_I[edge] * 0.5f));

        // speed controller output scaled by position controller output
        OUT = fabsf(outP) * MotRot_PID_OneOverMax * SpeedReg_I[edge];
        
        // ensure speed never changes target direction from position PID
        if ((OUT > 0) && (SpeedReg_I[edge] < 0)) OUT = 0;
        else if ((OUT < 0) && (SpeedReg_I[edge] > 0)) OUT = 0;
        
        SPD_PID_Monitor[0] = (int16_t) (Sens_ENC_GetDelta(edge) * 500.0f);
        SPD_PID_Monitor[2] = (int16_t) SpeedReg_I[edge];

        if (SPD_AvgFlag[edge]){
            if (newTargetCount[edge] >= 7){
                SPD_AvgFlag[edge] = false;
                SpeedReg_I[edge] = SpeedReg_I[edge] * 0.5f + Acts_ROT_GetSPDAvgNeighbour(edge);
                newTargetCount[edge] = 0;
            } else {
                newTargetCount[edge]++;
            }
        }
    }
    Acts_ROT_Out(edge, (int16_t) OUT); // output pwm value
}

void Acts_ROT_TempUpdateControl(uint8_t select, uint8_t value){
    if (select == 1) TMP_SpeedReg_GainI = (float)value;
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
    return (uint16_t) ((int16_t)(10 * Sens_ENC_Get(edge, WithLiveOffset)) + 1800);
//    return (uint16_t) map(rawAngle, -1800, 1800, 0, 3600);
}

/* ******************** RETURN WHETHER ALL IN DESIRED RANGE ***************** */
bool Acts_ROT_InRange(const uint8_t edge) {
    const uint16_t diff = abs(Acts_ROT_GetAngle(edge, true) - Acts_ROT_GetTarget(edge));
    if (diff <= MotRot_OkRange) return true;
    return false;
}

float sgn(const float value){
    if (value > 0) return 1.0f;
    if (value < 0) return -1.0f;
    return 0.0f;
}


float copysgn(const float value, const float check){
    if (check > 0.0f) return value;
    if (check < 0.0f) return -value;
    return 0.0f;
}

// https://stackoverflow.com/questions/427477/fastest-way-to-clamp-a-real-fixed-floating-point-value
float clamp_f(const float d, const float min, const float max) {
  const float t = d < min ? min : d;
  return t > max ? max : t;
}

uint16_t clamp_ui16(const uint16_t d, const uint16_t min, const uint16_t max) {
  const uint16_t t = d < min ? min : d;
  return t > max ? max : t;
}


void Acts_ROT_SetSPDAvgOut(const uint8_t edge, const int8_t value){
    SPD_AvgOut[edge] = value;
}

uint8_t Acts_ROT_GetSPDAvgOut(const uint8_t edge){
    return (uint8_t)SPD_AvgOut[edge];
}

void Acts_ROT_SetSPDAvgNeighbour(const uint8_t edge, const uint8_t value){
    SPD_AvgFlag[edge] = true;
    SPD_AvgIn[edge] = (int8_t)value;
}

int8_t Acts_ROT_GetSPDAvgNeighbour(const uint8_t edge){
    return SPD_AvgIn[edge];
}

void Acts_ROT_ResetOffsetInterval(){
    OffsetUpdateFlag = true;
}