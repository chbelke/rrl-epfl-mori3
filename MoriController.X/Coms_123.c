#include "Coms_ESP.h"
#include "Coms_REL.h"
#include "Coms_CMD.h"
#include "Defs_GLB.h"
#include "string.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/uart2.h"
#include "mcc_generated_files/uart3.h"
#include "Mnge_PWM.h"
#include "Mnge_RGB.h"
#include "Coms_123.h"
#include "Acts_CPL.h"
#include "Acts_LIN.h"
#include "Acts_ROT.h"

uint8_t EdgInAloc[3] = {0, 0, 0}; // incoming allocation byte (explained below)
volatile uint8_t EdgIdlCnt[3] = {0, 0, 0}; // no idle byte received counter
volatile uint8_t EdgConCnt[3] = {0, 0, 0}; // no conn/ackn byte received counter
volatile uint8_t EdgActCnt[3] = {0, 0, 0}; // no idle byte received counter
volatile bool Flg_IDCnfd[3] = {false, false, false}; // ID received by neighbour flag
volatile bool Flg_IDRcvd[3] = {false, false, false}; // ID received from neighbour flag
volatile bool Flg_AllEdgRdy[3] = {false, false, false}; // own edges ready send

// Neighbour ID variables
uint8_t EdgByteCount[3] = {0, 0, 0};
uint8_t NbrID[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t NbrIDTmp[18] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
uint8_t NbrIDCount[3] = {0, 0, 0};
uint8_t NbrCmdExt[3] = {0, 0, 0}; // extension command received by neighbour
uint8_t NbrCurExt[3] = {0, 0, 0}; // current extension received by neighbour
uint16_t NbrCmdAng[3] = {0, 0, 0}; // angle command received by neighbour
volatile bool NbrCmdMatch[3] = {false, false, false};
volatile bool NbrEdgesRdy[3] = {false, false, false};

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
 * - 0b010xoooo length command received, follows in one byte
 * - 0b010oxooo angle command received, follows in two bytes
 * - 0b010ooxoo coupling command received
 * - 0b010oooxx 11 cmd match and go, 01 cmd match and wait
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

/* ******************** EDGE COMMAND EVALUATION ***************************** */
void Coms_123_Eval(uint8_t edge) { // called in main
    static uint8_t EdgInCase[3] = {0, 0, 0}; // switch case variable

    if(!Coms_123_Ready(edge)) return; // check if byte received
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
                case 6: // xxx == 110, command
                    Coms_CMD_Handle(edge, EdgInAloc[edge] & 0b00011111);
                    EdgInCase[edge] = 30;
                    break;
                case 7: // xxx == 111, relay
                    Coms_REL_Handle(edge, EdgInAloc[edge] & 0b00011111);
                    EdgInCase[edge] = 40;
                    break;
                default:
                    EdgInCase[edge] = 50;
                    break;
            }
            break;

        case 1: // EMERGENCY STOP **********************************************
            if (EdgIn == EDG_End) {
                char message1 = (char)EdgInAloc[edge];
                Coms_ESP_Verbose_Write(&message1);                    
                Flg_EdgeSyn[edge] = false;
                Flg_EdgeAct[edge] = false;
                EdgInCase[edge] = 0;
            } else {
                EdgInCase[edge] = 50;
            }
            break;

        case 2: // ACTION COMMAND RECEIVED *************************************
            if (EdgInAloc[edge] & 0b00010000) { // extension command
                NbrCmdExt[edge] = EdgIn;
                EdgInCase[edge] = 3;
                break;
            }
        case 3:
            if (EdgInAloc[edge] & 0b00010000) { // current extension
                NbrCurExt[edge] = EdgIn;
                EdgInCase[edge] = 4;
                break;
            }
        case 4:
            if (EdgInAloc[edge] & 0b00001000) { // angle command
                NbrCmdAng[edge] = 0xFF00 & ((uint16_t) (EdgIn) << 8);
                EdgInCase[edge] = 5;
                break;
            }
        case 5:
            if (EdgInAloc[edge] & 0b00001000) {
                NbrCmdAng[edge] = NbrCmdAng[edge] | (uint16_t) EdgIn;
                EdgInCase[edge] = 7;
                break;
            }

        case 7: // verify action command
            if (EdgIn == EDG_End) { // check length necessary? 
                Coms_123_ResetIntervals(edge);
                EdgActCnt[edge] = 0;
                Coms_123_ActVerify(edge); // verify with own action commands
                EdgInCase[edge] = 0;
            } else {
                EdgInCase[edge] = 50;
            }
            break;

        case 10: // IDLE MODE **************************************************
            Coms_123_ResetIntervals(edge);
            if (EdgIn == EDG_End) {
                if (!Flg_EdgeSyn[edge] && Flg_IDRcvd[edge]){
                    Flg_EdgeSyn[edge] = true;
                }
                EdgInCase[edge] = 0;
            } else {
                EdgInCase[edge] = 50;
            }
            break;

        case 20: // CONNECTION DETECTED OR ACKNOWLEDGED ************************
            if ((EdgInAloc[edge] & 0b00010000)) { // con detected
                if (EdgIn == EDG_End) {
                    Flg_EdgeCon[edge] = true;
                    Flg_EdgeSyn[edge] = false;
                    Flg_IDCnfd[edge] = false;
                    Flg_IDRcvd[edge] = false;
                    EdgConCnt[edge] = 0;
                    EdgInCase[edge] = 0;
                } else {
                    EdgInCase[edge] = 50;
                }
            } else if ((EdgInAloc[edge] & 0b00001000)) { // con acknowledged
                // acknowledge byte followed by ID
                if (NbrIDCount[edge] < 6) {
                    if ((EdgIn < 48) || ((EdgIn > 57) && (EdgIn < 65)) ||
                            ((EdgIn > 90))) {// Not Ascii (ID)
                        NbrIDCount[edge] = 0;
                        EdgInCase[edge] = 50;
                        break;
                    }
                    NbrIDTmp[NbrIDCount[edge] + 6 * edge] = EdgIn;
                    NbrIDCount[edge]++;
                } else {
                    if (EdgIn == EDG_End) {
                        uint8_t i;
                        for (i = 0 + 6 * edge; i <= 5 + 6 * edge; i++) {
                            NbrID[i] = NbrIDTmp[i];
                        }
                        Flg_IDRcvd[edge] = true; // ID received, tell neighbour
                        Flg_EdgeCon[edge] = true; // make sure flag is set
                        Flg_EdgeSyn[edge] = false; // reset synced
                        EdgConCnt[edge] = 0;
                        if (EdgInAloc[edge] & 0b00000100)
                            Flg_IDCnfd[edge] = true; // ID confirmed
                        EdgInCase[edge] = 0;
                    } else {
                        EdgInCase[edge] = 50;
                    }
                    NbrIDCount[edge] = 0;
                }
            }
            break;

        case 30: // COMMAND ****************************************************
            Coms_123_ResetIntervals(edge);
            if (Coms_CMD_Handle(edge, EdgIn))
                EdgInCase[edge] = 0;
            break;

        case 40: // RELAY ******************************************************
            Coms_123_ResetIntervals(edge);
            if (Coms_REL_Handle(edge, EdgIn))
                EdgInCase[edge] = 0;
            break;

        case 50: // END BYTE NOT RECEIVED **************************************
            if (EdgIn == EDG_End) // wait for next end byte
                EdgInCase[edge] = 0;
            break;

        default: // DEFAULT ****************************************************
            EdgInCase[edge] = 50;
            break;
    }
}

