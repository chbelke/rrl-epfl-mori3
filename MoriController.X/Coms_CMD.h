#ifndef COMS_CMD_H
#define	COMS_CMD_H

bool Coms_CMD_Handle(uint8_t, uint8_t);
bool Coms_CMD_Reset(uint8_t*, bool*);
bool Coms_CMD_Verbose(uint8_t);
bool Coms_CMD_Shape(uint8_t, uint8_t);
void Coms_ESP_SetMots(uint8_t, uint16_t*);
bool Coms_CMD_SetWiFiEdge(uint8_t, uint8_t);
void Coms_CMD_OverflowError(void);
bool Coms_CMD_Restart_PIC(uint8_t);

#endif	/* COMS_CMD_H */

