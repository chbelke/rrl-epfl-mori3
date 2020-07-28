#include "Coms_123.h"
#include "define.h"
#include "mcc_generated_files/uart1.h"
#include "mcc_generated_files/uart2.h"
#include "mcc_generated_files/uart3.h"

uint8_t EdgInCase[3] = {0,0,0}; // switch case variable
uint8_t EdgInAloc[3] = {0,0,0}; // incoming allocation byte (explanation below)

uint8_t NeighbourID[18] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint8_t NeighbourIDTemp[18] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint8_t NeighbourByteCount[3] = {0,0,0};
uint8_t EdgByteCount[3] = {0,0,0};

uint16_t EdgLngCmd[3] = {0,0,0}; // edge length command received by neighbour


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
 * - 0b100ooooo
 * 111 = Relay mode - Kevin
 * - 0b111xxxxx
 */


/* ******************** EDGE COMMAND EVALUATION ***************************** */
void Coms_123_Eval(uint8_t edge){
    uint8_t EdgIn; // Incoming byte
    uint8_t i;
    switch (edge) {
        case 0:
            EdgIn = UART1_Read();
            break;
        case 1:
            EdgIn = UART2_Read();
            break;
        case 2:
            EdgIn = UART3_Read();
            break;
    }
    
    switch (EdgInCase[edge]) {
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
                case 4: // xxx == 100, connection detected or acknowledged
                    EdgInCase[edge] = 20;
                case 7: // xxx == 111, relay (Kevin)
                    EdgInCase[edge] = 30 + (EdgInAloc[edge] & 0b00011111);
                    break;
            }
            break;
        case 1: // EMERGENCY STOP **********************************************
            if (EdgIn == EDG_End){
                Flg_EdgeSyn[edge] = false;
            }
            break;
        case 2: // ACTION COMMAND RECEIVED *************************************
            if (EdgInAloc[edge] & 0b00010000) { // angle command received
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
            if (EdgInAloc[edge] & 0b00001000) { // length command received
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
            if (EdgInAloc[edge] & 0b00000100) { // coupling command received
                // open coupling ?
                EdgInCase[edge] = 7;
                break;
            }
        case 7: // verify action command
            if (EdgIn == EDG_End){
                // check length and verify with own action commands
            }
            break;
        case 10: // IDLE MODE **************************************************
            if (EdgIn == EDG_End){
                // verified
            }
            break;
        case 20: // CONNECTION ACKNOWLEDGED OR ACKNOWLEDGED ********************
            if (EdgIn == EDG_End){
                Flg_EdgeCon[edge] = true;
            }
            break;
        case 21: 
            if (NeighbourByteCount[edge] < 6) {
                NeighbourIDTemp[NeighbourByteCount[edge] + 6*edge] = EdgIn;
                NeighbourByteCount[edge] = NeighbourByteCount[edge] + 1;
            } else {
                if (EdgIn == EDG_End) {
                    for (i = 0 + 6*edge; i <= 5 + 6*edge; i++){
                        NeighbourID[i] = NeighbourIDTemp[i];
                    }
                }
                NeighbourByteCount[edge] = 0;
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