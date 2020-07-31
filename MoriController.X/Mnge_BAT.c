#include "Mnge_BAT.h"
#include "Defs.h"

uint8_t BatCount = 0;

// called in TMR5
void Battery_Check() {
    if (BAT_LBO) {
        BatCount++;
        if (BatCount >= (TMR5_f * BatCountMax)) {
            Flg_BatLow = true;
        }
    } else {
        if (BatCount != 0) {
            BatCount--;
        } else {
            Flg_BatLow = false;
        }
    }
}