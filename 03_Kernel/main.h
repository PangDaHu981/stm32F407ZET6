#ifndef __MAIN_H
#define __MAIN_H
#include "stm32f4xx.h"

#define STM32_FLASH_STARTADDR (0x08000000)												// STM32 Flash起始地址
#define STM32_PAGE_SIZE (1024)															// 一页（扇区）大小
#define STM32_PAGE_NUM (64)																// 总页数（扇区数）
#define STM32_B_PAGE_NUM (20)															// bootloader区大小
#define STM32_A_PAGE_NUM (STM32_PAGE_NUM - STM32_B_PAGE_NUM)							// 程序块大小
#define STM32_A_START_PAGE STM32_B_PAGE_NUM												// 程序块起始页编号（扇区编号）
#define STM32_A_START_ADDR (STM32_FLASH_STARTADDR + STM32_B_PAGE_NUM * STM32_PAGE_SIZE) // 程序块起始地址


//#define STM32_FLASH_STARTADDR  (0x08000000)
#define STM32_SECTOR_SIZE_16K  (16*1024)
#define STM32_SECTOR_SIZE_64K  (64*1024)
#define STM32_SECTOR_SIZE_128K (128*1024)

#define STM32_BOOT_SECTOR_NUM  2 // 0~4号扇区
#define STM32_APP_START_SECTOR 2 // APP从第5扇区（0x08008000）开始
#define STM32_APP_SECTOR_NUM 10  // APP区占用的扇区数量（如2~11号扇区，共10个）
#define STM32_APP_START_ADDR   0x08008000
typedef struct
{
	u32 OTA_flag;
	u32 FirmwareLen[11];
	u8 OTA_Version[32];
} OTA_InfoCB;

typedef struct
{
	u8 Updata_A_Buff[STM32_PAGE_SIZE]; // 更新A区时，用于保存从W25Q64中读取的数据,c8t6支持的最大flash写入为1k，所以缓存区给1k大小
	u32 Updata_A_from_W25Q64_Num;
	u32 XModem_Timer_Count;
	u16 XModemCRC;
	u32 XModem_Count;
} UpdateA_CB;

#define OTA_INFOCB_SIZE (sizeof(OTA_InfoCB))
#define OTA_SET_FLAG (0xAABB1122)

#define UPDATA_A_FLAG (0x00000001) // 状态标志位，置位表明需要更新A了
#define IAP_XModem_C_FLAG (0x00000002)
#define IAP_XModem_DATA_FLAG (0x00000004)
#define IAP_WRITE_VERSION_FLAG (0x00000008)
#define CMD_5_FLAG (0x00000010)
#define CMD_5_XMODEM_FLAG (0x00000020)
#define CMD_6_FLAG (0x00000040)
#define CMD_8_FLAG (0x00000080)

extern OTA_InfoCB OTA_Info;
extern UpdateA_CB UpdateA;
extern u32 BootStateFlag;

#endif
