/*
 *ϵͳ����
 */
#include "IAP.h"
#include <string.h>
#include "DGUS.h"
#include "FM24CL64.h"
#include "Key.h"
#include "beep.h"
#include "crc.h"
#include "led.h"
#include "stdio.h"
#include "W25Q64.h"
#include "Version.h"

IAP_StructTypedef IAP;                   // IAP��Ϣ
pFunction         Jump_To_Application;   // �û�������ת����
/* IAP��ʼ�� */
void IAP_Init(void) {
    uint8_t err_cnt = 5;

    /*��ʼ��Flash*/
    W25QXX_SPI_Init();
    /* ��ʼ������洢�� */
    FM24CL64_Init();
    do {
        /*��ȡIAP��Ϣ*/
        FM24CL64_Read(CHIP_IAP_INFO_ADDRESS, &IAP, sizeof(IAP_StructTypedef));
        /* ������ƿ��CRCУ���� */
    } while ((CRC16_Modbus(&IAP, sizeof(IAP) - 2) != IAP.crc) && (--err_cnt));

    IAP.FlashSize = ((*(uint16_t*) 0x1FFF7A22) * 1024);   // Flash��С

    if (err_cnt == 0) {
        IAP.Cmd                     = CMD_NormalStart;               // ��������
        IAP.Program[0].FlashAddress = SPI_FLASH_PROGRAM_1_ADDRESS;   // ����洢��ַ
        IAP.Program[0].TotalSize    = 0x00000000;                    // �����С
        IAP.Program[0].crc          = 0x00000000;                    // У��λ
        IAP.Program[1].FlashAddress = SPI_FLASH_PROGRAM_2_ADDRESS;   // ����洢��ַ
        IAP.Program[1].TotalSize    = 0x00000000;                    // �����С
        IAP.Program[1].crc          = 0x00000000;                    // У��λ
        IAP.HardVersions            = 100;                           // Ӳ���汾��
        /*����IAP��Ϣ*/
        Save_IAP();
    }
}

