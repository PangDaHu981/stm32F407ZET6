/*
 *ϵͳ����
 */
#ifndef __TASK_IAP_H__
#define __TASK_IAP_H__

#include "stm32f4xx.h"

#define SPI_FLASH_PROGRAM_1_ADDRESS  (0x00000000)                                         // �������ַ
#define SPI_FLASH_PROGRAM_2_ADDRESS  (0x00100000)                                         // �������ַ
#define CHIP_FLASH_START_ADDRESS     (0x08000000)                                         // Ƭ��FLASH��ʼ��ַ
#define CHIP_APP_START_OFFSET        (0x00008000)                                         // Ƭ���û�����ƫ�Ƶ�ַ
#define CHIP_APP_START_ADDRESS       (CHIP_FLASH_START_ADDRESS + CHIP_APP_START_OFFSET)   // Ӧ�ó�����ʼ��ַ
#define CHIP_APP_STACK_ADDRESS       (0x20000000)                                         // �û�Ӧ�ó���ջ����ַ
#define CHIP_IAP_INFO_ADDRESS        (0x2000 - sizeof(IAP_StructTypedef))                 // IAP��Ϣ�洢��ַ
#define FIRMWARE_INFO_OFFSET         (0x0188)                                             // �̼���Ϣ�洢λ��
#define FIRMWARE_INFO_DEVICE_OFFSET  (FIRMWARE_INFO_OFFSET + 0)                           // �̼���Ϣ�洢λ��
#define FIRMWARE_INFO_VERSION_OFFSET (FIRMWARE_INFO_OFFSET + 4)                           // �̼���Ϣ�洢λ��
#define FIRMWARE_INFO_LOG_OFFSET     (FIRMWARE_INFO_OFFSET + 4 + sizeof(SystemVersion))   // �̼���Ϣ�洢λ��
#define CHIP_FLASH_SECTOR(addr)      ((addr % 2048 == 0) ? addr : (addr / 2048) * 2048)   // Ƭ��Flash����У��
/* ��ȡ������С */
#define FLASH_SECTOR_SIZE(address) ((address < 0x08010000) ? (0x4000) : ((address < 0x08020000) ? (0x10000) : 0x20000))
/* ��ȡ��ַ���������� */
#define FLASH_SECTOR(address) (((address < 0x08010000) ? ((address & 0xFFFFFF) / 0x4000) : ((address < 0x08020000) ? (4) : ((address & 0xFFFFFF) / 0x20000 + 4))) * 8)
/* �жϵ�ǰ�Ƿ�Ϊ�����׵�ַ */
#define IS_FLASH_SECTOR_START(address) (address < 0x08010000) ? (((address & 0xFFFFFF) % 0x4000) ? 0 : 1) : ((address < 0x08020000) ? ((address == 0x08010000) ? 1 : 0) : (((address & 0xFFFFFF) % 0x20000) ? 0 : 1))

#define IAP_INFO_COL  12   // IAP������Ϣ����
#define IAP_INFO_SIZE 64   // IAP������Ϣ����

typedef void (*pFunction)(void);   // �û�������ת������������

/*IAP��Ϣ*/
typedef struct {
    /* �������� */
    enum {
        CMD_NormalStart      = 0x00000000,   // ��������
        CMD_StartComplete     = 0x5AA55A00,   // �������
        CMD_ProgramUpdate    = 0x5AA55A03,   // ��������
        CMD_ProgramBackup    = 0x5AA55A01,   // ���򱸷�
        CMD_BootProgram1     = 0x5AA55A02,   // ������������
        CMD_BootProgram2     = 0x5AA55A04,   // ������������
        CMD_BootProgramError = 0x5AA55A08,   // ����ʧ��
        CMD_BootProgramAgain  = 0x5AA55A0E,   // ������������
        CMD_BootProgramAgain1 = 0x5AA55A0A,   // ��������������
        CMD_BootProgramAgain2 = 0x5AA55A0C,   // ��������������
    } Cmd;
    /* �̼���Ϣ */
    struct {
        uint32_t FlashAddress;   // ����洢��ַ
        uint32_t TotalSize;      // �����С
        uint32_t crc;            // ����У����
    } Program[2];
    uint32_t FlashSize;      // Flash��С
    uint16_t HardVersions;   // Ӳ���汾��
    uint16_t crc;            // �ṹCRCУ��

} IAP_StructTypedef;

/* IAPϵͳ��ʼ�� */
void IAP_Init(void);
/* �������� */
void Bootloader(void);
/* ����������Ϣ */
static uint8_t Save_IAP(void);
/* ��ת���û����� */
uint8_t IAP_JumpApp(void);
/* ��SPI Flash��¼���ڲ�Flash */
static uint8_t SPI_Flash_2_Flash(uint32_t spi_f_addr, uint32_t f_addr, uint32_t size, uint32_t crc);
/* ���ڲ�Flash��¼��SPI Flash */
static uint8_t Flash_2_SPI_Flash(uint32_t f_addr, uint32_t spi_f_addr, uint32_t size, uint32_t* crc);

#endif
