/*
 *系统升级
 */
#ifndef __TASK_IAP_H__
#define __TASK_IAP_H__

#include "stm32f4xx.h"

#define SPI_FLASH_PROGRAM_1_ADDRESS  (0x00000000)                                         // 主程序地址
#define SPI_FLASH_PROGRAM_2_ADDRESS  (0x00100000)                                         // 辅程序地址
#define CHIP_FLASH_START_ADDRESS     (0x08000000)                                         // 片内FLASH起始地址
#define CHIP_APP_START_OFFSET        (0x00008000)                                         // 片内用户程序偏移地址
#define CHIP_APP_START_ADDRESS       (CHIP_FLASH_START_ADDRESS + CHIP_APP_START_OFFSET)   // 应用程序起始地址
#define CHIP_APP_STACK_ADDRESS       (0x20000000)                                         // 用户应用程序栈顶地址
#define CHIP_IAP_INFO_ADDRESS        (0x2000 - sizeof(IAP_StructTypedef))                 // IAP信息存储地址
#define FIRMWARE_INFO_OFFSET         (0x0188)                                             // 固件信息存储位置
#define FIRMWARE_INFO_DEVICE_OFFSET  (FIRMWARE_INFO_OFFSET + 0)                           // 固件信息存储位置
#define FIRMWARE_INFO_VERSION_OFFSET (FIRMWARE_INFO_OFFSET + 4)                           // 固件信息存储位置
#define FIRMWARE_INFO_LOG_OFFSET     (FIRMWARE_INFO_OFFSET + 4 + sizeof(SystemVersion))   // 固件信息存储位置
#define CHIP_FLASH_SECTOR(addr)      ((addr % 2048 == 0) ? addr : (addr / 2048) * 2048)   // 片内Flash扇区校正
/* 获取扇区大小 */
#define FLASH_SECTOR_SIZE(address) ((address < 0x08010000) ? (0x4000) : ((address < 0x08020000) ? (0x10000) : 0x20000))
/* 获取地址所在扇区名 */
#define FLASH_SECTOR(address) (((address < 0x08010000) ? ((address & 0xFFFFFF) / 0x4000) : ((address < 0x08020000) ? (4) : ((address & 0xFFFFFF) / 0x20000 + 4))) * 8)
/* 判断当前是否为扇区首地址 */
#define IS_FLASH_SECTOR_START(address) (address < 0x08010000) ? (((address & 0xFFFFFF) % 0x4000) ? 0 : 1) : ((address < 0x08020000) ? ((address == 0x08010000) ? 1 : 0) : (((address & 0xFFFFFF) % 0x20000) ? 0 : 1))

#define IAP_INFO_COL  12   // IAP升级信息行数
#define IAP_INFO_SIZE 64   // IAP升级信息行数

typedef void (*pFunction)(void);   // 用户程序跳转函数类型声明

/*IAP信息*/
typedef struct {
    /* 升级命令 */
    enum {
        CMD_NormalStart      = 0x00000000,   // 正常启动
        CMD_StartComplete     = 0x5AA55A00,   // 启动完成
        CMD_ProgramUpdate    = 0x5AA55A03,   // 程序升级
        CMD_ProgramBackup    = 0x5AA55A01,   // 程序备份
        CMD_BootProgram1     = 0x5AA55A02,   // 引导到主程序
        CMD_BootProgram2     = 0x5AA55A04,   // 引导到辅程序
        CMD_BootProgramError = 0x5AA55A08,   // 引导失败
        CMD_BootProgramAgain  = 0x5AA55A0E,   // 重新引导程序
        CMD_BootProgramAgain1 = 0x5AA55A0A,   // 重新引导主程序
        CMD_BootProgramAgain2 = 0x5AA55A0C,   // 重新引导辅程序
    } Cmd;
    /* 固件信息 */
    struct {
        uint32_t FlashAddress;   // 程序存储地址
        uint32_t TotalSize;      // 程序大小
        uint32_t crc;            // 程序校验码
    } Program[2];
    uint32_t FlashSize;      // Flash大小
    uint16_t HardVersions;   // 硬件版本号
    uint16_t crc;            // 结构CRC校验

} IAP_StructTypedef;

/* IAP系统初始化 */
void IAP_Init(void);
/* 引导程序 */
void Bootloader(void);
/* 保存升级信息 */
static uint8_t Save_IAP(void);
/* 跳转到用户程序 */
uint8_t IAP_JumpApp(void);
/* 从SPI Flash烧录到内部Flash */
static uint8_t SPI_Flash_2_Flash(uint32_t spi_f_addr, uint32_t f_addr, uint32_t size, uint32_t crc);
/* 从内部Flash烧录到SPI Flash */
static uint8_t Flash_2_SPI_Flash(uint32_t f_addr, uint32_t spi_f_addr, uint32_t size, uint32_t* crc);

#endif
