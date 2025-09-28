#ifndef __DGUS_H__
#define __DGUS_H__

#include "stm32f4xx.h"

#ifndef NULL
#define NULL 0
#endif
/*
 *指令结构
 * 指令帧头 指令长度 指令 起始地址 [数据长度] [数据内容] [CRC校验码]
 *
 *   每条指令都包括有指令帧头、指令长度、指令、起始地址四个部分。
 *   [数据长度][数据内容][CRC校验码 ]这三个参数不是每条指令都有。
 *
 */

/* 屏幕初始化 */
void DGUS_Init(void);
/* 传输数据 */
void DGUS_Transmit(uint8_t* data, uint16_t length);
/* 设定当前页 */
void DGUS_SetPage(uint16_t page);
/* 向屏幕打印字 */
void DGUS_Print(uint8_t new, const char* message, ...);
/* 清屏 */
void DGUS_Clean(void);
#endif
