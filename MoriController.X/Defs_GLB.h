/* ************************************************************************** */
/* ************************************************************************** */
/* **********   File:    Defines.h                                 ********** */
/* **********   Author:  Christoph H. Belke                        ********** */
/* **********   Date:    09.12.2020                                ********** */
/* ************************************************************************** */
/* ************************************************************************** */

#ifndef DEFS_H
#define	DEFS_H

#define FCY 3686400UL               // cycle frequency

#include <xc.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <libpic30.h>


/* ******************** MODULE ********************************************** */
#define MODULE 'C' // module name by letter

/* ******************** NOTES *********************************************** */
// Timer info
/* Timer 1: 100 Hz - angle feedback
 * Timer 3: 20 Hz - extension feedback, coupling controller
 * Timer 5: 5 Hz - updating LEDs 
 * TMRx_f NOT used to define frequency */
#define TMR1_f 100
#define TMR3_f 20
#define TMR5_f 5

// I2C MCC modification
/* I2C1BRG changed from MCC calculated 0x08 to 0x07, as FRM calculation
 * is as follows: ((1/0.4 - 0.120)*3.6864)-2 = 6.77
 * FRM: http://ww1.microchip.com/downloads/en/DeviceDoc/70000195g.pdf*/


/* ******************** MODE SELECTION ************************************** */
//#define MODE_DEBUG false
#define MODE_ENC_CON true
#define MODE_ACC_CON true

#define MODE_MotLin_Active true
#define MODE_MotRot_Active true
#define MODE_Cplngs_Active true

// RGB LED Default values
#define RGB_Default_Red 10
#define RGB_Default_Green 10
#define RGB_Default_Blue 0

/* ******************** BATTERY ********************************************* */
#define BatCountMax 10 // seconds of continuos low bat before flag is triggered

/* ******************** LIVE MODE VARS ************************************** */
extern volatile bool MODE_LED_ANGLE;
extern volatile bool MODE_LED_EDGES;
extern volatile bool MODE_LED_RNBOW;
extern volatile bool MODE_LED_PARTY;

/* ********************  FLAGS ********************************************** */
// FLG can be set externally, Flg is only used by program
extern volatile bool FLG_WaitAllEdges;
extern volatile bool FLG_MotLin_Active;
extern volatile bool FLG_MotRot_Active;
extern volatile bool FLG_Verbose;
extern volatile bool FLG_Emergency;

extern volatile bool Flg_BatLow;
extern volatile bool Flg_Button;

extern volatile bool Flg_Drive[3];
extern volatile bool Flg_DriveAndCouple[3];

extern volatile bool Flg_EdgeCon[3];
extern volatile bool Flg_EdgeSyn[3];
extern volatile bool Flg_EdgeAct[3];
extern volatile bool Flg_EdgeWig[3];
extern volatile bool Flg_EdgeReq_Ang[3];
extern volatile bool Flg_EdgeReq_Ext[3];
extern volatile bool Flg_EdgeReq_Cpl[3];
extern volatile bool Flg_EdgeReq_CplNbrWait[3];

extern volatile bool Flg_ID_check;

extern volatile bool Flg_i2c_PWM;
extern volatile bool Flg_i2c_ACC;
extern volatile bool Flg_i2c_DAC;

extern volatile bool Flg_Tmr3;
extern volatile bool Flg_Tmr5;

/* ******************** STATE INFO ****************************************** */
extern volatile uint8_t ESP_Update_Delay;
extern volatile uint8_t ESP_DataLog_Time_Elapsed;
extern volatile uint8_t ESP_ID[6];


/* ******************** PERIPHERALS ***************************************** */
// Output latches for LEDs
#define LED_R LATBbits.LATB0        // blue LED - 1 is off
#define LED_Y LATBbits.LATB1        // orange LED - 1 is off
#define BTN_Stat PORTAbits.RA1      // button port
#define WIFI_EN LATBbits.LATB4      // wifi enable
#define BAT_LBO PORTBbits.RB7       // low battery indicator

