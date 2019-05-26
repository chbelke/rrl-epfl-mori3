/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system intialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.95-b-SNAPSHOT
        Device            :  dsPIC33EP512GM604
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.36
        MPLAB 	          :  MPLAB X v5.10
 */

/*
    (c) 2016 Microchip Technology Inc. and its subsidiaries. You may use this
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
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "TLC59208.h"
#include "MMA8452Q.h"
#include "DAC5574.h"
#include "MotRot.h"
#include "MotLin.h"
#include "AS5048B.h"
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/mcc.h"

int main (void) {
    SYSTEM_Initialize(); // MCC & User inits

    //    MotRot_LIM(0,255);          // set current limit of edge 0 to max
    //    LED_R = 0;                  // switch red led on
    
    LED_R = 1;
    
    // - Set rotary motor current limits -
    // unexpected behaviour when limit not set (can set itself randomly 
    // between startups), consider defining it in an initialisation 
    // function, need to figure out what level to start with
    MotRot_LIM(0,255);
    MotRot_LIM(1,255);
    MotRot_LIM(2,255);
    
    Flg_LiveAngle = false; // rotary PID output off

    while (1) {
        // __delay_ms(100);
    }
    
    return 0;
}
/**
 End of File
 */