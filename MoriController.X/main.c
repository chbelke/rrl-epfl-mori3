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
#include "mcc_generated_files/system.h"
#include <stdio.h>
#include <stdlib.h>
#include "define.h"
#include "mcc_generated_files/mcc.h"
#include "TLC59208.h"
#include "MMA8452Q.h"
#include "DAC5574.h"
#include "MotRot.h"
#include "MotLin.h"
#include "AS5048B.h"

int main() {
    SYSTEM_Initialize(); // MCC & User inits

    //    MotRot_LIM(0,255);          // set current limit of edge 0 to max
    //    LED_Set(20,10,5);           // set color of rgb leds
    //    TLC59208_Write();           // update led driver
    //    LED_R = 0;                  // switch red led on
    
    LED_R = 1;

    LED_Set(20, 10, 5);
    TLC59208_Write();

    Flg_LiveAngle = false; // rotary PID output off
    DAC5574_Write(0, 255);
    DAC5574_Write(1, 255);
    DAC5574_Write(2, 255);

    //    Variables for example 1:
    /*//    uint8_t uart_read = 0;
    //    uint8_t led_on = 10;
    //    uint8_t led_off = 5;*/
    while (1) {

        //      example 1: UART reading from WiFi (from Alex)
        /*//        if (UART4_StatusGet() & UART4_RX_DATA_AVAILABLE) {
        //            uart_read = UART4_Read();
        //        } else {
        //            uart_read = 0;
        //        }
        //        
        //        if (uart_read == led_on) {
        //            LED_B = 1;
        //        } else if (uart_read == led_off) {
        //            LED_B = 0;
        //        }*/


        __delay_ms(100);
    }
    return 1;
}
/**
 End of File
 */

