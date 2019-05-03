/**
  PWM Generated Driver API Header File 

  @Company
    Microchip Technology Inc.

  @File Name
    pwm.h

  @Summary
    This is the generated header file for the PWM driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This header file provides APIs for driver for PWM. 
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

#ifndef _PWM_H
#define _PWM_H

/**
  Section: Included Files
*/

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif

/**
  Section: Data Types
*/

/** PWM Generator Definition
 
 @Summary 
   Defines the PWM generators available for PWM
 
 @Description
   This routine defines the PWM generators that are available for the module to use.
 
 Remarks:
   None
 */
typedef enum 
{
    PWM_GENERATOR_1 =  1,       
    PWM_GENERATOR_2 =  2,       
    PWM_GENERATOR_3 =  3,       
    PWM_GENERATOR_4 =  4,       
    PWM_GENERATOR_5 =  5,       
    PWM_GENERATOR_6 =  6,       
} PWM_GENERATOR;

/**
  Section: Interface Routines
*/

/**
  @Summary
    Initializes hardware and data for the given instance of the PWM module

  @Description
    This routine initializes hardware for the instance of the PWM module,
    using the hardware initialization given data.  It also initializes all
    necessary internal data.

  @Param
    None.

  @Returns
    None
 
  @Example 
    <code>
    bool status;
    uint16_t masterPeriod,masterDutyCycle;
    uint16_t postscaler,compareValue;

    postscaler = 0x1;
    compareValue = 0x1;

    masterPeriod = 0xFFFF;
    masterDutyCycle = 0xFF;


    PWM_Initialize();

    PWM_ModuleDisable();

    PWM_MasterDutyCycleSet(masterDutyCycle);
    PWM_PrimaryMasterPeriodSet(period);

    PWM_SpecialEventPrimaryInterruptDisable();
    PWM_SpecialEventTriggerInterruptFlagClear();
    PWM_PrimarySyncOutputDisable();
    PWM_SpecialEventPrimaryPostscalerSet(postscaler);
    PWM_SpecialEventPrimaryCompareValueSet(compareValue);
    PWM_PrimarySyncOutputEnable();
    PWM_SpecialEventPrimaryInterruptEnable();

    PWM_ModuleEnable();
    </code>
*/
void PWM_Initialize (void);

/**
  @Summary
    Enables the PWM module.

  @Description
    This routine is used to enable the PWM module.

  @Param
    None.

  @Returns
    None
 
  @Example 
    Refer to the example of PWM_Initialize();
*/
inline static void PWM_ModuleEnable(void)
{
	PTCONbits.PTEN = true; 
}

/**
  @Summary
    Disables the PWM module.

  @Description
    This routine is used to disable the PWM module.

  @Param
    None.

  @Returns
    None
 
  @Example 
    Refer to the example of PWM_Initialize();
*/
inline static void PWM_ModuleDisable(void)
{
	PTCONbits.PTEN = false; 
}

/**
  @Summary
    Used to set the PWM master duty cycle register.

  @Description
    This routine is used to set the PWM master duty cycle register.

  @Param
    masterDutyCycle - Master Duty Cyle value.

  @Returns
    None
 
  @Example 
    Refer to the example of PWM_Initialize();
*/
inline static void PWM_MasterDutyCycleSet(uint16_t masterDutyCycle)
{
    MDC = masterDutyCycle;
}

/**
  @Summary
    Sets the period value for the Primary Master Time Base generator.

  @Description
    This routine is used to set the period value for the Primary Master Time Base generator.

  @Param
    period - Period value.

  @Returns
    None
 
  @Example 
    Refer to the example of PWM_Initialize();
*/
inline static void PWM_PrimaryMasterPeriodSet(uint16_t period)
{
    PTPER = period;
}

/**
  @Summary
    Enables synchronization output from the Primary PWM timebase generator.

  @Description
    This routine is used to enable synchronization output from the Primary PWM timebase generator.

  @Param
    None.

  @Returns
    None
 
  @Example 
    Refer to the example of PWM_Initialize();
*/ 
inline static void PWM_PrimarySyncOutputEnable(void)
{
	PTCONbits.SYNCOEN = true; 
}

/**
  @Summary
    Disables synchronization output from the Primary PWM timebase generator.

  @Description
    This routine is used to disbale synchronization output from the Primary PWM timebase generator.

  @Param
    None.

  @Returns
    None
 
  @Example 
    Refer to the example of PWM_Initialize();
*/  
inline static void PWM_PrimarySyncOutputDisable(void)
{
	PTCONbits.SYNCOEN = false; 
}

/**
  @Summary
   Sets the postscaler value for the PWM special event trigger from the Primary time base generator.

  @Description
    This routine is used to set the postscaler value for the PWM special event trigger from the Primary time base generator.

  @Param
    postscaler - Special event postscaler value.

  @Returns
    None
 
  @Example 
    Refer to the example of PWM_Initialize();
*/  
inline static void PWM_SpecialEventPrimaryPostscalerSet(uint16_t postscaler)
{
    PTCONbits.SEVTPS = postscaler;
}

/**
  @Summary
    Sets the compare value for the Special Event Trigger from the Primary time base generator.

  @Description
    This routine is used to set the compare value for the Special Event Trigger from the Primary time base generator.

  @Param
    compareValue - Compare Value.

  @Returns
    None
 
  @Example 
    Refer to the example of PWM_Initialize();
*/  
inline static void PWM_SpecialEventPrimaryCompareValueSet(uint16_t compareValue)
{
    SEVTCMP = compareValue;
}

/**
  @Summary
    Clears PWM Special Event Trigger interrupt request flag

  @Description
    This routine is used to clear PWM Special Event Trigger interrupt request flag

  @Param
    None.

  @Returns
    None
 
  @Example 
    Refer to the example of PWM_Initialize();
*/ 
inline static void PWM_SpecialEventTriggerInterruptFlagClear(void)
{
    IFS3bits.PSEMIF = false;
}

