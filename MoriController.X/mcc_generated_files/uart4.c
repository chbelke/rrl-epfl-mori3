/**
  UART4 Generated Driver File 

  @Company
    Microchip Technology Inc.

  @File Name
    uart4.c

  @Summary 
    This is the generated source file for the UART4 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This source file provides APIs for driver for UART4. 
    Generation Information : 
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.145.0
        Device            :  dsPIC33EP512GM604
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.36b
        MPLAB             :  MPLAB X v5.25
*/

/*
    (c) 2019 Microchip Technology Inc. and its subsidiaries. You may use this
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
#include <stdbool.h>
#include <stdint.h>
#include "xc.h"
#include "uart4.h"
#include "../Coms.h"

/**
  Section: Data Type Definitions
*/

/** UART Driver Queue Status

  @Summary
    Defines the object required for the status of the queue.
*/

static uint8_t * volatile rxTail;
static uint8_t *rxHead;
static uint8_t *txTail;
static uint8_t * volatile txHead;
static bool volatile rxOverflowed;

/** UART Driver Queue Length

  @Summary
    Defines the length of the Transmit and Receive Buffers

*/

#define UART4_CONFIG_TX_BYTEQ_LENGTH (8+1)
#define UART4_CONFIG_RX_BYTEQ_LENGTH (8+1)

/** UART Driver Queue

  @Summary
    Defines the Transmit and Receive Buffers

*/

static uint8_t txQueue[UART4_CONFIG_TX_BYTEQ_LENGTH];
static uint8_t rxQueue[UART4_CONFIG_RX_BYTEQ_LENGTH];

void (*UART4_TxDefaultInterruptHandler)(void);
void (*UART4_RxDefaultInterruptHandler)(void);

/**
  Section: Driver Interface
*/

void UART4_Initialize(void)
{
    IEC5bits.U4TXIE = 0;
    IEC5bits.U4RXIE = 0;

    // STSEL 1; IREN disabled; PDSEL 8N; UARTEN enabled; RTSMD disabled; USIDL disabled; WAKE disabled; ABAUD disabled; LPBACK disabled; BRGH enabled; URXINV disabled; UEN TX_RX; 
    // Data Bits = 8; Parity = None; Stop Bits = 1;
    U4MODE = (0x8008 & ~(1<<15));  // disabling UART ON bit
    // UTXISEL0 TX_ONE_CHAR; UTXINV disabled; OERR NO_ERROR_cleared; URXISEL RX_ONE_CHAR; UTXBRK COMPLETED; UTXEN disabled; ADDEN disabled; 
    U4STA = 0x00;
    // BaudRate = 115200; Frequency = 3686400 Hz; BRG 7; 
    U4BRG = 0x07;
    
    txHead = txQueue;
    txTail = txQueue;
    rxHead = rxQueue;
    rxTail = rxQueue;
   
    rxOverflowed = 0;

    UART4_SetTxInterruptHandler(UART4_Transmit_ISR);

    UART4_SetRxInterruptHandler(UART4_Receive_ISR);

    IEC5bits.U4RXIE = 1;
    
    //Make sure to set LAT bit corresponding to TxPin as high before UART initialization
    U4MODEbits.UARTEN = 1;   // enabling UART ON bit
    U4STAbits.UTXEN = 1;
}

/**
    Maintains the driver's transmitter state machine and implements its ISR
*/

void UART4_SetTxInterruptHandler(void* handler)
{
    UART4_TxDefaultInterruptHandler = handler;
}

void __attribute__ ( ( interrupt, no_auto_psv ) ) _U4TXInterrupt ( void )
{ 
    (*UART4_TxDefaultInterruptHandler)();
}

void UART4_Transmit_ISR ( void )
    {
    if(txHead == txTail)
    {
        IEC5bits.U4TXIE = 0;
        return;
    }

    IFS5bits.U4TXIF = 0;

    while(!(U4STAbits.UTXBF == 1))
    {
        U4TXREG = *txHead++;

        if(txHead == (txQueue + UART4_CONFIG_TX_BYTEQ_LENGTH))
        {
            txHead = txQueue;
        }

        // Are we empty?
        if(txHead == txTail)
        {
            break;
        }
    }
}

