/**
  System Interrupts Generated Driver File 

  @Company:
    Microchip Technology Inc.

  @File Name:
    interrupt_manager.h

  @Summary:
    This is the generated driver implementation file for setting up the
    interrupts using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description:
    This source file provides implementations for PIC24 / dsPIC33 / PIC32MM MCUs interrupts.
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
    Section: Includes
*/
#include <xc.h>

/**
    void INTERRUPT_Initialize (void)
*/
void INTERRUPT_Initialize (void)
{
    //    UERI: UART2 Error
    //    Priority: 7
        IPC16bits.U2EIP = 7;
    //    UTXI: UART2 Transmitter
    //    Priority: 5
        IPC7bits.U2TXIP = 5;
    //    URXI: UART2 Receiver
    //    Priority: 5
        IPC7bits.U2RXIP = 5;
    //    UERI: UART3 Error
    //    Priority: 7
        IPC20bits.U3EIP = 7;
    //    UTXI: UART3 Transmitter
    //    Priority: 5
        IPC20bits.U3TXIP = 5;
    //    URXI: UART3 Receiver
    //    Priority: 5
        IPC20bits.U3RXIP = 5;
    //    UERI: UART4 Error
    //    Priority: 7
        IPC21bits.U4EIP = 7;
    //    UTXI: UART4 Transmitter
    //    Priority: 5
        IPC22bits.U4TXIP = 5;
    //    URXI: UART4 Receiver
    //    Priority: 5
        IPC22bits.U4RXIP = 5;
    //    UERI: UART1 Error
    //    Priority: 7
        IPC16bits.U1EIP = 7;
    //    UTXI: UART1 Transmitter
    //    Priority: 5
        IPC3bits.U1TXIP = 5;
    //    URXI: UART1 Receiver
    //    Priority: 5
        IPC2bits.U1RXIP = 5;
    //    ADI: ADC1 Convert Done
    //    Priority: 1
        IPC3bits.AD1IP = 1;
    //    INT1I: External Interrupt 1
    //    Priority: 1
        IPC5bits.INT1IP = 1;
    //    MICI: I2C1 Master Events
    //    Priority: 4
        IPC4bits.MI2C1IP = 5;
    //    SICI: I2C1 Slave Events
    //    Priority: 4
        IPC4bits.SI2C1IP = 5;
    //    TI: Timer 5
    //    Priority: 1
        IPC7bits.T5IP = 1;
    //    TI: Timer 3
    //    Priority: 2
        IPC2bits.T3IP = 2;
    //    TI: Timer 1
    //    Priority: 3
        IPC0bits.T1IP = 4;
}
