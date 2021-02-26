/**
  ADC1 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    adc1.c

  @Summary
    This is the generated driver implementation file for the ADC1 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This source file provides APIs for ADC1.
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

//#include <xc.h>
//#include <stdbool.h>
//#include <stdint.h>
//#include <stdlib.h>
//#include <libpic30.h>
#include "adc1.h"
#include "../Defs_GLB.h"
#include "../Defs_MOD.h"

volatile uint16_t ADC1_Values[3] = {512, 512, 512};

/**
  Section: File Specific Functions
 */

// ADC1 Default Interrupt Handler
static void (*ADC1_DefaultInterruptHandler)(void) = NULL;

/**
  Section: Driver Interface
 */

void ADC1_Initialize (void)
{
    // ASAM enabled; ADDMABM disabled; ADSIDL disabled; DONE disabled; SIMSAM Sequential; FORM Absolute decimal result, unsigned, right-justified; SAMP disabled; SSRC Internal counter ends sampling and starts conversion; AD12B 10-bit; ADON enabled; SSRCG disabled; 
    AD1CON1 = 0x80E4;
    // CSCNA disabled; VCFG0 AVDD; VCFG1 AVSS; ALTS disabled; BUFM disabled; SMPI Generates interrupt after completion of every sample/conversion operation; CHPS 1 Channel; 
    AD1CON2 = 0x00;
    // SAMC 12; ADRC FOSC/2; ADCS 14; 
    AD1CON3 = 0xC0E;
    // CH0SA AN29; CH0SB OA2/AN0; CH0NB VREFL; CH0NA VREFL; 
    AD1CHS0 = 0x1D;
    // CSS26 disabled; CSS25 disabled; CSS24 disabled; CSS31 disabled; CSS30 disabled; CSS29 disabled; CSS28 disabled; CSS27 disabled; 
    AD1CSSH = 0x00;
    // CSS2 disabled; CSS1 disabled; CSS0 disabled; CSS8 disabled; CSS7 disabled; CSS6 disabled; CSS5 disabled; CSS4 disabled; CSS3 disabled; 
    AD1CSSL = 0x00;
    // DMABL Allocates 1 word of buffer to each analog input; ADDMAEN disabled; 
    AD1CON4 = 0x00;
    // CH123SA2 disabled; CH123SB2 CH1=OA2/AN0,CH2=AN1,CH3=AN2; CH123NA disabled; CH123NB CH1=VREF-,CH2=VREF-,CH3=VREF-; 
    AD1CHS123 = 0x00;

    //Assign Default Callbacks
    ADC1_SetInterruptHandler(&ADC1_CallBack);

    // Enabling ADC1 interrupt.
    IEC0bits.AD1IE = 1;
}

void __attribute__((weak)) ADC1_CallBack(void) {

}

void ADC1_SetInterruptHandler(void* handler) {
    ADC1_DefaultInterruptHandler = handler;
}

void __attribute__((__interrupt__, auto_psv, weak)) _AD1Interrupt(void) {
    if (IFS0bits.AD1IF) {
        if (ADC1_DefaultInterruptHandler) {
            ADC1_DefaultInterruptHandler();
        }

        // clear the ADC interrupt flag
        IFS0bits.AD1IF = false;
    }
}

void ADC1_Update(void) {
//    uint16_t i;
    static uint16_t ADC1_ValuesA[4] = {512, 512, 512, 512};
    static uint16_t ADC1_ValuesB[4] = {512, 512, 512, 512};
    static uint16_t ADC1_ValuesC[4] = {512, 512, 512, 512};

    // update previous values
    ADC1_ValuesA[3] = ADC1_ValuesA[2];
    ADC1_ValuesA[2] = ADC1_ValuesA[1];
    ADC1_ValuesA[1] = ADC1_ValuesA[0];
    ADC1_ValuesB[3] = ADC1_ValuesB[2];
    ADC1_ValuesB[2] = ADC1_ValuesB[1];
    ADC1_ValuesB[1] = ADC1_ValuesB[0];
    ADC1_ValuesC[3] = ADC1_ValuesC[2];
    ADC1_ValuesC[2] = ADC1_ValuesC[1];
    ADC1_ValuesC[1] = ADC1_ValuesC[0];

    ADC1_Enable();
//    uint16_t i;

    ADC1_ChannelSelect(AI_A);
    ADC1_SoftwareTriggerEnable();
//    for (i = 0; i < 1000; i++) {
//    }
    __delay_us(20);
    ADC1_SoftwareTriggerDisable();
    while (!ADC1_IsConversionComplete(AI_A));
    ADC1_ValuesA[0] = ADC1_ConversionResultGet(AI_A);

    ADC1_ChannelSelect(AI_B);
    ADC1_SoftwareTriggerEnable();
//    for (i = 0; i < 1000; i++) {
//    }
    __delay_us(20);
    ADC1_SoftwareTriggerDisable();
    while (!ADC1_IsConversionComplete(AI_B));
    ADC1_ValuesB[0] = ADC1_ConversionResultGet(AI_B);

    ADC1_ChannelSelect(AI_C);
    ADC1_SoftwareTriggerEnable();
//    for (i = 0; i < 1000; i++) {
//    }
    __delay_us(20);
    ADC1_SoftwareTriggerDisable();
    while (!ADC1_IsConversionComplete(AI_C));
    ADC1_ValuesC[0] = ADC1_ConversionResultGet(AI_C);

    ADC1_Disable();

    // moving average
    ADC1_Values[0] = (ADC1_ValuesA[0] + ADC1_ValuesA[1] + ADC1_ValuesA[2]
            + ADC1_ValuesA[3]) / 4;
    ADC1_Values[1] = (ADC1_ValuesB[0] + ADC1_ValuesB[1] + ADC1_ValuesB[2]
            + ADC1_ValuesB[3]) / 4;
    ADC1_Values[2] = (ADC1_ValuesC[0] + ADC1_ValuesC[1] + ADC1_ValuesC[2]
            + ADC1_ValuesC[3]) / 4;
}

uint16_t ADC1_Return(uint8_t channel) {
    return ADC1_Values[channel];
}


/**
  End of File
 */

