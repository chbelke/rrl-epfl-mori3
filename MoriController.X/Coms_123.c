#include "Coms_ESP.h"
#include "Coms_REL.h"
#include "Coms_CMD.h"
#include "Defs.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/uart2.h"
#include "mcc_generated_files/uart3.h"
#include "Mnge_PWM.h"
#include "Mnge_RGB.h"
#include "Coms_123.h"

uint8_t EdgInCase[3] = {0, 0, 0}; // switch case variable
uint8_t EdgInAloc[3] = {0, 0, 0}; // incoming allocation byte (explained below)
uint8_t EdgIdlCnt[3] = {0, 0, 0}; // no idle byte received counter
uint8_t EdgConCnt[3] = {0, 0, 0}; // no conn/ackn byte received counter

bool Flg_IDCnfd[3] = {false, false, false}; // ID received by neighbour flag
bool Flg_IDRcvd[3] = {false, false, false}; // ID received from neighbour flag

// Neighbour ID variables
uint8_t EdgByteCount[3] = {0, 0, 0};
uint8_t NbrID[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t NbrIDTmp[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t NbrIDCount[3] = {0, 0, 0};

uint16_t EdgLngCmd[3] = {0, 0, 0}; // edge length command received by neighbour

// Outcoing bytes based on mode (EdgInAloc explanation below)
#define COMS_123_Conn 0b10010000
#define COMS_123_Ackn 0b10001000
#define COMS_123_IDOk 0b10001100
#define COMS_123_Idle 0b01100000

#define WHEEL 68.15f // wheel distance from vertex
#define SxOUT 0.9 // output speed factor for non-primary wheels

// EdgInAloc Explanation
/* EdgInAloc: 
 * 0bxxx00000, where xxx = indicator
 * 000 = stop moving
 * - 0b000xxxxx check if xxxxx is 11111, execute upon end byte
 * 001 = not used because of end byte
 * 010 = action sync byte
 * - 0b010xoooo angle command received, follows in two bytes
 * - 0b010oxooo length command received, follows in two bytes
 * - 0b010ooxoo coupling command received
 * - 0b010oooxo confirm match
 * 011 = idle mode
 * - 0b011ooooo
 * 100 = connection detected or acknowledged
 * - 0b100xoooo if x is 1, connection search only
 * - 0b100oxooo if x is 1, synchronisation byte, expect ID
 * 110 = Command for central controller (interpreted in Coms_CMD)
 * 111 = Relay mode (managed by Coms_REL)
 * - 0b111000xx xx defines edge/wifi: 00 = edg1, 01 = edg2, 10 = edg3, 11 = wifi
 * - 0b11100x00 if x is 1, relay to all (and ignore last two bits)
 */
//const char *coms_message = "Ctrl";  

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
                case 2: // xxx == 010, action sync received
                    EdgInCase[edge] = 2;
                    break;
                case 3: // xxx == 011, idle mode
                    EdgInCase[edge] = 10;
                    break;
                case 4: // xxx == 100, connection detected or acknowledged
                    EdgInCase[edge] = 20;
                    break;
                case 6: // xxx == 110, command
//                    Coms_ESP_Verbose_Write(coms_message);
                    Coms_CMD_Handle(edge, EdgInAloc[edge] & 0b00011111);
                    EdgInCase[edge] = 30;
                    break;
                case 7: // xxx == 111, relay
                    Coms_REL_Handle(edge, EdgInAloc[edge] & 0x07);
                    EdgInCase[edge] = 40;
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
                Coms_123_reset_intervals(edge);
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
                    EdgConCnt[edge] = 0;
                }
                EdgInCase[edge] = 0;
            } else if ((EdgInAloc[edge] & 0b00001000)) { // con acknowledged
                // acknowledge byte followed by ID
                if (NbrIDCount[edge] < 6) {
                    NbrIDTmp[NbrIDCount[edge] + 6 * edge] = EdgIn;
                    NbrIDCount[edge]++;
                } else {
                    if (EdgIn == EDG_End) {
                        uint8_t i;
                        for (i = 0 + 6 * edge; i <= 5 + 6 * edge; i++) {
                            NbrID[i] = NbrIDTmp[i];
                        }
                        Flg_EdgeCon[edge] = true; // make sure flag is set
                        Flg_IDRcvd[edge] = true; // ID received, tell neighbour
                        Flg_EdgeSyn[edge] = false; // reset synced
                        EdgConCnt[edge] = 0;
                    }
                    NbrIDCount[edge] = 0;
                    EdgInCase[edge] = 0;
                }
                if ((EdgInAloc[edge] & 0b00000100)) {
                    Flg_IDCnfd[edge] = true; // ID confirmed
                }
            }
            break;

        case 30: // COMMAND ****************************************************
