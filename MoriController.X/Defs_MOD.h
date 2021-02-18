/* 
 * File:   Defs_Mods.h
 * Author: belke
 *
 * Created on 03 September 2020, 14:41
 */

#include "Defs_GLB.h"

#ifndef DEFS_MOD_H
#define	DEFS_MOD_H

#define NUM_MODS 14

/* ******************** WIFI IDs ******************************************** */
#define ID_A1 48 // ascii code for 0AC171
#define ID_A2 65
#define ID_A3 67
#define ID_A4 49
#define ID_A5 55
#define ID_A6 49

#define ID_B1 48 // ascii code for 0AC172
#define ID_B2 65
#define ID_B3 67
#define ID_B4 49
#define ID_B5 55
#define ID_B6 50

#define ID_C1 68 // ascii code for D00BD5
#define ID_C2 48
#define ID_C3 48
#define ID_C4 66
#define ID_C5 68
#define ID_C6 53

#define ID_D1 48 // ascii code for 0AC175
#define ID_D2 65
#define ID_D3 67
#define ID_D4 49
#define ID_D5 55
#define ID_D6 53

#define ID_E1 48 // ascii code for 0AC170
#define ID_E2 65
#define ID_E3 67
#define ID_E4 49
#define ID_E5 55
#define ID_E6 48

#define ID_F1 48 // ascii code for 0AC173
#define ID_F2 65
#define ID_F3 67
#define ID_F4 49
#define ID_F5 55
#define ID_F6 51

#define ID_G1 68 // ascii code for D00BD2
#define ID_G2 48
#define ID_G3 48
#define ID_G4 66
#define ID_G5 68
#define ID_G6 50

#define ID_H1 68 // ascii code for D00BD3
#define ID_H2 48
#define ID_H3 48
#define ID_H4 66
#define ID_H5 67
#define ID_H6 66

#define ID_I1 68 // ascii code for D00BD1
#define ID_I2 48
#define ID_I3 48
#define ID_I4 66
#define ID_I5 68
#define ID_I6 49

#define ID_J1 68 // ascii code for D00BD0
#define ID_J2 48
#define ID_J3 48
#define ID_J4 66
#define ID_J5 68
#define ID_J6 48

#define ID_K1 68 // ascii code for D00BCE
#define ID_K2 48
#define ID_K3 48
#define ID_K4 66
#define ID_K5 67
#define ID_K6 69

#define ID_L1 68 // ascii code for D00BCF
#define ID_L2 48
#define ID_L3 48
#define ID_L4 66
#define ID_L5 67
#define ID_L6 70

#define ID_M1 68 // ascii code for D00BCD
#define ID_M2 48
#define ID_M3 48
#define ID_M4 66
#define ID_M5 67
#define ID_M6 68

#define ID_N1 68 // ascii code for D00BCA
#define ID_N2 48
#define ID_N3 48
#define ID_N4 66
#define ID_N5 67
#define ID_N6 65


/* ******************** ID SELF ALLOCATION ********************************** */
#if MODULE == 'A'
    #define ID1 ID_A1
    #define ID2 ID_A2
    #define ID3 ID_A3
    #define ID4 ID_A4
    #define ID5 ID_A5
    #define ID6 ID_A6
#elif  MODULE == 'B'
    #define ID1 ID_B1
    #define ID2 ID_B2
    #define ID3 ID_B3
    #define ID4 ID_B4
    #define ID5 ID_B5
    #define ID6 ID_B6
#elif  MODULE == 'C'
    #define ID1 ID_C1
    #define ID2 ID_C2
    #define ID3 ID_C3
    #define ID4 ID_C4
    #define ID5 ID_C5
    #define ID6 ID_C6
#elif  MODULE == 'D'
    #define ID1 ID_D1
    #define ID2 ID_D2
    #define ID3 ID_D3
    #define ID4 ID_D4
    #define ID5 ID_D5
    #define ID6 ID_D6
#elif  MODULE == 'E'
    #define ID1 ID_E1
    #define ID2 ID_E2
    #define ID3 ID_E3
    #define ID4 ID_E4
    #define ID5 ID_E5
    #define ID6 ID_E6
#elif  MODULE == 'F'
    #define ID1 ID_F1
    #define ID2 ID_F2
    #define ID3 ID_F3
    #define ID4 ID_F4
    #define ID5 ID_F5
    #define ID6 ID_F6
#elif  MODULE == 'G'
    #define ID1 ID_G1
    #define ID2 ID_G2
    #define ID3 ID_G3
    #define ID4 ID_G4
    #define ID5 ID_G5
    #define ID6 ID_G6
#elif  MODULE == 'H'
    #define ID1 ID_H1
    #define ID2 ID_H2
    #define ID3 ID_H3
    #define ID4 ID_H4
    #define ID5 ID_H5
    #define ID6 ID_H6
#elif  MODULE == 'I'
    #define ID1 ID_I1
    #define ID2 ID_I2
    #define ID3 ID_I3
    #define ID4 ID_I4
    #define ID5 ID_I5
    #define ID6 ID_I6
#elif  MODULE == 'J'
    #define ID1 ID_J1
    #define ID2 ID_J2
    #define ID3 ID_J3
    #define ID4 ID_J4
    #define ID5 ID_J5
    #define ID6 ID_J6
