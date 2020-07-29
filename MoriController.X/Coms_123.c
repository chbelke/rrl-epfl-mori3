#include "Coms_123.h"
#include "Coms_ESP.h"
#include "define.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/uart2.h"
#include "mcc_generated_files/uart3.h"
#include "TLC59208.h"

uint8_t EdgInCase[3] = {0, 0, 0}; // switch case variable
uint8_t EdgInAloc[3] = {0, 0, 0}; // incoming allocation byte (explained below)
uint8_t EdgIdleCount[3] = {0, 0, 0};

bool Flg_IDCnfd[3] = {false, false, false};
bool Flg_IDRcvd[3] = {false, false, false};

// Neighbour ID variables
uint8_t EdgByteCount[3] = {0,0,0};
uint8_t NbrID[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t NbrIDTemp[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t NbrIDCount[3] = {0, 0, 0};

uint16_t EdgLngCmd[3] = {0, 0, 0}; // edge length command received by neighbour

// Outcoing bytes based on mode (EdgInAloc explanation below)
#define COMS_123_Conn 0b10010000
#define COMS_123_Ackn 0b10001000
#define COMS_123_IDOk 0b10001100
#define COMS_123_Idle 0b01100000

// EdgInAloc Explanation
/* EdgInAloc: 
 * 0bxxx00000, where xxx = indicator
 * 000 = stop moving
 * - 0b000xxxxx check if xxxxx is 11111, execute upon end byte
 * 001 = not used because of end byte
 * 010 = action byte
 * - 0b010xoooo angle command received, follows in two bytes
 * - 0b010oxooo length command received, follows in two bytes
 * - 0b010ooxoo coupling command received
 * 011 = idle mode
 * - 0b011ooooo
 * 100 = connection detected or acknowledged
 * - 0b100xoooo if x is 1, connection search only
 * - 0b100oxooo if x is 1, synchronisation byte, expect ID
 * 111 = Relay mode - Kevin
 * - 0b111xxxxx
 */

/* ******************** EDGE COMMAND EVALUATION ***************************** */
void Coms_123_Eval(uint8_t edge) {
    uint8_t EdgIn = Coms_123_Read(edge); // ready incoming byte
    switch (EdgInCase[edge]) { // select case set by previous byte
        case 0: // INPUT ALLOCATION ********************************************
            EdgInAloc[edge] = EdgIn;
            switch ((EdgInAloc[edge] >> 5) & 0x07) {
                case 0: // xxx == 000, emergency stop
                    if ((EdgInAloc[edge] & 0x1F) == 37)
                        EdgInCase[edge] = 1;
                    break;
                case 2: // xxx == 010, action command received
                    EdgInCase[edge] = 2;
                    break;
                case 3: // xxx == 011, idle mode
                    EdgInCase[edge] = 10;
                    break;
                case 4: // xxx == 100, connection detected or acknowledged
                    EdgInCase[edge] = 20;
                    break;
                case 7: // xxx == 111, relay (Kevin)
                    EdgInCase[edge] = 30 + (EdgInAloc[edge] & 0b00011111);
                    break;
            }
            break;

        case 1: // EMERGENCY STOP **********************************************
            if (EdgIn == EDG_End) {
                Flg_EdgeSyn[edge] = false;
            }
            EdgInCase[edge] = 0;
            break;

        case 2: // ACTION COMMAND RECEIVED *************************************
            if (EdgInAloc[edge] & 0b00010000) { // angle command
                // first angle byte
                EdgInCase[edge] = 3;
                break;
            }
        case 3:
            if (EdgInAloc[edge] & 0b00010000) {
                // second angle byte
                EdgInCase[edge] = 4;
                break;
            }
        case 4:
            if (EdgInAloc[edge] & 0b00001000) { // length command
                // first length byte
                EdgInCase[edge] = 5;
                break;
            }
        case 5:
            if (EdgInAloc[edge] & 0b00001000) {
                // second length byte
                EdgInCase[edge] = 6;
                break;
            }
        case 6:
            if (EdgInAloc[edge] & 0b00000100) { // coupling command
                // open coupling ?
                EdgInCase[edge] = 7;
                break;
            }
        case 7: // verify action command
            if (EdgIn == EDG_End) {
                // check length and verify with own action commands
            }
            EdgInCase[edge] = 0;
            break;

        case 10: // IDLE MODE **************************************************
            if (EdgIn == EDG_End) {
                EdgIdleCount[edge] = EDG_IdleInterval; // reset interval
                if (!Flg_EdgeSyn[edge] && Flg_IDRcvd[edge])
                    Flg_EdgeSyn[edge] = true;
            }
            EdgInCase[edge] = 0;
            break;

        case 20: // CONNECTION DETECTED OR ACKNOWLEDGED ************************
            if ((EdgInAloc[edge] & 0b00010000)) { // con detected
                if (EdgIn == EDG_End) {
                    Flg_EdgeCon[edge] = true;
                    Flg_EdgeSyn[edge] = false;
                    Flg_IDCnfd[edge] = false;
                    Flg_IDRcvd[edge] = false;
                }
                EdgInCase[edge] = 0;
            } else if ((EdgInAloc[edge] & 0b00001000)) { // con acknowledged
                // acknowledge byte followed by ID
                if (NbrIDCount[edge] < 6) {
                    NbrIDTemp[NbrIDCount[edge] + 6 * edge] = EdgIn;
                    NbrIDCount[edge]++;
                } else {
                    if (EdgIn == EDG_End) {
                        uint8_t i;
                        for (i = 0 + 6 * edge; i <= 5 + 6 * edge; i++) {
                            NbrID[i] = NbrIDTemp[i];
                        }
                        Flg_EdgeCon[edge] = true; // make sure flag is set
                        Flg_IDRcvd[edge] = true; // ID received, tell neighbour
                        Flg_EdgeSyn[edge] = false; // reset synced
                    }
                    NbrIDCount[edge] = 0;
                    EdgInCase[edge] = 0;
                }
                if ((EdgInAloc[edge] & 0b00000100)) {
                    Flg_IDCnfd[edge] = true; // ID confirmed
                }
            }
            break;
            
// **************************************************************************
// ***************************** RELAY MODE *********************************
// **************************************************************************            
        
        case 30: // Verbose ****************************************
            if(EdgByteCount[edge] != 2)
            {
                EdgByteCount[edge]++;
                break;
            }
            else if (EdgIn == EDG_End)
            {
                Flg_Verbose = !Flg_Verbose;
                EdgInCase[edge] = 0;
                EdgByteCount[edge] = 0;
            }
            break;
        
        case 31: // Verbose ****************************************
            break;
            
// **************************************************************************
// ****************************** Default ***********************************
// **************************************************************************            
        default:
            EdgInCase[edge] = 0;
            break;
    }
}

/* ******************** NEIGHBOUR CONNECTION HANDLE ************************* */
void Coms_123_ConHandle() { // called in tmr5 at 5Hz
    uint8_t edge, byte;
    for (edge = 0; edge < 3; edge++) {
        // check if idle byte received in EDG_IdleInterval when synced
        if ((EdgIdleCount[edge] == 0) && (Flg_EdgeSyn[edge])) { // con lost
            Flg_EdgeCon[edge] = false;
            Flg_EdgeSyn[edge] = false;
            Flg_IDCnfd[edge] = false;
            Flg_IDRcvd[edge] = false;
        } else if (EdgIdleCount[edge]) {
            EdgIdleCount[edge]--; // count down interval
        }
        
        // check if ID received and confirmed = synced
        if (!Flg_EdgeSyn[edge] && Flg_IDRcvd[edge] && Flg_IDCnfd[edge]) {
            Flg_EdgeSyn[edge] = true;
            EdgIdleCount[edge] = EDG_IdleInterval;
        }

        // determine byte to be sent depending on con state flags
        if (Flg_EdgeSyn[edge]) {
            byte = COMS_123_Idle; // send idle command
        } else if (Flg_EdgeCon[edge] && Flg_IDRcvd[edge]) {
            byte = COMS_123_IDOk;
        } else if (Flg_EdgeCon[edge]) {
            byte = COMS_123_Ackn; // send acknowledge and ID
        } else {
            byte = COMS_123_Conn; // send connect search
        }

        // write byte (ID if in con but no sync) and end byte
        Coms_123_Write(edge, byte);
        if ((byte == COMS_123_Ackn) || (byte == COMS_123_IDOk))
            Coms_123_WriteID(edge);
        Coms_123_Write(edge, EDG_End);
    }
}

/* ******************** WRITE BYTE TO EDGE ********************************** */
void Coms_123_Write(uint8_t edge, uint8_t byte) {
    switch (edge) { // send byte to specific UART based on edge
        case 0:
            UART1_Write(byte);
            break;
        case 1:
            UART2_Write(byte);
            break;
        case 2:
            UART3_Write(byte);
            break;
    }
}

/* ******************** WRITE ID TO EDGE ************************************ */
void Coms_123_WriteID(uint8_t edge) { // called in Coms_123_ConHandle
    uint8_t i, byte;
    for (i = 0; i < 6; i++) { // loop through 6 ID bytes
        byte = Coms_ESP_ReturnID(i); // get byte from ESP Coms
        Coms_123_Write(edge, byte); // write to edge
    }
}

/* ******************** READ BYTE FROM EDGE ********************************* */
uint8_t Coms_123_Read(uint8_t edge) {
    uint8_t byte;
    switch (edge) {
        case 0:
            byte = UART1_Read();
            break;
        case 1:
            byte = UART2_Read();
            break;
        case 2:
            byte = UART3_Read();
            break;
    }
    return byte;
}