#include "Coms_123.h"
#include "Coms_ESP.h"
#include "Coms_CMD.h"
#include "Defs_GLB.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/uart2.h"
#include "mcc_generated_files/uart3.h"
#include "mcc_generated_files/uart4.h"
#include "Coms_REL.h"

uint8_t RelBytExp[4] = {0}; // expected number of bytes
uint8_t RelOutEdg[4] = {0}; // outgoing edge(s)
uint8_t RelBytDta[4][255]; // = {0}; // array to store relay data
uint8_t alloc[4] = {0, 0, 0, 0};
uint8_t WIFI_EDGE = 255;
uint8_t tmp_count = 0;

/* ******************** RELAY HANDLER *************************************** */
bool Coms_REL_Handle(uint8_t inEdge, uint8_t byte) {
    static uint8_t RelSwitch[4] = {0}; // switch case variable
    static uint8_t RelOutEdg[4] = {0}; // outgoing edge(s)
    switch (RelSwitch[inEdge]) {
        case 0:
            if(byte == EDG_End){
                RelSwitch[inEdge] = 50;
                return false;
            }
            alloc[inEdge] = byte;
            RelOutEdg[inEdge] = (alloc[inEdge] & 0b00000111);
            RelSwitch[inEdge] = 1;
        case 1:
            if(!Coms_REL_Ready(inEdge)) return false; // check if byte receive
            RelBytExp[inEdge] = Coms_REL_Read(inEdge);
            RelSwitch[inEdge]= 2;
        case 2:
            if(!Coms_REL_Ready(inEdge)) return false;
            if ((uint8_t)(UART_BUFF_SIZE-Coms_REL_getRecieveBufferSize(inEdge))
                    < (RelBytExp[inEdge]-2))
                return false;
            if(!Coms_REL_Ready(inEdge)) return false;
            if (Coms_REL_Peek_Buffer(inEdge, (RelBytExp[inEdge]-3)) == EDG_End) {
//                if(!Coms_REL_Ready(inEdge)) return false;
                Coms_REL_Relay(inEdge, RelOutEdg[inEdge]);
                RelSwitch[inEdge] = 0;
                return true;
            } else {
                RelSwitch[inEdge] = 50;
            }
            
        case 50: // END BYTE NOT RECEIVED **************************************
            byte = Coms_REL_Read(inEdge);
            if (byte == EDG_End){ // wait for next end byte
                RelSwitch[inEdge] = 0;
                return true;
            }
            break;    
            
        default:
            RelSwitch[inEdge] = 50;
            break;
    }
    return false;
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
    if (((Coms_REL_Peek_Buffer(inEdge, 0) >> 5) & 0x07) == 7) {
//        Coms_ESP_Verbose_One_Byte(0x0F);
        Coms_REL_RelayStandard(edge, inEdge);       
    } else { //If last byte is not a command
        Coms_REL_RelayCommand(edge, inEdge);
    }
}

/* ******************** RELAY TO WIFI HUB *********************************** */
void Coms_REL_ToHub(uint8_t edge, uint8_t inEdge) {
    Coms_REL_Write(edge, 0b11100110); // Necessary (Relay + wifi edge))
    Coms_REL_Write(edge, RelBytExp[inEdge]); // write length
    uint8_t count;
    for (count = 0; count < RelBytExp[inEdge] - 2; count++) {
        Coms_REL_Write(edge, Coms_REL_Read(inEdge)); //data
    }
}

/* ******************** GENERIC RELAY *************************************** */
void Coms_REL_RelayStandard(uint8_t edge, uint8_t inEdge) {
    Coms_REL_Write(edge, Coms_REL_Read(inEdge)); // write next aloc
    Coms_REL_Write(edge, (RelBytExp[inEdge] - 1)); // write length -1
//    Coms_REL_Write(edge, RelBytExp[inEdge] - 1); // write length -1
    uint8_t i;
    for (i=1; i < RelBytExp[inEdge] - 2; i++) {
        Coms_REL_Write(edge, Coms_REL_Read(inEdge)); //data
    }
//    Coms_REL_Read(inEdge);
}

/* ******************** RELAY COMMAND FOR NEIGHBOUR ************************* */
void Coms_REL_RelayCommand(uint8_t edge, uint8_t inEdge) {
    if (edge == 3) {    //An alloc to let the ESP know to interpret the command
        Coms_REL_Write(edge, 0b00100000);
        Coms_REL_Write(edge, RelBytExp[inEdge]-4);  
    }
    uint8_t i; //count minus 3: relay + len + rel end
    for (i=0; i < (RelBytExp[inEdge] - 3); i++) {
        Coms_REL_Write(edge, Coms_REL_Read(inEdge)); //data
    }
    Coms_REL_Read(inEdge); //discard last byte
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
    if (((Coms_REL_Peek_Buffer(inEdge,count) >> 5) & 0x07) == 7) {
        count = 1; //If data is relayed, skip first length
        end_len = 2;
    }
    while (((Coms_REL_Peek_Buffer(inEdge,count) >> 5) & 0x07) == 7) {
        count++;
    }

    for (count = count; count < RelBytExp[inEdge] - end_len; count++) {
        Coms_CMD_Handle(inEdge, Coms_REL_Peek_Buffer(inEdge,count)); //mysterious 5th edge
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

uint16_t Coms_REL_getRecieveBufferSize(uint8_t edge){
    switch(edge) {
        case 0:
            return UART1_ReceiveBufferSizeGet();
        case 1:
            return UART2_ReceiveBufferSizeGet();
        case 2:
            return UART3_ReceiveBufferSizeGet();
        case 3:
            return UART4_ReceiveBufferSizeGet();            
    }
    return 0xFFFF;
}


uint8_t Coms_REL_Peek_Buffer(uint8_t edge, uint8_t offset) {
    switch(edge) {
        case 0:
            return UART1_Peek(offset);
        case 1:
            return UART2_Peek(offset);
        case 2:
            return UART3_Peek(offset);
        case 3:
            return UART4_Peek(offset);            
    }    
    return 0;
}

uint8_t Coms_REL_Read(uint8_t edge) {
    switch (edge) {
        case 0:
            return UART1_Read();
        case 1:
            return UART2_Read();
        case 2:
            return UART3_Read();
        case 3:
            return UART4_Read();           
    }
    return 0;
}


bool Coms_REL_Ready(uint8_t edge) {
    switch (edge) {
        case 0:
            return UART1_IsRxReady();
        case 1:
            return  UART2_IsRxReady();
        case 2:
            return UART3_IsRxReady();
        case 3:
            return UART4_IsRxReady();    
    }
    return false;
}


bool Coms_REL_TxReady(uint8_t edge) {
    switch (edge) {
        case 0:
            return UART1_IsTxReady();
        case 1:
            return  UART2_IsTxReady();
        case 2:
            return UART3_IsTxReady();
        case 3:
            return UART4_IsTxReady();    
    }
    return false;
}