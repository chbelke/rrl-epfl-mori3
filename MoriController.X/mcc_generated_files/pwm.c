
/**
  PWM Generated Driver API Source File 

  @Company
    Microchip Technology Inc.

  @File Name
    pwm.c

  @Summary
    This is the generated source file for the PWM driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This source file provides APIs for driver for PWM. 
    Generation Information : 
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - pic24-dspic-pic32mm : 1.75.1
        Device            :  dsPIC33EP512GM604
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.35
        MPLAB 	          :  MPLAB X v5.05
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

#include <xc.h>
#include "pwm.h"
#include "../define.h"

/**
  Section: Driver Interface
*/


void PWM_Initialize (void)
{
    // PCLKDIV 1; 
    PTCON2 = 0x0;
    // SYNCOEN disabled; SEIEN disabled; SESTAT disabled; SEVTPS 1; SYNCSRC SYNCI1; SYNCEN disabled; EIPU disabled; SYNCPOL disabled; 
    STCON = 0x0;
    // PCLKDIV 1; 
    STCON2 = 0x0;
    // STPER 65528; 
    STPER = 0xFFF8;
    // SSEVTCMP 0; 
    SSEVTCMP = 0x0;
    // PTPER 0; 
    PTPER = 0x0;
    // SEVTCMP 0; 
    SEVTCMP = 0x0;
    // MDC 15; 
    MDC = 0xF;
    // CHOPCLK 0; CHPCLKEN disabled; 
    CHOP = 0x0;
    // PWMKEY 0; 
    PWMKEY = 0x0;
    // MDCS Primary; FLTIEN disabled; CAM Edge Aligned; DTC Positive dead time for all Output modes; TRGIEN disabled; XPRES disabled; ITB Primary; IUE disabled; CLIEN disabled; MTBS disabled; DTCP disabled; 
    PWMCON1 = 0x200;
    // MDCS Primary; FLTIEN disabled; CAM Edge Aligned; DTC Positive dead time for all Output modes; TRGIEN disabled; XPRES disabled; ITB Primary; IUE disabled; CLIEN disabled; MTBS disabled; DTCP disabled; 
    PWMCON2 = 0x200;
    // MDCS Primary; FLTIEN disabled; CAM Edge Aligned; DTC Positive dead time for all Output modes; TRGIEN disabled; XPRES disabled; ITB Primary; IUE disabled; CLIEN disabled; MTBS disabled; DTCP disabled; 
    PWMCON3 = 0x200;
    // MDCS Primary; FLTIEN disabled; CAM Edge Aligned; DTC Positive dead time for all Output modes; TRGIEN disabled; XPRES disabled; ITB Primary; IUE disabled; CLIEN disabled; MTBS disabled; DTCP disabled; 
    PWMCON4 = 0x200;
    // MDCS Primary; FLTIEN disabled; CAM Edge Aligned; DTC Positive dead time for all Output modes; TRGIEN disabled; XPRES disabled; ITB Primary; IUE disabled; CLIEN disabled; MTBS disabled; DTCP disabled; 
    PWMCON5 = 0x200;
    // MDCS Primary; FLTIEN disabled; CAM Edge Aligned; DTC Positive dead time for all Output modes; TRGIEN disabled; XPRES disabled; ITB Primary; IUE disabled; CLIEN disabled; MTBS disabled; DTCP disabled; 
    PWMCON6 = 0x200;
    //FLTDAT PWM1L Low, PWM1H Low; SWAP disabled; OVRENH disabled; PENL enabled; PMOD True Independent Output Mode; OVRENL disabled; OSYNC disabled; POLL disabled; PENH disabled; CLDAT PWM1L Low, PWM1H Low; OVRDAT PWM1L Low, PWM1H Low; POLH disabled; 
    __builtin_write_PWMSFR(&IOCON1, 0x4C00, &PWMKEY);
    //FLTDAT PWM2L Low, PWM2H Low; SWAP disabled; OVRENH disabled; PENL enabled; PMOD True Independent Output Mode; OVRENL disabled; OSYNC disabled; POLL disabled; PENH disabled; CLDAT PWM2L Low, PWM2H Low; OVRDAT PWM2L Low, PWM2H Low; POLH disabled; 
    __builtin_write_PWMSFR(&IOCON2, 0x4C00, &PWMKEY);
    //FLTDAT PWM3L Low, PWM3H Low; SWAP disabled; OVRENH disabled; PENL enabled; PMOD True Independent Output Mode; OVRENL disabled; OSYNC disabled; POLL disabled; PENH disabled; CLDAT PWM3L Low, PWM3H Low; OVRDAT PWM3L Low, PWM3H Low; POLH disabled; 
    __builtin_write_PWMSFR(&IOCON3, 0x4C00, &PWMKEY);
    //FLTDAT PWM4L Low, PWM4H Low; SWAP disabled; OVRENH disabled; PENL enabled; PMOD True Independent Output Mode; OVRENL disabled; OSYNC disabled; POLL disabled; PENH disabled; CLDAT PWM4L Low, PWM4H Low; OVRDAT PWM4L Low, PWM4H Low; POLH disabled; 
    __builtin_write_PWMSFR(&IOCON4, 0x4C00, &PWMKEY);
    //FLTDAT PWM5L Low, PWM5H Low; SWAP disabled; OVRENH disabled; PENL enabled; PMOD True Independent Output Mode; OVRENL disabled; OSYNC disabled; POLL disabled; PENH enabled; CLDAT PWM5L Low, PWM5H Low; OVRDAT PWM5L Low, PWM5H Low; POLH disabled; 
    __builtin_write_PWMSFR(&IOCON5, 0xCC00, &PWMKEY);
    //FLTDAT PWM6L Low, PWM6H Low; SWAP disabled; OVRENH disabled; PENL disabled; PMOD True Independent Output Mode; OVRENL disabled; OSYNC disabled; POLL disabled; PENH disabled; CLDAT PWM6L Low, PWM6H Low; OVRDAT PWM6L Low, PWM6H Low; POLH disabled; 
    __builtin_write_PWMSFR(&IOCON6, 0xC00, &PWMKEY);
    //FLTPOL disabled; CLPOL disabled; CLSRC FLT1; CLMOD disabled; FLTMOD Fault input is disabled; IFLTMOD disabled; FLTSRC FLT32; 
    __builtin_write_PWMSFR(&FCLCON1, 0xFB, &PWMKEY);
    //FLTPOL disabled; CLPOL disabled; CLSRC FLT1; CLMOD disabled; FLTMOD Fault input is disabled; IFLTMOD disabled; FLTSRC FLT32; 
    __builtin_write_PWMSFR(&FCLCON2, 0xFB, &PWMKEY);
    //FLTPOL disabled; CLPOL disabled; CLSRC FLT1; CLMOD disabled; FLTMOD Fault input is disabled; IFLTMOD disabled; FLTSRC FLT32; 
    __builtin_write_PWMSFR(&FCLCON3, 0xFB, &PWMKEY);
    //FLTPOL disabled; CLPOL disabled; CLSRC FLT1; CLMOD disabled; FLTMOD Fault input is disabled; IFLTMOD disabled; FLTSRC FLT32; 
    __builtin_write_PWMSFR(&FCLCON4, 0xFB, &PWMKEY);
    //FLTPOL disabled; CLPOL disabled; CLSRC FLT1; CLMOD disabled; FLTMOD Fault input is disabled; IFLTMOD disabled; FLTSRC FLT32; 
    __builtin_write_PWMSFR(&FCLCON5, 0xFB, &PWMKEY);
    //FLTPOL disabled; CLPOL disabled; CLSRC FLT1; CLMOD disabled; FLTMOD Fault input is disabled; IFLTMOD disabled; FLTSRC FLT32; 
    __builtin_write_PWMSFR(&FCLCON6, 0xFB, &PWMKEY);
    // PDC1 0; 
    PDC1 = 0x0;
    // PDC2 0; 
    PDC2 = 0x0;
    // PDC3 0; 
    PDC3 = 0x0;
    // PDC4 0; 
    PDC4 = 0x0;
    // PDC5 0; 
    PDC5 = 0x0;
    // PDC6 0; 
    PDC6 = 0x0;
    // PHASE1 0; 
    PHASE1 = 0x0;
    // PHASE2 0; 
    PHASE2 = 0x0;
    // PHASE3 0; 
    PHASE3 = 0x0;
    // PHASE4 0; 
    PHASE4 = 0x0;
    // PHASE5 1000; 
    PHASE5 = 0x3E8;
    // PHASE6 0; 
    PHASE6 = 0x0;
    // DTR1 0; 
    DTR1 = 0x0;
    // DTR2 0; 
    DTR2 = 0x0;
    // DTR3 0; 
    DTR3 = 0x0;
    // DTR4 0; 
    DTR4 = 0x0;
    // DTR5 0; 
    DTR5 = 0x0;
    // DTR6 0; 
    DTR6 = 0x0;
    // ALTDTR1 0; 
    ALTDTR1 = 0x0;
    // ALTDTR2 0; 
    ALTDTR2 = 0x0;
    // ALTDTR3 0; 
    ALTDTR3 = 0x0;
    // ALTDTR4 0; 
    ALTDTR4 = 0x0;
    // ALTDTR5 0; 
    ALTDTR5 = 0x0;
    // ALTDTR6 0; 
    ALTDTR6 = 0x0;
    // SDC1 0; 
    SDC1 = 0x0;
    // SDC2 0; 
    SDC2 = 0x0;
    // SDC3 0; 
    SDC3 = 0x0;
    // SDC4 0; 
    SDC4 = 0x0;
    // SDC5 0; 
    SDC5 = 0x0;
    // SDC6 0; 
    SDC6 = 0x0;
    // SPHASE1 1000; 
    SPHASE1 = 0x3E8;
    // SPHASE2 1000; 
    SPHASE2 = 0x3E8;
    // SPHASE3 1000; 
    SPHASE3 = 0x3E8;
    // SPHASE4 1000; 
    SPHASE4 = 0x3E8;
    // SPHASE5 1000; 
    SPHASE5 = 0x3E8;
    // SPHASE6 0; 
    SPHASE6 = 0x0;
    // TRGCMP 0; 
    TRIG1 = 0x0;
    // TRGCMP 0; 
    TRIG2 = 0x0;
    // TRGCMP 0; 
    TRIG3 = 0x0;
    // TRGCMP 0; 
    TRIG4 = 0x0;
    // TRGCMP 0; 
    TRIG5 = 0x0;
    // TRGCMP 0; 
    TRIG6 = 0x0;
    // TRGDIV 1; TRGSTRT 0; 
    TRGCON1 = 0x0;
    // TRGDIV 1; TRGSTRT 0; 
    TRGCON2 = 0x0;
    // TRGDIV 1; TRGSTRT 0; 
    TRGCON3 = 0x0;
    // TRGDIV 1; TRGSTRT 0; 
    TRGCON4 = 0x0;
    // TRGDIV 1; TRGSTRT 0; 
    TRGCON5 = 0x0;
    // TRGDIV 1; TRGSTRT 0; 
    TRGCON6 = 0x0;
    // BPLL disabled; BPHH disabled; BPLH disabled; BCH disabled; FLTLEBEN disabled; PLR disabled; CLLEBEN disabled; BCL disabled; PLF disabled; PHR disabled; BPHL disabled; PHF disabled; 
    LEBCON1 = 0x0;
    // BPLL disabled; BPHH disabled; BPLH disabled; BCH disabled; FLTLEBEN disabled; PLR disabled; CLLEBEN disabled; BCL disabled; PLF disabled; PHR disabled; BPHL disabled; PHF disabled; 
    LEBCON2 = 0x0;
    // BPLL disabled; BPHH disabled; BPLH disabled; BCH disabled; FLTLEBEN disabled; PLR disabled; CLLEBEN disabled; BCL disabled; PLF disabled; PHR disabled; BPHL disabled; PHF disabled; 
    LEBCON3 = 0x0;
    // BPLL disabled; BPHH disabled; BPLH disabled; BCH disabled; FLTLEBEN disabled; PLR disabled; CLLEBEN disabled; BCL disabled; PLF disabled; PHR disabled; BPHL disabled; PHF disabled; 
    LEBCON4 = 0x0;
    // BPLL disabled; BPHH disabled; BPLH disabled; BCH disabled; FLTLEBEN disabled; PLR disabled; CLLEBEN disabled; BCL disabled; PLF disabled; PHR disabled; BPHL disabled; PHF disabled; 
    LEBCON5 = 0x0;
    // BPLL disabled; BPHH disabled; BPLH disabled; BCH disabled; FLTLEBEN disabled; PLR disabled; CLLEBEN disabled; BCL disabled; PLF disabled; PHR disabled; BPHL disabled; PHF disabled; 
    LEBCON6 = 0x0;
    // LEB 0; 
    LEBDLY1 = 0x0;
    // LEB 0; 
    LEBDLY2 = 0x0;
    // LEB 0; 
    LEBDLY3 = 0x0;
    // LEB 0; 
    LEBDLY4 = 0x0;
    // LEB 0; 
    LEBDLY5 = 0x0;
    // LEB 0; 
    LEBDLY6 = 0x0;
    // CHOPLEN disabled; CHOPHEN disabled; BLANKSEL No state blanking; CHOPSEL No state blanking; 
    AUXCON1 = 0x0;
    // CHOPLEN disabled; CHOPHEN disabled; BLANKSEL No state blanking; CHOPSEL No state blanking; 
    AUXCON2 = 0x0;
    // CHOPLEN disabled; CHOPHEN disabled; BLANKSEL No state blanking; CHOPSEL No state blanking; 
    AUXCON3 = 0x0;
    // CHOPLEN disabled; CHOPHEN disabled; BLANKSEL No state blanking; CHOPSEL No state blanking; 
    AUXCON4 = 0x0;
    // CHOPLEN disabled; CHOPHEN disabled; BLANKSEL No state blanking; CHOPSEL No state blanking; 
    AUXCON5 = 0x0;
    // CHOPLEN disabled; CHOPHEN disabled; BLANKSEL No state blanking; CHOPSEL No state blanking; 
    AUXCON6 = 0x0;
    

    // SYNCOEN disabled; SEIEN disabled; SESTAT disabled; SEVTPS 1; SYNCSRC SYNCI1; SYNCEN disabled; PTSIDL disabled; PTEN enabled; EIPU disabled; SYNCPOL disabled; 
    PTCON = 0x8000;
}


void PWM_Set(uint8_t PWM_Output, uint16_t PWM_Duty){
    switch(PWM_Output) {
        case ROT_PWM_A:
            ROT_PWM_DutyReg_A = PWM_Duty;
            break;
        case ROT_PWM_B:
            ROT_PWM_DutyReg_B = PWM_Duty;
            break;
        case ROT_PWM_C:
            ROT_PWM_DutyReg_C = PWM_Duty;
            break;
        case LIN_PWM_A:
            LIN_PWM_DutyReg_A = PWM_Duty;
            break;
        case LIN_PWM_B:
            LIN_PWM_DutyReg_B = PWM_Duty;
            break;
        case LIN_PWM_C:
            LIN_PWM_DutyReg_C = PWM_Duty;
            break;
    }
    
}

/**
 End of File
*/
