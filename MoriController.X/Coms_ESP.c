#include "Defs_GLB.h"
#include "Defs_MOD.h"
#include "Acts_LIN.h"
#include "Acts_ROT.h"
#include "Acts_CPL.h"
#include "Mnge_PWM.h"
#include "Mnge_RGB.h"
#include "Mnge_ERR.h"
#include "math.h"
#include "dsp.h"
#include "mcc_generated_files/uart4.h"
#include "mcc_generated_files/adc1.h"
#include "Coms_CMD.h"
#include "Coms_REL.h"
#include "Coms_123.h"
#include <string.h>
#include "Coms_ESP.h"
#include "Sens_ENC.h"

uint8_t EspInCase = 0; // switch case variable

int8_t DrivePWM[3] = {0, 0, 0}; // manual drive mode PWM values by edge
uint8_t DriveSpd, DriveCrv = 0; // automatic drive mode speed and curve

uint8_t RgbPWM[3] = {0, 0, 0}; // rgb led values

const uint8_t ESP_IDexpected[6] = {ID1, ID2, ID3, ID4, ID5, ID6};
bool Flg_Booted_Up = false;

uint8_t WIFI_LED_STATE[3] = {0, 0, 0};
uint8_t WIFI_LED_BLINK_DES[3] = {0, 0, 0};
uint8_t Coms_ESP_Data_Transmission = 0;

uint8_t ESP_Update_Delay_Angle = 100;
uint8_t ESP_Update_Delay_Edge = 100;
uint8_t ESP_Update_Delay_Orient = 100;


/* This function evaluates the incoming bytes from the ESP module via UART4. 
 * It gets called from the ISR each time a byte is received. It evaluates the
 * allocation byte, steps through the appropriate cases depending on the
 * allocation byte, and verifies the validity of the incoming date through 
 * the control value ESP_End and the total number of bytes received. If the 
 * incoming data is valid, it triggers the appropriate function to implement
 * the commands received.
 * Fall-through is utilised to allocate the commands correctly according to
 * EspInAloc.
 */

/* ******************** ESP COMMAND EVALUATION ****************************** */
void Coms_ESP_Eval() { // called in main
    if (!UART4_IsRxReady()) return; // check if byte received    
    
    if(!Flg_Booted_Up){
        Coms_ESP_Boot();
        return;
    }
       
    uint8_t EspIn = 50;
    if(EspInCase != 7)
        EspIn = UART4_Read(); // Incoming byte
//    const char *message = "hello";
    switch (EspInCase) { // select case set by previous byte
        case 0: // INPUT ALLOCATION ********************************************
            switch ((EspIn >> 5) & 0x07) {
                case 0: // xxx == 000, emergency stop
                    if ((EspIn & 0x1F) == 31)
                        EspInCase = 1;
                    break;
//                case 2: // xxx == 010, action sync received
//                    EdgInCase[edge] = 2;
//                    break;
//                case 3: // xxx == 011, idle mode
//                    EdgInCase[edge] = 10;
//                    break;
                case 5: // xxx == 101, ESP only communication
                    Coms_ESP_Handle(EspIn & 0b00011111);
                    EspInCase = 5;
                    break;
                case 6: // xxx == 110, command
                    Coms_CMD_Handle(ESP_URT_NUM, EspIn & 0b00011111);
                    EspInCase = 6;
                    break;
//                case 7: // xxx == 111, relay
//                    if(Coms_REL_Handle(ESP_URT_NUM, EspIn & 0b00011111)){
//                        EspInCase = 0;
//                    } else {
//                        EspInCase = 7;
//                    }
//                    break;
                    
                default:
                    EspInCase = 50;
                    break;
            }
            break;   

        case 1:
            if (EspIn == ESP_End)
                Mnge_ERR_ActivateStop(0, ERR_ESPToldMe);
            EspInCase = 0;
            break;
            
        case 5:
            if (Coms_ESP_Handle(EspIn))
                EspInCase = 0;
            break;
            
        case 6:
            if (Coms_CMD_Handle(ESP_URT_NUM, EspIn))
                EspInCase = 0;
            break;
            
        case 7:
            if (Coms_REL_Handle(ESP_URT_NUM, EspIn))
                EspInCase = 0;
            break;
            
        case 50: // END BYTE NOT RECEIVED **************************************
            if ((EspIn == EDG_End) || (EspIn == ESP_End)) // wait for next end byte
                Coms_ESP_Verbose_Write("ESP_OVL");
                EspInCase = 0;
            break;            

        default:
            EspInCase = 50;
            break;
    }
            
}

