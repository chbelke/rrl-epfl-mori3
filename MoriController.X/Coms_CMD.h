#ifndef COMS_CMD_H
#define	COMS_CMD_H

bool Coms_CMD_Handle(uint8_t, uint8_t);
bool Coms_CMD_Shape(uint8_t, uint8_t);
bool Coms_CMD_Reset(uint8_t*, bool*)
void Coms_ESP_SetMots(uint8_t, uint16_t*);

#endif	/* COMS_CMD_H */