/* �������� */
void Bootloader(void) {
    printf("\r\n������������\r\n");
//    { /* ǿ�ƻ��˳��� */
//        uint32_t count = 0;
//        /* ���°����ϵ��ִ�� */
//        while ((Key_Get() & (KEY_1)) == (KEY_1)) {
//            if (++count >= 0x2FFFFF) {
//                count--;
//                /* ����������2 */
//                IAP.Cmd = CMD_BootProgram2;
//                Beep_Toggle();
//                for (uint32_t i = 0xFFFFF; i; i--) {
//                    asm("nop");
//                }
//            }
//        }
//        Beep_Off();
//    }
    /* �����⺬��ָ�� */
    if ((IAP.Cmd & 0xFFFFFF00) != 0x5AA55A00) {
        /*����IAP��Ϣ*/
        Save_IAP();
        printf("\r\n������ת���û�����\r\n");
        /* ��ת���û����� */
        IAP_JumpApp();
    }
    /* ��ת����������ҳ */
    DGUS_SetPage(33);
    DGUS_Clean();
    DGUS_Print(0, "<----------��������---------->");
    if ((IAP.Cmd & 0xFFFFFF00) != 0x5AA55A00) {
        /* ��ת�û�����ʧ�� */
        DGUS_Print(1, "��ת�û�����ʧ��");
        goto ex;
    }

    /*����ʧ��*/
    if ((IAP.Cmd & CMD_BootProgramError) == CMD_BootProgramError) {
        printf("\r\n����ʧ��\r\n");
        DGUS_Print(1, "����ʧ��");
        IAP.Cmd = CMD_NormalStart;
        /*����IAP��Ϣ*/
        Save_IAP();
        while (1) {
        }
    }

    /*��������*/
    if ((IAP.Cmd & CMD_ProgramUpdate) == CMD_ProgramUpdate) {
        printf("��������\r\n");
        DGUS_Print(1, "��ʼϵͳ����");
    }
    /*���ݳ���*/
    if ((IAP.Cmd & CMD_ProgramBackup) == CMD_ProgramBackup) {
        printf("���ݳ���\r\n");
        DGUS_Print(1, "����ԭʼ����");
        DGUS_Print(1, "[CHIP] 0x%08X -> [SPI_FLASH] 0x%08X = %d Byte", CHIP_APP_START_ADDRESS, IAP.Program[1].FlashAddress, IAP.FlashSize - (CHIP_APP_START_ADDRESS & 0xFFFF));
        IAP.Program[1].TotalSize = 0;
        /* �������û�������¼�������� */
        if (Flash_2_SPI_Flash(CHIP_APP_START_ADDRESS,
                              IAP.Program[1].FlashAddress,
                              IAP.FlashSize - (CHIP_APP_START_ADDRESS & 0xFFFF),
                              &IAP.Program[1].crc) == 1) {
            DGUS_Print(1, "����ʧ��");
            IAP.Cmd = CMD_NormalStart;
            for (uint8_t i = 100; i; i--) {
                Beep_Toggle();
                for (uint32_t i = 0xFFFFF; i; i--) {
                    asm("nop");
                }
            }
            IAP.Program[1].TotalSize = 0;
            goto ex;
        }
        /* ��¼�����ļ���С */
        IAP.Program[1].TotalSize = IAP.FlashSize - (CHIP_APP_START_ADDRESS & 0xFFFF);
    }
    /*������������*/
    if ((IAP.Cmd & CMD_BootProgram1) == CMD_BootProgram1) {
        printf("������������\r\n");
        DGUS_Print(1, "����������");
        DGUS_Print(1, "[SPI_FLASH] 0x%08X -> [CHIP] 0x%08X = %d Byte", IAP.Program[0].FlashAddress, CHIP_APP_START_ADDRESS, IAP.Program[0].TotalSize);
        if (SPI_Flash_2_Flash(IAP.Program[0].FlashAddress,
                              CHIP_APP_START_ADDRESS,
                              IAP.Program[0].TotalSize,
                              IAP.Program[0].crc) == 1) {
            printf("����ʧ��\r\n");
            DGUS_Print(1, "����ʧ��");
            IAP.Cmd = CMD_BootProgram2;
            for (uint8_t i = 100; i; i--) {
                Beep_Toggle();
                for (uint32_t i = 0xFFFFF; i; i--) {
                    asm("nop");
                }
            }
            goto ex;
        }
    }
    /*������������*/
    if ((IAP.Cmd & CMD_BootProgram2) == CMD_BootProgram2) {
        printf("������������\r\n");
        DGUS_Print(1, "���ر��ݳ���");
        DGUS_Print(1, "[SPI_FLASH] 0x%08X -> [CHIP] 0x%08X = %d Byte", IAP.Program[1].FlashAddress, CHIP_APP_START_ADDRESS, IAP.Program[1].TotalSize);
        if (SPI_Flash_2_Flash(IAP.Program[1].FlashAddress,
                              CHIP_APP_START_ADDRESS,
                              IAP.Program[1].TotalSize,
                              IAP.Program[1].crc) == 1) {
            printf("����ʧ��\r\n");
            DGUS_Print(1, "����ʧ��");
            IAP.Cmd = CMD_BootProgram1;
            for (uint8_t i = 100; i; i--) {
                Beep_Toggle();
                for (uint32_t i = 0xFFFFF; i; i--) {
                    asm("nop");
                }
            }
            goto ex;
        }
    }
    IAP.Cmd = CMD_NormalStart;
    DGUS_Print(1, "�������");

ex:
    printf("\r\n>-----������λ-----<\r\n");
    DGUS_Print(1, "��������");
    /*����IAP��Ϣ*/
    Save_IAP();
    /*��ʱ*/
    for (uint32_t i = 0x1FFFFFF; i; i--) {
        asm("nop");
    }
    /*��λ*/
    NVIC_SystemReset();
}

/* ��ת���û����� */
uint8_t IAP_JumpApp(void) {
    /*����Ƿ���Ӧ�ó���*/
    if (((*(__IO uint32_t*) CHIP_APP_START_ADDRESS) & 0x2FFE0000) == 0x20000000) {
        /*�����ж�������ָ��*/
        NVIC_SetVectorTable(NVIC_VectTab_FLASH, CHIP_APP_START_OFFSET);
        /*��ת����ָ���û�Ӧ�ó����ַ*/
        Jump_To_Application = (pFunction) (*(__IO uint32_t*) (CHIP_APP_START_ADDRESS + 4));
        /*��ʼ���û������ջָ��*/
        __set_MSP(*(__IO uint32_t*) CHIP_APP_START_ADDRESS);
        /*��ת�û�����*/
        Jump_To_Application();
    }
    return 1;
}

/* ����IAP��Ϣ */
static uint8_t Save_IAP(void) {
    uint8_t  buffer[sizeof(IAP)];
    uint16_t crc = 0;
    /* ������ƿ��CRCУ���� */
    IAP.crc = CRC16_Modbus(&IAP, sizeof(IAP) - 2);
    /* ��IAP��Ϣд��FRAM */
    FM24CL64_Write(CHIP_IAP_INFO_ADDRESS, &IAP, sizeof(IAP_StructTypedef));
    /* ��д�������ٴζ��� */
    FM24CL64_Read(CHIP_IAP_INFO_ADDRESS, buffer, sizeof(IAP_StructTypedef));
    /* ��ȡ�洢���е�CRC */
    crc = *(uint16_t*) (&buffer[sizeof(IAP) - 2]);
    /* ������ƿ��CRCУ���� */
    if (CRC16_Modbus(buffer, sizeof(IAP) - 2) == crc) {
        return 1;
    }
    return 0;
}