/**
  @Summary
    Enables interrupt request for Special Event Trigger from the Primary time base generator.

  @Description
    This routine is used to enable interrupt request for Special Event Trigger from the Primary time base generator.

  @Param
    None.

  @Returns
    None
 
  @Example 
    Refer to the example of PWM_Initialize();
*/  
inline static void PWM_SpecialEventPrimaryInterruptEnable(void)
{
	PTCONbits.SEIEN = true; 
}

/**
  @Summary
    Disables interrupt request for Special Event Trigger from the Primary time base generator.

  @Description
    This routine is used to disable interrupt request for Special Event Trigger from the Primary time base generator.

  @Param
    None.

  @Returns
    None
 
  @Example 
    Refer to the example of PWM_Initialize();
*/  
inline static void PWM_SpecialEventPrimaryInterruptDisable(void)
{
	PTCONbits.SEIEN = false; 
}

/**
  @Summary
    Enables PWM latched fault mode for specific instance.

  @Description
    This routine is used to enable PWM latched fault mode for specific instance.

  @Param
    genNum - PWM generator instance number.

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    
    genNum = PWM_GENERATOR_1;
    PWM_FaultModeLatchEnable(genNum);
    </code>
*/ 
inline static void PWM_FaultModeLatchEnable(PWM_GENERATOR genNum)
{
    switch(genNum) { 
        case PWM_GENERATOR_1:
                __builtin_write_PWMSFR(&FCLCON1, (FCLCON1 & 0xFFFC), &PWMKEY);                
                break;       
        case PWM_GENERATOR_2:
                __builtin_write_PWMSFR(&FCLCON2, (FCLCON2 & 0xFFFC), &PWMKEY);                
                break;       
        case PWM_GENERATOR_3:
                __builtin_write_PWMSFR(&FCLCON3, (FCLCON3 & 0xFFFC), &PWMKEY);                
                break;       
        case PWM_GENERATOR_4:
                __builtin_write_PWMSFR(&FCLCON4, (FCLCON4 & 0xFFFC), &PWMKEY);                
                break;       
        case PWM_GENERATOR_5:
                __builtin_write_PWMSFR(&FCLCON5, (FCLCON5 & 0xFFFC), &PWMKEY);                
                break;       
        case PWM_GENERATOR_6:
                __builtin_write_PWMSFR(&FCLCON6, (FCLCON6 & 0xFFFC), &PWMKEY);                
                break;       
        default:break;    
    }
}

/**
  @Summary
    Disables PWM latched fault mode for specific instance.

  @Description
    This routine is used to disable PWM latched fault mode for specific instance.

  @Param
    genNum - PWM generator instance number.

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    
    genNum = PWM_GENERATOR_1;
    PWM_FaultModeLatchDisable(genNum);
    </code>
*/  
inline static void PWM_FaultModeLatchDisable(PWM_GENERATOR genNum)
{
    switch(genNum) { 
        case PWM_GENERATOR_1: 
                __builtin_write_PWMSFR(&FCLCON1, (FCLCON1 | 0x0003), &PWMKEY);
                break;       
        case PWM_GENERATOR_2: 
                __builtin_write_PWMSFR(&FCLCON2, (FCLCON2 | 0x0003), &PWMKEY);
                break;       
        case PWM_GENERATOR_3: 
                __builtin_write_PWMSFR(&FCLCON3, (FCLCON3 | 0x0003), &PWMKEY);
                break;       
        case PWM_GENERATOR_4: 
                __builtin_write_PWMSFR(&FCLCON4, (FCLCON4 | 0x0003), &PWMKEY);
                break;       
        case PWM_GENERATOR_5: 
                __builtin_write_PWMSFR(&FCLCON5, (FCLCON5 | 0x0003), &PWMKEY);
                break;       
        case PWM_GENERATOR_6: 
                __builtin_write_PWMSFR(&FCLCON6, (FCLCON6 | 0x0003), &PWMKEY);
                break;       
        default:break;   
    }   
}

/**
  @Summary
    Clears PWM interupt request flag for specific instance.

  @Description
    This routine is used to clear PWM interupt request flag for specific instance.

  @Param
    genNum - PWM generator instance number.

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    
    genNum = PWM_GENERATOR_1;
    PWM_InterruptFlagClear(genNum);
    </code>
*/ 
inline static void PWM_InterruptFlagClear(PWM_GENERATOR genNum)
{	
    switch(genNum) { 
        case PWM_GENERATOR_1:
                IFS5bits.PWM1IF = false;                 
                break;       
        case PWM_GENERATOR_2:
                IFS5bits.PWM2IF = false;                 
                break;       
        case PWM_GENERATOR_3:
                IFS6bits.PWM3IF = false;                 
                break;       
        case PWM_GENERATOR_4:
                IFS6bits.PWM4IF = false;                 
                break;       
        case PWM_GENERATOR_5:
                IFS6bits.PWM5IF = false;                 
                break;       
        case PWM_GENERATOR_6:
                IFS6bits.PWM6IF = false;                 
                break;       
        default:break;    
    }
}

