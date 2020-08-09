/**
  UART3 Generated Driver File 

  @Company
    Microchip Technology Inc.

  @File Name
    uart3.c

  @Summary
    This is the generated source file for the UART3 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This source file provides APIs for driver for UART3. 
    Generation Information : 
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.166.1
        Device            :  dsPIC33EP512GM604
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.41
        MPLAB             :  MPLAB X v5.30
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
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "xc.h"
#include "uart3.h"
#include "../Coms_123.h"

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

/* We add one extra byte than requested so that we don't have to have a separate
 * bit to determine the difference between buffer full and buffer empty, but
 * still be able to hold the amount of data requested by the user.  Empty is
 * when head == tail.  So full will result in head/tail being off by one due to
 * the extra byte.
 */
#define UART3_CONFIG_TX_BYTEQ_LENGTH (128+1)
#define UART3_CONFIG_RX_BYTEQ_LENGTH (128+1)

/** UART Driver Queue

  @Summary
    Defines the Transmit and Receive Buffers

 */

static uint8_t txQueue[UART3_CONFIG_TX_BYTEQ_LENGTH];
static uint8_t rxQueue[UART3_CONFIG_RX_BYTEQ_LENGTH];

void (*UART3_TxDefaultInterruptHandler)(void);
void (*UART3_RxDefaultInterruptHandler)(void);

/**
  Section: Driver Interface
 */

void UART3_Initialize(void) {
    IEC5bits.U3TXIE = 0;
    IEC5bits.U3RXIE = 0;

    // STSEL 1; IREN disabled; PDSEL 8N; UARTEN enabled; RTSMD disabled; USIDL disabled; WAKE disabled; ABAUD disabled; LPBACK disabled; BRGH enabled; URXINV disabled; UEN TX_RX; 
    // Data Bits = 8; Parity = None; Stop Bits = 1;
    U3MODE = (0x8008 & ~(1<<15));  // disabling UART ON bit
    // UTXISEL0 TX_ONE_CHAR; UTXINV disabled; OERR NO_ERROR_cleared; URXISEL RX_ONE_CHAR; UTXBRK COMPLETED; UTXEN disabled; ADDEN disabled; 
    U3STA = 0x00;
    // BaudRate = 115200; Frequency = 3686400 Hz; BRG 7; 
    U3BRG = 0x07;

    txHead = txQueue;
    txTail = txQueue;
    rxHead = rxQueue;
    rxTail = rxQueue;

    rxOverflowed = false;

    UART3_SetTxInterruptHandler(&UART3_Transmit_CallBack);

    UART3_SetRxInterruptHandler(&UART3_Receive_CallBack);

    IEC5bits.U3RXIE = 1;

    //Make sure to set LAT bit corresponding to TxPin as high before UART initialization
    U3MODEbits.UARTEN = 1;   // enabling UART ON bit
    U3STAbits.UTXEN = 1;
}

/**
    Maintains the driver's transmitter state machine and implements its ISR
 */

void UART3_SetTxInterruptHandler(void* handler) {
    if (handler == NULL) {
        UART3_TxDefaultInterruptHandler = &UART3_Transmit_CallBack;
    } else {
        UART3_TxDefaultInterruptHandler = handler;
    }
}

void __attribute__((interrupt, no_auto_psv)) _U3TXInterrupt(void) {
    if (UART3_TxDefaultInterruptHandler) {
        UART3_TxDefaultInterruptHandler();
    }

    if (txHead == txTail) {
        IEC5bits.U3TXIE = 0;
    } else {
        IFS5bits.U3TXIF = 0;

        while (!(U3STAbits.UTXBF == 1)) {
            U3TXREG = *txHead++;

            if (txHead == (txQueue + UART3_CONFIG_TX_BYTEQ_LENGTH)) {
                txHead = txQueue;
            }

            // Are we empty?
            if (txHead == txTail) {
                break;
            }
        }
    }
}

