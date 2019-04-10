/* ************************************************************************** */
/* ************************************************************************** */
/* **********   File:    Defines.h                                 ********** */
/* **********   Author:  Christoph H. Belke                        ********** */
/* **********   Date:    09.12.2015                                ********** */
/* ************************************************************************** */
/* ************************************************************************** */

#ifndef DEFINE_H
#define	DEFINE_H

#define FCY 3686400UL

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>
#include <libpic30.h>

static volatile bool Flg_LiveAngle;


/* ******************** MODE SELECTION ************************************** */
#define MODE_DEBUG 0

/* ******************** PERIPHERALS ***************************************** */
// Output latches for LEDs
#define LED_R LATBbits.LATB0        // Blue LED - 1 is off
#define LED_Y LATBbits.LATB1        // Orange LED - 1 is off
#define BTN_Stat PORTAbits.RA1      // button port

/* ******************** EPS COMMUNICATION************************************ */
#define ESP_Beg 13                  // Start verification byte
#define ESP_End 14                  // End verification byte

/* ******************** PWM GENERATOR *************************************** */
// Duty cycle register
#define ROT_PWM_DutyReg_A SDC5      // Generator 5, Secondary
#define ROT_PWM_DutyReg_B PDC5      // Generator 5, Primary
#define ROT_PWM_DutyReg_C SDC4      // Generator 4, Secondary
#define LIN_PWM_DutyReg_A SDC3      // Generator 3, Secondary
#define LIN_PWM_DutyReg_B SDC2      // Generator 2, Secondary
#define LIN_PWM_DutyReg_C SDC1      // Generator 1, Secondary

// Duty cycle selector
#define ROT_PWM_A 1
#define ROT_PWM_B 2
#define ROT_PWM_C 3
#define LIN_PWM_A 4
#define LIN_PWM_B 5
#define LIN_PWM_C 6

// Ensure the following are set in the PWM Register selector
//#define PWM_Perd_RotA SPHASE5
//#define PWM_Perd_RotB PHASE5
//#define PWM_Perd_RotC SPHASE4
//#define PWM_Perd_LinA SPHASE3
//#define PWM_Perd_LinB SPHASE2    
//#define PWM_Perd_LinC SPHASE1    

/* ******************** LINEAR MOTORS *************************************** */
#define LIN_DIR_A LATBbits.LATB10
#define LIN_DIR_B LATBbits.LATB12
#define LIN_DIR_C LATBbits.LATB14

#define MotLin_MIN_A 108
#define MotLin_MAX_A 1022
#define MotLin_MIN_B 108
#define MotLin_MAX_B 1022
#define MotLin_MIN_C 108
#define MotLin_MAX_C 1022
#define MotLin_SlowRegion 90        // slow down motor to avoid crash
#define MotLin_SlowFactor 2

#define MotLin_PID_de 10            // acceptable error band ~ *0.01mm
#define MotLin_PID_dt 50            // timer period
#define MotLin_PID_kP 25            // proportional component
#define MotLin_PID_kI 12            // integral component
#define MotLin_PID_kD 0             // derivative component
    
#define MotLin_PID_Imax 15
#define MotLin_PID_Max 1000

/* ******************** ROTARY MOTORS *************************************** */
#define ROT_DIR_A LATCbits.LATC6
#define ROT_DIR_B LATCbits.LATC7
#define ROT_DIR_C LATAbits.LATA10
    
#define MotRot_AngleRange 180

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
//#define TLC59208_ALL_ADDRESS 0x48
#define TLC59208_CtrlReg 0xA2
#define TLC59208_MODE1Add 0x80 // address MODE1, auto increment enabled
#define TLC59208_MODE1 0x01 // respond to LED All Call, auto increment disabled
#define TLC59208_LEDOUT0Add 0x8C // address LEDOUT0, auto increment enabled
#define TLC59208_LEDOUT0 0xAA // LEDOUT0 all outputs PWM controlled
#define TLC59208_LEDOUT1Add 0x8D // address LEDOUT0, auto increment enabled
#define TLC59208_LEDOUT1 0xAA // LEDOUT0 all outputs PWM controlled





#ifdef	__cplusplus
extern "C" {
#endif

#ifdef	__cplusplus
}
#endif

#endif	/* DEFINE_H */