/**
  @Summary
    Clears PWM fault status for specific instance.

  @Description
    This routine is used to clear PWM fault status for specific instance.

  @Param
    genNum - PWM generator instance number.

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    
    genNum = PWM_GENERATOR_1;
    PWM_FaultInterruptStatusClear(genNum);
    </code>
*/ 
inline static void PWM_FaultInterruptStatusClear(PWM_GENERATOR genNum)
{
    switch(genNum) { 
        case PWM_GENERATOR_1:
                PWMCON1bits.FLTIEN = false;
                PWMCON1bits.FLTIEN = true;                
                break;       
        case PWM_GENERATOR_2:
                PWMCON2bits.FLTIEN = false;
                PWMCON2bits.FLTIEN = true;                
                break;       
        case PWM_GENERATOR_3:
                PWMCON3bits.FLTIEN = false;
                PWMCON3bits.FLTIEN = true;                
                break;       
        case PWM_GENERATOR_4:
                PWMCON4bits.FLTIEN = false;
                PWMCON4bits.FLTIEN = true;                
                break;       
        case PWM_GENERATOR_5:
                PWMCON5bits.FLTIEN = false;
                PWMCON5bits.FLTIEN = true;                
                break;       
        case PWM_GENERATOR_6:
                PWMCON6bits.FLTIEN = false;
                PWMCON6bits.FLTIEN = true;                
                break;       
        default:break;    
    }
}

/**
  @Summary
    Gets PWM fault status for specific instance.

  @Description
    This routine is used to get PWM fault status for specific instance.

  @Param
    genNum - PWM generator instance number.

  @Returns
    Fault interrupt status value.
 
  @Example 
    <code>    
    bool status;
    PWM_GENERATOR genNum;
    
    genNum = PWM_GENERATOR_1;
    status = PWM_FaultInterruptStatusGet(genNum);
    </code>
*/ 
inline static bool PWM_FaultInterruptStatusGet(PWM_GENERATOR genNum)
{		
    switch(genNum) { 
        case PWM_GENERATOR_1:
                return PWMCON1bits.FLTSTAT;                
                break;       
        case PWM_GENERATOR_2:
                return PWMCON2bits.FLTSTAT;                
                break;       
        case PWM_GENERATOR_3:
                return PWMCON3bits.FLTSTAT;                
                break;       
        case PWM_GENERATOR_4:
                return PWMCON4bits.FLTSTAT;                
                break;       
        case PWM_GENERATOR_5:
                return PWMCON5bits.FLTSTAT;                
                break;       
        case PWM_GENERATOR_6:
                return PWMCON6bits.FLTSTAT;                
                break;       
        default:break;    
    }
}

/**
  @Summary
    Clears PWM current limit status for specific instance.

  @Description
    This routine is used to clear PWM current limit status for specific instance.

  @Param
    genNum - PWM generator instance number.

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    
    genNum = PWM_GENERATOR_1;
    PWM_CurrentLimitInterruptStatusClear(genNum);
    </code>
*/ 
inline static void PWM_CurrentLimitInterruptStatusClear(PWM_GENERATOR genNum)
{
    switch(genNum) { 
        case PWM_GENERATOR_1:
                PWMCON1bits.CLIEN = false;
                PWMCON1bits.CLIEN = true;                
                break;       
        case PWM_GENERATOR_2:
                PWMCON2bits.CLIEN = false;
                PWMCON2bits.CLIEN = true;                
                break;       
        case PWM_GENERATOR_3:
                PWMCON3bits.CLIEN = false;
                PWMCON3bits.CLIEN = true;                
                break;       
        case PWM_GENERATOR_4:
                PWMCON4bits.CLIEN = false;
                PWMCON4bits.CLIEN = true;                
                break;       
        case PWM_GENERATOR_5:
                PWMCON5bits.CLIEN = false;
                PWMCON5bits.CLIEN = true;                
                break;       
        case PWM_GENERATOR_6:
                PWMCON6bits.CLIEN = false;
                PWMCON6bits.CLIEN = true;                
                break;       
        default:break;    
    }
}

/**
  @Summary
    Gets PWM current limit status for specific instance.

  @Description
    This routine is used to get PWM current limit status for specific instance.

  @Param
    genNum - PWM generator instance number.

  @Returns
    Current Limit interrupt status value.
 
  @Example 
    <code>    
    bool status;
    PWM_GENERATOR genNum;
    
    genNum = PWM_GENERATOR_1;
    status = PWM_CurrentLimitInterruptStatusGet(genNum);
    </code>
*/  
inline static bool PWM_CurrentLimitInterruptStatusGet(PWM_GENERATOR genNum)
{	
    switch(genNum) { 
        case PWM_GENERATOR_1:
                return PWMCON1bits.CLSTAT;                
                break;       
        case PWM_GENERATOR_2:
                return PWMCON2bits.CLSTAT;                
                break;       
        case PWM_GENERATOR_3:
                return PWMCON3bits.CLSTAT;                
                break;       
        case PWM_GENERATOR_4:
                return PWMCON4bits.CLSTAT;                
                break;       
        case PWM_GENERATOR_5:
                return PWMCON5bits.CLSTAT;                
                break;       
        case PWM_GENERATOR_6:
                return PWMCON6bits.CLSTAT;                
                break;       
        default:break;    
    }
}

/**
  @Summary
    Clears PWM trigger status for specific instance.

  @Description
    This routine is used to clear PWM trigger status for specific instance.

  @Param
    genNum - PWM generator instance number.

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    
    genNum = PWM_GENERATOR_1;
    PWM_TriggerInterruptStatusClear(genNum);
    </code>
*/ 
inline static void PWM_TriggerInterruptStatusClear(PWM_GENERATOR genNum)
{
    switch(genNum) { 
        case PWM_GENERATOR_1:
                PWMCON1bits.TRGIEN = false;
                PWMCON1bits.TRGIEN = true;                
                break;       
        case PWM_GENERATOR_2:
                PWMCON2bits.TRGIEN = false;
                PWMCON2bits.TRGIEN = true;                
                break;       
        case PWM_GENERATOR_3:
                PWMCON3bits.TRGIEN = false;
                PWMCON3bits.TRGIEN = true;                
                break;       
        case PWM_GENERATOR_4:
                PWMCON4bits.TRGIEN = false;
                PWMCON4bits.TRGIEN = true;                
                break;       
        case PWM_GENERATOR_5:
                PWMCON5bits.TRGIEN = false;
                PWMCON5bits.TRGIEN = true;                
                break;       
        case PWM_GENERATOR_6:
                PWMCON6bits.TRGIEN = false;
                PWMCON6bits.TRGIEN = true;                
                break;       
        default:break;    
    }
}

