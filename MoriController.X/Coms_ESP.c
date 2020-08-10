#include "Coms_ESP.h"
#include "Defs.h"
#include "Acts_LIN.h"
#include "Acts_ROT.h"
#include "Acts_CPL.h"
#include "Mnge_PWM.h"
#include "Mnge_RGB.h"
#include "math.h"
#include "dsp.h"
#include "mcc_generated_files/uart4.h"
#include "mcc_generated_files/adc1.h"
#include "Coms_CMD.h"
#include "Coms_REL.h"
#include <string.h>

uint8_t EspInCase = 0; // switch case variable
uint8_t EspInAloc = 0; // incoming allocation byte (explanation below)
uint8_t EspInBits = 0; // bit count in allocation byte
uint8_t EspInByts = 0; // incoming byte count
uint16_t EspInLost = 0; // counting lost byte instances since start
uint16_t EspIn0End = 0; // counting instances of no end byte since start

int8_t DrivePWM[3] = {0, 0, 0}; // manual drive mode PWM values by edge
uint8_t DriveSpd, DriveCrv = 0; // automatic drive mode speed and curve

#define WHEEL 68.15f // wheel distance from vertex
#define SxOUT 0.9 // output speed factor for non-primary wheels
#define ESP_URT_NUM 3

uint8_t RgbPWM[3] = {0, 0, 0}; // rgb led values

uint8_t SelfID[6] = {0, 0, 0, 0, 0, 0};

uint8_t WIFI_LED_STATE[3] = {0, 0, 0};
uint8_t WIFI_LED_BLINK_ACT[3] = {0, 0, 0};
uint8_t WIFI_LED_BLINK_DES[3] = {0, 0, 0};

/* This function evaluates the incoming bytes from the ESP module via UART4. 
 * It gets called from the ISR each time a byte is received. It first checks
 * whether the first byte is the control value ESP_Beg. It then evaluates the
 * allocation byte, steps through the appropriate cases depending on the
 * allocation byte, and verifies the validity of the incoming date through 
 * the control value ESP_End and the total number of bytes received. If the 
 * incoming data is valid, it triggers the appropriate function to implement
 * the commands received.
 * Fall-through is utilised to allocate the commands correctly according to
 * EspInAloc.
 */

/* ******************** ESP COMMAND EVALUATION ****************************** */
void Coms_ESP_Eval() {
    static uint8_t EspInCase = 0;
    uint8_t EspIn = UART4_Read(); // Incoming byte
//    const char *message = "hello";
    switch (EspInCase) { // select case set by previous byte
        case 0: // INPUT ALLOCATION ********************************************
            switch ((EspIn >> 5) & 0x07) {
                case 0: // xxx == 000, emergency stop
                    if ((EspIn & 0x1F) == 37)
                        EspInCase = 1;
                    break;
//                case 2: // xxx == 010, action sync received
//                    EdgInCase[edge] = 2;
//                    break;
//                case 3: // xxx == 011, idle mode
//                    EdgInCase[edge] = 10;
//                    break;
//                case 4: // xxx == 100, connection detected or acknowledged
//                    EdgInCase[edge] = 20;
//                    break;
                case 6: // xxx == 110, command
                    Coms_CMD_Handle(ESP_URT_NUM, EspIn & 0b00011111);
                    EspInCase = 6;
                    break;
                case 7: // xxx == 111, relay
                    Coms_REL_Handle(ESP_URT_NUM, EspIn & 0b00011111);
                    EspInCase = 7;
                    break;
            }
            break;   
            
        case 6:
            if (Coms_CMD_Handle(ESP_URT_NUM, EspIn))
                EspInCase = 0;
            break;
            
        case 7:
            if (Coms_REL_Handle(ESP_URT_NUM, EspIn))
                EspInCase = 0;
            break;
    }
            
}