static uint8_t buffer[4096];
/* ��SPI Flash��¼���ڲ�Flash */
static uint8_t SPI_Flash_2_Flash(uint32_t spi_f_addr, uint32_t f_addr, uint32_t size, uint32_t crc) {
    uint32_t file_finish  = 0;        // ����ɴ�С
    uint32_t file_size    = size;     // �ļ���С
    uint32_t data_crc32   = 0;        // ����У����
    uint32_t flashAddress = f_addr;   // ����洢��ַ
    uint32_t r_cnt;                   // ��ȡ����
    printf("\r\n[SPI_FLASH->CHIP]:0x%08x->0x%08x=%dByte\r\n", spi_f_addr, f_addr, size);
    /*�����ڲ�Flash*/
    FLASH_Unlock();
    /*�ر�Flash���ݻ���*/
    FLASH_DataCacheCmd(DISABLE);
    DGUS_Print(1, "���ڸ���:");
    /*�����ļ�*/
    while (1) {
        /* д�������׵�ַ */
        if (IS_FLASH_SECTOR_START(flashAddress)) {
            /*��������*/
            FLASH_EraseSector(FLASH_SECTOR(flashAddress), VoltageRange_3);   // һ��Ҫ�ж��Ƿ�����ɹ�
        }
        /* ���ʣ���ֽ���,��ʣ���ֽڴ��ڻ��� */
        if ((file_size - file_finish) > sizeof(buffer)) {
            /*��ȡ�����С�ļ�*/
            r_cnt = sizeof(buffer);
        }
        /* ʣ���ֽ�������0С�ڻ��� */
        else if ((file_size - file_finish) > 0) {
            /* ��ȡʣ���ֽ��� */
            r_cnt = (file_size - file_finish);
        }
        /*�ֽ���Ϊ0,��������*/
        else
            break;
        LED_On(FATFS);
        W25QXX_Read(buffer, (spi_f_addr + file_finish), r_cnt);
        LED_Off(FATFS);
        /* ��������У���� */
        data_crc32 = CRC32_Update(data_crc32, buffer, r_cnt);
        /* ������д��Flash */
        for (uint16_t i = 0; i < r_cnt; i++) {
            FLASH_ProgramByte(flashAddress++, buffer[i]);
        }
        /* ���� */
        file_finish += r_cnt;
        DGUS_Print(0, "���ڸ���: %d%%  %d/%d Byte", (file_finish * 100 / file_size), file_finish, file_size);
    }
    /*����Flash���ݻ���*/
    FLASH_DataCacheCmd(ENABLE);
    /*Flash����*/
    FLASH_Lock();
    printf("\r\n��¼��� У����: %08X : %08X\r\n", data_crc32, crc);
    DGUS_Print(1, "������� У����: 0x%08X : 0x%08X", data_crc32, crc);
    /*���У����*/
    if (data_crc32 != crc && !(crc == 0x00000000 || crc == 0xFFFFFFFF)) {
        printf("\r\nУ��ʧ��\r\n");
        DGUS_Print(1, "У��ʧ��: 0x%08X : 0x%08X", data_crc32, crc);
        /* У��ʧ�� */
        return 1;
    }

    file_finish  = 0;        // ����ɴ�С
    file_size    = size;     // �ļ���С
    flashAddress = f_addr;   // ����洢��ַ
    data_crc32   = 0;        // CRCУ��
    printf("\r\n��ʼ�˶Գ���\r\n");
    DGUS_Print(1, "����У��:");
    /*У������ļ�*/
    while (1) {
        /*���ʣ���ֽ���,��ʣ���ֽڴ��ڻ���*/
        if ((file_size - file_finish) > 0x800) {
            r_cnt = 0x800;
            /*��ȡ�����С�ļ�*/
            memcpy(buffer, (void*) (flashAddress + file_finish), r_cnt);
        }
        /*ʣ���ֽ�������0С�ڻ���*/
        else if ((file_size - file_finish) > 0) {
            r_cnt = (file_size - file_finish);
            /*��ȡʣ���ֽ���*/
            memcpy(buffer, (void*) (flashAddress + file_finish), r_cnt);
        }
        /*�ֽ���Ϊ0,��������*/
        else
            break;
        /*��������У����*/
        data_crc32 = CRC32_Update(data_crc32, buffer, r_cnt);
        /*����*/
        file_finish += r_cnt;
        DGUS_Print(0, "����У��: %d%%  %d/%d Byte", (file_finish * 100 / file_size), file_finish, file_size);
    }
    /*���У����*/
    if (data_crc32 != crc && !(crc == 0x00000000 || crc == 0xFFFFFFFF)) {
        printf("\r\nУ��ʧ��\r\n");
        DGUS_Print(0, "����У��: 100%%  %d/%d Byte Error", file_finish, file_size);
        /* У��ʧ�� */
        return 1;
    }
    DGUS_Print(0, "����У��: 100%%  %d/%d Byte OK", file_finish, file_size);
    printf("\r\n��¼�ɹ�\r\n");
    return 0;
}
/* ���ڲ�Flash��¼��SPI Flash */
static uint8_t Flash_2_SPI_Flash(uint32_t f_addr, uint32_t spi_f_addr, uint32_t size, uint32_t* crc) {
    uint32_t file_finish = 0;      // ����ɴ�С
    uint32_t file_size   = size;   // �ļ���С
    uint32_t data_crc32  = 0;      // ����У����
    uint32_t r_cnt;                // ��ȡ����
    printf("\r\n[CHIP->SPI_FLASH]:0x%08x->0x%08x=%dByte\r\n", f_addr, spi_f_addr, size);
    DGUS_Print(1, "���ڸ���;");
    /*�����ļ�*/
    while (1) {
        /*���ʣ���ֽ���,��ʣ���ֽڴ��ڻ���*/
        if ((file_size - file_finish) > 0x1000) {
            /*��ȡ�����С�ļ�*/
            r_cnt = 0x1000;
        }
        /*ʣ���ֽ�������0С�ڻ���*/
        else if ((file_size - file_finish) > 0) {
            /*��ȡʣ���ֽ���*/
            r_cnt = (file_size - file_finish);
        }
        /*�ֽ���Ϊ0,��������*/
        else
            break;
        /* ��ȡ���� */
        memcpy(buffer, (uint8_t*) (f_addr + file_finish), r_cnt);
        /*��������У����*/
        data_crc32 = CRC32_Update(data_crc32, buffer, r_cnt);
        LED_On(FATFS);
        /* ����Flash���� */
        W25QXX_EraseSector(spi_f_addr + file_finish);
        /* д������ */
        W25QXX_Write(buffer, spi_f_addr + file_finish, r_cnt);
        LED_Off(FATFS);
        /*����*/
        file_finish += r_cnt;
        DGUS_Print(0, "���ڸ���: %d%%  %d/%d Byte", (file_finish * 100 / file_size), file_finish, file_size);
    }
    printf("\r\n��¼��� У����: %08X\r\n", data_crc32);
    DGUS_Print(1, "������� У����: 0x%08X", data_crc32);
    file_size   = file_finish;   // �ļ���С
    file_finish = 0;             // ����ɴ�С
    *crc        = data_crc32;    // ����CRC����
    data_crc32  = 0;             // ��λCRC
    printf("\r\n��ʼУ�����\r\n");
    DGUS_Print(1, "����У��:");
    /*У������ļ�*/
    while (1) {
        /*���ʣ���ֽ���,��ʣ���ֽڴ��ڻ���*/
        if ((file_size - file_finish) > 0x1000) {
            /*��ȡ�����С�ļ�*/
            r_cnt = 0x1000;
        }
        /*ʣ���ֽ�������0С�ڻ���*/
        else if ((file_size - file_finish) > 0) {
            /*��ȡʣ���ֽ���*/
            r_cnt = (file_size - file_finish);
        }
        /*�ֽ���Ϊ0,��������*/
        else
            break;
        LED_On(FATFS);
        /* ��ȡ���� */
        W25QXX_Read(buffer, (spi_f_addr + file_finish), r_cnt);
        LED_Off(FATFS);
        /*��������У����*/
        data_crc32 = CRC32_Update(data_crc32, buffer, r_cnt);
        /* ���� */
        file_finish += r_cnt;
        DGUS_Print(0, "����У��: %d%%  %d/%d Byte", (file_finish * 100 / file_size), file_finish, file_size);
    }
    /*���У����*/
    if (data_crc32 != *crc) {
        *crc = 0;
        printf("\r\nУ��ʧ��\r\n");
        DGUS_Print(0, "����У��: 100%%  %d/%d Byte Error", file_finish, file_size);
        return 1;
    }
    printf("\r\n��¼�ɹ�\r\n");
    DGUS_Print(0, "����У��: 100%%  %d/%d Byte OK", file_finish, file_size);
    return 0;
}