/**
  @Summary
    Gets PWM trigger status for specific instance.

  @Description
    This routine is used to get PWM trigger status for specific instance.

  @Param
    genNum - PWM generator instance number.

  @Returns
    Trigger interrupt status value.
 
  @Example 
    <code>    
    bool status;
    PWM_GENERATOR genNum;
    
    genNum = PWM_GENERATOR_1;
    status = PWM_TriggerInterruptStatusGet(genNum);
    </code>
*/  
inline static bool PWM_TriggerInterruptStatusGet(PWM_GENERATOR genNum)
{
    switch(genNum) { 
        case PWM_GENERATOR_1:
                return PWMCON1bits.TRGSTAT;                
                break;       
        case PWM_GENERATOR_2:
                return PWMCON2bits.TRGSTAT;                
                break;       
        case PWM_GENERATOR_3:
                return PWMCON3bits.TRGSTAT;                
                break;       
        case PWM_GENERATOR_4:
                return PWMCON4bits.TRGSTAT;                
                break;       
        case PWM_GENERATOR_5:
                return PWMCON5bits.TRGSTAT;                
                break;       
        case PWM_GENERATOR_6:
                return PWMCON6bits.TRGSTAT;                
                break;       
        default:break;  
    }
}

/**
  @Summary
    Enables PWM override on PWML output for specific instance.

  @Description
    This routine is used to enable PWM override on PWML output for specific instance.

  @Param
    genNum - PWM generator instance number.

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    
    genNum = PWM_GENERATOR_1;
    PWM_OverrideLowEnable(genNum);
    </code>
*/  
inline static void PWM_OverrideLowEnable(PWM_GENERATOR genNum)
{
    switch(genNum) { 
        case PWM_GENERATOR_1:
                __builtin_write_PWMSFR(&IOCON1, (IOCON1 | 0x0100), &PWMKEY);                
                break;       
        case PWM_GENERATOR_2:
                __builtin_write_PWMSFR(&IOCON2, (IOCON2 | 0x0100), &PWMKEY);                
                break;       
        case PWM_GENERATOR_3:
                __builtin_write_PWMSFR(&IOCON3, (IOCON3 | 0x0100), &PWMKEY);                
                break;       
        case PWM_GENERATOR_4:
                __builtin_write_PWMSFR(&IOCON4, (IOCON4 | 0x0100), &PWMKEY);                
                break;       
        case PWM_GENERATOR_5:
                __builtin_write_PWMSFR(&IOCON5, (IOCON5 | 0x0100), &PWMKEY);                
                break;       
        case PWM_GENERATOR_6:
                __builtin_write_PWMSFR(&IOCON6, (IOCON6 | 0x0100), &PWMKEY);                
                break;       
        default:break;  
    }
}

/**
  @Summary
    Disables PWM override on PWML output for specific instance.

  @Description
    This routine is used to disable PWM override on PWML output for specific instance.

  @Param
    genNum - PWM generator instance number.

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    
    genNum = PWM_GENERATOR_1;
    PWM_OverrideLowDisable(genNum);
    </code>
*/  
inline static void PWM_OverrideLowDisable(PWM_GENERATOR genNum)
{
    switch(genNum) { 
        case PWM_GENERATOR_1:
                __builtin_write_PWMSFR(&IOCON1, (IOCON1 & 0xFEFF), &PWMKEY);
                break;       
        case PWM_GENERATOR_2:
                __builtin_write_PWMSFR(&IOCON2, (IOCON2 & 0xFEFF), &PWMKEY);
                break;       
        case PWM_GENERATOR_3:
                __builtin_write_PWMSFR(&IOCON3, (IOCON3 & 0xFEFF), &PWMKEY);
                break;       
        case PWM_GENERATOR_4:
                __builtin_write_PWMSFR(&IOCON4, (IOCON4 & 0xFEFF), &PWMKEY);
                break;       
        case PWM_GENERATOR_5:
                __builtin_write_PWMSFR(&IOCON5, (IOCON5 & 0xFEFF), &PWMKEY);
                break;       
        case PWM_GENERATOR_6:
                __builtin_write_PWMSFR(&IOCON6, (IOCON6 & 0xFEFF), &PWMKEY);
                break;       
        default:break;  
    }
}

/**
  @Summary
    Enables PWM override on PWMH output for specific instance.

  @Description
    This routine is used to enable PWM override on PWMH output for specific instance.

  @Param
    genNum - PWM generator instance number.

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    
    genNum = PWM_GENERATOR_1;
    PWM_OverrideHighEnable(genNum);
    </code>
*/  
inline static void PWM_OverrideHighEnable(PWM_GENERATOR genNum)
{
    switch(genNum) { 
        case PWM_GENERATOR_1:
                __builtin_write_PWMSFR(&IOCON1, (IOCON1 | 0x0200), &PWMKEY);                
                break;       
        case PWM_GENERATOR_2:
                __builtin_write_PWMSFR(&IOCON2, (IOCON2 | 0x0200), &PWMKEY);                
                break;       
        case PWM_GENERATOR_3:
                __builtin_write_PWMSFR(&IOCON3, (IOCON3 | 0x0200), &PWMKEY);                
                break;       
        case PWM_GENERATOR_4:
                __builtin_write_PWMSFR(&IOCON4, (IOCON4 | 0x0200), &PWMKEY);                
                break;       
        case PWM_GENERATOR_5:
                __builtin_write_PWMSFR(&IOCON5, (IOCON5 | 0x0200), &PWMKEY);                
                break;       
        case PWM_GENERATOR_6:
                __builtin_write_PWMSFR(&IOCON6, (IOCON6 | 0x0200), &PWMKEY);                
                break;       
        default:break;  
    }
}