void __attribute__((weak)) UART3_Transmit_CallBack(void) {

}

void UART3_SetRxInterruptHandler(void* handler) {
    if (handler == NULL) {
        UART3_RxDefaultInterruptHandler = &UART3_Receive_CallBack;
    } else {
        UART3_RxDefaultInterruptHandler = handler;
    }
}

void __attribute__((interrupt, no_auto_psv)) _U3RXInterrupt(void) {
    if (UART3_RxDefaultInterruptHandler) {
        UART3_RxDefaultInterruptHandler();
    }

    IFS5bits.U3RXIF = 0;

    while ((U3STAbits.URXDA == 1)) {
        *rxTail = U3RXREG;

        // Will the increment not result in a wrap and not result in a pure collision?
        // This is most often condition so check first
        if ((rxTail != (rxQueue + UART3_CONFIG_RX_BYTEQ_LENGTH - 1)) &&
                ((rxTail + 1) != rxHead)) {
            rxTail++;
        }
        else if ((rxTail == (rxQueue + UART3_CONFIG_RX_BYTEQ_LENGTH - 1)) &&
                (rxHead != rxQueue)) {
            // Pure wrap no collision
            rxTail = rxQueue;
        }
        else // must be collision
        {
            rxOverflowed = true;
        }
        
        Coms_123_Eval(2);
    }
}

void __attribute__((weak)) UART3_Receive_CallBack(void) {

}

void __attribute__((interrupt, no_auto_psv)) _U3ErrInterrupt(void) {
    if ((U3STAbits.OERR == 1)) {
        U3STAbits.OERR = 0;
    }

    IFS5bits.U3EIF = 0;
}

/**
  Section: UART Driver Client Routines
 */

uint8_t UART3_Read(void) {
    uint8_t data = 0;

    while (rxHead == rxTail) {
    }

    data = *rxHead;

    rxHead++;

    if (rxHead == (rxQueue + UART3_CONFIG_RX_BYTEQ_LENGTH)) {
        rxHead = rxQueue;
    }
    return data;
}

void UART3_Write(uint8_t byte) {
    while (UART3_IsTxReady() == 0) {
    }

    *txTail = byte;

    txTail++;

    if (txTail == (txQueue + UART3_CONFIG_TX_BYTEQ_LENGTH)) {
        txTail = txQueue;
    }

    IEC5bits.U3TXIE = 1;
}

bool UART3_IsRxReady(void) {
    return !(rxHead == rxTail);
}

bool UART3_IsTxReady(void) {
    uint16_t size;
    uint8_t *snapshot_txHead = (uint8_t*) txHead;

    if (txTail < snapshot_txHead) {
        size = (snapshot_txHead - txTail - 1);
    } else {
        size = (UART3_CONFIG_TX_BYTEQ_LENGTH - (txTail - snapshot_txHead) - 1);
    }

    return (size != 0);
}

bool UART3_IsTxDone(void) {
    if (txTail == txHead) {
        return (bool) U3STAbits.TRMT;
    }

    return false;
}

/*******************************************************************************

  !!! Deprecated API !!!
  !!! These functions will not be supported in future releases !!!

 *******************************************************************************/

static uint8_t UART3_RxDataAvailable(void) {
    uint16_t size;
    uint8_t *snapshot_rxTail = (uint8_t*) rxTail;

    if (snapshot_rxTail < rxHead) {
        size = (UART3_CONFIG_RX_BYTEQ_LENGTH - (rxHead - snapshot_rxTail));
    } else {
        size = ((snapshot_rxTail - rxHead));
    }

    if (size > 0xFF) {
        return 0xFF;
    }

    return size;
}

static uint8_t UART3_TxDataAvailable(void) {
    uint16_t size;
    uint8_t *snapshot_txHead = (uint8_t*) txHead;

    if (txTail < snapshot_txHead) {
        size = (snapshot_txHead - txTail - 1);
    } else {
        size = (UART3_CONFIG_TX_BYTEQ_LENGTH - (txTail - snapshot_txHead) - 1);
    }

    if (size > 0xFF) {
        return 0xFF;
    }

    return size;
}

