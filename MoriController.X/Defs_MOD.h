/* 
 * File:   Defs_Mods.h
 * Author: belke
 *
 * Created on 03 September 2020, 14:41
 */

#include "Defs_GLB.h"

#ifndef DEFS_MODS_H
#define	DEFS_MODS_H

/* ******************** WIFI ID ********************************************* */
#if MODULE == 'A' // ascii code for 0AC171
    #define ID1 48
    #define ID2 65
    #define ID3 67
    #define ID4 49
    #define ID5 55
    #define ID6 49
#elif  MODULE == 'B' // ascii code for 0AC172
    #define ID1 48
    #define ID2 65
    #define ID3 67
    #define ID4 49
    #define ID5 55
    #define ID6 50
#elif  MODULE == 'C' // ascii code for D00BD5
    #define ID1 68
    #define ID2 48
    #define ID3 48
    #define ID4 66
    #define ID5 68
    #define ID6 53
#elif  MODULE == 'D' // ascii code for 0AC175
    #define ID1 48
    #define ID2 65
    #define ID3 67
    #define ID4 49
    #define ID5 55
    #define ID6 53
#elif  MODULE == 'E' // ascii code for 0AC170
    #define ID1 48
    #define ID2 65
    #define ID3 67
    #define ID4 49
    #define ID5 55
    #define ID6 48
#elif  MODULE == 'F' // ascii code for 0AC173
    #define ID1 48
    #define ID2 65
    #define ID3 67
    #define ID4 49
    #define ID5 55
    #define ID6 51
#else // safe values
    #define ID1 1
    #define ID2 2
    #define ID3 3
    #define ID4 4
    #define ID5 5
    #define ID6 6
#endif

/* ******************** LINEAR MOTORS LIMITS ******************************** */
#if MODULE == 'A' // MINs verified
    #define MotLin_MIN_1 68
    #define MotLin_MAX_1 990
    #define MotLin_MIN_2 78
    #define MotLin_MAX_2 999
    #define MotLin_MIN_3 77
    #define MotLin_MAX_3 993
#elif  MODULE == 'B' // MINs verified
    #define MotLin_MIN_1 80
    #define MotLin_MAX_1 985
    #define MotLin_MIN_2 79
    #define MotLin_MAX_2 1003
    #define MotLin_MIN_3 73
    #define MotLin_MAX_3 992
#elif  MODULE == 'C' // MINs verified
    #define MotLin_MIN_1 72
    #define MotLin_MAX_1 988
    #define MotLin_MIN_2 79
    #define MotLin_MAX_2 996
    #define MotLin_MIN_3 85
    #define MotLin_MAX_3 995
#elif  MODULE == 'D' // MINs verified
    #define MotLin_MIN_1 87
    #define MotLin_MAX_1 999
    #define MotLin_MIN_2 75
    #define MotLin_MAX_2 989
    #define MotLin_MIN_3 82
    #define MotLin_MAX_3 995
#elif  MODULE == 'E' // MINs verified
    #define MotLin_MIN_1 83
    #define MotLin_MAX_1 998
    #define MotLin_MIN_2 69
    #define MotLin_MAX_2 987
    #define MotLin_MIN_3 79
    #define MotLin_MAX_3 993
#elif  MODULE == 'F'
    #define MotLin_MIN_1 101
    #define MotLin_MAX_1 996
    #define MotLin_MIN_2 83
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


#endif	/* DEFS_MODS_H */