/**
  @Summary
    Disables PWM override on PWMH output for specific instance.

  @Description
    This routine is used to disable PWM override on PWMH output for specific instance.

  @Param
    genNum - PWM generator instance number.

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    
    genNum = PWM_GENERATOR_1;
    PWM_OverrideHighDisable(genNum);
    </code>
*/  
inline static void PWM_OverrideHighDisable(PWM_GENERATOR genNum)
{
    switch(genNum) { 
        case PWM_GENERATOR_1:   
                __builtin_write_PWMSFR(&IOCON1, (IOCON1 & 0xFDFF), &PWMKEY);
                break;       
        case PWM_GENERATOR_2:   
                __builtin_write_PWMSFR(&IOCON2, (IOCON2 & 0xFDFF), &PWMKEY);
                break;       
        case PWM_GENERATOR_3:   
                __builtin_write_PWMSFR(&IOCON3, (IOCON3 & 0xFDFF), &PWMKEY);
                break;       
        case PWM_GENERATOR_4:   
                __builtin_write_PWMSFR(&IOCON4, (IOCON4 & 0xFDFF), &PWMKEY);
                break;       
        case PWM_GENERATOR_5:   
                __builtin_write_PWMSFR(&IOCON5, (IOCON5 & 0xFDFF), &PWMKEY);
                break;       
        case PWM_GENERATOR_6:   
                __builtin_write_PWMSFR(&IOCON6, (IOCON6 & 0xFDFF), &PWMKEY);
                break;       
        default:break;  
    }
}

/**
  @Summary
    Updates PWM override data bits with the requested value for a specific instance.

  @Description
    This routine is used to updates PWM override data bits with the requested value for a specific instance.

  @Param
    genNum          - PWM generator instance number.
    overrideData    - Override data

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    uint16_t overrideData;

    overrideData = 0x01;
    
    genNum = PWM_GENERATOR_1;
    PWM_OverrideDataSet(genNum, overrideData);
    </code>
*/  
inline static void PWM_OverrideDataSet(PWM_GENERATOR genNum,uint16_t overrideData)
{
    switch(genNum) { 
        case PWM_GENERATOR_1:
                overrideData = ((overrideData & 0xFFFC)<<6);
                __builtin_write_PWMSFR(&IOCON1, (IOCON1 | overrideData), &PWMKEY);                
                break;       
        case PWM_GENERATOR_2:
                overrideData = ((overrideData & 0xFFFC)<<6);
                __builtin_write_PWMSFR(&IOCON2, (IOCON2 | overrideData), &PWMKEY);                
                break;       
        case PWM_GENERATOR_3:
                overrideData = ((overrideData & 0xFFFC)<<6);
                __builtin_write_PWMSFR(&IOCON3, (IOCON3 | overrideData), &PWMKEY);                
                break;       
        case PWM_GENERATOR_4:
                overrideData = ((overrideData & 0xFFFC)<<6);
                __builtin_write_PWMSFR(&IOCON4, (IOCON4 | overrideData), &PWMKEY);                
                break;       
        case PWM_GENERATOR_5:
                overrideData = ((overrideData & 0xFFFC)<<6);
                __builtin_write_PWMSFR(&IOCON5, (IOCON5 | overrideData), &PWMKEY);                
                break;       
        case PWM_GENERATOR_6:
                overrideData = ((overrideData & 0xFFFC)<<6);
                __builtin_write_PWMSFR(&IOCON6, (IOCON6 | overrideData), &PWMKEY);                
                break;       
        default:break;  
    }
}

/**
  @Summary
    Sets the PWM duty cycle for specific instance.

  @Description
    This routine is used to set the PWM duty cycle for specific instance.

  @Param
    genNum      - PWM generator instance number.
    dutyCycle   - Duty cycle value

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    uint16_t dutyCycle;

    dutyCycle = 0x01;
    
    genNum = PWM_GENERATOR_1;
    PWM_DutyCycleSet(genNum, dutyCycle);
    </code>
*/  
inline static void PWM_DutyCycleSet(PWM_GENERATOR genNum,uint16_t dutyCycle)
{
    switch(genNum) { 
        case PWM_GENERATOR_1:
                PDC1 = dutyCycle;                
                break;       
        case PWM_GENERATOR_2:
                PDC2 = dutyCycle;                
                break;       
        case PWM_GENERATOR_3:
                PDC3 = dutyCycle;                
                break;       
        case PWM_GENERATOR_4:
                PDC4 = dutyCycle;                
                break;       
        case PWM_GENERATOR_5:
                PDC5 = dutyCycle;                
                break;       
        case PWM_GENERATOR_6:
                PDC6 = dutyCycle;                
                break;       
        default:break;  
    }
}

/**
  @Summary
    Sets the PWM period value while in Center-Aligned PWM mode for specific instance.

  @Description
    This routine is used to disable interrupt request for Special Event Trigger from the Primary time base generator.

  @Param
    genNum  - PWM generator instance number.
    period  - Period value

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    uint16_t period;

    period = 0x01;
    
    genNum = PWM_GENERATOR_1;
    PWM_PeriodCenterAlignedModeSet(genNum, period);
    </code>
*/  
inline static void PWM_PeriodCenterAlignedModeSet(PWM_GENERATOR genNum,uint16_t period)
{
    switch(genNum) { 
        case PWM_GENERATOR_1:
                PHASE1 = period;                
                break;       
        case PWM_GENERATOR_2:
                PHASE2 = period;                
                break;       
        case PWM_GENERATOR_3:
                PHASE3 = period;                
                break;       
        case PWM_GENERATOR_4:
                PHASE4 = period;                
                break;       
        case PWM_GENERATOR_5:
                PHASE5 = period;                
                break;       
        case PWM_GENERATOR_6:
                PHASE6 = period;                
                break;       
        default:break;  
    }
}

