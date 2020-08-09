#include "Defs.h"
#include "Coms_CMD.h"
#include "Coms_ESP.h"
#include "Acts_LIN.h"
#include "Acts_CPL.h"
#include "Acts_ROT.h"
#include "Mnge_RGB.h"

const char *coms_message = "Ctrl";  
const char *coms_end = "end";  

const char *coms_1 = "State1";  
const char *coms_2 = "State2";  
const char *coms_3 = "State3";  
const char *coms_4 = "State4";  

/* ******************** COMMAND HANDLER************************************** */
bool Coms_CMD_Handle(uint8_t edge, uint8_t byte){
    static bool alloc[4] = {true, true, true, true};
    static uint8_t state[4];
    
    if (alloc[edge])
    {
        state[edge] = byte;
        alloc[edge] = false;
        return false;
    }
    
    switch (state[edge]) {
        case 0:
            if(Coms_CMD_Verbose(byte))
                return Coms_CMD_Reset(&state[edge], &alloc[edge]);
            break;
            
        case 13:
            if(Coms_CMD_Shape(edge, byte))
            {
//                Coms_ESP_Verbose_Write(coms_message);
                return Coms_CMD_Reset(&state[edge], &alloc[edge]);
            }
            break;
        
        default:
            return Coms_CMD_Reset(&state[edge], &alloc[edge]);
    }  
    return false;
}


bool Coms_CMD_Reset(uint8_t* state, bool* alloc)
{
    *alloc = true;
    *state = 0;
    return true;
}


bool Coms_CMD_Verbose(uint8_t byte)
{
    if (byte == ESP_End) {
        Flg_Verbose = !Flg_Verbose;
        return true;
    } else {
        const char *casetoo = "tooLong";    
        Coms_ESP_Verbose_Write(casetoo);
    }
    return true;
}


/* EspInAloc: 
 * 0bxx000000, where xx = indicator
 * 00 = extension and angular values
 * - 0b00xxxxxx linear & rotary indicators 0,1,2 (1 = value follows in order)
 * 01 = drive input
 * - 0b01xooooo 1 = automatic (modules evaluates), 0 = manual (direct to PWM)
 * - 0b010ooxxx manual indicator followed by 3 pwm values (1 signed byte each)
 * - 0b011xxooo automatic indicator followed by reference edge 0,1,2
 * - 0b011ooxoo direction (0 = inwards, 1 = outwards)
 * - 0b011oooxx tbd
 * 10 = coupling & led input
 * - 0b10xxxooo retract couplings 0,1,2 (if already open, interval prolonged)
 * - 0b10oooxxx rgb led values follow in order
 * 11 = tbd - mode selection?
 * - 0b11xxxxxx tbd
 */

