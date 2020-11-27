#include "Mnge_ERR.h"
#include "Defs_GLB.h"
#include "Defs_MOD.h"
#include "Mnge_RGB.h"
#include "Coms_ESP.h"

void Mnge_ERR_ActivateStop(){
    if (!FLG_Emergency){
        FLG_Emergency = true;
        uint8_t e;
        for (e = 0; e < 3; e++){
            Flg_EdgeSyn[e] = false;
            Flg_EdgeAct[e] = false;
            Flg_EdgeReq_Ext[e] = false;
            Flg_EdgeReq_Ang[e] = false;
        }
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