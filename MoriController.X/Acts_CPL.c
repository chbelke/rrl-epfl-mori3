#include "Defs.h"
#include "Mnge_PWM.h"
#include "Acts_CPL.h"

volatile uint8_t CPL_Count[3] = {0, 0, 0};

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
    CPL_Count[edge] = SMA_Period;
    Acts_CPL_Set(edge, SMA_Duty);
}

/* ******************** CLOSE COUPLING ************************************** */
void Acts_CPL_Off(uint8_t edge) {
    CPL_Count[edge] = 0;
    Acts_CPL_Set(edge, 0);
}

/* ******************** COUPLING SMA CONTROLLER ***************************** */
void Acts_CPL_Ctrl(void) { // called in tmr3, switches off when counter runs out
    uint8_t m;
    for (m = 0; m <= 2; m++) {
        if (CPL_Count[m]) {
            CPL_Count[m] = CPL_Count[m] - 1;
            if (CPL_Count[m] <= 0) {
                Acts_CPL_Off(m);
            }
        } else {
            Acts_CPL_Off(m);
        }
    }
    Mnge_PWM_Write();
}