
/**
  TMR5 Generated Driver API Source File 

  @Company
    Microchip Technology Inc.

  @File Name
    tmr5.c

  @Summary
    This is the generated source file for the TMR5 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This source file provides APIs for driver for TMR5. 
    Generation Information : 
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.169.0
        Device            :  dsPIC33EP512GM604
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.50
        MPLAB             :  MPLAB X v5.40
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

#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <libpic30.h>
#include "tmr5.h"
#include "uart4.h"
#include "adc1.h"
#include "../Defs.h"
#include "../Acts_LIN.h"
#include "../Mnge_PWM.h"
#include "../Sens_ACC.h"
#include "../Mnge_BAT.h"
#include "../Mnge_BTN.h"
#include "../Mnge_RGB.h"
#include "../Coms_123.h"
#include "../Coms_ESP.h"

/**
 Section: File specific functions
*/
void (*TMR5_InterruptHandler)(void) = NULL;
void TMR5_CallBack(void);

/**
  Section: Data Type Definitions
 */

/** TMR Driver Hardware Instance Object

  @Summary
    Defines the object required for the maintenance of the hardware instance.

  @Description
    This defines the object required for the maintenance of the hardware
    instance. This object exists once per hardware instance of the peripheral.

  Remarks:
    None.
 */

typedef struct _TMR_OBJ_STRUCT
{
    /* Timer Elapsed */
    volatile bool           timerElapsed;
    /*Software Counter value*/
    volatile uint8_t        count;

} TMR_OBJ;

static TMR_OBJ tmr5_obj;

/**
  Section: Driver Interface
 */

void TMR5_Initialize (void)
{
    //TMR5 0; 
    TMR5 = 0x00;
    //Period = 0.2 s; Frequency = 3686400 Hz; PR5 11520; 
    PR5 = 0x2D00;
    //TCKPS 1:64; TON enabled; TSIDL disabled; TCS FOSC/2; TGATE disabled; 
    T5CON = 0x8020;

    if(TMR5_InterruptHandler == NULL)
    {
        TMR5_SetInterruptHandler(&TMR5_CallBack);
    }

    IFS1bits.T5IF = false;
    IEC1bits.T5IE = true;

    tmr5_obj.timerElapsed = false;

}


void __attribute__ ( ( interrupt, no_auto_psv ) ) _T5Interrupt (  )
{
    /* Check if the Timer Interrupt/Status is set */

    //***User Area Begin

    // ticker function call;
    // ticker is 1 -> Callback function gets called everytime this ISR executes
    if(TMR5_InterruptHandler) 
    { 
           TMR5_InterruptHandler(); 
    }

    //***User Area End

    tmr5_obj.count++;
    tmr5_obj.timerElapsed = true;
    IFS1bits.T5IF = false;
}

void TMR5_Period16BitSet( uint16_t value )
{
    /* Update the counter values */
    PR5 = value;
    /* Reset the status information */
    tmr5_obj.timerElapsed = false;
}

uint16_t TMR5_Period16BitGet( void )
{
    return( PR5 );
}

void TMR5_Counter16BitSet ( uint16_t value )
{
    /* Update the counter values */
    TMR5 = value;
    /* Reset the status information */
    tmr5_obj.timerElapsed = false;
}

uint16_t TMR5_Counter16BitGet( void )
{
    return( TMR5 );
}


void __attribute__ ((weak)) TMR5_CallBack(void)
{
    Coms_123_ConHandle(); // inter-module connection handler
    
    static uint8_t i = 0;
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
        static uint8_t RGBow[3] = {0, 80, 160};
        RGBow[0] += 20;
        RGBow[1] += 20;
        RGBow[2] += 20;
        Mnge_RGB_SetAll(RGBow[0]/8, RGBow[1]/8, RGBow[2]/8);
    }

    if (Flg_Verbose && WIFI_EN) {
        static uint8_t k = 0;
        if(!(k % 5)) Coms_ESP_Verbose();
        k++;
    }
}

void  TMR5_SetInterruptHandler(void (* InterruptHandler)(void))
{ 
    IEC1bits.T5IE = false;
    TMR5_InterruptHandler = InterruptHandler; 
    IEC1bits.T5IE = true;
}

void TMR5_Start( void )
{
    /* Reset the status information */
    tmr5_obj.timerElapsed = false;

    /*Enable the interrupt*/
    IEC1bits.T5IE = true;

    /* Start the Timer */
    T5CONbits.TON = 1;
}

void TMR5_Stop( void )
{
    /* Stop the Timer */
    T5CONbits.TON = false;

    /*Disable the interrupt*/
    IEC1bits.T5IE = false;
}

bool TMR5_GetElapsedThenClear(void)
{
    bool status;

    status = tmr5_obj.timerElapsed;

    if(status == true)
    {
        tmr5_obj.timerElapsed = false;
    }
    return status;
}

int TMR5_SoftwareCounterGet(void)
{
    return tmr5_obj.count;
}

void TMR5_SoftwareCounterClear(void)
{
    tmr5_obj.count = 0;
}

/**
 End of File
 */
