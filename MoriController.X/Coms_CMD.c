#include "Defs.h"
#include "Coms_CMD.h"
#include "Coms_ESP.h"
#include "Acts_LIN.h"
#include "Acts_CPL.h"
#include "Acts_ROT.h"
#include "Mnge_RGB.h"

/* ******************** COMMAND HANDLER************************************** */
bool Coms_CMD_Handle(uint8_t edge, uint8_t byte){
    static bool alloc = true;
    static uint8_t state;
    
    if (alloc)
    {
        state = byte;
        alloc = false;
        return true;
    }
    
    switch (state) {
        case 0:
            if(Coms_CMD_Verbose())
                return Coms_CMD_Reset(&state, &alloc);
            break;
            
        case 13:
            if(Coms_CMD_Shape(edge, byte))
                return Coms_CMD_Reset(&state, &alloc);
            break;
        
        default:
            return Coms_CMD_Reset(&state, &alloc);
    }  
    return false;
}


bool Coms_CMD_Reset(uint8_t* state, bool* alloc)
{
    *alloc = true;
    *state = 0;
    return true;
}


bool Coms_CMD_Verbose()
{
    Flg_Verbose = !Flg_Verbose;
    return true;
}

bool Coms_CMD_Shape(uint8_t edge, uint8_t byte)
{
    static uint16_t MotLinTemp[3] = {0, 0, 0}; // linear motor extension value (temporary)
    static uint16_t MotRotTemp[3] = {0, 0, 0}; // rotary motor angle value (temporary)
    static uint8_t EspInCase = 0;
    static uint8_t EspInBits = 0;
    static uint8_t EspInByts = 0;
    static uint16_t EspInLost = 0; // counting lost byte instances since start
    static uint16_t EspIn0End = 0; // counting instances of no end byte since start
    static int8_t DrivePWM[3] = {0, 0, 0}; // manual drive mode PWM values by edge
    static uint8_t DriveSpd, DriveCrv = 0; // automatic drive mode speed and curv
    static uint8_t RgbPWM[3] = {0, 0, 0}; // rgb led values
//    static uint8_t SelfID[6] = {0, 0, 0, 0, 0, 0};
    
    switch (EspInCase) {
    case 0: // CHECK START BYTE
            EspInCase = 1;
            EspInByts = 1;
        break;

    case 1: // INPUT ALLOCATION ********************************************
        // if xx == 00, count bits 
        if (((byte >> 6) & 0x03) == 0) { // *** ANGLE & EXTENSION INPUT
            EspInCase = 2;
            // Brian Kernighan: http://graphics.stanford.edu ...
            //  ... /~seander/bithacks.html#CountBitsSetNaive
            uint8_t EspInAlocTemp = byte;
            for (EspInBits = 0; EspInAlocTemp; EspInBits++) {
                EspInAlocTemp &= EspInAlocTemp - 1; // clear the LSB set
            }
        } else if (((byte >> 6) & 0x03) == 1) { // ******** DRIVE INPUT
            if (byte & 0b00100000) { // automatic drive mode
                EspInCase = 19;
            } else { // manual drive mode
                EspInCase = 15;
                // only last three bits relevant when counting rec. bytes
                uint8_t EspInAlocTemp = (byte & 0b00000111);
                for (EspInBits = 0; EspInAlocTemp; EspInBits++) {
                    EspInAlocTemp &= EspInAlocTemp - 1; // clear LSB set
                }
            }
        } else if (((byte >> 6) & 0x03) == 2) { // COUPLING & LED INPUT
            if (byte & 0b00000111) {
                EspInCase = 22;
                // only last three bits relevant when counting rec. bytes
                uint8_t EspInAlocTemp = (byte & 0b00000111);
                for (EspInBits = 0; EspInAlocTemp; EspInBits++) {
                    EspInAlocTemp &= EspInAlocTemp - 1; // clear LSB set
                }
            } else {
                EspInCase = 25;
                EspInBits = 0;
            }
        }
        EspInByts = EspInByts + 1;
        break;

    case 2: // LINEAR MOTOR INPUTS *****************************************
        /* cases 2 to 7, two bytes per motor input */
        if (byte & 0b00100000) {
            MotLinTemp[0] = ((uint16_t) (byte) << 8) & 0xFF00;
            EspInByts = EspInByts + 1;
            EspInCase = 3;
            break;
        }
    case 3:
        if (byte & 0b00100000) {
            MotLinTemp[0] = MotLinTemp[0] | (uint16_t) byte;
            EspInByts = EspInByts + 1;
            EspInCase = 4;
            break;
        }
    case 4:
        if (byte & 0b00010000) {
            MotLinTemp[1] = ((uint16_t) (byte) << 8) & 0xFF00;
            EspInByts = EspInByts + 1;
            EspInCase = 5;
            break;
        }
    case 5:
        if (byte & 0b00010000) {
            MotLinTemp[1] = MotLinTemp[1] | (uint16_t) byte;
            EspInByts = EspInByts + 1;
            EspInCase = 6;
            break;
        }
    case 6:
        if (byte & 0b00001000) {
            MotLinTemp[2] = ((uint16_t) (byte) << 8) & 0xFF00;
            EspInByts = EspInByts + 1;
            EspInCase = 7;
            break;
        }
    case 7:
        if (byte & 0b00001000) {
            MotLinTemp[2] = MotLinTemp[2] | (uint16_t) byte;
            EspInByts = EspInByts + 1;
            EspInCase = 8;
            break;
        }

    case 8: // ROTARY MOTOR INPUTS *****************************************
        /* cases 8 to 13, two bytes per motor input */
        if (byte & 0b00000100) {
            MotRotTemp[0] = ((uint16_t) (byte) << 8) & 0xFF00;
            EspInByts = EspInByts + 1;
            EspInCase = 9;
            break;
        }
    case 9:
        if (byte & 0b00000100) {
            MotRotTemp[0] = MotRotTemp[0] | (uint16_t) byte;
            EspInByts = EspInByts + 1;
            EspInCase = 10;
            break;
        }
    case 10:
        if (byte & 0b00000010) {
            MotRotTemp[1] = ((uint16_t) (byte) << 8) & 0xFF00;
            EspInByts = EspInByts + 1;
            EspInCase = 11;
            break;
        }
    case 11:
        if (byte & 0b00000010) {
            MotRotTemp[1] = MotRotTemp[1] | (uint16_t) byte;
            EspInByts = EspInByts + 1;
            EspInCase = 12;
            break;
        }
    case 12:
        if (byte & 0b00000001) {
            MotRotTemp[2] = ((uint16_t) (byte) << 8) & 0xFF00;
            EspInByts = EspInByts + 1;
            EspInCase = 13;
            break;
        }
    case 13:
        if (byte & 0b00000001) {
            MotRotTemp[2] = MotRotTemp[2] | (uint16_t) byte;
            EspInByts = EspInByts + 1;
            EspInCase = 14;
            break;
        }

    case 14: // verify motor inputs
        if (byte == ESP_End) {
            if (EspInByts == (2 + EspInBits * 2)) {
                Coms_ESP_SetMots(byte, MotLinTemp);
            } else {
                EspInLost = EspInLost + 1; // data lost
            }
        } else {
            EspIn0End = EspIn0End + 1; // no end byte
        }
        EspInCase = 0;
        return true;
        break;

    case 15: // MANUAL DRIVE MODE ******************************************
        if (byte & 0b00000100) {
            DrivePWM[0] = byte;
            EspInByts = EspInByts + 1;
            EspInCase = 16;
            break;
        }
    case 16:
        if (byte & 0b00000010) {
            DrivePWM[1] = byte;
            EspInByts = EspInByts + 1;
            EspInCase = 17;
            break;
        }
    case 17:
        if (byte & 0b00000001) {
            DrivePWM[2] = byte;
            EspInByts = EspInByts + 1;
            EspInCase = 18;
            break;
        }
    case 18: // verify drive inputs
        if (byte == ESP_End) {
            if (EspInByts == (2 + EspInBits)) {
                uint8_t m;
                for (m = 0; m <= 2; m++) {
                    if ((byte >> (2 - m)) & 0b00000001) {
                        Acts_ROT_Out(m, DrivePWM[m]*8);
                    }
                }
            } else {
                EspInLost = EspInLost + 1; // data lost
            }
        } else {
            EspIn0End = EspIn0End + 1; // no end byte
        }
        EspInCase = 0;
        return true;
        break;

    case 19: // AUTOMATIC DRIVE MODE ***************************************
        DriveSpd = byte;
        EspInByts = EspInByts + 1;
        EspInCase = 20;
        break;

    case 20:
        DriveCrv = byte;
        EspInByts = EspInByts + 1;
        EspInCase = 21;
        break;

    case 21: // verify drive inputs
        if (byte == ESP_End) {
            if (EspInByts == 4) {
                Coms_ESP_Drive(DriveSpd, DriveCrv,
                        ((byte & 0x18) >> 3), ((byte & 0x04) >> 2));
            } else {
                EspInLost = EspInLost + 1; // data lost
            }
        } else {
            EspIn0End = EspIn0End + 1; // no end byte
        }
        EspInCase = 0;
        return true;
        break;

    case 22: // COUPLING & LED INPUT ***************************************
        if (byte & 0b00000100) {
            RgbPWM[0] = byte;
            EspInByts = EspInByts + 1;
            EspInCase = 23;
            break;
        }

    case 23:
        if (byte & 0b00000010) {
            RgbPWM[1] = byte;
            EspInByts = EspInByts + 1;
            EspInCase = 24;
            break;
        }

    case 24:
        if (byte & 0b00000001) {
            RgbPWM[2] = byte;
            EspInByts = EspInByts + 1;
            EspInCase = 25;
            break;
        }

    case 25: // verify coupling inputs
        if (byte == ESP_End) {
            if (EspInByts == (2 + EspInBits)) {
                // set smas
                uint8_t m;
                for (m = 0; m <= 2; m++) {
                    if (byte & (0b00100000 >> m)) {
                        Acts_CPL_On(m);
                    }
                }
                // update leds
                for (m = 0; m <= 2; m++) {
                    if ((byte >> (2 - m)) & 0b00000001) {
                        Mnge_RGB_Set(m, RgbPWM[m]);
                    }
                }
            } else {
                EspInLost = EspInLost + 1; // data lost
            }
        } else {
            EspIn0End = EspIn0End + 1; // no end byte
        }
        EspInCase = 0;
        return true;
        break;

    case 26:
        break;

    default:
        EspInCase = 0;
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
