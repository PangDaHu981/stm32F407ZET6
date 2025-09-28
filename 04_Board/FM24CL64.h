#ifndef __FM24CL64_H__
#define __FM24CL64_H__

#include "I2C.h"

#define FM24CL64_I2C_Init()         _I2C_Init(FM24CL64_I2C)
#define FM24CL64_I2C_SendByte(byte) _I2C_SendByte(FM24CL64_I2C, byte)
#define FM24CL64_I2C_Start()        _I2C_Start(FM24CL64_I2C)
#define FM24CL64_I2C_Stop()         _I2C_Stop(FM24CL64_I2C)
#define FM24CL64_I2C_ReadByte(ack)  _I2C_ReadByte(FM24CL64_I2C, ack)

#define FM24CL64_ADDRESS 0xA0

void FM24CL64_Init(void);
void FM24CL64_Read(uint16_t addr, void* buff, uint16_t lens);
void FM24CL64_Write(uint16_t addr, void* buff, uint16_t lens);
void AT24C02_ReadOTAInfo(void);
void AT24C02_WriteOTAInfo(void);
#endif
