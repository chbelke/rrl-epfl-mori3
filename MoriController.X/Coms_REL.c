#include "Coms_123.h"
#include "Coms_REL.h"
#include "Defs.h"
#include "mcc_generated_files/uart4.h"

uint8_t RelSwitch[4] = {0}; // switch case variable
uint8_t RelBytCnt[4] = {0}; // incoming byte counter
uint8_t RelBytExp[4] = {0}; // expected number of bytes
uint8_t RelOutEdg[4] = {0}; // outgoing edge(s)
uint8_t RelBytDta[4][100];// = {0}; // array to store relay data

/* ******************** RELAY HANDLER *************************************** */
bool Coms_REL_Handle (uint8_t inEdge, uint8_t byte){
    bool out = false;
    switch (RelSwitch[inEdge]){
        case 0:
            RelOutEdg[inEdge] = byte;
            RelBytCnt[inEdge] = 1;
            RelSwitch[inEdge] = 1;
            break;
        case 1:
            RelBytExp[inEdge] = byte;
            RelBytCnt[inEdge]++;
            RelSwitch[inEdge]++;
            break;
        case 2:
            RelBytDta[inEdge][RelBytCnt[inEdge]-2] = byte;
            RelBytCnt[inEdge]++;
            if (RelBytCnt[inEdge] >= RelBytExp[inEdge]){
                out = true;
                RelSwitch[inEdge] = 0;
                if (byte == EDG_End)
                    Coms_REL_Relay(inEdge, RelOutEdg[inEdge]);
            }
            break;
        default:
            RelSwitch[inEdge] = 0;
            break;
    }
    return out;
}

/* ******************** ACTUAL RELAY **************************************** */
void Coms_REL_Relay(uint8_t inEdge, uint8_t outEdge){
    uint8_t edge, count;
    if (outEdge == 5){ // relay to all
        for (edge = 0; edge < 5; edge++){
            if (edge != outEdge){ // ignore self
                Coms_Write(edge, RelBytDta[inEdge][0]); // write next aloc
                Coms_Write(edge, RelBytExp[inEdge]-1); // write length -1
                for (count = 1; count < RelBytExp[inEdge]; count++){
                    Coms_Write(edge, RelBytDta[inEdge][count]); //data
                }
            }
        }
    } else {
        Coms_Write(outEdge, RelBytDta[inEdge][0]); // write next aloc
        Coms_Write(outEdge, RelBytExp[inEdge]-1); // write length -1
        for (count = 1; count < RelBytExp[inEdge]; count++){
            Coms_Write(outEdge, RelBytDta[inEdge][count]); //data
        }
    }
}

/* ******************** GENERIC UART WRITE ********************************** */
void Coms_Write(uint8_t edge, uint8_t byte){
    if (edge < 4){
        Coms_123_Write(edge, byte);
    } else if (edge == 4){
        UART4_Write(byte);
    }
}