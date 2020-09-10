#include "Mnge_PWM.h"
#include "Mnge_RGB.h"

/* ******************** SET ONE RGB LED ************************************* */
void Mnge_RGB_Set(uint8_t RGBcolor, uint8_t duty) {
    switch (RGBcolor) {
        case 0:
            Mnge_PWM_SetValues(2, duty); // red
            break;
        case 1:
            Mnge_PWM_SetValues(1, duty); // green
            break;
        case 2:
            Mnge_PWM_SetValues(0, duty); // blue
            break;
        default:
            break;
    }
    Flg_i2c_PWM = true; // Mnge_PWM_Write() called in tmr1
}

/* ******************** SET ALL RGB LEDS ************************************ */
void Mnge_RGB_SetAll(uint8_t R, uint8_t G, uint8_t B) {
    Mnge_PWM_SetValues(2, R); // red
    Mnge_PWM_SetValues(1, G);  // green
    Mnge_PWM_SetValues(0, B);  // blue
    Flg_i2c_PWM = true; // Mnge_PWM_Write() called in tmr1
}