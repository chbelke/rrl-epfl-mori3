/* 
 * File:   Tmrs_CBK.h
 * Author: holdcrof
 *
 * Created on September 23, 2020, 11:31 AM
 */

#ifndef TMRS_CBK_H
#define	TMRS_CBK_H

void Tmrs_CBK_Evaluate_Timers(void);
void Tmrs_CBK_Timer3_Handle(void);
void Tmrs_CBK_Timer5_Handle(void);
void Tmrs_CBK_UpdateStableFlag(bool*);

#endif	/* TMRS_CBK_H */

