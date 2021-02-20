#include "Defs_GLB.h"
#include "mcc_generated_files/uart4.h"
#include "mcc_generated_files/adc1.h"
#include "Acts_LIN.h"
#include "Acts_ROT.h"
#include "Mnge_PWM.h"
#include "Sens_ACC.h"
#include "Sens_ENC.h"
#include "Mnge_BAT.h"
#include "Mnge_BTN.h"
#include "Mnge_RGB.h"
#include "Coms_123.h"
#include "Coms_ESP.h"
#include "Acts_CPL.h"
#include "Tmrs_CBK.h"


void Tmrs_CBK_Evaluate_Timers(void) {
    if(Flg_Tmr3) {
        Tmrs_CBK_Timer3_Handle();
        Flg_Tmr3 = false;
    }
    if(Flg_Tmr5) {
        Tmrs_CBK_Timer5_Handle();
        Flg_Tmr5 = false;
    }           
}

void Tmrs_CBK_Timer3_Handle(void) {
    Coms_123_ActHandle(); // action synchronisation handle

    uint8_t edge;
    for (edge = 0; edge < 3; edge++) { // open coupling if requested
        if ((Flg_EdgeReq_Cpl[edge]) && ((!Flg_EdgeCon[edge]) ||
                (Flg_EdgeReq_CplNbrWait[edge]))){
            Acts_CPL_On(edge);
            Flg_EdgeReq_Cpl[edge] = false;
            Flg_EdgeReq_CplNbrWait[edge] = false;
        }
         // coupling controller when open coupling sma controller
        if (Acts_CPL_IsOpen(edge)) Acts_CPL_Ctrl(edge);
    }
    
    ADC1_Update(); // read analog potentiometer inputs
    for (edge = 0; edge < 3; edge++) { // extension control loops
        if (Flg_EdgeReq_Ext[edge] && (Flg_EdgeAct[edge] || !Flg_EdgeCon[edge]))
            Acts_LIN_PID(edge, ADC1_Return(edge), Acts_LIN_GetTarget(edge));
        else
            Acts_LIN_Out(edge, 0);// make sure motors are off
    }
}


void Tmrs_CBK_Timer5_Handle(void) {
    static uint16_t seed = 0; // rand() seed 
    static bool seedflag = false; // rand() seed flag
    static bool flg_stable_state = true;
    uint8_t edge;
    
    Coms_123_ConHandle(); // inter-module connection handler
    Acts_ROT_DrvHandle();
    
    for (edge = 0; edge < 3; edge++)
        if (Flg_EdgeNbr_Offset[edge]){
            Sens_ENC_SetGlobalOffset(edge);
            Flg_EdgeNbr_Offset[edge] = false;
        }
            
    
    Battery_Check(); // check if LBO has been trigger for interval
    
    if (Flg_Button) Button_Eval(); // if button has been pressed, process
    
    if (MODE_ACC_CON) Flg_i2c_ACC = true; // read accelerometer, called in tmr1
    
    if(!Flg_ID_check && WIFI_EN) { // check correct name and ESP 
        static uint8_t i = 0;
        if(!(i % 3)) Coms_ESP_Request_ID(); // every 600ms
        i++;
    }
    
    if (FLG_Verbose && WIFI_EN) {
        static uint8_t j = 0;
        if(!(j % 5)) Coms_ESP_Verbose();
        j++;
    }
    
    // RGB LED and party mode handles
    if (MODE_LED_ANGLE) {
        uint8_t RGB[3] = {0, 0, 0};
        RGB[0] = (uint8_t) map(Sens_ACC_GetAngle(0)/10,90,270,0,50);
        RGB[1] = (uint8_t) map(Sens_ACC_GetAngle(1)/10,90,270,0,50);
        RGB[2] = (uint8_t) map(Sens_ACC_GetAngle(2)/10,0,360,0,50);
        Mnge_RGB_SetAll(RGB[0], RGB[1], RGB[2]);
        
    } else if (MODE_LED_EDGES) {
        uint8_t RGB[3] = {0, 0, 0};
        for (edge = 0; edge < 3; edge++)
            RGB[edge] = (uint8_t) map(Acts_LIN_GetCurrent(edge),0,120,0,50);
        Mnge_RGB_SetAll(RGB[0], RGB[1], RGB[2]);
        
    } else if (MODE_LED_RNBOW) {
        static uint8_t RGBow[3] = {0, 80, 160};
        for (edge = 0; edge < 3; edge++) RGBow[edge] += 20;
        Mnge_RGB_SetAll(RGBow[0]/8, RGBow[1]/8, RGBow[2]/8);
        
    } else if (MODE_LED_PARTY){ // ***** PARROT PARTY PARTY PARROT *************
        static bool DrvFlg = true, DrvRvs = false, ColFlg[3] = {true, true, true};
        static uint8_t DrvRst, DrvCnt, ColRst[3], ColCnt[3], ColVal[3];
        if (!seedflag){ // set rand() seed
            srand(seed);
            seedflag = true;
        }
         // RGB LED blinking
        for (edge = 0; edge < 3; edge++){
            if (ColFlg[edge]){
                ColRst[edge] = rand() % 5;
                ColVal[edge] = 10 + rand() % 245;
                ColFlg[edge] = false;
            } else {
                ColCnt[edge]++;
                if (ColCnt[edge] > ColRst[edge]){
                    Mnge_RGB_Set(edge, 0);
                    ColFlg[edge] = true;
                    ColCnt[edge] = 0;
                } else if (ColCnt[edge] <= ColRst[edge]){
                    Mnge_RGB_Set(edge, ColVal[edge]);
                }
            }
        }
        // MotRot dance
        if (DrvFlg){
            DrvRst = 2 + rand() % 15;
            DrvFlg = false;
        }
        DrvCnt++;
        if (DrvCnt > DrvRst){
            int16_t partymotorout;
            if (DrvRvs) partymotorout = -1024;
            else partymotorout = 1024;
            DrvRvs = !DrvRvs;            
            if (!Flg_EdgeCon[0] && !Flg_EdgeCon[1] && !Flg_EdgeCon[2])
                for (edge = 0; edge < 3; edge++) Acts_ROT_Out(edge, partymotorout);
            DrvFlg = true; 
            DrvCnt = 0;
        }
    }

    // varying ID check time between modules ensures varying rand() seed
    if (Flg_ID_check && !seedflag) seed++;
    
    Tmrs_CBK_UpdateStableFlag(&flg_stable_state);
    Coms_ESP_SendStable(flg_stable_state);
}

void Tmrs_CBK_UpdateStableFlag(bool *flg_stable_state) {
    bool out = true;
    uint8_t edge;
    for (edge = 0; edge < 3; edge++){
        if (Flg_EdgeReq_Ext[edge])
            if (!Act_LIN_InRange(edge))
                out = false;
        if (Flg_EdgeReq_Ang[edge])
            if (!Acts_ROT_InRange(edge))
                out = false;
    }
    *flg_stable_state = out;
}