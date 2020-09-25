#include "Coms_123.h"
#include "Coms_ESP.h"
#include "Coms_CMD.h"
#include "Defs.h"
#include "mcc_generated_files/uart4.h"
#include "Coms_REL.h"

uint8_t RelSwitch[4] = {0}; // switch case variable
uint8_t RelBytCnt[4] = {0}; // incoming byte counter
uint8_t RelBytExp[4] = {0}; // expected number of bytes
uint8_t RelOutEdg[4] = {0}; // outgoing edge(s)
uint8_t RelBytDta[4][255]; // = {0}; // array to store relay data
uint8_t TmpByteBuffer[100];
uint8_t alloc[4] = {0, 0, 0, 0};
uint8_t WIFI_EDGE = 255;

/* ******************** RELAY HANDLER *************************************** */
bool Coms_REL_Handle(uint8_t inEdge, uint8_t byte) {
    bool out = false;
    switch (RelSwitch[inEdge]) {
        case 0:
            if(byte == EDG_End)
                break;
            alloc[inEdge] = byte;
            RelOutEdg[inEdge] = (alloc[inEdge] & 0b00000111);
            RelBytCnt[inEdge] = 1;
            RelSwitch[inEdge] = 1;
            break;
        case 1:
            RelBytExp[inEdge] = byte;
            RelBytCnt[inEdge]++;
            RelSwitch[inEdge]++;
            break;
        case 2:
            RelBytDta[inEdge][RelBytCnt[inEdge] - 2] = byte;
            RelBytCnt[inEdge]++;
            if (RelBytCnt[inEdge] >= RelBytExp[inEdge]) {
                RelSwitch[inEdge] = 0;
                if (byte == EDG_End) {
                    out = true;
                    Coms_REL_Relay(inEdge, RelOutEdg[inEdge]);
                } else {
//                    Coms_ESP_Verbose_Write("hello");
                    RelSwitch[inEdge] = 50;
                }
            }
            break;
            
        case 50: // END BYTE NOT RECEIVED **************************************
            if (byte == EDG_End) // wait for next end byte
                RelSwitch[inEdge] = 0;
                out = true;
            break;    
            
        default:
            RelSwitch[inEdge] = 0;
            break;
    }
    return out;
}

/* ******************** ACTUAL RELAY **************************************** */
void Coms_REL_Relay(uint8_t inEdge, uint8_t outEdge) {
    uint8_t edge;
    if (outEdge == 5) { // relay to all
        for (edge = 0; edge < 3; edge++) {
            if (edge != inEdge) { // ignore self
                Coms_REL_ToEdge(edge, inEdge);
            }
        }
    } else if (outEdge == 6) { //relay to WiFi hub
        if (WIFI_EDGE <= 3) {
            Coms_REL_ToHub(WIFI_EDGE, inEdge);
        } else {
            Coms_ESP_Request_WiFiEdge();
        }
    } else {
        Coms_REL_ToEdge(outEdge, inEdge);
    }
    if ((alloc[inEdge] & 0b00011000) == 0b00010000) {
        Coms_REL_Interpret(inEdge);
    }
}

/* ******************** RELAY TO NEIGHBOUR ********************************** */
void Coms_REL_ToEdge(uint8_t edge, uint8_t inEdge) {
    //    while(Flg_Uart_Lock[edge])   //wait for uart to unlock
    //    {
    //    }
    Flg_Uart_Lock[edge] = true; //locks s.t. the sequence is uninterrupted

    if (((RelBytDta[inEdge][0] >> 5) & 0x07) == 7) {
        Coms_REL_RelayStandard(edge, inEdge);
    } else { //If last byte is not a command
        Coms_REL_RelayCommand(edge, inEdge);
    }

    Flg_Uart_Lock[edge] = false;
}

/* ******************** RELAY TO WIFI HUB *********************************** */
void Coms_REL_ToHub(uint8_t edge, uint8_t inEdge) {
    Coms_REL_Write(edge, 0b11100110); // Necessary (Relay + wifi edge))
    Coms_REL_Write(edge, RelBytExp[inEdge]); // write length -1
    uint8_t count;
    for (count = 0; count < RelBytExp[inEdge] - 2; count++) {
        Coms_REL_Write(edge, RelBytDta[inEdge][count]); //data
    }
}

/* ******************** GENERIC RELAY *************************************** */
void Coms_REL_RelayStandard(uint8_t edge, uint8_t inEdge) {
    Coms_REL_Write(edge, RelBytDta[inEdge][0]); // write next aloc
    Coms_REL_Write(edge, RelBytExp[inEdge] - 1); // write length -1
    uint8_t count;
    for (count = 1; count < RelBytExp[inEdge] - 2; count++) {
        Coms_REL_Write(edge, RelBytDta[inEdge][count]); //data
    }
}

/* ******************** RELAY COMMAND FOR NEIGHBOUR ************************* */
void Coms_REL_RelayCommand(uint8_t edge, uint8_t inEdge) {
    if (edge == 3) {    //An alloc to let the ESP know to interpret the command
        Coms_REL_Write(edge, 0b00100000);
        Coms_REL_Write(edge, RelBytExp[inEdge]-4);  
    }
    uint8_t count; //count minus 3: relay + len + rel end
    for (count = 0; count < RelBytExp[inEdge] - 3; count++) {
        Coms_REL_Write(edge, RelBytDta[inEdge][count]); //data
    }
}

/* ******************** SET WIFI EDGE *************************************** */
void Coms_REL_SetWiFiEdge(uint8_t edge) {
    WIFI_EDGE = edge;
}

/* ******************** RETURN WIFI EDGE ************************************ */
uint8_t Coms_REL_GetWiFiEdge() {
    return WIFI_EDGE;
}

/* ******************** INTERPRET RELAYED COMMAND *************************** */
void Coms_REL_Interpret(uint8_t inEdge) {
    uint8_t count = 0;
    uint8_t end_len = 3;
    if (((RelBytDta[inEdge][count] >> 5) & 0x07) == 7) {
        count = 1; //If data is relayed, skip first length
        end_len = 2;
    }
    while (((RelBytDta[inEdge][count] >> 5) & 0x07) == 7) {
        count++;
    }

    for (count = count; count < RelBytExp[inEdge] - end_len; count++) {
        Coms_CMD_Handle(inEdge, RelBytDta[inEdge][count]); //mysterious 5th edge
    }
}

/* ******************** GENERIC UART WRITE ********************************** */
void Coms_REL_Write(uint8_t edge, uint8_t byte) {
    if (edge < 3) {
        Coms_123_Write(edge, byte);
    } else if (edge == 3) {
        UART4_Write(byte);
    }
}