/**
  @Summary
    Sets the PWM deadtime value while in Center-Aligned PWM mode for specific instance.

  @Description
    This routine is used to set the PWM deadtime value while in Center-Aligned PWM mode for specific instance.

  @Param
    genNum      - PWM generator instance number.
    deadtime    - Dead time value.

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    uint16_t deadtime;

    deadtime = 0x01;
    
    genNum = PWM_GENERATOR_1;
    PWM_DeadTimeCenterAlignedModeSet(genNum, deadtime);
    </code>
*/  
inline static void PWM_DeadTimeCenterAlignedModeSet(PWM_GENERATOR genNum,uint16_t deadtime)
{
    switch(genNum) { 
        case PWM_GENERATOR_1:
                ALTDTR1 = deadtime;                
                break;       
        case PWM_GENERATOR_2:
                ALTDTR2 = deadtime;                
                break;       
        case PWM_GENERATOR_3:
                ALTDTR3 = deadtime;                
                break;       
        case PWM_GENERATOR_4:
                ALTDTR4 = deadtime;                
                break;       
        case PWM_GENERATOR_5:
                ALTDTR5 = deadtime;                
                break;       
        case PWM_GENERATOR_6:
                ALTDTR6 = deadtime;                
                break;       
        default:break;  
    }
}

/**
  @Summary
    Sets the PWM trigger output divider to the desired value for specific instance.

  @Description
    This routine is used to set the PWM trigger output divider to the desired value for specific instance.

  @Param
    genNum          - PWM generator instance number.
    trigDivValue    - Trigger value.

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    uint16_t trigDivValue;

    trigDivValue = 0x01;
    
    genNum = PWM_GENERATOR_1;
    PWM_TriggerDividerSet(genNum, trigDivValue);
    </code>
*/  
inline static void PWM_TriggerDividerSet(PWM_GENERATOR genNum,uint16_t trigDivValue)
{
    switch(genNum) { 
        case PWM_GENERATOR_1: 
                TRGCON1bits.TRGDIV = trigDivValue;               
                break;       
        case PWM_GENERATOR_2: 
                TRGCON2bits.TRGDIV = trigDivValue;               
                break;       
        case PWM_GENERATOR_3: 
                TRGCON3bits.TRGDIV = trigDivValue;               
                break;       
        case PWM_GENERATOR_4: 
                TRGCON4bits.TRGDIV = trigDivValue;               
                break;       
        case PWM_GENERATOR_5: 
                TRGCON5bits.TRGDIV = trigDivValue;               
                break;       
        case PWM_GENERATOR_6: 
                TRGCON6bits.TRGDIV = trigDivValue;               
                break;       
        default:break;  
    }
}

/**
  @Summary
    Sets the PWM trigger start delay to the desired value for specific instance.

  @Description
    This routine is used to set the PWM trigger start delay to the desired value for specific instance.

  @Param
    genNum      - PWM generator instance number.
    delayValue  - Trigger start delay value.

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    uint16_t delayValue;

    delayValue = 0x01;
    
    genNum = PWM_GENERATOR_1;
    PWM_TriggerStartDelaySet(genNum, delayValue);
    </code>
*/  
inline static void PWM_TriggerStartDelaySet(PWM_GENERATOR genNum,uint16_t delayValue)
{
    switch(genNum) { 
        case PWM_GENERATOR_1: 
                TRGCON1bits.TRGSTRT = delayValue;               
                break;       
        case PWM_GENERATOR_2: 
                TRGCON2bits.TRGSTRT = delayValue;               
                break;       
        case PWM_GENERATOR_3: 
                TRGCON3bits.TRGSTRT = delayValue;               
                break;       
        case PWM_GENERATOR_4: 
                TRGCON4bits.TRGSTRT = delayValue;               
                break;       
        case PWM_GENERATOR_5: 
                TRGCON5bits.TRGSTRT = delayValue;               
                break;       
        case PWM_GENERATOR_6: 
                TRGCON6bits.TRGSTRT = delayValue;               
                break;       
        default:break;  
    }
}

/**
  @Summary
    Sets the PWM trigger compare to the desired value for specific instance.

  @Description
    This routine is used to set the PWM trigger compare to the desired value for specific instance.

  @Param
    genNum          - PWM generator instance number.
    trigCompValue   - Trigger compare value.

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    uint16_t trigCompValue;

    trigCompValue = 0x01;
    
    genNum = PWM_GENERATOR_1;
    PWM_TriggerCompareValueSet(genNum, trigCompValue);
    </code>
*/  
inline static void PWM_TriggerCompareValueSet(PWM_GENERATOR genNum,uint16_t trigCompValue)
{
    switch(genNum) { 
        case PWM_GENERATOR_1: 
                TRIG1 = trigCompValue;               
                break;       
        case PWM_GENERATOR_2: 
                TRIG2 = trigCompValue;               
                break;       
        case PWM_GENERATOR_3: 
                TRIG3 = trigCompValue;               
                break;       
        case PWM_GENERATOR_4: 
                TRIG4 = trigCompValue;               
                break;       
        case PWM_GENERATOR_5: 
                TRIG5 = trigCompValue;               
                break;       
        case PWM_GENERATOR_6: 
                TRIG6 = trigCompValue;               
                break;       
        default:break;  
    }
}

