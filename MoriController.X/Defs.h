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
#define MODE_ENC_CON false
#define MODE_ACC_CON false

#define STAT_MotLin_Active false
#define STAT_MotRot_Active false


/* ******************** BATTERY ********************************************* */
#define BatCountMax 10 // seconds of continuos low bat before flag is triggered

/* ******************** LIVE MODE VARS ************************************** */
extern volatile bool MODE_LED_ANGLE;
extern volatile bool MODE_LED_EDGES;
extern volatile bool MODE_LED_RNBOW;

/* ********************  FLAGS ********************************************** */
extern volatile bool Flg_LiveAng;
extern volatile bool Flg_LiveExt;
extern volatile bool Flg_EdgeCon[3];
extern volatile bool Flg_EdgeSyn[3];
extern volatile bool Flg_EdgeAct[3];
extern volatile bool Flg_BatLow;
extern volatile bool Flg_Button;

extern volatile bool Flg_EdgeDemo;

extern volatile bool Flg_DelayStart;
extern volatile bool Flg_Verbose;
extern volatile bool Flg_Uart_Lock[4];

extern volatile bool Flg_MotLin_Active;
extern volatile bool Flg_MotRot_Active;
extern volatile bool Flg_ID_check;

/* ******************** STATE INFO ****************************************** */
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
#define ESP_Beg 13                  // start byte
#define ESP_End 14                  // end byte
#define ESP_Relay 0b00000101
#define ESP_URT_NUM 3

/* ******************** EDGE COMMUNICATION *********************************** */
#define EDG_End 42                  // end byte
#define EDG_IdlIntrvl 3             // idle check at 5Hz, 5 = 1 sec = con lost
#define EDG_ConIntrvl 10            // con check at 5Hz, 5 = 2 sec = con lost

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

#define MotLin_MIN_1 108            // min pot value A
#define MotLin_MAX_1 1022           // max pot value A
#define MotLin_MIN_2 108            // min pot value B
#define MotLin_MAX_2 1022           // max pot value B
#define MotLin_MIN_3 108            // min pot value C
#define MotLin_MAX_3 1022           // max pot value C
#define MotLin_SlowRegion 50        // slow region near min and max
#define MotLin_SlowFactor 2         // linear slow down factor in slow region

#define MotLin_PID_de 12            // acceptable error band ~ *0.01mm
#define MotLin_PID_dt 50            // timer period
#define MotLin_PID_kP 25            // proportional component
#define MotLin_PID_kI 12            // integral component
#define MotLin_PID_kD 0             // derivative component
    
#define MotLin_PID_Imax 15
#define MotLin_PID_Max 1000


/* ******************** ROTARY MOTORS *************************************** */
#define ROT_DIR_1 LATCbits.LATC6
#define ROT_DIR_2 LATCbits.LATC7
#define ROT_DIR_3 LATAbits.LATA10

#define MotRot_AngleRange 180       // overall range (in degrees)

#define MotRot_PID_dt 0.1           // timer period (currently not used)
#define MotRot_PID_kP 150           // proportional component
#define MotRot_PID_kI 10            // integral component
#define MotRot_PID_kD 0             // derivative component

#define MotRot_PID_Imax 20
#define MotRot_PID_Max 1000


/* ******************** I2C ************************************************* */
#define SLAVE_I2C_GENERIC_RETRY_MAX           20
#define SLAVE_I2C_GENERIC_DEVICE_TIMEOUT      50


/* ******************** ENCODERS AS5048B ************************************ */
#define AS5048B_Address 0x40
#define AS5048B_Reg_AngleMSB 0xFE
#define AS5048B_Res 16384.0


/* ******************** ACCELEROMETER MMA8452Q ****************************** */
#define MMA8452Q_Address 0x1C //i2c address
#define MMA8452Q_CTRL_REG1_ADDR 0x2A //Ctrl reg address to be modified at Setup
#define MMA8452Q_CTRL_REG1 0x01 //Value ctrl reg must be modified to at Setup
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

#define SMA_Period 200 // SMA on-time (updated in 20 Hz loop) - 100 = 5 sec.
#define SMA_Duty 100 // 8-bit PWM value

#endif	/* DEFS_H */