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
#include <stdlib.h>
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

    typedef enum {
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

    } UART4_STATUS;

    /** UART4 Driver Transfer Flags

      @Summary
        Specifies the status of the receive or transmit

      @Description
        This type specifies the status of the receive or transmit operation.
        More than one of these values may be OR'd together to create a complete
        status value.  To test a value of this type, the bit of interest must be
        AND'ed with value and checked to see if the result is non-zero.
     */

    typedef enum {
        /* Indicates that the core driver buffer is full */
        UART4_TRANSFER_STATUS_RX_FULL
        /*DOM-IGNORE-BEGIN*/  = (1 << 0) /*DOM-IGNORE-END*/,

        /* Indicates that at least one byte of Data has been received */
        UART4_TRANSFER_STATUS_RX_DATA_PRESENT
        /*DOM-IGNORE-BEGIN*/  = (1 << 1) /*DOM-IGNORE-END*/,

        /* Indicates that the core driver receiver buffer is empty */
        UART4_TRANSFER_STATUS_RX_EMPTY
        /*DOM-IGNORE-BEGIN*/  = (1 << 2) /*DOM-IGNORE-END*/,

        /* Indicates that the core driver transmitter buffer is full */
        UART4_TRANSFER_STATUS_TX_FULL
        /*DOM-IGNORE-BEGIN*/  = (1 << 3) /*DOM-IGNORE-END*/,

        /* Indicates that the core driver transmitter buffer is empty */
        UART4_TRANSFER_STATUS_TX_EMPTY
        /*DOM-IGNORE-BEGIN*/  = (1 << 4) /*DOM-IGNORE-END*/

    } UART4_TRANSFER_STATUS;

    /**
      Section: UART4 Driver Routines
     */

    /**
      @Summary
        Initializes the UART instance : 4

      @Description
        This routine initializes the UART driver instance for : 4
        index.
        This routine must be called before any other UART routine is called.
    
      @Preconditions
        None.

      @Returns
        None.

      @Param
        None.

      @Comment
    
 
      @Example
        <code>
            const uint8_t writeBuffer[35] = "1234567890ABCDEFGHIJKLMNOP\n" ;
            unsigned int numBytes = 0;
            int  writebufferLen = strlen((char *)writeBuffer);
            UART4_Initialize();
            while(numBytes < writebufferLen)
            {    
                int bytesToWrite = UART4_TransmitBufferSizeGet();
                numBytes += UART4_WriteBuffer ( writeBuffer+numBytes, bytesToWrite)  ;
            }
        </code>

     */

    void UART4_Initialize (void);

    /**
      @Summary
        Read a byte of data from the UART4

      @Description
        This routine reads a byte of data from the UART4.

      @Preconditions
        UART4_Initializer function should have been called 
        before calling this function. The transfer status should be checked to see 
        if the receiver is not empty before calling this function.

      @Param
        None.

      @Returns
        A data byte received by the driver.

      @Example
        <code>
        char            myBuffer[MY_BUFFER_SIZE];
        unsigned int    numBytes;

        numBytes = 0;
        do
        {
            if( UART4_TRANSFER_STATUS_RX_DATA_PRESENT & UART4_TransferStatusGet() )
            {
                myBuffer[numBytes++] = UART4_Read();
            }

            // Do something else...

        } while( numBytes < MY_BUFFER_SIZE);
        </code>
     */

    uint8_t UART4_Read ( void);

    /**
      @Summary
        Returns the number of bytes read by the UART4 peripheral

      @Description
        This routine returns the number of bytes read by the Peripheral and fills the
        application read buffer with the read data.

      @Preconditions
        UART4_Initializer function should have been called 
        before calling this function

      @Param
        buffer       - Buffer into which the data read from the UART4

      @Param
        numbytes     - Total number of bytes that need to be read from the UART4
                       (must be equal to or less than the size of the buffer)

      @Returns
        Number of bytes actually copied into the caller's buffer or -1 if there
        is an error.

      @Example
        <code>
        char                     myBuffer[MY_BUFFER_SIZE];
        unsigned int             numBytes;
        UART4_TRANSFER_STATUS status ;

        // Pre-initialize myBuffer with MY_BUFFER_SIZE bytes of valid data.

        numBytes = 0;
        while( numBytes < MY_BUFFER_SIZE);
        {
            status = UART4_TransferStatusGet ( ) ;
            if (status & UART4_TRANSFER_STATUS_RX_FULL)
            {
                numBytes += UART4_ReadBuffer( myBuffer + numBytes, MY_BUFFER_SIZE - numBytes )  ;
                if(numBytes < readbufferLen)
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
            else
            {
                continue;
            }

            // Do something else...
        }
        </code>
     */

    unsigned int UART4_ReadBuffer ( uint8_t *buffer ,  const unsigned int numbytes);

    /**
      @Summary
        Writes a byte of data to the UART4

      @Description
        This routine writes a byte of data to the UART4.

      @Preconditions
        UART4_Initializer function should have been called 
        before calling this function. The transfer status should be checked to see if
        transmitter is not full before calling this function.

      @Param
        byte         - Data byte to write to the UART4

      @Returns
        None.

      @Example
        <code>
        char            myBuffer[MY_BUFFER_SIZE];
        unsigned int    numBytes;

        // Pre-initialize myBuffer with MY_BUFFER_SIZE bytes of valid data.

        numBytes = 0;
        while( numBytes < MY_BUFFER_SIZE);
        {
            if( !(UART4_TRANSFER_STATUS_TX_FULL & UART4_TransferStatusGet()) )
            {
                UART4_Write(handle, myBuffer[numBytes++]);
            }

            // Do something else...
        }
        </code>
     */

    void UART4_Write ( const uint8_t byte);

    /**
      @Summary
        Returns the number of bytes written into the internal buffer

      @Description
        This API transfers the data from application buffer to internal buffer and 
        returns the number of bytes added in that queue

      @Preconditions
        UART4_Initializer function should have been called 
        before calling this function

      @Example
        <code>
        char                     myBuffer[MY_BUFFER_SIZE];
        unsigned int             numBytes;
        UART4_TRANSFER_STATUS status ;

        // Pre-initialize myBuffer with MY_BUFFER_SIZE bytes of valid data.

        numBytes = 0;
        while( numBytes < MY_BUFFER_SIZE);
        {
            status = UART4_TransferStatusGet ( ) ;
            if (status & UART4_TRANSFER_STATUS_TX_EMPTY)
            {
                numBytes += UART4_WriteBuffer ( myBuffer + numBytes, MY_BUFFER_SIZE - numBytes )  ;
                if(numBytes < writebufferLen)
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
            else
            {
                continue;
            }

            // Do something else...
        }
        </code>
     */

    unsigned int UART4_WriteBuffer ( const uint8_t *buffer , const unsigned int numbytes );

    /**
      @Summary
        Returns the transmitter and receiver transfer status

      @Description
        This returns the transmitter and receiver transfer status.The returned status 
        may contain a value with more than one of the bits
        specified in the UART4_TRANSFER_STATUS enumeration set.  
        The caller should perform an "AND" with the bit of interest and verify if the
        result is non-zero (as shown in the example) to verify the desired status
        bit.

      @Preconditions
        UART4_Initializer function should have been called 
        before calling this function

      @Param
        None.

      @Returns
        A UART4_TRANSFER_STATUS value describing the current status 
        of the transfer.

      @Example
        Refer to UART4_ReadBuffer and UART4_WriteBuffer for example

     */

    UART4_TRANSFER_STATUS UART4_TransferStatusGet (void );

    /**
      @Summary
        Returns the character in the read sequence at the offset provided, without
        extracting it

      @Description
        This routine returns the character in the read sequence at the offset provided,
        without extracting it
 
      @Param
        None.
    
      @Example 
        <code>
        const uint8_t readBuffer[5];
        unsigned int data, numBytes = 0;
        unsigned int readbufferLen = sizeof(readBuffer);
        UART4_Initializer();
    
        while(numBytes < readbufferLen)        
        {   
            UART4_TasksReceive ( );
            //Check for data at a particular place in the buffer
            data = UART4_Peek(3);
            if(data == 5)
            {
                //discard all other data if byte that is wanted is received.    
                //continue other operation
                numBytes += UART4_ReadBuffer ( readBuffer + numBytes , readbufferLen ) ;
            }
            else
            {
                break;
            }
        }
        </code>
 
     */

    uint8_t UART4_Peek (uint16_t offset);

    /**
      @Summary
        Validates the offset input and get the character in the read sequence at the 
        offset provided, without extracting it

      @Description
        This routine validates the offset input and get the character in the read 
        sequence at the offset provided, without extracting it. 
 
      @Param
        dataByte     - Data byte to be read from UART4 RX buffer based on offset position.
        offset       - UART4 RX buffer peek position. Offset input range is should be
                       0 to (UART4_CONFIG_RX_BYTEQ_LENGTH - 1).

      @Return   
        false        - If the UART4 RX buffer is empty or dataByte is NULL or UART4 RX 
                       buffer is empty.
        true         - Valid offset input position.
 
      @Example 
        <code>
        const uint8_t readBuffer[5];
        unsigned int data, numBytes = 0;
        unsigned int readbufferLen = sizeof(readBuffer);
        UART4_Initializer();
    
        while(numBytes < readbufferLen)        
        {   
            UART4_TasksReceive ( );
            //Check for data at a particular place in the buffer
            if(UART4_PeekSafe(&data, 3))
            {
                if(data == 5)
                {
                    //discard all other data if byte that is wanted is received.    
                    //continue other operation
                    numBytes += UART4_ReadBuffer ( readBuffer + numBytes , readbufferLen ) ;
                }
                else
                {
                    break;
                }
            }
        }
        </code>
 
     */

    bool UART4_PeekSafe (uint8_t *dataByte, uint16_t offset);

    /**
      @Summary
        Returns the size of the receive buffer

      @Description
        This routine returns the size of the receive buffer.

      @Param
        None.

      @Returns
        Size of receive buffer.
    
      @Example 
        <code>
        const uint8_t readBuffer[5];
        unsigned int size, numBytes = 0;
        unsigned int readbufferLen = sizeof(readBuffer);
        UART4__Initializer();
    
        while(size < readbufferLen)
        {
            UART4_TasksReceive ( );
            size = UART4_ReceiveBufferSizeGet();
        }
        numBytes = UART4_ReadBuffer ( readBuffer , readbufferLen ) ;
        </code>
 
     */

    unsigned int UART4_ReceiveBufferSizeGet (void);

    /**
      @Summary
        Returns the size of the transmit buffer

      @Description
        This routine returns the size of the transmit buffer.

     @Param
        None.
 
     @Returns
        Size of transmit buffer.

     @Example
        Refer to UART4_Initializer(); for example.
     */

    unsigned int UART4_TransmitBufferSizeGet (void);

    /**
      @Summary
        Returns the status of the receive buffer

      @Description
        This routine returns if the receive buffer is empty or not.

      @Param
        None.
 
      @Returns
        True if the receive buffer is empty
        False if the receive buffer is not empty
    
      @Example
        <code>
        char                     myBuffer[MY_BUFFER_SIZE];
        unsigned int             numBytes;
        UART4_TRANSFER_STATUS status ;

        // Pre-initialize myBuffer with MY_BUFFER_SIZE bytes of valid data.

        numBytes = 0;
        while( numBytes < MY_BUFFER_SIZE);
        {
            status = UART4_TransferStatusGet ( ) ;
            if (!UART4_ReceiveBufferIsEmpty())
            {
                numBytes += UART4_ReadBuffer( myBuffer + numBytes, MY_BUFFER_SIZE - numBytes )  ;
                if(numBytes < readbufferLen)
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
            else
            {
                continue;
            }

            // Do something else...
        }
        </code>
 
     */

    bool UART4_ReceiveBufferIsEmpty (void);

    /**
      @Summary
        Returns the status of the transmit buffer

      @Description
        This routine returns if the transmit buffer is full or not.

     @Param
        None.
 
     @Returns
        True if the transmit buffer is full
        False if the transmit buffer is not full

     @Example
        Refer to UART4_Initializer() for example.
 
     */

    bool UART4_TransmitBufferIsFull (void);

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

    ///// CHBXX ADDED FUNCTION /////
    void UART4_Write16 (uint16_t txData);

#ifdef __cplusplus  // Provide C++ Compatibility

}

#endif

#endif  // UART4_H
