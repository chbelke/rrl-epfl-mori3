#ifndef COMS_CMD_H
#define	COMS_CMD_H

bool Coms_CMD_Handle(uint8_t, uint8_t);
bool Coms_CMD_Reset(uint8_t*, bool*);
bool Coms_CMD_Verbose(uint8_t);
bool Coms_CMD_Shape(uint8_t, uint8_t);
void Coms_CMD_SetEdge(uint8_t, uint8_t);
bool Coms_CMD_SetWiFiEdge(uint8_t, uint8_t);
void Coms_CMD_OverflowError(void);

bool Coms_CMD_Request_Edges(uint8_t);
bool Coms_CMD_Request_Angles(uint8_t);      
bool Coms_CMD_Request_Orient(uint8_t);
bool Coms_CMD_Request_Neighbour(uint8_t);
bool Coms_CMD_Request_WiFiEdge(uint8_t);
bool Coms_CMD_No_WifiEdge(uint8_t);
bool Coms_CMD_Set_PARTYMODE(uint8_t);

bool Coms_CMD_Set_ID(uint8_t);
bool Coms_CMD_SetMotRotOn(uint8_t);
bool Coms_CMD_SetMotRotOff(uint8_t);
bool Coms_CMD_SetMotLinOn(uint8_t);
bool Coms_CMD_SetMotLinOff(uint8_t);

bool Coms_CMD_Restart_PIC(uint8_t);

#endif	/* COMS_CMD_H */

