/**
  UART4 Generated Driver API Header File

  @Company
    Microchip Technology Inc.

  @File Name
    uart4.h

  @Summary
    This is the generated header file for the UART4 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This header file provides APIs for driver for UART4.
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

#ifndef UART4_H
#define UART4_H

/**
  Section: Included Files
*/

#include <xc.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif

/**
  Section: Data Types
*/

/** UART4 Driver Hardware Flags

  @Summary
    Specifies the status of the hardware receive or transmit

  @Description
    This type specifies the status of the hardware receive or transmit.
    More than one of these values may be OR'd together to create a complete
    status value.  To test a value of this type, the bit of interest must be
    AND'ed with value and checked to see if the result is non-zero.
*/
typedef enum
{
    /* Indicates that Receive buffer has data, at least one more character can be read */
    UART4_RX_DATA_AVAILABLE
        /*DOM-IGNORE-BEGIN*/  = (1 << 0) /*DOM-IGNORE-END*/,
    
    /* Indicates that Receive buffer has overflowed */
    UART4_RX_OVERRUN_ERROR
        /*DOM-IGNORE-BEGIN*/  = (1 << 1) /*DOM-IGNORE-END*/,

    /* Indicates that Framing error has been detected for the current character */
    UART4_FRAMING_ERROR
        /*DOM-IGNORE-BEGIN*/  = (1 << 2) /*DOM-IGNORE-END*/,

    /* Indicates that Parity error has been detected for the current character */
    UART4_PARITY_ERROR
        /*DOM-IGNORE-BEGIN*/  = (1 << 3) /*DOM-IGNORE-END*/,

    /* Indicates that Receiver is Idle */
    UART4_RECEIVER_IDLE
        /*DOM-IGNORE-BEGIN*/  = (1 << 4) /*DOM-IGNORE-END*/,

    /* Indicates that the last transmission has completed */
    UART4_TX_COMPLETE
        /*DOM-IGNORE-BEGIN*/  = (1 << 8) /*DOM-IGNORE-END*/,

    /* Indicates that Transmit buffer is full */
    UART4_TX_FULL
        /*DOM-IGNORE-BEGIN*/  = (1 << 9) /*DOM-IGNORE-END*/

}UART4_STATUS;

/**
  Section: Macro Declarations
*/

#define UART4_DataReady  (U4STAbits.URXDA == 1)

/**
  Section: UART4 APIs
*/

/**
  @Summary
    Initialization routine that takes inputs from the UART4 GUI.

  @Description
    This routine initializes the UART4 driver.
    This routine must be called before any other UART4 routine is called.

  @Preconditions
    None

  @Param
    None

  @Returns
    None

  @Comment
    
*/

void UART4_Initialize(void);

/**
  @Summary
    Read a byte of data from the UART4.

  @Description
    This routine reads a byte of data from the UART4.

  @Preconditions
    UART4_Initialize() function should have been called
    before calling this function. This is a blocking function.
    So the recieve status should be checked to see
    if the receiver is not empty before calling this function.

  @Param
    None

  @Returns
    A data byte received by the driver.
*/

uint8_t UART4_Read(void);

 /**
  @Summary
    Writes a byte of data to the UART4.

  @Description
    This routine writes a byte of data to the UART4.

  @Preconditions
    UART4_Initialize() function should have been called
    before calling this function. The transfer status should be checked to see
    if transmitter is not busy before calling this function.

  @Param
    txData  - Data byte to write to the UART4

  @Returns
    None
*/

void UART4_Write(uint8_t txData);
void UART4_Write16(uint16_t txData);

/**
  @Summary
    Returns the transmitter and receiver status

  @Description
    This returns the transmitter and receiver status. The returned status 
    contains a 16 bit value. 
    The caller should perform an "AND" with the bit of interest and verify if the
    result is non-zero (as shown in the example) to verify the desired status
    bit.

  @Preconditions
    UART4_Initializer function should have been called 
    before calling this function

  @Param
    None.

  @Returns
    16 bit value describing the current status of the transfer.

  @Example
    <code>
        while(!(UART4_StatusGet() & UART4_TX_COMPLETE ))
        {
           // Wait for the tranmission to complete
        }
    </code>
*/

uint16_t UART4_StatusGet (void );

#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

#endif  // UART4_H
