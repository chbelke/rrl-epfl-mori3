#include "Coms.h"
#include "define.h"
#include "mcc_generated_files/uart4.h"
#include "MotLin.h"
#include "MotRot.h"

uint8_t EspInCase = 0; // Switch case variable
uint8_t EspInAlloc = 0; // Incoming allocation byte
uint8_t EspInBits = 0; // Bit count in allocation byte
uint8_t EspInBytes = 0; // Incoming byte count
uint16_t MotLinTemp[3] = {0, 0, 0}; // Linear motor set value (temporary)
uint16_t MotRotTemp[3] = {0, 0, 0}; // Rotary motor set value (temporary)
uint8_t MotLinDrivePWM[3] = {0, 0, 0}; // Manual drive mode PWM values by edge

/* EspInAlloc: 
 * 0bxx000000, where xx = indicator
 * 00 = extension and angular values
 * - 0b00xxxxxx in order: linear indicators 1,2,3, rotary indicators 1,2,3
 * 01 = drive input
 * - 0b01xooooo 0 = automatic (modules evaluates), 1 = manual (direct to PWM)
 * - 0b010ooxxx manual indicator followed by 3 pwm values (1 signed byte each)
 * - 0b011xxooo automatic indicator followed by reference edge 0,1,2
 * - 0b011ooxoo direction (0 = inwards, 1 = outwards)
 * - 0b011oooxx tbd
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
        case 0: // CHECK START BYTE
            if (EspIn == ESP_Beg) {
                EspInCase = 1;
                EspInBytes = 1;
            }
            break;

        case 1: // INPUT ALLOCATION *******************************************
            EspInAlloc = EspIn;
            // if xx == 00, count bits 
            if (((EspInAlloc >> 6) & 0x03) == 0) { // ANGLE & EXTENSION INPUT
                EspInCase = 2;
                // Brian Kernighan: http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetNaive
                uint8_t EspInAllocTemp = EspInAlloc;
                for (EspInBits = 0; EspInAllocTemp; EspInBits++) {
                    EspInAllocTemp &= EspInAllocTemp - 1; // clear the LSB set
                }
            } else if (((EspInAlloc >> 6) & 0x03) == 1) { // DRIVE INPUT
                if (EspInAlloc & 0b00100000) { // automatic drive mode
                    EspInCase = 19;
                } else { // manual drive mode
                    EspInCase = 15;
                    // only last three bits relevant when counting rec. bytes
                    uint8_t EspInAllocTemp = (EspInAlloc & 0b00000111);
                    for (EspInBits = 0; EspInAllocTemp; EspInBits++) {
                        EspInAllocTemp &= EspInAllocTemp - 1; // clear the LSB set
                    }
                }
            }
            EspInBytes = EspInBytes + 1;
            break;

        case 2: // LINEAR MOTOR INPUTS ****************************************
            /* cases 2 to 7, two bytes per motor input */
            if (EspInAlloc & 0b00100000) {
                MotLinTemp[0] = ((uint16_t) (EspIn) << 8) & 0xFF00;
                EspInBytes = EspInBytes + 1;
                EspInCase = 3;
                break;
            }
        case 3:
            if (EspInAlloc & 0b00100000) {
                MotLinTemp[0] = MotLinTemp[0] | (uint16_t) EspIn;
                EspInBytes = EspInBytes + 1;
                EspInCase = 4;
                break;
            }
        case 4:
            if (EspInAlloc & 0b00010000) {
                MotLinTemp[1] = ((uint16_t) (EspIn) << 8) & 0xFF00;
                EspInBytes = EspInBytes + 1;
                EspInCase = 5;
                break;
            }
        case 5:
            if (EspInAlloc & 0b00010000) {
                MotLinTemp[1] = MotLinTemp[1] | (uint16_t) EspIn;
                EspInBytes = EspInBytes + 1;
                EspInCase = 6;
                break;
            }
        case 6:
            if (EspInAlloc & 0b00001000) {
                MotLinTemp[2] = ((uint16_t) (EspIn) << 8) & 0xFF00;
                EspInBytes = EspInBytes + 1;
                EspInCase = 7;
                break;
            }
        case 7:
            if (EspInAlloc & 0b00001000) {
                MotLinTemp[2] = MotLinTemp[2] | (uint16_t) EspIn;
                EspInBytes = EspInBytes + 1;
                EspInCase = 8;
                break;
            }

        case 8: // ROTARY MOTOR INPUTS ****************************************
            /* cases 8 to 13, two bytes per motor input */
            if (EspInAlloc & 0b00000100) {
                MotRotTemp[0] = ((uint16_t) (EspIn) << 8) & 0xFF00;
                EspInBytes = EspInBytes + 1;
                EspInCase = 9;
                break;
            }
        case 9:
            if (EspInAlloc & 0b00000100) {
                MotRotTemp[0] = MotRotTemp[0] | (uint16_t) EspIn;
                EspInBytes = EspInBytes + 1;
                EspInCase = 10;
                break;
            }
        case 10:
            if (EspInAlloc & 0b00000010) {
                MotRotTemp[1] = ((uint16_t) (EspIn) << 8) & 0xFF00;
                EspInBytes = EspInBytes + 1;
                EspInCase = 11;
                break;
            }
        case 11:
            if (EspInAlloc & 0b00000010) {
                MotRotTemp[1] = MotRotTemp[1] | (uint16_t) EspIn;
                EspInBytes = EspInBytes + 1;
                EspInCase = 12;
                break;
            }
        case 12:
            if (EspInAlloc & 0b00000001) {
                MotRotTemp[2] = ((uint16_t) (EspIn) << 8) & 0xFF00;
                EspInBytes = EspInBytes + 1;
                EspInCase = 13;
                break;
            }
        case 13:
            if (EspInAlloc & 0b00000001) {
                MotRotTemp[2] = MotRotTemp[2] | (uint16_t) EspIn;
                EspInBytes = EspInBytes + 1;
                EspInCase = 14;
                break;
            }

        case 14: // VERIFY MOTOR INPUTS ***************************************
            if (EspIn == ESP_End) {
                if (EspInBytes == (2 + EspInBits * 2)) {
                    Coms_ESP_SetMots();
                } else {
                    // data lost
                }
            }
            EspInCase = 0;
            break;

        case 15: // MANUAL DRIVE MODE *****************************************
            if (EspInAlloc & 0b00000100) {
                MotLinDrivePWM[0] = EspIn;
                EspInBytes = EspInBytes + 1;
                EspInCase = 16;
                break;
            }
        case 16:
            if (EspInAlloc & 0b00000010) {
                MotLinDrivePWM[1] = EspIn;
                EspInBytes = EspInBytes + 1;
                EspInCase = 17;
                break;
            }
        case 17:
            if (EspInAlloc & 0b00000001) {
                MotLinDrivePWM[2] = EspIn;
                EspInBytes = EspInBytes + 1;
                EspInCase = 18;
                break;
            }
        case 18:
            if (EspIn == ESP_End) {
                if (EspInBytes == (2 + EspInBits)) {
                    uint8_t m;
                    for (m = 0; m <= 2; m++) {
                        if ((EspInAlloc >> (2-m)) & 0b00000001){
                            MotRot_OUT(m, MotLinDrivePWM[m]*256);
                        }
                    }
                } else {
                    // data lost
                }
            }
            EspInCase = 0;
            break;
            
        case 19: // AUTOMATIC DRIVE MODE **************************************
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
            MotLin_Set(j - 1, MotLinTemp[j - 1]);
        }
    }

    // Set rotary motors (not yet implemented)
    for (k = 1; k == 3; k++) {
        if ((EspInAlloc >> (2 - (k - 1))) & 0x01) {
            //            MotLin_Set(k-1,MotLinTemp[k]);
        }
    }
}