// On-off confusion prevention
#define LED_On 0                    // drained by pin, low V = led on
#define LED_Off 1
#define WIFI_On 1                   // high enables ESP
#define WIFI_Off 0
#define WIFI_LED_ON 1
#define WIFI_LED_OFF 0


/* ******************** ESP COMMUNICATION *********************************** */
#define ESP_End 14                  // end byte
#define ESP_Relay 0b00000101
#define ESP_URT_NUM 3


/* ******************** EDGE COMMUNICATION *********************************** */
#define EDG_End 42                  // end byte
#define EDG_IdlIntrvl 3             // idle check at 5Hz, 5 = 1 sec = con lost
#define EDG_ConIntrvl 10            // con check at 5Hz, 10 = 2 sec = con lost
#define EDG_ActIntrvl 2             // act check at 20Hz, 3 = 
#define EDG_ExtNbrRng 5             // current neighbour ext. must be (own +-)
#define EDG_ExtSlwRng 3             // if nbr <= this >= EDG_ExtNbrRng, slw down
#define EDG_ExtSlwVal 768           // slow down to this 


/* ******************** PWM GENERATOR *************************************** */
// Duty cycle register
#define ROT_PWM_DutyReg_1 SDC5      // generator 5, secondary
#define ROT_PWM_DutyReg_2 PDC5      // generator 5, primary
#define ROT_PWM_DutyReg_3 SDC4      // generator 4, secondary
#define LIN_PWM_DutyReg_1 SDC3      // generator 3, secondary
#define LIN_PWM_DutyReg_2 SDC2      // generator 2, secondary
#define LIN_PWM_DutyReg_3 SDC1      // generator 1, secondary
// ROT PWM full range 1024 (SPHASEx, PHASEx)
// LIN PWM limited to 1024 (SPHASEx)

// Duty cycle selector
#define ROT_PWM_1 1
#define ROT_PWM_2 2
#define ROT_PWM_3 3
#define LIN_PWM_1 4
#define LIN_PWM_2 5
#define LIN_PWM_3 6

// Ensure the following are set in the PWM Register selector
//#define PWM_Perd_RotA SPHASE5
//#define PWM_Perd_RotB PHASE5
//#define PWM_Perd_RotC SPHASE4
//#define PWM_Perd_LinA SPHASE3
//#define PWM_Perd_LinB SPHASE2    
//#define PWM_Perd_LinC SPHASE1    


/* ******************** LINEAR MOTORS *************************************** */
#define LIN_DIR_1 LATBbits.LATB10
#define LIN_DIR_2 LATBbits.LATB12
#define LIN_DIR_3 LATBbits.LATB14

#define MotLin_MinInput 0           // linear motor controlled with 0.1mm input
#define MotLin_MaxInput 120         // from 0 to 12 mm -> min: 0, max:120

#define MotLin_SlowRegion 50        // slow region near min and max
#define MotLin_SlowFactor 1.2       // linear slow down factor in slow region
#define MotLin_OkRange 2            // +- (*0.1mm) (automatic CMD update)

#define MotLin_PID_erband 4         // acceptable error band ~ *0.01mm
#define MotLin_PID_stable 3         // stable time in sec
#define MotLin_PID_period 0.05f     // timer period
#define MotLin_PID_kP 162.0f        // proportional component
#define MotLin_PID_kI 101.3f        // integral component
#define MotLin_PID_kD 8.4f          // integral component
#define MotLin_PID_DMax 1024        // derivative limit
#define MotLin_PID_Imax 1024        // integral limit
#define MotLin_PID_Max 1024         // duty cycle limit


/* ******************** ROTARY MOTORS *************************************** */
#define ROT_DIR_1 LATCbits.LATC6
#define ROT_DIR_2 LATCbits.LATC7
#define ROT_DIR_3 LATAbits.LATA10

#define MotRot_AngleRange 240       // overall range (degrees)
#define MotRot_OkRange 15           // +- (0.1*degrees) (automatic CMD update)