void UART4_SetRxInterruptHandler(void* handler)
{
    UART4_RxDefaultInterruptHandler = handler;
}

void __attribute__ ( ( interrupt, no_auto_psv ) ) _U4RXInterrupt( void )
{
    (*UART4_RxDefaultInterruptHandler)();
}

void UART4_Receive_ISR(void)
{

    while((U4STAbits.URXDA == 1))
    {
        *rxTail = U4RXREG;

        // Will the increment not result in a wrap and not result in a pure collision?
        // This is most often condition so check first
        if ( ( rxTail    != (rxQueue + UART4_CONFIG_RX_BYTEQ_LENGTH-1)) &&
             ((rxTail+1) != rxHead) )
        {
            rxTail++;
        }
        else if ( (rxTail == (rxQueue + UART4_CONFIG_RX_BYTEQ_LENGTH-1)) &&
                  (rxHead !=  rxQueue) )
        {
            // Pure wrap no collision
            rxTail = rxQueue;
        }
        else // must be collision
        {
            rxOverflowed = true;
        }
        
        Coms_ESP_Eval();
    }

    IFS5bits.U4RXIF = false;
}

void __attribute__ ( ( interrupt, no_auto_psv ) ) _U4ErrInterrupt( void )
{
    if ((U4STAbits.OERR == 1))
    {
        U4STAbits.OERR = 0;
    }
    
    IFS5bits.U4EIF = 0;
}

/**
  Section: UART Driver Client Routines
*/

uint8_t UART4_Read( void)
{
    uint8_t data = 0;

    while (rxHead == rxTail )
    {
    }

    data = *rxHead;

    rxHead++;

    if (rxHead == (rxQueue + UART4_CONFIG_RX_BYTEQ_LENGTH))
    {
        rxHead = rxQueue;
    }
    return data;
}

void UART4_Write( uint8_t byte)
    {
    while(UART4_IsTxReady() == 0)
    {
    }

    *txTail = byte;

    txTail++;
    
    if (txTail == (txQueue + UART4_CONFIG_TX_BYTEQ_LENGTH))
    {
        txTail = txQueue;
}

    IEC5bits.U4TXIE = 1;
}

bool UART4_IsRxReady(void)
{
    return !(rxHead == rxTail);
}

bool UART4_IsTxReady(void)
    {
    uint16_t size;
    uint8_t *snapshot_txHead = (uint8_t*)txHead;
    
    if (txTail < snapshot_txHead)
        {
        size = (snapshot_txHead - txTail - 1);
        }
        else
        {
        size = ( UART4_CONFIG_TX_BYTEQ_LENGTH - (txTail - snapshot_txHead) - 1 );
        }

    return (size != 0);
}

bool UART4_IsTxDone(void)
{
    if(txTail == txHead)
    {
        return (bool)U4STAbits.TRMT;
    }
    
    return false;
}

    
/*******************************************************************************

  !!! Deprecated API !!!
  !!! These functions will not be supported in future releases !!!

*******************************************************************************/

static uint8_t UART4_RxDataAvailable(void)
    {
    uint16_t size;
    uint8_t *snapshot_rxTail = (uint8_t*)rxTail;
    
    if (snapshot_rxTail < rxHead) 
    {
        size = ( UART4_CONFIG_RX_BYTEQ_LENGTH - (rxHead-snapshot_rxTail));
    }
    else
    {
        size = ( (snapshot_rxTail - rxHead));
    }

    if(size > 0xFF)
    {
        return 0xFF;
    }

    return size;
}

static uint8_t UART4_TxDataAvailable(void)
{
    uint16_t size;
    uint8_t *snapshot_txHead = (uint8_t*)txHead;

    if (txTail < snapshot_txHead)
    {
        size = (snapshot_txHead - txTail - 1);
        }
        else
        {
        size = ( UART4_CONFIG_TX_BYTEQ_LENGTH - (txTail - snapshot_txHead) - 1 );
        }
    
    if(size > 0xFF)
    {
        return 0xFF;
    }

    return size;
}