/**
  @Summary
    Enables trigger event interrupt requests from specific PWM instance.

  @Description
    This routine is used to enable trigger event interrupt requests from specific PWM instance.

  @Param
    genNum - PWM generator instance number.

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    
    genNum = PWM_GENERATOR_1;
    PWM_TriggerInterruptEnable(genNum);
    </code>
*/  
inline static void PWM_TriggerInterruptEnable(PWM_GENERATOR genNum)
{	
     switch(genNum) { 
        case PWM_GENERATOR_1: 
                PWMCON1bits.TRGIEN = true;               
                break;       
        case PWM_GENERATOR_2: 
                PWMCON2bits.TRGIEN = true;               
                break;       
        case PWM_GENERATOR_3: 
                PWMCON3bits.TRGIEN = true;               
                break;       
        case PWM_GENERATOR_4: 
                PWMCON4bits.TRGIEN = true;               
                break;       
        case PWM_GENERATOR_5: 
                PWMCON5bits.TRGIEN = true;               
                break;       
        case PWM_GENERATOR_6: 
                PWMCON6bits.TRGIEN = true;               
                break;       
        default:break;  
    }
}

/**
  @Summary
    Disables trigger event interrupt requests from specific PWM instance.

  @Description
    This routine is used to disable trigger event interrupt requests from specific PWM instance.

  @Param
    genNum - PWM generator instance number.

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    
    genNum = PWM_GENERATOR_1;
    PWM_TriggerInterruptDisable(genNum);
    </code>
*/  
inline static void PWM_TriggerInterruptDisable(PWM_GENERATOR genNum)
{	
     switch(genNum) { 
        case PWM_GENERATOR_1: 
                PWMCON1bits.TRGIEN = false;               
                break;       
        case PWM_GENERATOR_2: 
                PWMCON2bits.TRGIEN = false;               
                break;       
        case PWM_GENERATOR_3: 
                PWMCON3bits.TRGIEN = false;               
                break;       
        case PWM_GENERATOR_4: 
                PWMCON4bits.TRGIEN = false;               
                break;       
        case PWM_GENERATOR_5: 
                PWMCON5bits.TRGIEN = false;               
                break;       
        case PWM_GENERATOR_6: 
                PWMCON6bits.TRGIEN = false;               
                break;       
        default:break;  
    }
}

/**
  @Summary
    Enables current limit interrupt requests from specific PWM instance.

  @Description
    This routine is used to enable current limit interrupt requests from specific PWM instance.

  @Param
    genNum - PWM generator instance number.

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    
    genNum = PWM_GENERATOR_1;
    PWM_CurrentLimitInterruptEnable(genNum);
    </code>
*/  
inline static void PWM_CurrentLimitInterruptEnable(PWM_GENERATOR genNum)
{	
    switch(genNum) { 
        case PWM_GENERATOR_1: 
                PWMCON1bits.CLIEN = true;               
                break;       
        case PWM_GENERATOR_2: 
                PWMCON2bits.CLIEN = true;               
                break;       
        case PWM_GENERATOR_3: 
                PWMCON3bits.CLIEN = true;               
                break;       
        case PWM_GENERATOR_4: 
                PWMCON4bits.CLIEN = true;               
                break;       
        case PWM_GENERATOR_5: 
                PWMCON5bits.CLIEN = true;               
                break;       
        case PWM_GENERATOR_6: 
                PWMCON6bits.CLIEN = true;               
                break;       
        default:break;  
    }
}

/**
  @Summary
    Disables current limit interrupt requests from specific PWM instance.

  @Description
    This routine is used to disable current limit interrupt requests from specific PWM instance.

  @Param
    genNum - PWM generator instance number.

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    
    genNum = PWM_GENERATOR_1;
    PWM_CurrentLimitInterruptDisable(genNum);
    </code>
*/  
inline static void PWM_CurrentLimitInterruptDisable(PWM_GENERATOR genNum)
{	
     switch(genNum) { 
        case PWM_GENERATOR_1: 
                PWMCON1bits.CLIEN = false;               
                break;       
        case PWM_GENERATOR_2: 
                PWMCON2bits.CLIEN = false;               
                break;       
        case PWM_GENERATOR_3: 
                PWMCON3bits.CLIEN = false;               
                break;       
        case PWM_GENERATOR_4: 
                PWMCON4bits.CLIEN = false;               
                break;       
        case PWM_GENERATOR_5: 
                PWMCON5bits.CLIEN = false;               
                break;       
        case PWM_GENERATOR_6: 
                PWMCON6bits.CLIEN = false;               
                break;       
        default:break;  
    }
}

/**
  @Summary
    Enables fault event interrupt requests from specific PWM instance.

  @Description
    This routine is used to enable fault event interrupt requests from specific PWM instance.

  @Param
    genNum - PWM generator instance number.

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    
    genNum = PWM_GENERATOR_1;
    PWM_FaultInterruptEnable(genNum);
    </code>
*/  
inline static void PWM_FaultInterruptEnable(PWM_GENERATOR genNum)
{	
    switch(genNum) { 
        case PWM_GENERATOR_1: 
                PWMCON1bits.FLTIEN = true;               
                break;       
        case PWM_GENERATOR_2: 
                PWMCON2bits.FLTIEN = true;               
                break;       
        case PWM_GENERATOR_3: 
                PWMCON3bits.FLTIEN = true;               
                break;       
        case PWM_GENERATOR_4: 
                PWMCON4bits.FLTIEN = true;               
                break;       
        case PWM_GENERATOR_5: 
                PWMCON5bits.FLTIEN = true;               
                break;       
        case PWM_GENERATOR_6: 
                PWMCON6bits.FLTIEN = true;               
                break;       
        default:break;  
    } 
}