bool Coms_ESP_Handle(uint8_t byte) {
    static bool alloc = true;
    static uint8_t state;

    if (alloc) {
        state = (byte & 0b00011111);
        alloc = false;
        return false;
    }
    
    //Note: can use Coms_CMD_Reset; using pointers to local variables
    switch (state) {
        case 0:
            if (Coms_ESP_SendErrorCode(byte))
                return Coms_CMD_Reset(&state, &alloc); 
            break;
        case 1:
            if (Coms_ESP_SetDatalogFlags(byte))
                return Coms_CMD_Reset(&state, &alloc); 
            break;
        case 2:
            if (Coms_ESP_SetDatalogPeriod(byte))
                return Coms_CMD_Reset(&state, &alloc); 
            break;
        case 3:
            if (Coms_ESP_SetClientLetter(byte))
                return Coms_CMD_Reset(&state, &alloc); 
            break;            
            
        default:
            break;
                   
    }
    return false;
}


void Coms_ESP_OverflowError(void)
{
    EspInCase = 50;
}

void Coms_ESP_Boot(void)
{
    if(UART4_Read() == ESP_End){ //Purges all in queue until first end byte
        Flg_Booted_Up = true;
    } 
}


/* ******************** RETURN ID BY BYTE *********************************** */
uint8_t Coms_ESP_ReturnID(uint8_t byteNum) {
    return ESP_IDexpected[byteNum];
}


/* ******************** VERBOSE OUTPUT ************************************** */
void Coms_ESP_Verbose() 
{
    UART4_Write(0);
    UART4_Write(6); // Message length
    uint8_t i;
    for (i = 0; i < 3; i++) {
        UART4_Write(Acts_LIN_GetCurrent(i));
    }
    UART4_Write(ESP_End);
}


/* ******************** VERBOSE OUTPUT ************************************** */
void Coms_ESP_Verbose_Write(const char *message) 
{    
    UART4_Write(0);
    uint8_t len = strlen(message);
    UART4_Write(len); // Message length
    
    uint8_t i;
    for(i=0; i<len; i++)
    {
        UART4_Write(*message);
        message++;
    }
    UART4_Write(ESP_End);
}

/* ******************** VERBOSE OUTPUT ************************************** */
void Coms_ESP_Verbose_One_Byte(uint8_t byte) 
{
    UART4_Write(0);
    UART4_Write(1); // Message length
    UART4_Write(byte);
    UART4_Write(ESP_End);
}


void Coms_ESP_SendStable(bool flg_stable_state) 
{
    uint8_t alloc = 0b10001100; //case 12 - 13 on ESP
    alloc |= flg_stable_state;
    UART4_Write(alloc);
    UART4_Write(ESP_End);
}

void Coms_ESP_TurnOnWifi(void) {
    UART4_Write(0b10011111);  // 100 = states, 11111 = WiFi On
    UART4_Write(ESP_End);
}    


void Coms_ESP_LED_State(uint8_t edge, uint8_t state)
{
    static uint8_t interval[3] = {0, 7, 14}; // only update if state change or 3s passed
    interval[edge]++;           // 0 7 and 14 to avoid sending all edges at the same time
    if (((WIFI_LED_STATE[edge] - state) == 0) && (interval[edge] <= 21))
        return;
    interval[edge] = 0;
    
    if (!MODE_LED_PARTY){
        switch(state){
            case 0: //off
                Coms_ESP_LED_On(edge, WIFI_LED_OFF);
                Coms_ESP_Neighbour_Disconnected(edge);
                break;

            case 1: //on
                Coms_ESP_LED_On(edge, WIFI_LED_ON);
                Coms_ESP_Request_Neighbour(edge);
                break;

            case 2: //toggle
                Coms_ESP_LED_Tgl(edge);
                break;

            case 3: //blink
                Coms_ESP_LED_Blk(edge, WIFI_LED_BLINK_DES[edge]);
                break;
            default:
                break;
        }
    }
    WIFI_LED_STATE[edge] = state;
}         



/* ******************** SET ESP EDGE LEDS *********************************** */
void Coms_ESP_LED_On(uint8_t edge, bool OnOff) {
    uint8_t alloc = 0b01000001 + OnOff;   // Cmd, ---, blink
    edge++;
    alloc |= (edge << 3) & 0b00011000;
    UART4_Write(alloc);  // LED R, Set Blink Freq
    UART4_Write(ESP_End);
}


