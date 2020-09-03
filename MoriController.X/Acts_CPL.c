#include "Defs.h"
#include "Mnge_PWM.h"
#include "Acts_CPL.h"
#include "Coms_ESP.h"

volatile uint8_t CPL_Count_1[3] = {0, 0, 0};
volatile uint8_t CPL_Count_2[3] = {0, 0, 0};
volatile bool Acts_CPL_Open[3] = {false, false, false};

/* ******************** SET COUPLING PWM VALUE ****************************** */
void Acts_CPL_Set(uint8_t edge, uint8_t duty) {
    switch (edge) {
        case 0:
            Mnge_PWM_SetValues(3, duty);
            break;
        case 1:
            Mnge_PWM_SetValues(4, duty);
            break;
        case 2:
            Mnge_PWM_SetValues(5, duty);
            break;
        default:
            break;
    }
}

/* ******************** OPEN COUPLING *************************************** */
void Acts_CPL_On(uint8_t edge) {
    Acts_CPL_Open[edge] = true;
    CPL_Count_1[edge] = 0;
    CPL_Count_2[edge] = SMA_Period_2;
    Acts_CPL_Set(edge, SMA_Duty_1);
    Coms_ESP_LED_Set_Blink_Freq(edge, 5);
    Coms_ESP_LED_State(edge, 3);
}

/* ******************** CLOSE COUPLING ************************************** */
void Acts_CPL_Off(uint8_t edge) {
    Acts_CPL_Open[edge] = false;
    CPL_Count_1[edge] = SMA_Period_1;
    CPL_Count_2[edge] = SMA_Period_2;
    Acts_CPL_Set(edge, 0);
    Coms_ESP_LED_State(edge, 0);
}

/* ******************** COUPLING SMA CONTROLLER ***************************** */
void Acts_CPL_Ctrl(void) { // called in tmr3, switches off when counter runs out
    uint8_t m;
    for (m = 0; m <= 2; m++) {
        if (CPL_Count_1[m] < SMA_Period_1) { // first pwm phase (high current)
            CPL_Count_1[m]++;
            if (CPL_Count_1[m] >= SMA_Period_1) {
                Acts_CPL_Set(m, SMA_Duty_2);
                CPL_Count_2[m] = 0;
                Coms_ESP_LED_Set_Blink_Freq(m, 10);
                Coms_ESP_LED_State(m, 3);
            }
        } else if (CPL_Count_2[m] < SMA_Period_2) { // second pwm phase (maintain)
            CPL_Count_2[m]++;
            if (CPL_Count_2[m] >= SMA_Period_2) {
                Acts_CPL_Off(m);
            }
        } else {
            Acts_CPL_Off(m);
        }
    }
    Mnge_PWM_Write();
}

bool Acts_CPL_IsOpen(uint8_t edge){
    return Acts_CPL_Open[edge];
}