#elif  MODULE == 'K'
    #define ID1 ID_K1
    #define ID2 ID_K2
    #define ID3 ID_K3
    #define ID4 ID_K4
    #define ID5 ID_K5
    #define ID6 ID_K6
#elif  MODULE == 'L'
    #define ID1 ID_L1
    #define ID2 ID_L2
    #define ID3 ID_L3
    #define ID4 ID_L4
    #define ID5 ID_L5
    #define ID6 ID_L6
#elif  MODULE == 'M'
    #define ID1 ID_M1
    #define ID2 ID_M2
    #define ID3 ID_M3
    #define ID4 ID_M4
    #define ID5 ID_M5
    #define ID6 ID_M6
#elif  MODULE == 'N'
    #define ID1 ID_N1
    #define ID2 ID_N2
    #define ID3 ID_N3
    #define ID4 ID_N4
    #define ID5 ID_N5
    #define ID6 ID_N6
#else // safe values
    #define ID1 1
    #define ID2 2
    #define ID3 3
    #define ID4 4
    #define ID5 5
    #define ID6 6
#endif

/* ******************** LINEAR MOTORS LIMITS ******************************** */
#if MODULE == 'A'
    #define MotLin_MIN_1 68
    #define MotLin_MAX_1 990
    #define MotLin_MIN_2 75
    #define MotLin_MAX_2 996
    #define MotLin_MIN_3 77
    #define MotLin_MAX_3 993
#elif  MODULE == 'B'
    #define MotLin_MIN_1 72
    #define MotLin_MAX_1 985
    #define MotLin_MIN_2 79
    #define MotLin_MAX_2 1003
    #define MotLin_MIN_3 76
    #define MotLin_MAX_3 992
#elif  MODULE == 'C'
    #define MotLin_MIN_1 70
    #define MotLin_MAX_1 988
    #define MotLin_MIN_2 78
    #define MotLin_MAX_2 996
    #define MotLin_MIN_3 79
    #define MotLin_MAX_3 995
#elif  MODULE == 'D'
    #define MotLin_MIN_1 79
    #define MotLin_MAX_1 999
    #define MotLin_MIN_2 75
    #define MotLin_MAX_2 994
    #define MotLin_MIN_3 74
    #define MotLin_MAX_3 995
#elif  MODULE == 'E'
    #define MotLin_MIN_1 79
    #define MotLin_MAX_1 1000
    #define MotLin_MIN_2 69
    #define MotLin_MAX_2 988
    #define MotLin_MIN_3 72
    #define MotLin_MAX_3 993
#elif  MODULE == 'F'
    #define MotLin_MIN_1 77
    #define MotLin_MAX_1 993
    #define MotLin_MIN_2 81
    #define MotLin_MAX_2 1000
    #define MotLin_MIN_3 71
    #define MotLin_MAX_3 995
#elif  MODULE == 'G'
    #define MotLin_MIN_1 80
    #define MotLin_MAX_1 999
    #define MotLin_MIN_2 77
    #define MotLin_MAX_2 993
    #define MotLin_MIN_3 75
    #define MotLin_MAX_3 996
#elif  MODULE == 'H'
    #define MotLin_MIN_1 77
    #define MotLin_MAX_1 999
    #define MotLin_MIN_2 80
    #define MotLin_MAX_2 999
    #define MotLin_MIN_3 78
    #define MotLin_MAX_3 999
#elif  MODULE == 'I'
    #define MotLin_MIN_1 81
    #define MotLin_MAX_1 999
    #define MotLin_MIN_2 86
    #define MotLin_MAX_2 1004
    #define MotLin_MIN_3 80
    #define MotLin_MAX_3 1001
#elif  MODULE == 'J'
    #define MotLin_MIN_1 76
    #define MotLin_MAX_1 999
    #define MotLin_MIN_2 86
    #define MotLin_MAX_2 1004
    #define MotLin_MIN_3 80
    #define MotLin_MAX_3 999
#elif  MODULE == 'K'
    #define MotLin_MIN_1 80
    #define MotLin_MAX_1 1000
    #define MotLin_MIN_2 75
    #define MotLin_MAX_2 995
    #define MotLin_MIN_3 77
    #define MotLin_MAX_3 990
#elif  MODULE == 'L'
    #define MotLin_MIN_1 80
    #define MotLin_MAX_1 1000
    #define MotLin_MIN_2 73
    #define MotLin_MAX_2 990
    #define MotLin_MIN_3 84
    #define MotLin_MAX_3 1003
#elif  MODULE == 'M'
    #define MotLin_MIN_1 76
    #define MotLin_MAX_1 995
    #define MotLin_MIN_2 82
    #define MotLin_MAX_2 1002
    #define MotLin_MIN_3 76
    #define MotLin_MAX_3 994
#elif  MODULE == 'N'
    #define MotLin_MIN_1 76
    #define MotLin_MAX_1 998
    #define MotLin_MIN_2 79
    #define MotLin_MAX_2 995
    #define MotLin_MIN_3 96
    #define MotLin_MAX_3 1015
#else // safe values
    #define MotLin_MIN_1 200
    #define MotLin_MAX_1 800
    #define MotLin_MIN_2 200
    #define MotLin_MAX_2 800
    #define MotLin_MIN_3 200
    #define MotLin_MAX_3 800
#endif

#endif	/* DEFS_MODS_H */