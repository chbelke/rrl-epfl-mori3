/* 
 * File:   Coms_REL.h
 * Author: belke
 *
 * Created on 30 July 2020, 14:23
 */

#ifndef COMS_REL_H
#define	COMS_REL_H

bool Coms_REL_Handle (uint8_t, uint8_t);
void Coms_REL_Relay (uint8_t, uint8_t);
void Coms_Write (uint8_t, uint8_t);
void Coms_Rel_Order(uint8_t, uint8_t);

void Coms_Rel_Edge(uint8_t, uint8_t);
void Coms_Rel_To_Comp(uint8_t, uint8_t);
void Coms_Rel_Relay(uint8_t, uint8_t);
void Coms_Rel_Interpret(uint8_t);
void Coms_Rel_Set_WiFi_Edge(uint8_t);
uint8_t Coms_Rel_Get_WiFi_Edge();

#endif	/* COMS_REL_H */