//            Coms_123_reset_intervals(edge);
            if (Coms_CMD_Handle(edge, EdgIn)){
                EdgInCase[edge] = 0;
            }
            break;            
            
        case 40: // RELAY ****************************************************
            if (Coms_REL_Handle(edge, EdgIn)){
                EdgInCase[edge] = 0;
            }
            break;
            
        default: // DEFAULT ****************************************************
            EdgInCase[edge] = 0;
            break;
    }
}

void Coms_123_reset_intervals(uint8_t edge)
{
    EdgIdlCnt[edge] = 0; // reset interval
    EdgConCnt[edge] = 0; // reset interval
    if (!Flg_EdgeSyn[edge] && Flg_IDRcvd[edge])
        Flg_EdgeSyn[edge] = true;
}

/* ******************** NEIGHBOUR CONNECTION HANDLE ************************* */
void Coms_123_ConHandle() { // called in tmr5 at 5Hz
    uint8_t edge, byte;
    for (edge = 0; edge < 3; edge++) {
        // check if idle byte received in EDG_IdlIntrvl when synced
        if (((EdgIdlCnt[edge] >= EDG_IdlIntrvl) && Flg_EdgeSyn[edge]) || // lost
                ((EdgConCnt[edge] >= EDG_ConIntrvl) && Flg_EdgeCon[edge])) {
            Flg_EdgeCon[edge] = false;
            Flg_EdgeSyn[edge] = false;
            Flg_IDCnfd[edge] = false;
            Flg_IDRcvd[edge] = false;
        } else {
            if (EdgIdlCnt[edge] < EDG_IdlIntrvl)
                EdgIdlCnt[edge]++;
            if (EdgConCnt[edge] < EDG_ConIntrvl)
                EdgConCnt[edge]++;
        }

        // check if ID received and confirmed = synced
        if (!Flg_EdgeSyn[edge] && Flg_IDRcvd[edge] && Flg_IDCnfd[edge]) {
            Flg_EdgeSyn[edge] = true;
            EdgIdlCnt[edge] = 0;
        }

        // determine byte to be sent depending on con state flags
        if (Flg_EdgeSyn[edge]) {
            byte = COMS_123_Idle; // send idle command
            Coms_ESP_LED_State(edge, 1);
        } else if (Flg_EdgeCon[edge] && Flg_IDRcvd[edge]) {
            byte = COMS_123_IDOk;
            Coms_ESP_LED_Set_Blink_Freq(edge, 50);
            Coms_ESP_LED_State(edge, 3);
        } else if (Flg_EdgeCon[edge]) {
            byte = COMS_123_Ackn; // send acknowledge and ID
            Coms_ESP_LED_Set_Blink_Freq(edge, 50);
            Coms_ESP_LED_State(edge, 3);
        } else {
            byte = COMS_123_Conn; // send connect search
            Coms_ESP_LED_State(edge, 0); // XXX replace with function to turn esp leds off
            
        }
        
        if(Flg_Uart_Lock[edge]==false)
        {
            Flg_Uart_Lock[edge] = true;
            // write byte (ID if in con but no sync) and end byte
            Coms_123_Write(edge, byte);
            if ((byte == COMS_123_Ackn) || (byte == COMS_123_IDOk))
                Coms_123_WriteID(edge);
            Coms_123_Write(edge, EDG_End);            
            Flg_Uart_Lock[edge] = false;
        }
    }
}

/* ******************** NEIGHBOUR ACTION HANDLE ***************************** */
void Coms_123_ActHandle() { // called in tmr3 at 20Hz
    //    uint8_t edge;
    //    uint8_t byte = 0b01000000;
    //    for (edge = 0; edge < 3; edge++) {
    //        if (Flg_ActAng[edge])
    //            byte = byte | 0b00010000;
    //        if (Flg_ActExt[edge])
    //            byte = byte | 0b00001000;
    //        if (Flg_ActExt[edge])
    //            byte = byte | 0b00000100;
    //    }
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