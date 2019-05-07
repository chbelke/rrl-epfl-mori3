#include "Coms.h"
#include "define.h"
#include "mcc_generated_files/uart4.h"
#include "MotLin.h"
#include "MotRot.h"
#include "math.h"
#include "dsp.h"

uint8_t EspInCase = 0; // Switch case variable
uint8_t EspInAlloc = 0; // Incoming allocation byte
uint8_t EspInBits = 0; // Bit count in allocation byte
uint8_t EspInBytes = 0; // Incoming byte count
uint16_t MotLinTemp[3] = {0, 0, 0}; // Linear motor set value (temporary)
uint16_t MotRotTemp[3] = {0, 0, 0}; // Rotary motor set value (temporary)
int8_t MotLinDrivePWM[3] = {0, 0, 0}; // Manual drive mode PWM values by edge
uint8_t MotLinDriveSpd = 0;
int8_t MotLinDriveCrv = 0;
#define length 68.15

/* EspInAlloc: 
 * 0bxx000000, where xx = indicator
 * 00 = extension and angular values
 * - 0b00xxxxxx in order: linear indicators 1,2,3, rotary indicators 1,2,3
 * 01 = drive input
 * - 0b01xooooo 1 = automatic (modules evaluates), 0 = manual (direct to PWM)
 * - 0b010ooxxx manual indicator followed by 3 pwm values (1 signed byte each)
 * - 0b011xxooo automatic indicator followed by reference edge 0,1,2
 * - 0b011ooxoo direction (0 = inwards, 1 = outwards)
 * - 0b011oooxx tbd
 * 10 = tbd
 * - 0b10xxxxxx tbd
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
                            MotRot_OUT(m, MotLinDrivePWM[m]*8);
                        }
                    }
                } else {
                    // data lost
                }
            }
            EspInCase = 0;
            break;
            
        case 19: // AUTOMATIC DRIVE MODE **************************************
            MotLinDriveSpd = EspIn;
            EspInBytes = EspInBytes + 1;
            EspInCase = 20;
            break;
        
        case 20:
            MotLinDriveCrv = EspIn;
            EspInBytes = EspInBytes + 1;
            EspInCase = 21;
            break;
            
        case 21:
            if (EspIn == ESP_End) {
                if (EspInBytes == 4) {
                    Coms_ESP_Drive(MotLinDriveSpd, MotLinDriveCrv, ((EspInAlloc & 0b00011000)>>3), ((EspInAlloc & 0b00000100)>>2));
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

void Coms_ESP_Drive(uint8_t speed, int8_t curve, uint8_t edge, uint8_t direction) {
    float Mo = curve*20; // from matlab moments up to +-2160 relative to 100% speed
    float Sa = speed; 
    if (!direction) { // inwards or outwards
        Sa = -1*Sa;
    }
    
    float a,b,c; // extension values from 180
    switch (edge) {
        case 0:
            a = 180+(MotLin_Get(0)-MotLin_MIN_A)*12/MotLin_MAX_A;
            b = 180+(MotLin_Get(1)-MotLin_MIN_B)*12/MotLin_MAX_B;
            c = 180+(MotLin_Get(2)-MotLin_MIN_C)*12/MotLin_MAX_C;
            break;
        case 1:
            a = 180+(MotLin_Get(1)-MotLin_MIN_B)*12/MotLin_MAX_B;
            b = 180+(MotLin_Get(2)-MotLin_MIN_C)*12/MotLin_MAX_C;
            c = 180+(MotLin_Get(0)-MotLin_MIN_A)*12/MotLin_MAX_A;
            break;
        case 2:
            a = 180+(MotLin_Get(2)-MotLin_MIN_C)*12/MotLin_MAX_C;
            b = 180+(MotLin_Get(0)-MotLin_MIN_A)*12/MotLin_MAX_A;
            c = 180+(MotLin_Get(1)-MotLin_MIN_B)*12/MotLin_MAX_B;
            break;
        default:
            a = 180+(MotLin_Get(0)-MotLin_MIN_A)*12/MotLin_MAX_A;
            b = 180+(MotLin_Get(1)-MotLin_MIN_B)*12/MotLin_MAX_B;
            c = 180+(MotLin_Get(2)-MotLin_MIN_C)*12/MotLin_MAX_C;
            break;
    }
    
    // vertex angles
    // float alpha = acosf((b*b + c*c - a*a)/(2*b*c));
    float beta = acosf((a*a + c*c - b*b)/(2*a*c));
    float gamm = acosf((a*a + b*b - c*c)/(2*a*b));
    
    
    // wheel coordinates (for a: [length, 0])
    float Wb[2] = {(b-length)*cosf(gamm), (b-length)*sinf(gamm)};
    float Wc[2] = {a-length*cosf(beta), length*sinf(beta)};
    UART4_Write((int8_t)Wb[0]);
    UART4_Write((int8_t)Wb[1]);
    
    // second point in wheel direction
    float Wb2[2] = {Wb[0]-cosf(PI/2-gamm), Wb[1]+sinf(PI/2-gamm)};
    float Wc2[2] = {Wc[0]+cosf(PI/2-beta), Wc[1]+sinf(PI/2-beta)};
    
    // centroid coordinates
    float D[2] = {(b*cosf(gamm)+a)/3, b*sinf(gamm)};
    
    // moment arm of wheel force to centroid
    float Da = fabsf(D[0]-length);
    float Db = fabsf((Wb2[1]-Wb[1])*D[0] - (Wb2[0]-Wb[0])*D[1] + Wb2[0]*Wb[1] - Wb2[1]*Wb[0]) / sqrtf(powf(Wb2[1]-Wb[1],2) + powf(Wb2[0]-Wb[0],2));
    float Dc = fabsf((Wc2[1]-Wc[1])*D[0] - (Wc2[0]-Wc[0])*D[1] + Wc2[0]*Wc[1] - Wc2[1]*Wc[0]) / sqrtf(powf(Wc2[1]-Wc[1],2) + powf(Wc2[0]-Wc[0],2));
    
    float Sc = (Mo-Sa*Da)/(Db*cosf(PI/2-beta)/cosf(PI/2-gamm) + Dc);
    float Sb = Sc*cosf(PI/2-beta)/cosf(PI/2-gamm);
    
//    UART4_Write(edge);
//    UART4_Write((int8_t)Sa);
//    UART4_Write((int8_t)Sb);
//    UART4_Write((int8_t)Sc);
    
    switch (edge) {
        case 0:
            MotRot_OUT(0,Sa*10);
            MotRot_OUT(1,Sb*10);
            MotRot_OUT(2,Sc*10);
            break;
        case 1:
            MotRot_OUT(0,Sb*10);
            MotRot_OUT(1,Sc*10);
            MotRot_OUT(2,Sa*10);
            break;
        case 2:
            MotRot_OUT(0,Sc*10);
            MotRot_OUT(1,Sa*10);
            MotRot_OUT(2,Sb*10);
            break;
        default:
            MotRot_OUT(0,Sa*10);
            MotRot_OUT(1,Sb*10);
            MotRot_OUT(2,Sc*10);
            break;
    }
}