/* ******************** ESP COMMAND TO DRIVE ******************************** */
void Coms_ESP_Drive(uint8_t speed, int8_t curve, uint8_t edge, uint8_t direc) {
    float Mo = curve * 137.9;
    float Sa = speed * 4;
    if (!direc) { // inwards or outwards
        Sa = -1 * Sa;
    }

    float a, b, c; // extension values from 180
    switch (edge) {
        case 0:
            a = 180 + (MotLin_MAX_1 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_1 - MotLin_MIN_1);
            b = 180 + (MotLin_MAX_2 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_2 - MotLin_MIN_2);
            c = 180 + (MotLin_MAX_3 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_3 - MotLin_MIN_3);
            break;
        case 1:
            a = 180 + (MotLin_MAX_2 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_2 - MotLin_MIN_2);
            b = 180 + (MotLin_MAX_3 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_3 - MotLin_MIN_3);
            c = 180 + (MotLin_MAX_1 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_1 - MotLin_MIN_1);
            break;
        case 2:
            a = 180 + (MotLin_MAX_3 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_3 - MotLin_MIN_3);
            b = 180 + (MotLin_MAX_1 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_1 - MotLin_MIN_1);
            c = 180 + (MotLin_MAX_2 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_2 - MotLin_MIN_2);
            break;
        default:
            a = 180 + (MotLin_MAX_1 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_1 - MotLin_MIN_1);
            b = 180 + (MotLin_MAX_2 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_2 - MotLin_MIN_2);
            c = 180 + (MotLin_MAX_3 - Acts_LIN_GetTarget(0))*12 / (MotLin_MAX_3 - MotLin_MIN_3);
            break;
    }

    // vertex angles (float alpha = acosf((b*b + c*c - a*a)/(2*b*c));)
    float beta = acosf((a * a + c * c - b * b) / (2 * a * c));
    float gamm = acosf((a * a + b * b - c * c) / (2 * a * b));

    // wheel coordinates (for a: [WHEEL, 0])
    float Wb[2] = {(b - WHEEL) * cosf(gamm), (b - WHEEL) * sinf(gamm)};
    float Wc[2] = {a - WHEEL * cosf(beta), WHEEL * sinf(beta)};

    // second point in wheel direction
    float Wb2[2] = {Wb[0] - cosf(PI / 2 - gamm), Wb[1] + sinf(PI / 2 - gamm)};
    float Wc2[2] = {Wc[0] + cosf(PI / 2 - beta), Wc[1] + sinf(PI / 2 - beta)};

    // centroid coordinates
    float D[2] = {(b * cosf(gamm) + a) / 3, b * sinf(gamm) / 3};

    // moment arm of wheel force to centroid
    float Da = fabsf(D[0] - WHEEL);
    float Db = fabsf((Wb2[1] - Wb[1]) * D[0]
            - (Wb2[0] - Wb[0]) * D[1] + Wb2[0] * Wb[1] - Wb2[1] * Wb[0])
            / sqrtf(powf(Wb2[1] - Wb[1], 2) + powf(Wb2[0] - Wb[0], 2));
    float Dc = fabsf((Wc2[1] - Wc[1]) * D[0]
            - (Wc2[0] - Wc[0]) * D[1] + Wc2[0] * Wc[1] - Wc2[1] * Wc[0])
            / sqrtf(powf(Wc2[1] - Wc[1], 2) + powf(Wc2[0] - Wc[0], 2));

    // wheel speeds
    float Sc = (Mo - Sa * Da) / (Db * cosf(PI / 2 - beta) / cosf(PI / 2 - gamm) + Dc);
    float Sb = Sc * cosf(PI / 2 - beta) / cosf(PI / 2 - gamm);

    Sc = SxOUT*Sc;
    Sb = SxOUT*Sb;

    // output depending on driving edge
    switch (edge) {
        case 0:
            Acts_ROT_Out(0, Sa);
            Acts_ROT_Out(1, Sb);
            Acts_ROT_Out(2, Sc);
            break;
        case 1:
            Acts_ROT_Out(1, Sa);
            Acts_ROT_Out(2, Sb);
            Acts_ROT_Out(0, Sc);
            break;
        case 2:
            Acts_ROT_Out(2, Sa);
            Acts_ROT_Out(0, Sb);
            Acts_ROT_Out(1, Sc);
            break;
        default:
            Acts_ROT_Out(0, Sa);
            Acts_ROT_Out(1, Sb);
            Acts_ROT_Out(2, Sc);
            break;
    }
}