/**
  @Summary
    Disables fault event interrupt requests from specific PWM instance.

  @Description
    This routine is used to disable fault event interrupt requests from specific PWM instance.

  @Param
    genNum - PWM generator instance number.

  @Returns
    None
 
  @Example 
    <code>    
    PWM_GENERATOR genNum;
    
    genNum = PWM_GENERATOR_1;
    PWM_FaultInterruptDisable(genNum);
    </code>
*/  
inline static void PWM_FaultInterruptDisable(PWM_GENERATOR genNum)
{	
    switch(genNum) { 
        case PWM_GENERATOR_1: 
                PWMCON1bits.FLTIEN = false;               
                break;       
        case PWM_GENERATOR_2: 
                PWMCON2bits.FLTIEN = false;               
                break;       
        case PWM_GENERATOR_3: 
                PWMCON3bits.FLTIEN = false;               
                break;       
        case PWM_GENERATOR_4: 
                PWMCON4bits.FLTIEN = false;               
                break;       
        case PWM_GENERATOR_5: 
                PWMCON5bits.FLTIEN = false;               
                break;       
        case PWM_GENERATOR_6: 
                PWMCON6bits.FLTIEN = false;               
                break;       
        default:break;  
    }
}

/**
  @Summary
    Callback for PWM Special Event.

  @Description
    This routine is callback for PWM Special Event

  @Param
    None.

  @Returns
    None
 
  @Example 
	Refer to PWM_Initialize(); for an example
*/
void PWM_SpecialEvent_CallBack(void);

/**
  @Summary
    Polled implementation

  @Description
    This routine is used to implement the tasks for polled implementations.
  
  @Preconditions
    PWM_Initialize() function should have been 
    called before calling this function.
 
  @Returns 
    None
 
  @Param
    None
 
  @Example
    Refer to PWM_Initialize(); for an example
    
*/
void PWM_SpecialEvent_Tasks(void);

/**
  @Summary
    Callback for PWM Generator1.

  @Description
    This routine is callback for PWM Generator1

  @Param
    None.

  @Returns
    None
 
  @Example 
	Refer to PWM_Initialize(); for an example
*/
void PWM_Generator1_CallBack(void);

/**
  @Summary
    Polled implementation

  @Description
    This routine is used to implement the tasks for polled implementations.
  
  @Preconditions
    PWM_Initialize() function should have been 
    called before calling this function.
 
  @Returns 
    None
 
  @Param
    None
 
  @Example
    Refer to PWM_Initialize(); for an example
    
*/
void PWM_Generator1_Tasks(void);
/**
  @Summary
    Callback for PWM Generator2.

  @Description
    This routine is callback for PWM Generator2

  @Param
    None.

  @Returns
    None
 
  @Example 
	Refer to PWM_Initialize(); for an example
*/
void PWM_Generator2_CallBack(void);

/**
  @Summary
    Polled implementation

  @Description
    This routine is used to implement the tasks for polled implementations.
  
  @Preconditions
    PWM_Initialize() function should have been 
    called before calling this function.
 
  @Returns 
    None
 
  @Param
    None
 
  @Example
    Refer to PWM_Initialize(); for an example
    
*/
void PWM_Generator2_Tasks(void);
/**
  @Summary
    Callback for PWM Generator3.

  @Description
    This routine is callback for PWM Generator3

  @Param
    None.

  @Returns
    None
 
  @Example 
	Refer to PWM_Initialize(); for an example
*/
void PWM_Generator3_CallBack(void);

/**
  @Summary
    Polled implementation

  @Description
    This routine is used to implement the tasks for polled implementations.
  
  @Preconditions
    PWM_Initialize() function should have been 
    called before calling this function.
 
  @Returns 
    None
 
  @Param
    None
 
  @Example
    Refer to PWM_Initialize(); for an example
    
*/
void PWM_Generator3_Tasks(void);
/**
  @Summary
    Callback for PWM Generator4.

  @Description
    This routine is callback for PWM Generator4

  @Param
    None.

  @Returns
    None
 
  @Example 
	Refer to PWM_Initialize(); for an example
*/
void PWM_Generator4_CallBack(void);

/**
  @Summary
    Polled implementation

  @Description
    This routine is used to implement the tasks for polled implementations.
  
  @Preconditions
    PWM_Initialize() function should have been 
    called before calling this function.
 
  @Returns 
    None
 
  @Param
    None
 
  @Example
    Refer to PWM_Initialize(); for an example
    
*/
void PWM_Generator4_Tasks(void);
/**
  @Summary
    Callback for PWM Generator5.

  @Description
    This routine is callback for PWM Generator5

  @Param
    None.

  @Returns
    None
 
  @Example 
	Refer to PWM_Initialize(); for an example
*/
void PWM_Generator5_CallBack(void);

/**
  @Summary
    Polled implementation

  @Description
    This routine is used to implement the tasks for polled implementations.
  
  @Preconditions
    PWM_Initialize() function should have been 
    called before calling this function.
 
  @Returns 
    None
 
  @Param
    None
 
  @Example
    Refer to PWM_Initialize(); for an example
    
*/
void PWM_Generator5_Tasks(void);
/**
  @Summary
    Callback for PWM Generator6.

  @Description
    This routine is callback for PWM Generator6

  @Param
    None.

  @Returns
    None
 
  @Example 
	Refer to PWM_Initialize(); for an example
*/
void PWM_Generator6_CallBack(void);

/**
  @Summary
    Polled implementation

  @Description
    This routine is used to implement the tasks for polled implementations.
  
  @Preconditions
    PWM_Initialize() function should have been 
    called before calling this function.
 
  @Returns 
    None
 
  @Param
    None
 
  @Example
    Refer to PWM_Initialize(); for an example
    
*/
void PWM_Generator6_Tasks(void);

//CHB
void PWM_Set(uint8_t PWM_Output, uint16_t PWM_Duty);

#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

#endif //_PWM_H
    
/**
 End of File
*/
