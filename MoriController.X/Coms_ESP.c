#include "Coms_ESP.h"
#include "define.h"
#include "MotLin.h"
#include "MotRot.h"
#include "TLC59208.h"
#include "math.h"
#include "dsp.h"
#include "mcc_generated_files/uart4.h"
#include "mcc_generated_files/adc1.h"

uint8_t EspInCase = 0; // switch case variable
uint8_t EspInAloc = 0; // incoming allocation byte (explanation below)
uint8_t EspInBits = 0; // bit count in allocation byte
uint8_t EspInByts = 0; // incoming byte count
uint16_t EspInLost = 0; // counting lost byte instances since start
uint16_t EspIn0End = 0; // counting instances of no end byte since start

uint16_t MotLinTemp[3] = {0, 0, 0}; // linear motor extension value (temporary)
uint16_t MotRotTemp[3] = {0, 0, 0}; // rotary motor angle value (temporary)

int8_t DrivePWM[3] = {0, 0, 0}; // manual drive mode PWM values by edge
uint8_t DriveSpd, DriveCrv = 0; // automatic drive mode speed and curve
#define WHEEL 68.15f // wheel distance from vertex
#define SxOUT 0.9 // output speed factor for non-primary wheels

uint8_t RgbPWM[3] = {0, 0, 0}; // rgb led values

uint8_t SelfID[6] = {0,0,0,0,0,0};