/* ******************** RETURN ID BY BYTE *********************************** */
uint8_t Coms_ESP_ReturnID(uint8_t byteNum) {
    return SelfID[byteNum];
}

/* ******************** VERBOSE OUTPUT ************************************** */
void Coms_ESP_Verbose() 
{
    while(Flg_Uart_Lock[ESP_URT_NUM])   //wait for uart to unlock
    {
    }
    Flg_Uart_Lock[ESP_URT_NUM] = true;   //locks s.t. the sequence is uninterrupted    
    UART4_Write(0);
    UART4_Write(6); // Message length
    uint8_t i;
    for (i = 0; i < 3; i++) {
        UART4_Write16(ADC1_Return(i));
    }
    UART4_Write(ESP_End);
    Flg_Uart_Lock[ESP_URT_NUM] = false;
}


/* ******************** VERBOSE OUTPUT ************************************** */
void Coms_ESP_Verbose_Write(const char *message) 
{
    while(Flg_Uart_Lock[ESP_URT_NUM])   //wait for uart to unlock
    {
    }
    Flg_Uart_Lock[ESP_URT_NUM] = true;   //locks s.t. the sequence is uninterrupted        
    
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
    Flg_Uart_Lock[ESP_URT_NUM] = false;
}

void Coms_ESP_LED_State(uint8_t edge, uint8_t state)
{
    if((WIFI_LED_STATE[edge] - state) == 0)
    {
        return;
    }
    switch(state){
        case 0: //off
            Coms_ESP_LED_On(edge, WIFI_LED_OFF);
            break;

        case 1: //on
            Coms_ESP_LED_On(edge, WIFI_LED_ON);
            break;

        case 2: //toggle
            Coms_ESP_LED_Tgl(edge);
            break;

        case 3: //blink
            if(WIFI_LED_BLINK_DES[edge] != WIFI_LED_BLINK_ACT[edge])
            {
                Coms_ESP_LED_Blk(edge, WIFI_LED_BLINK_DES[edge]);
                WIFI_LED_BLINK_ACT[edge] = WIFI_LED_BLINK_DES[edge];
            }
            break;
        default:
            break;
    }
    WIFI_LED_STATE[edge] = state;
}         



/* ******************** SET ESP EDGE LEDS *********************************** */
void Coms_ESP_LED_On(uint8_t edge, bool OnOff) {
    uint8_t alloc = 0b01000001 + OnOff;   // Cmd, ---, blink
    edge++;
    alloc |= (edge << 3) & 0b00011000;
    while(Flg_Uart_Lock[ESP_URT_NUM])   //wait for uart to unlock
    {
    }
    Flg_Uart_Lock[ESP_URT_NUM] = true;   //locks s.t. the sequence is uninterrupted            
    UART4_Write(alloc);  // LED R, Set Blink Freq
    UART4_Write(ESP_End);
    Flg_Uart_Lock[ESP_URT_NUM] = false;
}


void Coms_ESP_LED_Tgl(uint8_t edge) {
    uint8_t alloc = 0b01000000;   // Cmd, ---, blink
    edge++;
    alloc |= (edge << 3) & 0b00011000;
    while(Flg_Uart_Lock[ESP_URT_NUM])   //wait for uart to unlock
    {
    }
    Flg_Uart_Lock[ESP_URT_NUM] = true;   //locks s.t. the sequence is uninterrupted            
    UART4_Write(alloc);  // LED R, Set Blink Freq
    UART4_Write(ESP_End);
    Flg_Uart_Lock[ESP_URT_NUM] = false;
}


void Coms_ESP_LED_Blk(uint8_t edge, uint8_t blink) {
    uint8_t alloc = 0b01000100;   // Cmd, ---, blink
    edge++;
    alloc |= (edge << 3) & 0b00011000;
    while(Flg_Uart_Lock[ESP_URT_NUM])   //wait for uart to unlock
    {
    }
    Flg_Uart_Lock[ESP_URT_NUM] = true;   //locks s.t. the sequence is uninterrupted            
    UART4_Write(alloc);  // LED R, Set Blink Freq
    UART4_Write(blink);
    UART4_Write(ESP_End);
    Flg_Uart_Lock[ESP_URT_NUM] = false;
}