void Coms_ESP_LED_Tgl(uint8_t edge) {
    uint8_t alloc = 0b01000000;   // Cmd, ---, blink
    edge++;
    alloc |= (edge << 3) & 0b00011000;
    UART4_Write(alloc);  // LED R, Set Blink Freq
    UART4_Write(ESP_End);
}


void Coms_ESP_LED_Blk(uint8_t edge, uint8_t blink) { // blink*4 = period;
    uint8_t alloc = 0b01000100;   // Cmd, ---, blink
    edge++;
    alloc |= (edge << 3) & 0b00011000;
    UART4_Write(alloc);  // LED R, Set Blink Freq
    UART4_Write(blink);
    UART4_Write(ESP_End);
}

void Coms_ESP_LED_Set_Blink_Freq(uint8_t edge, uint8_t blink)
{
    WIFI_LED_BLINK_DES[edge] = blink;
}


void Coms_ESP_Interpret() {
    static uint8_t ESP_bnk_frq = 128;
    UART4_Write(0b01001100);  // LED R, Set Blink Freq
    UART4_Write(ESP_bnk_frq);
    UART4_Write(ESP_End);
    ESP_bnk_frq++;
}

/* ******************** Wifi Edges *********************************** */
void Coms_ESP_No_WiFi_Edge() {
    UART4_Write(0b10011001);  // 100 = states, 00000 = WiFi Edge
    UART4_Write(ESP_End);
}


void Coms_ESP_Return_WiFi_Edge(uint8_t edge) {
    UART4_Write(0b10011000);  // 100 = states, 11000 = Set WiFi Edge
    UART4_Write(edge);
    UART4_Write(ESP_End);
}


/* ******************** Requests ******************** */
void Coms_ESP_Request_Edges() {
    UART4_Write(0b10010100);
    Coms_ESP_Write_Edges();
    UART4_Write(ESP_End);
}

void Coms_ESP_Write_Edges() {
    uint8_t i;
    for (i = 0; i < 3; i++) {
        UART4_Write(Acts_LIN_GetCurrent(i));
    }
}

void Coms_ESP_Request_Angles() {
    UART4_Write(0b10010101);
    Coms_ESP_Write_Angles();
    UART4_Write(ESP_End);
}

//writes just angle data, as opposed to start bytes
void Coms_ESP_Write_Angles() {
    uint8_t i;
    for (i = 0; i < 3; i++) {
        UART4_Write16(Acts_ROT_GetAngle(i, true));
    }
}


void Coms_ESP_Request_Orient() {
    UART4_Write(0b10010110);
    Coms_ESP_Write_Orient();
    UART4_Write(ESP_End);
}


void Coms_ESP_Write_Orient() {
    uint8_t i;
    for (i = 0; i < 3; i++) {
        UART4_Write16(Sens_ACC_GetAngle(i));
    }
}


void Coms_ESP_SendSerialOverflow(uint8_t edge) {
    UART4_Write(0b10000001);
    UART4_Write(edge);
    UART4_Write(ESP_End);
}

void Coms_ESP_Request_Neighbour(uint8_t edge)
{
    uint8_t *neighbour;
    uint8_t i;
    neighbour = Coms_123_GetNeighbourIDs();
    
    if(*(neighbour+edge*7)==0)
    {
        Coms_ESP_Neighbour_Disconnected(edge);
        return;
    }
    
    UART4_Write(0b10010111);
    UART4_Write(edge);
    for(i=edge*7; i<6+edge*7; i++)
        UART4_Write(*(neighbour+i));
    UART4_Write(ESP_End);
}


void Coms_ESP_Request_WiFiEdge()
{
    UART4_Write(0b10011000);
    UART4_Write(Coms_REL_GetWiFiEdge());
    UART4_Write(ESP_End);
}

void Coms_ESP_Neighbour_Disconnected(uint8_t edge)
{
    UART4_Write(0b10010010);
    UART4_Write(edge);
    UART4_Write(ESP_End);
}


void Coms_ESP_Request_ID()
{
    UART4_Write(0b10010011);
    UART4_Write(ESP_End);
}


bool Coms_ESP_VerifyID() {
    bool ID_Ok = true;
    uint8_t i;
    for (i = 0; i < 6; i++){
        if ((uint8_t)ESP_ID[i] == (uint8_t)ESP_IDexpected[i]);
        else ID_Ok = false;
    }
    return ID_Ok;
}


