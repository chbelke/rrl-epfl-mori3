#include "Mnge_ERR.h"
#include "Defs_GLB.h"
#include "Defs_MOD.h"
#include "Mnge_RGB.h"
#include "Coms_ESP.h"
#include "Coms_123.h"

uint8_t errCode = 0;
uint8_t errEdge = 0;

void Mnge_ERR_ActivateStop(uint8_t edge, uint8_t code){
    if (!FLG_Emergency){
        FLG_Emergency = true;
        errCode = code;
        errEdge = edge;
        
        uint8_t e;
        for (e = 0; e < 3; e++)
            Coms_123_Disconnected(e);
        
        FLG_MotLin_Active = false;
        FLG_MotRot_Active = false;
        
        // switch off LED flags and turn red
        MODE_LED_ANGLE = false;
        MODE_LED_EDGES = false;
        MODE_LED_RNBOW = false;
        MODE_LED_PARTY = false;
        Mnge_RGB_SetAll(20,0,0);
        LED_R = LED_On;
        
        Coms_ESP_TurnOnWifi();
    }
}

uint8_t Mnge_ERR_GetErrorCode(){
    return errCode;
}

uint8_t Mnge_ERR_GetErrorEdge(){
    return errEdge;
}