/* 
 * File:   Defs_Mods.h
 * Author: belke
 *
 * Created on 03 September 2020, 14:41
 */

#include "Defs.h"

#ifndef DEFS_MODS_H
#define	DEFS_MODS_H

/* ******************** LINEAR MOTORS LIMITS ******************************** */
#if MODULE == A
    #define MotLin_MIN_1 92
    #define MotLin_MAX_1 1012
    #define MotLin_MIN_2 81
    #define MotLin_MAX_2 1011
    #define MotLin_MIN_3 72
    #define MotLin_MAX_3 987
#elif  MODULE == B
    #define MotLin_MIN_1 59
    #define MotLin_MAX_1 988
    #define MotLin_MIN_2 68
    #define MotLin_MAX_2 996
    #define MotLin_MIN_3 63
    #define MotLin_MAX_3 995
#elif  MODULE == C
    #define MotLin_MIN_1 62
    #define MotLin_MAX_1 988
    #define MotLin_MIN_2 69
    #define MotLin_MAX_2 996
    #define MotLin_MIN_3 73
    #define MotLin_MAX_3 995
#elif  MODULE == D
    #define MotLin_MIN_1 79
    #define MotLin_MAX_1 999
    #define MotLin_MIN_2 63
    #define MotLin_MAX_2 989
    #define MotLin_MIN_3 72
    #define MotLin_MAX_3 995
#elif  MODULE == E
    #define MotLin_MIN_1 75
    #define MotLin_MAX_1 998
    #define MotLin_MIN_2 62
    #define MotLin_MAX_2 987
    #define MotLin_MIN_3 73
    #define MotLin_MAX_3 993
#elif  MODULE == F
    #define MotLin_MIN_1 97
    #define MotLin_MAX_1 996
    #define MotLin_MIN_2 77
    #define MotLin_MAX_2 1000
    #define MotLin_MIN_3 66
    #define MotLin_MAX_3 990
#else // safe values
    #define MotLin_MIN_1 200
    #define MotLin_MAX_1 800
    #define MotLin_MIN_2 200
    #define MotLin_MAX_2 800
    #define MotLin_MIN_3 200
    #define MotLin_MAX_3 800
#endif



/* ******************** WIFI ID ********************************************* */
#if MODULE == A
    #define WIFI_ID AAAAAA
#elif  MODULE == B
    #define WIFI_ID BBBBBB
#elif  MODULE == C
    #define WIFI_ID BBBBBB
#elif  MODULE == D
    #define WIFI_ID BBBBBB
#elif  MODULE == E
    #define WIFI_ID BBBBBB
#elif  MODULE == F
    #define WIFI_ID BBBBBB
#else // safe values
    #define WIFI_ID BBBBBB
#endif




#endif	/* DEFS_MODS_H */