bool Coms_CMD_Shape(uint8_t edge, uint8_t byte)
{
    static uint8_t alloc[4];
    static uint16_t MotLinTemp[4][3]; // linear motor extension value (temporary)
    static uint16_t MotRotTemp[4][3]; // rotary motor angle value (temporary)
    static uint8_t EspInCase[4];
    static uint8_t EspInBits[4];
    static uint8_t EspInByts[4];
    static uint16_t EspInLost[4]; // counting lost byte instances since start
    static uint16_t EspIn0End[4]; // counting instances of no end byte since start
    static int8_t DrivePWM[4][3]; // manual drive mode PWM values by edge
    static uint8_t DriveSpd[3];
    static uint8_t DriveCrv[3]; // automatic drive mode speed and curv
    static uint8_t RgbPWM[4][3]; // rgb led values
//    static uint8_t SelfID[6] = {0, 0, 0, 0, 0, 0}; 
    
    
    switch (EspInCase[edge]) {
    case 0: // CHECK START BYTE
            EspInCase[edge] = 1;
            EspInByts[edge] = 1;
            alloc[edge] = byte;
            

    case 1: // INPUT ALLOCATION ********************************************
        // if xx == 00, count bits         
        if (((byte >> 6) & 0x03) == 0) { // *** ANGLE & EXTENSION INPUT
            EspInCase[edge] = 2;
            // Brian Kernighan: http://graphics.stanford.edu ...
            //  ... /~seander/bithacks.html#CountBitsSetNaive
            uint8_t EspInAlocTemp = byte;
            for (EspInBits[edge] = 0; EspInAlocTemp; EspInBits[edge]++) {
                EspInAlocTemp &= EspInAlocTemp - 1; // clear the LSB set
            }
        } else if (((byte >> 6) & 0x03) == 1) { // ******** DRIVE INPUT
            if (byte & 0b00100000) { // automatic drive mode
                EspInCase[edge] = 19;
            } else { // manual drive mode
                EspInCase[edge] = 15;
                // only last three bits relevant when counting rec. bytes
                uint8_t EspInAlocTemp = (byte & 0b00000111);
                for (EspInBits[edge] = 0; EspInAlocTemp; EspInBits[edge]++) {
                    EspInAlocTemp &= EspInAlocTemp - 1; // clear LSB set
                }
            }
        } else if (((byte >> 6) & 0x03) == 2) { // COUPLING & LED INPUT
            Coms_ESP_Verbose_Write(coms_message);
            if (byte & 0b00000111) {
                EspInCase[edge] = 22;
                // only last three bits relevant when counting rec. bytes
                uint8_t EspInAlocTemp = (byte & 0b00000111);
                for (EspInBits[edge] = 0; EspInAlocTemp; EspInBits[edge]++) {
                    EspInAlocTemp &= EspInAlocTemp - 1; // clear LSB set
                }
                Coms_ESP_Verbose_Write(coms_1);
            } else {
                EspInCase[edge] = 25;
                EspInBits[edge] = 0;
            }
        }
        EspInByts[edge] = EspInByts[edge] + 1;
        break;

    case 2: // LINEAR MOTOR INPUTS *****************************************
        /* cases 2 to 7, two bytes per motor input */
        if (alloc[edge] & 0b00100000) {
            MotLinTemp[edge][0] = ((uint16_t) (byte) << 8) & 0xFF00;
            EspInByts[edge] = EspInByts[edge] + 1;
            EspInCase[edge] = 3;
            break;
        }
    case 3:
        if (alloc[edge] & 0b00100000) {
            MotLinTemp[edge][0] = MotLinTemp[edge][0] | (uint16_t) byte;
            EspInByts[edge] = EspInByts[edge] + 1;
            EspInCase[edge] = 4;
            break;
        }
    case 4:
        if (alloc[edge] & 0b00010000) {
            MotLinTemp[edge][1] = ((uint16_t) (byte) << 8) & 0xFF00;
            EspInByts[edge] = EspInByts[edge] + 1;
            EspInCase[edge] = 5;
            break;
        }
    case 5:
        if (alloc[edge] & 0b00010000) {
            MotLinTemp[edge][1] = MotLinTemp[edge][1] | (uint16_t) byte;
            EspInByts[edge] = EspInByts[edge] + 1;
            EspInCase[edge] = 6;
            break;
        }
    case 6:
        if (alloc[edge] & 0b00001000) {
            MotLinTemp[edge][2] = ((uint16_t) (byte) << 8) & 0xFF00;
            EspInByts[edge] = EspInByts[edge] + 1;
            EspInCase[edge] = 7;
            break;
        }
    case 7:
        if (alloc[edge] & 0b00001000) {
            MotLinTemp[edge][2] = MotLinTemp[edge][2] | (uint16_t) byte;
            EspInByts[edge] = EspInByts[edge] + 1;
            EspInCase[edge] = 8;
            break;
        }

    case 8: // ROTARY MOTOR INPUTS *****************************************
        /* cases 8 to 13, two bytes per motor input */
        if (alloc[edge] & 0b00000100) {
            MotRotTemp[edge][0] = ((uint16_t) (byte) << 8) & 0xFF00;
            EspInByts[edge] = EspInByts[edge] + 1;
            EspInCase[edge] = 9;
            break;
        }
    case 9:
        if (alloc[edge] & 0b00000100) {
            MotRotTemp[edge][0] = MotRotTemp[edge][0] | (uint16_t) byte;
            EspInByts[edge] = EspInByts[edge] + 1;
            EspInCase[edge] = 10;
            break;
        }
    case 10:
        if (alloc[edge] & 0b00000010) {
            MotRotTemp[edge][1] = ((uint16_t) (byte) << 8) & 0xFF00;
            EspInByts[edge] = EspInByts[edge] + 1;
            EspInCase[edge] = 11;
            break;
        }
    case 11:
        if (alloc[edge] & 0b00000010) {
            MotRotTemp[edge][1] = MotRotTemp[edge][1] | (uint16_t) byte;
            EspInByts[edge] = EspInByts[edge] + 1;
            EspInCase[edge] = 12;
            break;
        }
    case 12:
        if (alloc[edge] & 0b00000001) {
            MotRotTemp[edge][2] = ((uint16_t) (byte) << 8) & 0xFF00;
            EspInByts[edge] = EspInByts[edge] + 1;
            EspInCase[edge] = 13;
            break;
        }
    case 13:
        if (alloc[edge] & 0b00000001) {
            MotRotTemp[edge][2] = MotRotTemp[edge][2] | (uint16_t) byte;
            EspInByts[edge] = EspInByts[edge] + 1;
            EspInCase[edge] = 14;
            break;
        }

    case 14: // verify motor inputs
        if (alloc[edge] == ESP_End) {
            if (EspInByts[edge] == (2 + EspInBits[edge] * 2)) {
                Coms_ESP_SetMots(alloc[edge], MotLinTemp[edge]);
            } else {
                EspInLost[edge] = EspInLost[edge] + 1; // data lost
            }
        } else {
            EspIn0End[edge] = EspIn0End[edge] + 1; // no end byte
        }
        EspInCase[edge] = 0;
        return true;
        break;

    case 15: // MANUAL DRIVE MODE ******************************************
        if (alloc[edge] & 0b00000100) {
            DrivePWM[edge][0] = byte;
            EspInByts[edge] = EspInByts[edge] + 1;
            EspInCase[edge] = 16;
            break;
        }
    case 16:
        if (alloc[edge] & 0b00000010) {
            DrivePWM[edge][1] = byte;
            EspInByts[edge] = EspInByts[edge] + 1;
            EspInCase[edge] = 17;
            break;
        }
    case 17:
        if (alloc[edge] & 0b00000001) {
            DrivePWM[edge][2] = byte;
            EspInByts[edge] = EspInByts[edge] + 1;
            EspInCase[edge] = 18;
            break;
        }
    case 18: // verify drive inputs
        if (alloc[edge] == ESP_End) {
            if (EspInByts[edge] == (2 + EspInBits[edge])) {
                uint8_t m;
                for (m = 0; m <= 2; m++) {
                    if ((alloc[edge] >> (2 - m)) & 0b00000001) {
                        Acts_ROT_Out(m, DrivePWM[edge][m]*8);
                    }
                }
            } else {
                EspInLost[edge] = EspInLost[edge] + 1; // data lost
            }
        } else {
            EspIn0End[edge] = EspIn0End[edge] + 1; // no end byte
        }
        EspInCase[edge] = 0;
        return true;
        break;

    case 19: // AUTOMATIC DRIVE MODE ***************************************
        DriveSpd[edge] = byte;
        EspInByts[edge] = EspInByts[edge] + 1;
        EspInCase[edge] = 20;
        break;

    case 20:
        DriveCrv[edge] = byte;
        EspInByts[edge] = EspInByts[edge] + 1;
        EspInCase[edge] = 21;
        break;

    case 21: // verify drive inputs
        if (alloc[edge] == ESP_End) {
            if (EspInByts[edge] == 4) {
                Coms_ESP_Drive(DriveSpd[edge], DriveCrv[edge],
                        ((alloc[edge] & 0x18) >> 3), ((alloc[edge] & 0x04) >> 2));
            } else {
                EspInLost[edge] = EspInLost[edge] + 1; // data lost
            }
        } else {
            EspIn0End[edge] = EspIn0End[edge] + 1; // no end byte
        }
        EspInCase[edge] = 0;
        return true;
        break;

    case 22: // COUPLING & LED INPUT ***************************************
        Coms_ESP_Verbose_Write(coms_message);
        if (alloc[edge] & 0b00000100) {
            RgbPWM[edge][0] = byte;
            EspInByts[edge] = EspInByts[edge] + 1;
            EspInCase[edge] = 23;
            break;
        }

    case 23:
        if (alloc[edge] & 0b00000010) {
            RgbPWM[edge][1] = byte;
            EspInByts[edge] = EspInByts[edge] + 1;
            EspInCase[edge] = 24;
            break;
        }

    case 24:
        if (alloc[edge] & 0b00000001) {
            RgbPWM[edge][2] = byte;
            EspInByts[edge] = EspInByts[edge] + 1;
            EspInCase[edge] = 25;
            break;
        }

    case 25: // verify coupling inputs
        if (byte == ESP_End) {
            Coms_ESP_Verbose_Write(coms_end);
            if (EspInByts[edge] == (2 + EspInBits[edge])) {
                Coms_ESP_Verbose_Write(coms_3);
                // set smas
                uint8_t m;
                for (m = 0; m <= 2; m++) {
                    if (alloc[edge] & (0b00100000 >> m)) {
                        Acts_CPL_On(m);
                    }
                }
                // update leds
                for (m = 0; m <= 2; m++) {
                    if ((alloc[edge] >> (2 - m)) & 0b00000001) {
                        Mnge_RGB_Set(m, RgbPWM[edge][m]);
                    }
                }
            } else {
                EspInLost[edge] = EspInLost[edge] + 1; // data lost
                Coms_ESP_Verbose_Write(coms_2);
            }
        } else {
            EspIn0End[edge] = EspIn0End[edge] + 1; // no end byte
        }
        EspInCase[edge] = 0;
        return true;
        break;

    case 26:
        break;

    default:
        EspInCase[edge] = 0;
        break;
    }
    return false;
}
    
    
/* ******************** ESP COMMAND TO MOTORS ******************************* */
void Coms_ESP_SetMots(uint8_t byte, uint16_t* MotLinTemp) {
    uint8_t j, k;

    // Set linear motors
    for (j = 1; j <= 3; j++) {
        if ((byte >> (5 - (j - 1)))) {
            Acts_LIN_SetTarget(j - 1, MotLinTemp[j - 1]);
        }
    }

    // Set rotary motors (not yet implemented)
    for (k = 1; k <= 3; k++) {
        if ((byte >> (2 - (k - 1))) & 0x01) {
            //            MotLin_Set(k-1,MotLinTemp[k]);
        }
    }
}