#define MotRot_PID_period 0.01f     // timer period
#define MotRot_PID_kP 153.0f        // proportional gain
#define MotRot_PID_kI 53.9f         // integral gain
#define MotRot_PID_kD 3.4f          // derivative gain
#define MotRot_PID_Dmax 1024        // derivative limit
#define MotRot_PID_Imax 1024        // integral limit
#define MotRot_PID_Max 1024         // duty cycle limit

#define MotRot_SPD_kP 60.0f         // speed control proportional gain
#define MotRot_SPD_kI 15.0f         // speed control integral gain
#define MotRot_SPD_kD 8.0f          // speed control derivative gain
#define MotRot_SPD_Imax 1024        // speed integral limit
#define MotRot_SPD_Max 1024         // speed duty cycle limit
#define MotRot_SpeedInit 255          // limit speed at start-up (/255)
#define MotRot_SpeedMax 60.0f         // max speed (degrees/second) (60 @tau=149)

// Maxon motor torque limit - 237 stall, 149 GPX safe, 63 backdrive safe
#define MotRot_TorqueLimit 149      // (/255)
#define MotRot_WiggleTime 15        // seconds
#define MotRot_WiggleTorque 80      // wiggle torque limit (/255)
#define MotRot_DefaultDrvInterval 5 // drv commands hold for 1 second by default
#define MotRot_DrvCplPushInterval 100 // push interval at 20Hz (< SMA_Period_2)


/* ******************** I2C ************************************************* */
#define SLAVE_I2C_GENERIC_RETRY_MAX 5
#define SLAVE_I2C_GENERIC_DEVICE_TIMEOUT 20


/* ******************** ENCODERS AS5048B ************************************ */
#define AS5048B_Address 0x40
#define AS5048B_Reg_AngleMSB 0xFE
#define AS5048B_Res 16384.0


/* ******************** ACCELEROMETER MMA8452Q ****************************** */
#define MMA8452Q_Address 0x1C //i2c address
#define MMA8452Q_CTRL_REG1_ADDR 0x2A //Ctrl reg address to be modified at Setup
#define MMA8452Q_CTRL_REG1_STBY 0x00 //Value ctrl reg must be modified at Setup
#define MMA8452Q_CTRL_REG1_ACTV 0x01 //Value ctrl reg must be modified at Setup
#define MMA8452Q_CTRL_REG2_ADDR 0x0E //XYZ_DATA_CFG register (range and filter)
#define MMA8452Q_CTRL_REG2_RNGE 0x00 //2g range and high-pass filter off
#define MMA8452Q_OUT_X_MSB_ADDR 0x01 //Address of first data register to be read



/* ******************** LED DRIVER TLC59208 ****************************** */
#define TLC59208_ADDRESS 0x20   //Addresss pins A0-A2 tied to GND (not in DS?)
//#define TLC59208_ALL_ADDRESS 0x48 // not needed for now
#define TLC59208_CtrlReg 0xA2
#define TLC59208_MODE1Add 0x80 // address MODE1, auto increment enabled
#define TLC59208_MODE1 0x01 // respond to LED All Call, auto increment disabled
#define TLC59208_LEDOUT0Add 0x8C // address LEDOUT0, auto increment enabled
#define TLC59208_LEDOUT0 0xAA // LEDOUT0 all outputs PWM controlled
#define TLC59208_LEDOUT1Add 0x8D // address LEDOUT0, auto increment enabled
#define TLC59208_LEDOUT1 0xAA // LEDOUT0 all outputs PWM controlled

#define SMA_Period_1 140 // SMA on-time (updated in 20 Hz loop) -> 100 = 5 sec.
#define SMA_Period_2 200 // split into high I open (1) and low I stay phases (2)
#define SMA_Duty_1 130 // 150 // 8-bit PWM value for first phase
#define SMA_Duty_2 65 // 8-bit PWM value for second phase

#endif	/* DEFS_H */