#include "Button.h"
#include "define.h"
#include "mcc_generated_files/uart4.h"
#include "Coms.h"
#include "MotRot.h"

volatile int k = 0;

void Button_Eval() {
    k = k + 1;
//    switch (k) {
//        case 1:
//            MotRot_OUT(0, 1024);
//            MotRot_OUT(1, 512);
//            MotRot_OUT(2, 0);
//            break;
//        case 2:
//            MotRot_OUT(0, -1024);
//            MotRot_OUT(1, -512);
//            MotRot_OUT(2, 0);
//            break;
//        case 3:
//            MotRot_OUT(0, 512);
//            MotRot_OUT(1, 256);
//            MotRot_OUT(2, 0);
//            break;
//        default:
//            MotRot_OUT(0, 0);
//            MotRot_OUT(1, 0);
//            MotRot_OUT(2, 0);
//            k = 0;
//            break;
//    }

    switch (k) {
        case 1:
            Coms_ESP_Drive(255, -127, 0, 1);
            break;
        case 2:
            Coms_ESP_Drive(255, -96, 0, 1);
            break;
        case 3:
            Coms_ESP_Drive(255, -64, 0, 1);
            break;
        case 4:
            Coms_ESP_Drive(255, -32, 0, 1);
            break;
        case 5:
            Coms_ESP_Drive(255, 0, 0, 1);
            break;
        case 6:
            Coms_ESP_Drive(255, 32, 0, 1);
            break;
        case 7:
            Coms_ESP_Drive(255, 64, 0, 1);
            break;
        case 8:
            Coms_ESP_Drive(255, 96, 0, 1);
            break;
        case 9:
            Coms_ESP_Drive(255, 127, 0, 1);
            break;
        default:
            Coms_ESP_Drive(0, 0, 0, 1);
            k = 0;
            break;
    }
}

int Button_ReturnState() {
    return k;
}