unsigned int __attribute__((deprecated)) UART3_ReadBuffer(uint8_t *buffer, unsigned int numbytes) {
    unsigned int rx_count = UART3_RxDataAvailable();
    unsigned int i;

    if (numbytes < rx_count) {
        rx_count = numbytes;
    }

    for (i = 0; i < rx_count; i++) {
        *buffer++ = UART3_Read();
    }

    return rx_count;
}

unsigned int __attribute__((deprecated)) UART3_WriteBuffer(uint8_t *buffer, unsigned int numbytes) {
    unsigned int tx_count = UART3_TxDataAvailable();
    unsigned int i;

    if (numbytes < tx_count) {
        tx_count = numbytes;
    }

    for (i = 0; i < tx_count; i++) {
        UART3_Write(*buffer++);
    }

    return tx_count;
}

UART3_TRANSFER_STATUS __attribute__((deprecated)) UART3_TransferStatusGet(void) {
    UART3_TRANSFER_STATUS status = 0;
    uint8_t rx_count = UART3_RxDataAvailable();
    uint8_t tx_count = UART3_TxDataAvailable();

    switch (rx_count) {
        case 0:
            status |= UART3_TRANSFER_STATUS_RX_EMPTY;
            break;
        case UART3_CONFIG_RX_BYTEQ_LENGTH:
            status |= UART3_TRANSFER_STATUS_RX_FULL;
            break;
        default:
            status |= UART3_TRANSFER_STATUS_RX_DATA_PRESENT;
            break;
    }

    switch (tx_count) {
        case 0:
            status |= UART3_TRANSFER_STATUS_TX_FULL;
            break;
        case UART3_CONFIG_RX_BYTEQ_LENGTH:
            status |= UART3_TRANSFER_STATUS_TX_EMPTY;
            break;
        default:
            break;
    }

    return status;
}

uint8_t __attribute__((deprecated)) UART3_Peek(uint16_t offset) {
    uint8_t *peek = rxHead + offset;

    while (peek > (rxQueue + UART3_CONFIG_RX_BYTEQ_LENGTH)) {
        peek -= UART3_CONFIG_RX_BYTEQ_LENGTH;
    }

    return *peek;
}

bool __attribute__((deprecated)) UART3_ReceiveBufferIsEmpty(void) {
    return (UART3_RxDataAvailable() == 0);
}

bool __attribute__((deprecated)) UART3_TransmitBufferIsFull(void) {
    return (UART3_TxDataAvailable() == 0);
}

uint16_t __attribute__((deprecated)) UART3_StatusGet(void) {
    return U3STA;
}

unsigned int __attribute__((deprecated)) UART3_TransmitBufferSizeGet(void) {
    if (UART3_TxDataAvailable() != 0) {
        if (txHead > txTail) {
            return ((txHead - txTail) - 1);
        } else {
            return ((UART3_CONFIG_TX_BYTEQ_LENGTH - (txTail - txHead)) - 1);
        }
    }
    return 0;
}

unsigned int __attribute__((deprecated)) UART3_ReceiveBufferSizeGet(void) {
    if (UART3_RxDataAvailable() != 0) {
        if (rxHead > rxTail) {
            return ((rxHead - rxTail) - 1);
        } else {
            return ((UART3_CONFIG_RX_BYTEQ_LENGTH - (rxTail - rxHead)) - 1);
        }
    }
    return 0;
}

void __attribute__((deprecated)) UART3_Enable(void) {
    U3MODEbits.UARTEN = 1;
    U3STAbits.UTXEN = 1;
}

void __attribute__((deprecated)) UART3_Disable(void) {
    U3MODEbits.UARTEN = 0;
    U3STAbits.UTXEN = 0;
}