/* EspInAloc: 
 * 0bxx000000, where xx = indicator
 * 00 = extension and angular values
 * - 0b00xxxxxx linear & rotary indicators 0,1,2 (1 = value follows in order)
 * 01 = drive input
 * - 0b01xooooo 1 = automatic (modules evaluates), 0 = manual (direct to PWM)
 * - 0b010ooxxx manual indicator followed by 3 pwm values (1 signed byte each)
 * - 0b011xxooo automatic indicator followed by reference edge 0,1,2
 * - 0b011ooxoo direction (0 = inwards, 1 = outwards)
 * - 0b011oooxx tbd
 * 10 = coupling & led input
 * - 0b10xxxooo retract couplings 0,1,2 (if already open, interval prolonged)
 * - 0b10oooxxx rgb led values follow in order
 * 11 = tbd - mode selection?
 * - 0b11xxxxxx tbd
 */

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
    uint8_t EspIn = UART4_Read(); // Incoming byte
    switch (EspInCase) {
        case 0: // CHECK START BYTE
            if (EspIn == ESP_Beg) {
                EspInCase = 1;
                EspInByts = 1;
            }
            break;

        case 1: // INPUT ALLOCATION ********************************************
            EspInAloc = EspIn;
            // if xx == 00, count bits 
            if (((EspInAloc >> 6) & 0x03) == 0) { // *** ANGLE & EXTENSION INPUT
                EspInCase = 2;
                // Brian Kernighan: http://graphics.stanford.edu ...
                //  ... /~seander/bithacks.html#CountBitsSetNaive
                uint8_t EspInAlocTemp = EspInAloc;
                for (EspInBits = 0; EspInAlocTemp; EspInBits++) {
                    EspInAlocTemp &= EspInAlocTemp - 1; // clear the LSB set
                }
            } else if (((EspInAloc >> 6) & 0x03) == 1) { // ******** DRIVE INPUT
                if (EspInAloc & 0b00100000) { // automatic drive mode
                    EspInCase = 19;
                } else { // manual drive mode
                    EspInCase = 15;
                    // only last three bits relevant when counting rec. bytes
                    uint8_t EspInAlocTemp = (EspInAloc & 0b00000111);
                    for (EspInBits = 0; EspInAlocTemp; EspInBits++) {
                        EspInAlocTemp &= EspInAlocTemp - 1; // clear LSB set
                    }
                }
            } else if (((EspInAloc >> 6) & 0x03) == 2) { // COUPLING & LED INPUT
                if (EspInAloc & 0b00000111) { 
                    EspInCase = 22;
                    // only last three bits relevant when counting rec. bytes
                    uint8_t EspInAlocTemp = (EspInAloc & 0b00000111);
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
            if (EspInAloc & 0b00100000) {
                MotLinTemp[0] = ((uint16_t) (EspIn) << 8) & 0xFF00;
                EspInByts = EspInByts + 1;
                EspInCase = 3;
                break;
            }
        case 3:
            if (EspInAloc & 0b00100000) {
                MotLinTemp[0] = MotLinTemp[0] | (uint16_t) EspIn;
                EspInByts = EspInByts + 1;
                EspInCase = 4;
                break;
            }
        case 4:
            if (EspInAloc & 0b00010000) {
                MotLinTemp[1] = ((uint16_t) (EspIn) << 8) & 0xFF00;
                EspInByts = EspInByts + 1;
                EspInCase = 5;
                break;
            }
        case 5:
            if (EspInAloc & 0b00010000) {
                MotLinTemp[1] = MotLinTemp[1] | (uint16_t) EspIn;
                EspInByts = EspInByts + 1;
                EspInCase = 6;
                break;
            }
        case 6:
            if (EspInAloc & 0b00001000) {
                MotLinTemp[2] = ((uint16_t) (EspIn) << 8) & 0xFF00;
                EspInByts = EspInByts + 1;
                EspInCase = 7;
                break;
            }
        case 7:
            if (EspInAloc & 0b00001000) {
                MotLinTemp[2] = MotLinTemp[2] | (uint16_t) EspIn;
                EspInByts = EspInByts + 1;
                EspInCase = 8;
                break;
            }

        case 8: // ROTARY MOTOR INPUTS *****************************************
            /* cases 8 to 13, two bytes per motor input */
            if (EspInAloc & 0b00000100) {
                MotRotTemp[0] = ((uint16_t) (EspIn) << 8) & 0xFF00;
                EspInByts = EspInByts + 1;
                EspInCase = 9;
                break;
            }
        case 9:
            if (EspInAloc & 0b00000100) {
                MotRotTemp[0] = MotRotTemp[0] | (uint16_t) EspIn;
                EspInByts = EspInByts + 1;
                EspInCase = 10;
                break;
            }
        case 10:
            if (EspInAloc & 0b00000010) {
                MotRotTemp[1] = ((uint16_t) (EspIn) << 8) & 0xFF00;
                EspInByts = EspInByts + 1;
                EspInCase = 11;
                break;
            }
        case 11:
            if (EspInAloc & 0b00000010) {
                MotRotTemp[1] = MotRotTemp[1] | (uint16_t) EspIn;
                EspInByts = EspInByts + 1;
                EspInCase = 12;
                break;
            }
        case 12:
            if (EspInAloc & 0b00000001) {
                MotRotTemp[2] = ((uint16_t) (EspIn) << 8) & 0xFF00;
                EspInByts = EspInByts + 1;
                EspInCase = 13;
                break;
            }
        case 13:
            if (EspInAloc & 0b00000001) {
                MotRotTemp[2] = MotRotTemp[2] | (uint16_t) EspIn;
                EspInByts = EspInByts + 1;
                EspInCase = 14;
                break;
            }

        case 14: // verify motor inputs
            if (EspIn == ESP_End) {
                if (EspInByts == (2 + EspInBits * 2)) {
                    Coms_ESP_SetMots();
                } else {
                    EspInLost = EspInLost + 1; // data lost
                }
            } else {
                EspIn0End = EspIn0End + 1; // no end byte
            }
            EspInCase = 0;
            break;

        case 15: // MANUAL DRIVE MODE ******************************************
            if (EspInAloc & 0b00000100) {
                DrivePWM[0] = EspIn;
                EspInByts = EspInByts + 1;
                EspInCase = 16;
                break;
            }
        case 16:
            if (EspInAloc & 0b00000010) {
                DrivePWM[1] = EspIn;
                EspInByts = EspInByts + 1;
                EspInCase = 17;
                break;
            }
        case 17:
            if (EspInAloc & 0b00000001) {
                DrivePWM[2] = EspIn;
                EspInByts = EspInByts + 1;
                EspInCase = 18;
                break;
            }
        case 18: // verify drive inputs
            if (EspIn == ESP_End) {
                if (EspInByts == (2 + EspInBits)) {
                    uint8_t m;
                    for (m = 0; m <= 2; m++) {
                        if ((EspInAloc >> (2-m)) & 0b00000001){
                            MotRot_OUT(m, DrivePWM[m]*8);
                        }
                    }
                } else {
                    EspInLost = EspInLost + 1; // data lost
                }
            } else {
                EspIn0End = EspIn0End + 1; // no end byte
            }
            EspInCase = 0;
            break;
            
        case 19: // AUTOMATIC DRIVE MODE ***************************************
            DriveSpd = EspIn;
            EspInByts = EspInByts + 1;
            EspInCase = 20;
            break;
        
        case 20:
            DriveCrv = EspIn;
            EspInByts = EspInByts + 1;
            EspInCase = 21;
            break;
            
        case 21: // verify drive inputs
            if (EspIn == ESP_End) {
                if (EspInByts == 4) {
                    Coms_ESP_Drive(DriveSpd, DriveCrv, 
                            ((EspInAloc & 0x18)>>3), ((EspInAloc & 0x04)>>2));
                } else {
                    EspInLost = EspInLost + 1; // data lost
                }
            } else {
                EspIn0End = EspIn0End + 1; // no end byte
            }
            EspInCase = 0;
            break;
            
        case 22: // COUPLING & LED INPUT ***************************************
            if (EspInAloc & 0b00000100) {
                RgbPWM[0] = EspIn;
                EspInByts = EspInByts + 1;
                EspInCase = 23;
                break;
            }
        
            case 23:
            if (EspInAloc & 0b00000010) {
                RgbPWM[1] = EspIn;
                EspInByts = EspInByts + 1;
                EspInCase = 24;
                break;
            }
            
            case 24:
            if (EspInAloc & 0b00000001) {
                RgbPWM[2] = EspIn;
                EspInByts = EspInByts + 1;
                EspInCase = 25;
                break;
            }
            
        case 25: // verify coupling inputs
            if (EspIn == ESP_End) {
                if (EspInByts == (2 + EspInBits)) {
                    // set smas
                    uint8_t m;
                    for (m = 0; m <= 2; m++) {
                        if (EspInAloc & (0b00100000 >> m)){
                            SMA_On(m);
                        }
                    }
                    // update leds
                    for (m = 0; m <= 2; m++) {
                        if ((EspInAloc >> (2-m)) & 0b00000001){
                            LED_Set(m, RgbPWM[m]);
                        }
                    }
                } else {
                    EspInLost = EspInLost + 1; // data lost
                }
            } else {
                EspIn0End = EspIn0End + 1; // no end byte
            }
            EspInCase = 0;
            break;
            
        case 26:
            break;
            
        default:
            EspInCase = 0;
            break;
    }
}

/* ******************** ESP COMMAND TO MOTORS ******************************* */
void Coms_ESP_SetMots() {
    uint8_t j, k;

    // Set linear motors
    for (j = 1; j <= 3; j++) {
        if ((EspInAloc >> (5 - (j - 1)))) {
            MotLin_Set(j - 1, MotLinTemp[j - 1]);
        }
    }

    // Set rotary motors (not yet implemented)
    for (k = 1; k <= 3; k++) {
        if ((EspInAloc >> (2 - (k - 1))) & 0x01) {
            //            MotLin_Set(k-1,MotLinTemp[k]);
        }
    }
}

/* ******************** ESP COMMAND TO DRIVE ******************************** */
void Coms_ESP_Drive(uint8_t speed, int8_t curve, uint8_t edge, uint8_t direc) {
    float Mo = curve * 137.9;
    float Sa = speed * 4;
    if (!direc) { // inwards or outwards
        Sa = -1*Sa;
    }
    
    float a,b,c; // extension values from 180
    switch (edge) {
        case 0:
            a = 180+(MotLin_MAX_1-MotLin_Get(0))*12/(MotLin_MAX_1-MotLin_MIN_1);
            b = 180+(MotLin_MAX_2-MotLin_Get(0))*12/(MotLin_MAX_2-MotLin_MIN_2);
            c = 180+(MotLin_MAX_3-MotLin_Get(0))*12/(MotLin_MAX_3-MotLin_MIN_3);
            break;
        case 1:
            a = 180+(MotLin_MAX_2-MotLin_Get(0))*12/(MotLin_MAX_2-MotLin_MIN_2);
            b = 180+(MotLin_MAX_3-MotLin_Get(0))*12/(MotLin_MAX_3-MotLin_MIN_3);
            c = 180+(MotLin_MAX_1-MotLin_Get(0))*12/(MotLin_MAX_1-MotLin_MIN_1);
            break;
        case 2:
            a = 180+(MotLin_MAX_3-MotLin_Get(0))*12/(MotLin_MAX_3-MotLin_MIN_3);
            b = 180+(MotLin_MAX_1-MotLin_Get(0))*12/(MotLin_MAX_1-MotLin_MIN_1);
            c = 180+(MotLin_MAX_2-MotLin_Get(0))*12/(MotLin_MAX_2-MotLin_MIN_2);
            break;
        default:
            a = 180+(MotLin_MAX_1-MotLin_Get(0))*12/(MotLin_MAX_1-MotLin_MIN_1);
            b = 180+(MotLin_MAX_2-MotLin_Get(0))*12/(MotLin_MAX_2-MotLin_MIN_2);
            c = 180+(MotLin_MAX_3-MotLin_Get(0))*12/(MotLin_MAX_3-MotLin_MIN_3);
            break;
    }
    
    // vertex angles (float alpha = acosf((b*b + c*c - a*a)/(2*b*c));)
    float beta = acosf((a*a + c*c - b*b)/(2*a*c));
    float gamm = acosf((a*a + b*b - c*c)/(2*a*b));
    
    // wheel coordinates (for a: [WHEEL, 0])
    float Wb[2] = {(b-WHEEL)*cosf(gamm), (b-WHEEL)*sinf(gamm)};
    float Wc[2] = {a-WHEEL*cosf(beta), WHEEL*sinf(beta)};
    
    // second point in wheel direction
    float Wb2[2] = {Wb[0]-cosf(PI/2-gamm), Wb[1]+sinf(PI/2-gamm)};
    float Wc2[2] = {Wc[0]+cosf(PI/2-beta), Wc[1]+sinf(PI/2-beta)};
    
    // centroid coordinates
    float D[2] = {(b*cosf(gamm)+a)/3, b*sinf(gamm)/3};
    
    // moment arm of wheel force to centroid
    float Da = fabsf(D[0]-WHEEL);
    float Db = fabsf((Wb2[1]-Wb[1])*D[0]
        - (Wb2[0]-Wb[0])*D[1] + Wb2[0]*Wb[1] - Wb2[1]*Wb[0]) 
        / sqrtf(powf(Wb2[1]-Wb[1],2) + powf(Wb2[0]-Wb[0],2));
    float Dc = fabsf((Wc2[1]-Wc[1])*D[0]
        - (Wc2[0]-Wc[0])*D[1] + Wc2[0]*Wc[1] - Wc2[1]*Wc[0])
        / sqrtf(powf(Wc2[1]-Wc[1],2) + powf(Wc2[0]-Wc[0],2));
    
    // wheel speeds
    float Sc = (Mo-Sa*Da)/(Db*cosf(PI/2-beta)/cosf(PI/2-gamm) + Dc);
    float Sb = Sc*cosf(PI/2-beta)/cosf(PI/2-gamm);
    
    Sc = SxOUT*Sc;
    Sb = SxOUT*Sb;
    
    // output depending on driving edge
    switch (edge) {
        case 0:
            MotRot_OUT(0,Sa);
            MotRot_OUT(1,Sb);
            MotRot_OUT(2,Sc);
            break;
        case 1:
            MotRot_OUT(1,Sa);
            MotRot_OUT(2,Sb);
            MotRot_OUT(0,Sc);
            break;
        case 2:
            MotRot_OUT(2,Sa);
            MotRot_OUT(0,Sb);
            MotRot_OUT(1,Sc);
            break;
        default:
            MotRot_OUT(0,Sa);
            MotRot_OUT(1,Sb);
            MotRot_OUT(2,Sc);
            break;
    }
}

uint8_t Coms_ESP_ReturnID(uint8_t byteNum) {
    return SelfID[byteNum];
}


void Coms_ESP_Verbose() {
    UART4_Write(WIFI_RELAY_BYTE);
    UART4_Write(6);           // Message length
    uint8_t i;
    for(i = 0; i < 3; i++){     
        UART4_Write16(ADC1_Return(i));
    }
    UART4_Write(ESP_End);
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