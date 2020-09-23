#include "Defs.h"
#include "mcc_generated_files/uart4.h"
#include "mcc_generated_files/adc1.h"
#include "Acts_LIN.h"
#include "Acts_ROT.h"
#include "Mnge_PWM.h"
#include "Sens_ACC.h"
#include "Mnge_BAT.h"
#include "Mnge_BTN.h"
#include "Mnge_RGB.h"
#include "Coms_123.h"
#include "Coms_ESP.h"
#include "Acts_CPL.h"

void Tmrs_CBK_Timer3_Handle(void)
{
    Coms_123_ActHandle(); // action synchronisation handle

    uint8_t edge;
    for (edge = 0; edge < 3; edge++) { // open coupling if requested
        if ((Flg_EdgeRequest_Cpl[edge]) &&
                (Flg_EdgeAct[edge] || !Flg_EdgeCon[edge])) {
            Acts_CPL_On(edge);
            Flg_EdgeRequest_Cpl[edge] = false;
        }
    }
    Acts_CPL_Ctrl(); // coupling sma controller

    ADC1_Update(); // read analog potentiometer inputs
    for (edge = 0; edge < 3; edge++) { // extension control loops
        if (Flg_EdgeRequest_Ext[edge] &&
                (Flg_EdgeAct[edge] || !Flg_EdgeCon[edge]))
            Acts_LIN_PID(edge, ADC1_Return(edge), Acts_LIN_GetTarget(edge));
        else 
            Acts_LIN_Out(edge, 0);// make sure motors are off
    }
}


void Tmrs_CBK_Timer5_Handle(void)
{
    static uint16_t seed = 0;
    static bool seedflag = true;
    static bool DrvFlg = true, DrvRvs = false, ColFlg[3] = {true, true, true};
    static uint8_t DrvRst, DrvCnt, ColRst[3], ColCnt[3], ColVal[3];
    static uint8_t i = 0, j = 0, l = 0;
    
    Coms_123_ConHandle(); // inter-module connection handler
    
    if(!Flg_ID_check && WIFI_EN) { // check correct name and ESP 
        if(!(i % 3)) Coms_ESP_Request_ID(); // every 600ms
        i++;
    }
    
    // Add your custom callback code here
    if (Flg_Button){ // if button has been pressed, process
        Button_Eval();
        Flg_Button = false;
    }
    
    Battery_Check();
    
    if (MODE_ACC_CON) Flg_i2c_ACC = true; // read accelerometer, called in tmr1
    
    if (MODE_LED_ANGLE) {
        uint8_t RGB[3] = {0, 0, 0};
        RGB[0] = (uint8_t) map(Sens_ACC_GetAngle(0)/10,90,270,0,50);
        RGB[1] = (uint8_t) map(Sens_ACC_GetAngle(1)/10,90,270,0,50);
        RGB[2] = (uint8_t) map(Sens_ACC_GetAngle(2)/10,0,360,0,50);
        Mnge_RGB_SetAll(RGB[0], RGB[1], RGB[2]);
    } else if (MODE_LED_EDGES) {
        uint8_t RGB[3] = {0, 0, 0};
        RGB[0] = (uint8_t) map(Acts_LIN_GetCurrent(0),0,120,0,50);
        RGB[1] = (uint8_t) map(Acts_LIN_GetCurrent(1),0,120,0,50);
        RGB[2] = (uint8_t) map(Acts_LIN_GetCurrent(2),0,120,0,50);
        Mnge_RGB_SetAll(RGB[0], RGB[1], RGB[2]);
    } else if (MODE_LED_RNBOW) {
        static uint8_t RGBow[3] = {0, 0, 0};
        uint8_t colors;
        for (colors=0; colors<3; colors++)
        {
            if(RGBow[colors] == 0)
            {
                RGBow[colors] = (uint8_t)rand();
            } else {
                RGBow[colors] += 20;
            }
        }
        Mnge_RGB_SetAll(RGBow[0]/8, RGBow[1]/8, RGBow[2]/8);
    } else if (MODE_LED_PARTY){ // ***** PARROT PARTY PARTY PARROT *************
        if (seedflag) srand(seed);
        for (j = 0; j < 3; j++){
            if (ColFlg[j]){
                ColRst[j] = rand() % 5;
                ColVal[j] = 10 + rand() % 245;
                ColFlg[j] = false;
            } else {
                ColCnt[j]++;
                if (ColCnt[j] > ColRst[j]){
                    Mnge_RGB_Set(j, 0);
                    ColFlg[j] = true;
                    ColCnt[j] = 0;
                } else if (ColCnt[j] <= ColRst[j]){
                    Mnge_RGB_Set(j, ColVal[j]);
                }
            }
        }
        if (DrvFlg){
            DrvRst = 1 + rand() % 10;
            DrvFlg = false;
            DrvCnt = 0;
        }
        DrvCnt++;
        if (DrvCnt > DrvRst){
            if (!DrvRvs){
                if (!Flg_EdgeCon[0] && !Flg_EdgeCon[1] && !Flg_EdgeCon[2])
                    for (l = 0; l < 3; l++) Acts_ROT_Out(l,1024);
                DrvRvs = true;
                DrvFlg = true;
            } else {
                if (!Flg_EdgeCon[0] && !Flg_EdgeCon[1] && !Flg_EdgeCon[2])
                    for (l = 0; l < 3; l++) Acts_ROT_Out(l,-1024);
                DrvRvs = false;
                DrvFlg = true; 
            }
        }
    }

    if (Flg_Verbose && WIFI_EN) {
        static uint8_t k = 0;
        if(!(k % 5)) Coms_ESP_Verbose();
        k++;
    }
    if (Flg_ID_check) seed++;
    
    
}

