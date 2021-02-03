/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.169.0
        Device            :  dsPIC33EP512GM604
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.50
        MPLAB 	          :  MPLAB X v5.40
 */

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
 */

/**
  Section: Included Files
 */
//#include <stdio.h>
#include <stdlib.h>
#include "Defs_GLB.h"
#include "Mnge_PWM.h"
#include "Mnge_RGB.h"
#include "Sens_ACC.h"
#include "Mnge_DAC.h"
#include "Acts_ROT.h"
#include "Acts_LIN.h"
#include "Acts_CPL.h"
#include "Sens_ENC.h"
#include "Coms_ESP.h"
#include "Coms_123.h"
#include "Tmrs_CBK.h"
#include "mcc_generated_files/system.h"

/* GLOBAL MODES */
volatile bool MODE_LED_ANGLE = false;
volatile bool MODE_LED_EDGES = false;
volatile bool MODE_LED_RNBOW = false;
volatile bool MODE_LED_PARTY = false;

/* GLOBAL FLAGS */
volatile bool FLG_WaitAllEdges = true;
volatile bool FLG_MotLin_Active = false;
volatile bool FLG_MotRot_Active = false;
volatile bool FLG_Verbose = false;
volatile bool FLG_Emergency = false;

volatile bool Flg_BatLow = false;
volatile bool Flg_Button = false;
volatile bool Flg_Develop = false;

// driving flags
volatile bool Flg_Drive[3] = {false, false, false};
volatile bool Flg_DriveAndCouple[3] = {false, false, false};

// edge state handles
volatile bool Flg_EdgeCon[3] = {false, false, false}; // connection detected
volatile bool Flg_EdgeSyn[3] = {false, false, false}; // connection acknowledged
volatile bool Flg_EdgeAct[3] = {false, false, false}; // executing action
volatile bool Flg_EdgeWig[3] = {false, false, false}; // wiggle flag
volatile bool Flg_EdgeReq_Ang[3] = {false, false, false};
volatile bool Flg_EdgeReq_Ext[3] = {false, false, false};
volatile bool Flg_EdgeReq_Cpl[3] = {false, false, false};
volatile bool Flg_EdgeReq_CplNbrWait[3] = {true, true, true};

volatile bool Flg_ID_check = false;

// i2c flags set anywhere, checked and executed in tmr1
volatile bool Flg_i2c_PWM = false;
volatile bool Flg_i2c_ACC = false;
volatile bool Flg_i2c_DAC = false;

// Flags declaring that the timer interrupt has triggered
volatile bool Flg_Tmr3 = true;
volatile bool Flg_Tmr5 = true;

// Determines the frequency that the PIC updates ESP (10s of ms)
volatile uint8_t ESP_DataLog_Time_Elapsed = 0;
volatile uint8_t ESP_ID[6] = {0, 0, 0, 0, 0, 0};

/*
                         Main application
 */
int main(void) {
    uint8_t edge;
    __delay_ms(1000); // start-up delay
    SYSTEM_Initialize(); // initialize the device

    LED_R = LED_Off;
    WIFI_EN = WIFI_On;

    // verify own id with esp
    while (!Flg_ID_check) {
        LED_Y = LED_Off;    
        for (edge = 0; edge < 3; edge++)
        {
            if(Flg_Tmr3) {
                Tmrs_CBK_Timer3_Handle();
                Flg_Tmr3 = false;
            }
            if(Flg_Tmr5) {
                Tmrs_CBK_Timer5_Handle();
                Flg_Tmr5 = false;
            }        
            Coms_123_Eval(edge);
        }
        Coms_ESP_Eval();
    }
    if (!Coms_ESP_VerifyID()) { // if bad id
        Mnge_RGB_SetAll(50,0,0); // set everything to red
        LED_R = LED_On;
        __delay_ms(1000); // wait to set RGB LEDs
        INTERRUPT_GlobalDisable();
        while (1);
    }

    FLG_MotLin_Active = true;
    FLG_MotRot_Active = true;
    
    // - Set rotary motor current limits -
    /* unexpected behaviour when limit not set (can set itself randomly 
     * between startups), consider defining it in an initialisation 
     * function, need to figure out what level to start with */
    for (edge = 0; edge < 3; edge++) Acts_ROT_SetCurrentLimit(edge, MotRot_TorqueLimit);

    while (1){
        Tmrs_CBK_Evaluate_Timers();
        Coms_123_Eval(0);
        Tmrs_CBK_Evaluate_Timers();
        Coms_123_Eval(1);
        Tmrs_CBK_Evaluate_Timers();
        Coms_123_Eval(2);
        Tmrs_CBK_Evaluate_Timers();
        Coms_ESP_Eval();
        Tmrs_CBK_Evaluate_Timers();
        Coms_ESP_StateUpdate();
    }
    return 1;
}
/**
 End of File
 */

