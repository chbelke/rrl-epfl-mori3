#include "Defs_GLB.h"
#include "Mnge_PWM.h"
#include "Mnge_RGB.h"
#include "Acts_CPL.h"
#include "Acts_ROT.h"
#include "Coms_ESP.h"

volatile uint8_t CPL_Count_1[3] = {SMA_Period_1, SMA_Period_1, SMA_Period_1};
volatile uint8_t CPL_Count_2[3] = {SMA_Period_2, SMA_Period_2, SMA_Period_2};
volatile bool Acts_CPL_Open[3] = {false, false, false};

/* ******************** SET COUPLING PWM VALUE ****************************** */
void Acts_CPL_Set(uint8_t edge, uint8_t duty) {
    switch (edge) {
        case 0:
            Mnge_PWM_SetValues(3, duty);
            break;
        case 1:
            Mnge_PWM_SetValues(4, duty);
            break;
        case 2:
            Mnge_PWM_SetValues(5, duty);
            break;
        default:
            break;
    }
    Flg_i2c_PWM = true;
}

/* ******************** OPEN COUPLING *************************************** */
void Acts_CPL_On(uint8_t edge) { // called in tmr3 if request flag is set
    Acts_CPL_Open[edge] = true;
    CPL_Count_1[edge] = 0;
    CPL_Count_2[edge] = SMA_Period_2;
    if (MODE_Cplngs_Active) Acts_CPL_Set(edge, SMA_Duty_1);
    Coms_ESP_LED_Set_Blink_Freq(edge, 5);
}

/* ******************** CLOSE COUPLING ************************************** */
void Acts_CPL_Off(uint8_t edge) {
    Acts_CPL_Open[edge] = false;
    CPL_Count_1[edge] = SMA_Period_1;
    CPL_Count_2[edge] = SMA_Period_2;
    Acts_CPL_Set(edge, 0);
}

/* ******************** COUPLING SMA CONTROLLER ***************************** */
void Acts_CPL_Ctrl(uint8_t edge) { // called in tmr3, switches off when counter runs out
    if (CPL_Count_1[edge] < SMA_Period_1) { // first pwm phase (high current)
        CPL_Count_1[edge]++;
        if (CPL_Count_1[edge] >= SMA_Period_1) {
            if (MODE_Cplngs_Active) Acts_CPL_Set(edge, SMA_Duty_2);
            CPL_Count_2[edge] = 0;
            if (Flg_DriveAndCouple[edge]){ // if part of drive&couple sequence
                if (!Flg_EdgeCon[0] && !Flg_EdgeCon[1] && !Flg_EdgeCon[2]){
                    Acts_ROT_Drive(255, 0, edge, 1);
                    uint8_t i;
                    for (i = 0; i < 3; i++) // overwrite interval in _Drive
                        Acts_ROT_SetDrvInterval(i, (uint8_t)(SMA_Period_2/20)*5);
                }
            }
        }
    } else if (CPL_Count_2[edge] < SMA_Period_2) { // second pwm phase (maintain)
        CPL_Count_2[edge]++;
        if ((CPL_Count_2[edge] >= (SMA_Period_2 - MotRot_DrvCplPushInterval)) 
                && (CPL_Count_2[edge] < SMA_Period_2)
                && (Flg_DriveAndCouple[edge])){
            uint8_t i;
            for (i = 0; i < 3; i++)
                if (i != edge) {
                    Acts_ROT_SetDrvInterval(i, MotRot_WiggleTime*5);
                    Acts_ROT_Out(i, ((int8_t)128)*8);
                } else 
                    Acts_ROT_Out(i, 0);
        } else if (CPL_Count_2[edge] >= SMA_Period_2) {
            Acts_CPL_Off(edge);
            if (Flg_DriveAndCouple[edge]){ // if part of drive&couple sequence
                Acts_ROT_SetWiggle(edge);
                Flg_DriveAndCouple[edge] = false;
            }
        }
    } else {
        Acts_CPL_Off(edge);
    }
}

bool Acts_CPL_IsOpen(uint8_t edge){
    return Acts_CPL_Open[edge];
}