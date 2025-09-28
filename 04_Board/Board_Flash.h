#ifndef __BOARD_FLASH_H
#define __BOARD_FLASH_H
#include "stm32f4xx.h"



void STM32_EraseFlash(u16 start, u16 num);		//start表示要擦除的起始页编号，num表示要连续擦几块
void STM32_WriteFlash(u32 startaddr, u32 *wdata, u32 wnum);	//startaddr表示写入的起始地址，wdata表示待写入数据数组，num表示待写入的字节个数

#endif
















