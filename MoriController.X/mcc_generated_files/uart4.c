/**
  UART4 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    uart4.c

  @Summary
    This is the generated driver implementation file for the UART4 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This header file provides implementations for driver APIs for UART4.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.75.1
        Device            :  dsPIC33EP512GM604
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.35
        MPLAB             :  MPLAB X v5.05
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
#include "uart4.h"

/**
  Section: UART4 APIs
*/

void UART4_Initialize(void)
{
/**    
     Set the UART4 module to the options selected in the user interface.
     Make sure to set LAT bit corresponding to TxPin as high before UART initialization
*/
    // STSEL 1; IREN disabled; PDSEL 8N; UARTEN enabled; RTSMD disabled; USIDL disabled; WAKE disabled; ABAUD disabled; LPBACK disabled; BRGH enabled; URXINV disabled; UEN TX_RX; 
    // Data Bits = 8; Parity = None; Stop Bits = 1;
    U4MODE = (0x8008 & ~(1<<15));  // disabling UARTEN bit   
    // UTXISEL0 TX_ONE_CHAR; UTXINV disabled; OERR NO_ERROR_cleared; URXISEL RX_ONE_CHAR; UTXBRK COMPLETED; UTXEN disabled; ADDEN disabled; 
    U4STA = 0x00;
    // BaudRate = 115200; Frequency = 3686400 Hz; BRG 7; 
    U4BRG = 0x07;
    
    U4MODEbits.UARTEN = 1;  // enabling UARTEN bit
    U4STAbits.UTXEN = 1; 
}

uint8_t UART4_Read(void)
{
    while(!(U4STAbits.URXDA == 1))
    {
        
    }

    if ((U4STAbits.OERR == 1))
    {
        U4STAbits.OERR = 0;
    }

    return U4RXREG;
}

void UART4_Write(uint8_t txData)
{
    while(U4STAbits.UTXBF == 1)
    {
        
    }

    U4TXREG = txData;    // Write the data byte to the USART.
}

uint16_t UART4_StatusGet (void)
{
    return U4STA;
}


///// ADDED FUNCTION /////
void UART4_Write16(uint16_t txData)
{
    uint8_t LSByte;
    uint8_t MSByte;
    
    // Extract LSB and MSB 
    LSByte = txData & 0x00FF;
    MSByte = (txData >> 8) & 0x00FF; 
    
    //MSByte FIRST! (uart write lsbit of the byte first though)
    UART4_Write(MSByte);
    
    UART4_Write(LSByte);
}