void Coms_ESP_StateUpdate(void) {
    // Only updates every N ms
    static uint8_t ESP_DataLog_Time_Angle;
    static uint8_t ESP_DataLog_Time_Edge;
    static uint8_t ESP_DataLog_Time_Orient;
    
    //Updates elapsed time only if we are logging the field
    //Otherwise time never updates and never transmits
    if (Coms_ESP_Data_Transmission & 0b00000001)
        ESP_DataLog_Time_Angle += ESP_DataLog_Time_Elapsed;
    if (Coms_ESP_Data_Transmission & 0b00000010)
        ESP_DataLog_Time_Edge += ESP_DataLog_Time_Elapsed;
    if (Coms_ESP_Data_Transmission & 0b00000100)
        ESP_DataLog_Time_Orient += ESP_DataLog_Time_Elapsed;
    
    ESP_DataLog_Time_Elapsed = 0;
    
    //Checks if time has surpassed the time we set
    uint8_t ESP_Update_Reset_Trigger = 0;    
    if (ESP_DataLog_Time_Angle >= ESP_Update_Delay_Angle) {
        ESP_Update_Reset_Trigger |= 0b00000001;
        ESP_DataLog_Time_Angle %= ESP_Update_Delay_Angle;
//        ESP_DataLog_Time_Angle = 0;
    }
    if (ESP_DataLog_Time_Edge >= ESP_Update_Delay_Edge) {
        ESP_Update_Reset_Trigger |= 0b00000010;
        ESP_DataLog_Time_Edge %= ESP_Update_Delay_Edge;
//        ESP_DataLog_Time_Edge = 0;
    }
    if (ESP_DataLog_Time_Orient >= ESP_Update_Delay_Orient) {
        ESP_Update_Reset_Trigger |= 0b00000100;
        ESP_DataLog_Time_Orient %= ESP_Update_Delay_Orient;
//        ESP_DataLog_Time_Orient = 0;
    }

    // If no fields need update
    if ((ESP_Update_Reset_Trigger & 0b00000111) == 0) return;
    
    UART4_Write((0b11000000 | (ESP_Update_Reset_Trigger & 0b00000111))); //New alloc byte
    
    if (ESP_Update_Reset_Trigger & 0b00000001)
        Coms_ESP_Write_Angles();
        
    if (ESP_Update_Reset_Trigger & 0b00000010)
        Coms_ESP_Write_Edges();

    if (ESP_Update_Reset_Trigger & 0b00000100)
        Coms_ESP_Write_Orient();    
    
    UART4_Write(ESP_End);
}


bool Coms_ESP_SetDatalogFlags(uint8_t byte) {
    static uint8_t count = 0;
    static uint8_t flags;
    if (count >= 1) {
        if (byte == ESP_End) {
            Coms_ESP_Data_Transmission = flags;
        } else {
            Coms_ESP_OverflowError();
        }
        count = 0;
        return true;
    } else {
        flags = byte;
        count++;
    }
    return false;    
}

bool Coms_ESP_SetDatalogPeriod(uint8_t byte) {
    static uint8_t count = 0;
    static uint8_t period;
    static uint8_t requested_data;
    if (count >= 2) {
        if (byte == ESP_End) {
            if (requested_data & 0b00000001) {
                ESP_Update_Delay_Angle = period;
            }
            else if (requested_data & 0b00000010) {
                ESP_Update_Delay_Edge = period;
            }
            else if (requested_data & 0b00000100) {
                ESP_Update_Delay_Orient = period;
            }
            else {
                ESP_Update_Delay_Angle = period;
                ESP_Update_Delay_Edge = period;
                ESP_Update_Delay_Orient = period;                
            }
        } else {
            Coms_ESP_OverflowError();
        }
        count = 0;
        requested_data = 0;
        return true;
    }
    else if (count == 0) {
        requested_data = byte;
    }
    else {
        period = byte;
    }
    count++;
    return false;    
}


bool Coms_ESP_SetClientLetter(uint8_t byte) {
    if (byte == ESP_End) {
        UART4_Write(0b10010001);
        UART4_Write(MODULE);
        UART4_Write(ESP_End);        
    } else {
        Coms_ESP_OverflowError();
    }
    return true;
}

bool Coms_ESP_SendErrorCode(uint8_t byte){
    if (byte == ESP_End) {
        UART4_Write(0b10000000);
        UART4_Write(Mnge_ERR_GetErrorCode());
        UART4_Write(Mnge_ERR_GetErrorEdge());
        UART4_Write(ESP_End);
    } else {
        Coms_ESP_OverflowError();
    }
    return true;    
}