unsigned int __attribute__((deprecated)) UART4_ReadBuffer( uint8_t *buffer ,  unsigned int numbytes)
{
    unsigned int rx_count = UART4_RxDataAvailable();
    unsigned int i;

    if(numbytes < rx_count)
    {
        rx_count = numbytes;
    }

    for(i=0; i<rx_count; i++)
    {
        *buffer++ = UART4_Read();
    }

    return rx_count;    
    }

unsigned int __attribute__((deprecated)) UART4_WriteBuffer( uint8_t *buffer , unsigned int numbytes )
    {
    unsigned int tx_count = UART4_TxDataAvailable();
    unsigned int i;
    
    if(numbytes < tx_count)
    {
        tx_count = numbytes;
    }
    
    for(i=0; i<tx_count; i++)
    {
        UART4_Write(*buffer++);
    }
    
    return tx_count;  
}

UART4_TRANSFER_STATUS __attribute__((deprecated)) UART4_TransferStatusGet (void )
{
    UART4_TRANSFER_STATUS status = 0;
    uint8_t rx_count = UART4_RxDataAvailable();
    uint8_t tx_count = UART4_TxDataAvailable();
    
    switch(rx_count)
    {
        case 0:
            status |= UART4_TRANSFER_STATUS_RX_EMPTY;
            break;
        case UART4_CONFIG_RX_BYTEQ_LENGTH:
            status |= UART4_TRANSFER_STATUS_RX_FULL;
            break;
        default:
            status |= UART4_TRANSFER_STATUS_RX_DATA_PRESENT;
            break;
    }
    
    switch(tx_count)
    {
        case 0:
            status |= UART4_TRANSFER_STATUS_TX_FULL;
            break;
        case UART4_CONFIG_RX_BYTEQ_LENGTH:
            status |= UART4_TRANSFER_STATUS_TX_EMPTY;
            break;
        default:
            break;
    }

    return status;    
}

uint8_t __attribute__((deprecated)) UART4_Peek(uint16_t offset)
{
    uint8_t *peek = rxHead + offset;
    
    while(peek > (rxQueue + UART4_CONFIG_RX_BYTEQ_LENGTH))
    {
        peek -= UART4_CONFIG_RX_BYTEQ_LENGTH;
    }
        
    return *peek;
}

bool __attribute__((deprecated)) UART4_ReceiveBufferIsEmpty (void)
        {
    return (UART4_RxDataAvailable() == 0);
        }

bool __attribute__((deprecated)) UART4_TransmitBufferIsFull(void)
        {
    return (UART4_TxDataAvailable() == 0);
        }

uint16_t __attribute__((deprecated)) UART4_StatusGet (void)
        {
    return U4STA;
        }

unsigned int __attribute__((deprecated)) UART4_TransmitBufferSizeGet(void)
{
    if(UART4_TxDataAvailable() != 0)
    {
        if(txHead > txTail)
        {
            return(txHead - txTail);
        }
        else
        {
            return(UART4_CONFIG_TX_BYTEQ_LENGTH - (txTail - txHead));
        } 
    }
    return 0;
}

unsigned int __attribute__((deprecated)) UART4_ReceiveBufferSizeGet(void)
{
    if(UART4_RxDataAvailable() != 0)
    { 
        if(rxHead > rxTail)
        {
            return(rxHead - rxTail);
        }
        else
        {
            return(UART4_CONFIG_RX_BYTEQ_LENGTH - (rxTail - rxHead));
        }
    }
    return 0;
}

void __attribute__((deprecated)) UART4_Enable(void)
{
    U4MODEbits.UARTEN = 1;
    U4STAbits.UTXEN = 1;
}

void __attribute__((deprecated)) UART4_Disable(void)
{
    U4MODEbits.UARTEN = 0;
    U4STAbits.UTXEN = 0;
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