void Coms_123_ResetIntervals(uint8_t edge) {
    EdgIdlCnt[edge] = 0; // reset interval
    EdgConCnt[edge] = 0; // reset interval
}

/* ******************** NEIGHBOUR CONNECTION HANDLE ************************* */
void Coms_123_ConHandle() { // called in tmr5 at 5Hz
    uint8_t edge, byte;
    for (edge = 0; edge < 3; edge++) {
        // check if idle byte received in EDG_IdlIntrvl when synced
        if (((EdgIdlCnt[edge] >= EDG_IdlIntrvl) && Flg_EdgeSyn[edge]) || // lost
                ((EdgConCnt[edge] >= EDG_ConIntrvl) && Flg_EdgeCon[edge])) {
            Coms_123_Disconnected(edge);
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
            Coms_ESP_LED_Set_Blink_Freq(edge, 20);
            Coms_ESP_LED_State(edge, 3);
        } else if (Flg_EdgeCon[edge]) {
            byte = COMS_123_Ackn; // send acknowledge and ID
            Coms_ESP_LED_Set_Blink_Freq(edge, 50);
            Coms_ESP_LED_State(edge, 3);
        } else {
            byte = COMS_123_Conn; // send connect search
            if (!Acts_CPL_IsOpen(edge))
                Coms_ESP_LED_State(edge, 0);
        }
        
        // if module ID not verified, send out conn search
        if(!Flg_ID_check) byte = COMS_123_Conn;

        if (Flg_Uart_Lock[edge] == false) {
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
    uint8_t edge, byte;
    for (edge = 0; edge < 3; edge++) {
        if (Flg_EdgeCon[edge]){
            if (NbrCmdMatch[edge])
                Flg_AllEdgRdy[edge] = true;
            else 
                Flg_AllEdgRdy[edge] = false;
        } else { // not connected or no command received, other edges can go
            Flg_AllEdgRdy[edge] = true;
            NbrEdgesRdy[edge] = true;
        }
    }
    for (edge = 0; edge < 3; edge++) {
        if (Flg_EdgeSyn[edge]) {
            if (Flg_EdgeAct[edge]) {
                if (EdgActCnt[edge] >= EDG_ActIntrvl) {
                    Flg_EdgeAct[edge] = false;
                    NbrCmdMatch[edge] = false;
                } else {
                    EdgActCnt[edge]++;
                }
            }

            byte = 0b01000000;
            if (Flg_EdgeReq_Ext[edge] && FLG_MotLin_Active && MODE_MotLin_Active)
                byte = byte | 0b00010000;
            if (Flg_EdgeReq_Ang[edge] && FLG_MotRot_Active && MODE_MotRot_Active)
                byte = byte | 0b00001000;
            if (Flg_EdgeReq_Cpl[edge] && MODE_Cplngs_Active)
                byte = byte | 0b00000100;
            if (NbrCmdMatch[edge]){ // neighbour matched own cmd
                if (FLG_WaitAllEdges){
                    if (Flg_AllEdgRdy[0] && Flg_AllEdgRdy[1] && Flg_AllEdgRdy[2]) {
                        byte = byte | 0b00000001; // my edges are ready, wait
                        if (NbrEdgesRdy[0] && NbrEdgesRdy[1] && NbrEdgesRdy[2])
                            byte = byte | 0b00000010; // my nbrs are ready, go
                    }
                } else {
                    byte = byte | 0b00000011;
                }
            }

            if (!Flg_Uart_Lock[edge]){ // request check removed for nbr sync
                Flg_Uart_Lock[edge] = true;
                Coms_123_Write(edge, byte);
                if (Flg_EdgeReq_Ext[edge]) {
                    Coms_123_Write(edge, Acts_LIN_GetTarget(edge));
                    Coms_123_Write(edge, Acts_LIN_GetCurrent(edge));
                }
                if (Flg_EdgeReq_Ang[edge]) {
                    uint16_t AngTemp = Acts_ROT_GetTarget(edge);
                    Coms_123_Write(edge, (uint8_t) ((AngTemp & 0xFF00) >> 8));
                    Coms_123_Write(edge, (uint8_t) (AngTemp & 0x00FF));
                }
                Coms_123_Write(edge, EDG_End);
                Flg_Uart_Lock[edge] = false;
            }
        } else {
            Flg_EdgeAct[edge] = false;
        }
    }
}

/* ******************** NEIGHBOUR ACTION VERIFY ***************************** */
void Coms_123_ActVerify(uint8_t edge) {
    bool NbrCmdNoGo = false;

    // extensions command verification
    if ((EdgInAloc[edge] & 0b00010000) && (Flg_EdgeReq_Ext[edge])) {
        if (NbrCmdExt[edge] != Acts_LIN_GetTarget(edge))
            NbrCmdNoGo = true; // values do not match, NOGO
        // check if neighbour current extension is in range of own
        if ((NbrCurExt[edge] < Acts_LIN_GetCurrent(edge) - EDG_ExtCurRng) ||
                (NbrCurExt[edge] > Acts_LIN_GetCurrent(edge) + EDG_ExtCurRng))
            NbrCmdNoGo = true; // values not in range, NOGO
    } else if (((EdgInAloc[edge] & 0b00010000) == 0) && (!Flg_EdgeReq_Ext[edge])) {
        // ok, no commands from either side
    } else {
        NbrCmdNoGo = true; // mismatch, NOGO
    }

    // angle command verification
    if ((EdgInAloc[edge] & 0b00001000) && (Flg_EdgeReq_Ang[edge])) {
        if (NbrCmdAng[edge] != Acts_ROT_GetTarget(edge))
            NbrCmdNoGo = true; // values do not match, NOGO
    } else if (((EdgInAloc[edge] & 0b00001000) == 0) && (!Flg_EdgeReq_Ang[edge])) {
        // ok, no commands from either side
    } else {
        NbrCmdNoGo = true; // mismatch, NOGO
    }

    // coupling command verification
    if ((EdgInAloc[edge] & 0b00000100) && (Flg_EdgeReq_Cpl[edge])) {
        ; // open coupling? XXX
    } else if (((EdgInAloc[edge] & 0b00000100) == 0) && (!Flg_EdgeReq_Cpl[edge])) {
        // ok, no commands from either side
    } else {
        NbrCmdNoGo = true; // mismatch, NOGO
    }

    // everything is ok // and something received
    if (!NbrCmdNoGo){ // && (EdgInAloc[edge] & 0b00011111)) {
        NbrCmdMatch[edge] = true;
        if ((EdgInAloc[edge] & 0b00000011) == 3) { // good to go
            Flg_EdgeAct[edge] = true;
            NbrEdgesRdy[edge] = true;
        } else {
            Flg_EdgeAct[edge] = false;
            if ((EdgInAloc[edge] & 0b00000011) == 1) // nbr ready
                NbrEdgesRdy[edge] = true;
            else
                NbrEdgesRdy[edge] = false;
        }
    } else {
        NbrEdgesRdy[edge] = false;
        NbrCmdMatch[edge] = false;
        Flg_EdgeAct[edge] = false;
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
    uint8_t i;
    for (i = 0; i < 6; i++) { // loop through 6 ID bytes
        Coms_123_Write(edge, (uint8_t) Coms_ESP_ReturnID(i)); // write to edge
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
        default:
            byte = 0;
            break;
    }
    return byte;
}

bool Coms_123_Ready(uint8_t edge) {
    switch (edge) {
        case 0:
            return UART1_IsRxReady();
        case 1:
            return UART2_IsRxReady();
        case 2:
            return UART3_IsRxReady();
    }
    return false;
}

void Coms_123_purge_uart(uint8_t edge) {
    while(Coms_123_Ready(edge)) {
        Coms_123_Read(edge);
    }
}


void Coms_123_Disconnected(uint8_t edge) {
    Flg_EdgeCon[edge] = false;
    Flg_EdgeSyn[edge] = false;
    Flg_EdgeAct[edge] = false;
    Flg_IDCnfd[edge] = false;
    Flg_IDRcvd[edge] = false;

    // reset requests so it doesnt start moving because it's no longer connected
    Flg_EdgeReq_Ext[edge] = false;
    Flg_EdgeReq_Ang[edge] = false;
    Flg_EdgeReq_Cpl[edge] = false;
}

uint8_t * Coms_123_GetNeighbour(uint8_t edge) {
    return NbrID;
}