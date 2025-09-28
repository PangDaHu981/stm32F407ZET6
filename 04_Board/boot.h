#ifndef __BOOT_H
#define __BOOT_H

#include "stm32f4xx.h"
#include "main.h"
#include "usart.h"
#include "delay.h"
#include "Board_Flash.h"
#include "FM24CL64.h"
#include "W25Q64.h"

typedef void (*pFunction)(void);

#define SLOT_SIZE   0x80000 //(512 * 1024)      // 每个APP区大小512KB
#define SLOT_NUM    2
#define FLASH_END  0x800000 //(8 * 1024 * 1024) // 8MB = 0x800000

void BootLoader_Branch(void);


void MSR_SP(uint32_t addr);
//__ASM void MSR_SP(u32 addr);
void LOAD_A(u32 addr);
void BootLoader_Clear(void);
u8 BootLoader_Enter_Command(u8 timeout);
void BootLoader_Info(void);
void Bootloader_Event_Process(u8 *data, u16 datalen);
u16 XModem_CRC16(u8 *data, u16 datalen);
uint32_t GetSlotBaseAddr(uint8_t slot_index);
#endif
