#ifndef __W25Q64_H__
#define __W25Q64_H__

#include "stm32f4xx.h"
#include "spi.h"

// ָ���
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

    SPI_Flash_WorkState_OK = 0,     // �����ɹ�
    SPI_Flash_WorkState_Busy,       // æ
    SPI_Flash_WorkState_Error,      // ����
    SPI_Flash_WorkState_Idle,       // ����
    SPI_Flash_WorkState_Writing,    // ����д��
    SPI_Flash_WorkState_Reading,    // ���ڶ�ȡ
    SPI_Flash_WorkState_Eraseing,   // ���ڲ���
    SPI_Flash_WorkState_Init,       // ��ʼ״̬

} SPI_FlashWorkState;

void     W25QXX_SPI_Init(void);                                               // ��ʼ��W25Qxx
uint8_t  W25QXX_ReadSR(void);                                             // ��SR�Ĵ���
void     W25QXX_Write_Enable(void);                                       // W25Qxx дʹ��
void     W25QXX_Write_Disable(void);                                      // W25Qxx д��ֹ
uint16_t W25QXX_ReadID(void);                                             // W25Qxx ��ȡоƬID
uint32_t W25QXX_ReadCapacity(void);                                       // ��ȡоƬ����
void     W25QXX_Read(void* r_bf, uint32_t r_addr, uint16_t count);        // ֱ�Ӷ�ȡ����
void     W25QXX_WritePage(void* w_bf, uint32_t w_addr, uint16_t count);   // дҳ ���256�ֽ�
void     W25QXX_Write(void* w_bf, uint32_t w_addr, uint16_t count);       // ֱ��д������ �Զ���ҳ ��У��
void     W25QXX_EraseSector(uint32_t address);                            // ��������
void     W25QXX_WaitBusy(void);                                           // æλ�ȴ�
void     W25QXX_PowerDown(void);                                          // �������ģʽ
void     W25QXX_WAKEUP(void);                                             // ����
void W25QXX_EraseSector(uint32_t address);
void W25Q64_Erase64K(uint32_t addr);
void W25Q64_PageWrite(uint8_t *wbuff, uint16_t pageNB);
// void W25Q64_EraseSlotBySlotIndex(uint8_t slot_index);
void W25Q64_EraseSlot(uint8_t slot_index);
void W25Q64_TestSlot(uint8_t slot_index);

void W25Q64_TestAddr(uint32_t addr);
#endif
