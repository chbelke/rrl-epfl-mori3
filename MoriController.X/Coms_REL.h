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
void Coms_REL_ToEdge(uint8_t, uint8_t);
void Coms_REL_ToHub(uint8_t, uint8_t);
void Coms_REL_RelayStandard(uint8_t, uint8_t);
void Coms_REL_RelayCommand(uint8_t, uint8_t);
void Coms_REL_SetWiFiEdge(uint8_t);
uint8_t Coms_REL_GetWiFiEdge();
void Coms_REL_Interpret(uint8_t);
void Coms_REL_Write (uint8_t, uint8_t);
uint16_t Coms_REL_getRecieveBufferSize(uint8_t);
uint8_t Coms_REL_Peek_Buffer(uint8_t, uint8_t);
uint8_t Coms_REL_Read(uint8_t);
bool Coms_REL_Ready(uint8_t);
bool Coms_REL_TxReady(uint8_t);

#endif	/* COMS_REL_H */
