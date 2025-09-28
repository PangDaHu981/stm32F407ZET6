#ifndef __W25Q64_H__
#define __W25Q64_H__

#include "stm32f4xx.h"
#include "spi.h"

// 指令表
#define W25QX_WriteEnable      0x06
#define W25QX_WriteDisable     0x04
#define W25QX_ReadStatusReg    0x05
#define W25QX_WriteStatusReg   0x01
#define W25QX_ReadData         0x03
#define W25QX_FastReadData     0x0B
#define W25QX_FastReadDual     0x3B
#define W25QX_PageProgram      0x02
#define W25QX_BlockErase       0xD8
#define W25QX_SectorErase      0x20
#define W25QX_ChipErase        0xC7
#define W25QX_PowerDown        0xB9
#define W25QX_ReleasePowerDown 0xAB
#define W25QX_DeviceID         0xAB
#define W25QX_ManufactDeviceID 0x90
#define W25QX_JedecDeviceID    0x9F

#define W25QXX_BLOCK_SIZE (4096)

#define W25QXX_CS_RCCCLOCK RCC_AHB1Periph_GPIOC
#define W25QXX_CS_PORT     GPIOC
#define W25QXX_CS_PIN      GPIO_Pin_4

#define W25QXX_CS_0                                    \
    {                                                  \
        GPIO_ResetBits(W25QXX_CS_PORT, W25QXX_CS_PIN); \
    }
#define W25QXX_CS_1                                  \
    {                                                \
        GPIO_SetBits(W25QXX_CS_PORT, W25QXX_CS_PIN); \
    }

// #define W25QXX_SPI_Init()                            \
//     {                                                \
//         SoftwareSPI_Init(W25QXX_SPI, SPI_MODE_0, 1); \
//     }
//#define W25QXX_ReadWriteByte(data) SoftwareSPI_ReadWriteByte(W25QXX_SPI, data)
// #define W25QXX_Bus_Take()          SoftwareSPI_Take(W25QXX_SPI)
// #define W25QXX_Bus_Give()          SoftwareSPI_Give(W25QXX_SPI)

typedef enum {

    SPI_Flash_WorkState_OK = 0,     // 操作成功
    SPI_Flash_WorkState_Busy,       // 忙
    SPI_Flash_WorkState_Error,      // 错误
    SPI_Flash_WorkState_Idle,       // 空闲
    SPI_Flash_WorkState_Writing,    // 正在写入
    SPI_Flash_WorkState_Reading,    // 正在读取
    SPI_Flash_WorkState_Eraseing,   // 正在擦除
    SPI_Flash_WorkState_Init,       // 初始状态

} SPI_FlashWorkState;

void     W25QXX_SPI_Init(void);                                               // 初始化W25Qxx
uint8_t  W25QXX_ReadSR(void);                                             // 读SR寄存器
void     W25QXX_Write_Enable(void);                                       // W25Qxx 写使能
void     W25QXX_Write_Disable(void);                                      // W25Qxx 写禁止
uint16_t W25QXX_ReadID(void);                                             // W25Qxx 读取芯片ID
uint32_t W25QXX_ReadCapacity(void);                                       // 读取芯片容量
void     W25QXX_Read(void* r_bf, uint32_t r_addr, uint16_t count);        // 直接读取数据
void     W25QXX_WritePage(void* w_bf, uint32_t w_addr, uint16_t count);   // 写页 最大256字节
void     W25QXX_Write(void* w_bf, uint32_t w_addr, uint16_t count);       // 直接写入数据 自动换页 无校验
void     W25QXX_EraseSector(uint32_t address);                            // 擦除扇区
void     W25QXX_WaitBusy(void);                                           // 忙位等待
void     W25QXX_PowerDown(void);                                          // 进入掉电模式
void     W25QXX_WAKEUP(void);                                             // 唤醒
void W25QXX_EraseSector(uint32_t address);
void W25Q64_Erase64K(uint32_t addr);
void W25Q64_PageWrite(uint8_t *wbuff, uint16_t pageNB);
// void W25Q64_EraseSlotBySlotIndex(uint8_t slot_index);
void W25Q64_EraseSlot(uint8_t slot_index);
void W25Q64_TestSlot(uint8_t slot_index);

void W25Q64_TestAddr(uint32_t addr);
#endif
