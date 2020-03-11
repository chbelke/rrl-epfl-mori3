#include "Button.h"
#include "define.h"
#include "mcc_generated_files/uart4.h"
#include "Coms.h"
#include "MotRot.h"

volatile int k = 0;

void Button_Eval() {
//    k = k + 1;
//    switch (k) {
//        case 1:
//            Coms_ESP_Drive(255, -127, 0, 1);
//            break;
//        case 2:
//            Coms_ESP_Drive(255, 0, 0, 1);
//            break;
//        case 3:
//            Coms_ESP_Drive(255, 127, 0, 1);
//            break;
//        case 4:
//            Coms_ESP_Drive(0, 0, 0, 1);
//            Flg_EdgeDemo = true;
//            break;
//        default:
//            Coms_ESP_Drive(0, 0, 0, 1);
//            Flg_EdgeDemo = false;
//            k = 0;
//            break;
//    }
    
    SMA_On(0);
}

int Button_ReturnState() {
    return k;
}