void Coms_ESP_LED_Set_Blink_Freq(uint8_t edge, uint8_t blink)
{
    WIFI_LED_BLINK_DES[edge] = blink;
}

void Coms_ESP_Interpret() {
    static uint8_t ESP_bnk_frq = 128;
    while(Flg_Uart_Lock[ESP_URT_NUM])   //wait for uart to unlock
    {
    }
    Flg_Uart_Lock[ESP_URT_NUM] = true;   //locks s.t. the sequence is uninterrupted            
    UART4_Write(0b01001100);  // LED R, Set Blink Freq
    UART4_Write(ESP_bnk_frq);
    UART4_Write(ESP_End);
    Flg_Uart_Lock[ESP_URT_NUM] = false;
    ESP_bnk_frq++;
}

/* Com_ESP_Drive - Online calc verification */
/* https://repl.it/languages/c

#include <stdio.h>
#include <stdint.h>
#include "math.h"
#define WHEEL 68.15f
#define PI 3.1415926535897931159979634685441851615905761718750
#define Sfact 0.9 // Step 3: maximising Sb & Sc at a=b=192,c=180

int
main ()
{
  uint8_t speed = 255;
  int8_t curve = -127;
  uint8_t edge = 0;
  uint8_t direc = 0;

  float Mo = curve * 137.9; // Step 2: minimising Sb & Sc at a=b=180,c=193.5
  float Sa = speed * 4; // Step 1: from 8 bit to 10 bit
  if (!direc)
    {				// inwards or outwards
      Sa = -1 * Sa;
    }

  float a = 192;
  float b = 192;
  float c = 180;

  // vertex angles (float alpha = acosf((b*b + c*c - a*a)/(2*b*c));)
  float beta = acosf ((a * a + c * c - b * b) / (2 * a * c));
  float gamm = acosf ((a * a + b * b - c * c) / (2 * a * b));

  // wheel coordinates (for a: [WHEEL, 0])
  float Wb[2] = { (b - WHEEL) * cosf (gamm), (b - WHEEL) * sinf (gamm) };
  float Wc[2] = { a - WHEEL * cosf (beta), WHEEL * sinf (beta) };

  // second point in wheel direction
  float Wb2[2] =
    { Wb[0] - cosf (PI / 2 - gamm), Wb[1] + sinf (PI / 2 - gamm) };
  float Wc2[2] =
    { Wc[0] + cosf (PI / 2 - beta), Wc[1] + sinf (PI / 2 - beta) };

  // centroid coordinates
  float D[2] = { (b * cosf (gamm) + a) / 3, b * sinf (gamm) / 3 };

  printf ("D1: %4.4f \n", D[0]);
  printf ("D2: %4.4f \n", D[1]);

  // moment arm of wheel force to centroid
  float Da = fabsf (D[0] - WHEEL);
  float Db = fabsf ((Wb2[1] - Wb[1]) * D[0]
            - (Wb2[0] - Wb[0]) * D[1] + Wb2[0] * Wb[1] -
            Wb2[1] * Wb[0]) / sqrtf (powf (Wb2[1] - Wb[1],
                           2) + powf (Wb2[0] - Wb[0],
                                  2));
  float Dc =
    fabsf ((Wc2[1] - Wc[1]) * D[0] - (Wc2[0] - Wc[0]) * D[1] +
       Wc2[0] * Wc[1] - Wc2[1] * Wc[0]) / sqrtf (powf (Wc2[1] - Wc[1],
                               2) + powf (Wc2[0] -
                                      Wc[0],
                                      2));

  // wheel speeds
  float Sc =
    (Mo - Sa * Da) / (Db * cosf (PI / 2 - beta) / cosf (PI / 2 - gamm) + Dc);
  float Sb = Sc * cosf (PI / 2 - beta) / cosf (PI / 2 - gamm);
  
  printf ("fact: %4.4f \n", cosf (PI / 2 - beta) / cosf (PI / 2 - gamm));
  printf ("Sa: %4.4f \n", Sa);
  printf ("Sb: %4.4f \n", Sb*Sfact);
  printf ("Sc: %4.4f \n", Sc*Sfact);
  return 0;
}
 */