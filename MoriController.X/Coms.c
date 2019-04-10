#include "Coms.h"
#include "define.h"
#include "mcc_generated_files/uart4.h"
#include "MotLin.h"

uint8_t EspInCase = 0; // Switch case variable
uint8_t EspInAlloc = 0; // Incoming allocation byte
uint8_t EspInBits = 0; // Bit count in allocation byte
uint8_t EspInBytes = 0; // Incoming byte count
uint16_t MotLinTemp[3] = {0, 0, 0}; // Linear motor set value (temporary)
uint16_t MotRotTemp[3] = {0, 0, 0}; // Rotary motor set value (temporary)

/* EspInAlloc: 
 * 0bxx000000
 * xx = indicator
 * 00 = MotLin + MotRot
 */

/* This function evaluates the incoming bytes from the ESP module via UART4. 
 * It gets called from the ISR each time a byte is received. It first checks
 * whether the first byte is the control value ESP_Beg. It then evaluates the
 * allocation byte, steps through the appropriate cases depending on the
 * allocation byte, and verifies the validity of the incoming date through 
 * the control value ESP_End and the total number of bytes received. If the 
 * incoming data is valid, it triggers the appropriate function to implement
 * the commands received.
 * Fall-through is utilised to allocate the commands correctly according to
 * EspInAlloc.
 */

/* ******************** ESP COMMAND EVALUATION ****************************** */
void Coms_ESP_Eval() {
    uint8_t EspIn = UART4_Read(); // Incoming byte
    switch (EspInCase) {
        case 0: // check for start verification byte
            if (EspIn == ESP_Beg) {
                EspInCase = 1;
                EspInBytes = 1;
            }
            break;
        case 1: // check what will be received
            EspInAlloc = EspIn;
            // if xx == 00, count bits 
            if (((EspInAlloc >> 6) & 0x03) == 0) {
                EspInCase = 2;
                // Brian Kernighan: http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetNaive
                uint8_t EspInAllocTemp = EspInAlloc;
                for (EspInBits = 0; EspInAllocTemp; EspInBits++) {
                    EspInAllocTemp &= EspInAllocTemp - 1; // clear the LSB set
                }
            }
            EspInBytes = EspInBytes + 1;
            break;

        case 2: /* *** LINEAR MOTOR EVALUATION ****************************** */
            /* cases 2 to 7, two bytes per motor input */
            if (EspInAlloc & 0b00100000) {
                MotLinTemp[1] = ((uint16_t) (EspIn) << 8) & 0xFF00;
                EspInBytes = EspInBytes + 1;
                EspInCase = 3;
                break;
            }
        case 3:
            if (EspInAlloc & 0b00100000) {
                MotLinTemp[1] = MotLinTemp[1] | (uint16_t) EspIn;
                EspInBytes = EspInBytes + 1;
                EspInCase = 4;
                break;
            }
        case 4:
            if (EspInAlloc & 0b00010000) {
                MotLinTemp[2] = ((uint16_t) (EspIn) << 8) & 0xFF00;
                EspInBytes = EspInBytes + 1;
                EspInCase = 5;
                break;
            }
        case 5:
            if (EspInAlloc & 0b00010000) {
                MotLinTemp[2] = MotLinTemp[2] | (uint16_t) EspIn;
                EspInBytes = EspInBytes + 1;
                EspInCase = 6;
                break;
            }
        case 6:
            if (EspInAlloc & 0b00001000) {
                MotLinTemp[3] = ((uint16_t) (EspIn) << 8) & 0xFF00;
                EspInBytes = EspInBytes + 1;
                EspInCase = 7;
                break;
            }
        case 7:
            if (EspInAlloc & 0b00001000) {
                MotLinTemp[3] = MotLinTemp[3] | (uint16_t) EspIn;
                EspInBytes = EspInBytes + 1;
                EspInCase = 8;
                break;
            }
            
        case 8: /* *** ROTARY MOTOR EVALUATION ****************************** */
            /* cases 8 to 13, two bytes per motor input */
            if (EspInAlloc & 0b00000100) {
                MotRotTemp[1] = ((uint16_t) (EspIn) << 8) & 0xFF00;
                EspInBytes = EspInBytes + 1;
                EspInCase = 9;
                break;
            }
        case 9:
            if (EspInAlloc & 0b00000100) {
                MotRotTemp[1] = MotRotTemp[1] | (uint16_t) EspIn;
                EspInBytes = EspInBytes + 1;
                EspInCase = 10;
                break;
            }
        case 10:
            if (EspInAlloc & 0b00000010) {
                MotRotTemp[2] = ((uint16_t) (EspIn) << 8) & 0xFF00;
                EspInBytes = EspInBytes + 1;
                EspInCase = 11;
                break;
            }
        case 11:
            if (EspInAlloc & 0b00000010) {
                MotRotTemp[2] = MotRotTemp[2] | (uint16_t) EspIn;
                EspInBytes = EspInBytes + 1;
                EspInCase = 12;
                break;
            }
        case 12:
            if (EspInAlloc & 0b00000001) {
                MotRotTemp[3] = ((uint16_t) (EspIn) << 8) & 0xFF00;
                EspInBytes = EspInBytes + 1;
                EspInCase = 13;
                break;
            }
        case 13:
            if (EspInAlloc & 0b00000001) {
                MotRotTemp[3] = MotRotTemp[3] | (uint16_t) EspIn;
                EspInBytes = EspInBytes + 1;
                EspInCase = 14;
                break;
            }

            /* *** MOTs INPUT VERIFICATION ********************************** */
        case 14:
            if (EspIn == ESP_End) {
                if (EspInBytes == (2 + EspInBits * 2)) {
                    Coms_ESP_SetMots();
                } else {
                    // data lost
                }
            }
            EspInCase = 0;
            break;

        default:
            EspInCase = 0;
    }

}

/* ******************** ESP COMMAND TO MOTORS ******************************* */
void Coms_ESP_SetMots() {
    uint8_t j, k;

    // Set linear motors
    for (j = 1; j <= 3; j++) {
        if ((EspInAlloc >> (5 - (j - 1)))) {
            MotLin_Set(j, MotLinTemp[j]);
        }
    }

    // Set rotary motors (not yet implemented)
    for (k = 1; k == 3; k++) {
        if ((EspInAlloc >> (2 - (k - 1))) & 0x01) {
            //            MotLin_Set(k,MotLinTemp[k]);
        }
    }
}