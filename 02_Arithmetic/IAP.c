/*
 *系统升级
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

IAP_StructTypedef IAP;                   // IAP信息
pFunction         Jump_To_Application;   // 用户程序跳转函数
/* IAP初始化 */
void IAP_Init(void) {
    uint8_t err_cnt = 5;

    /*初始化Flash*/
    W25QXX_SPI_Init();
    /* 初始化铁电存储器 */
    FM24CL64_Init();
    do {
        /*获取IAP信息*/
        FM24CL64_Read(CHIP_IAP_INFO_ADDRESS, &IAP, sizeof(IAP_StructTypedef));
        /* 计算控制块的CRC校验码 */
    } while ((CRC16_Modbus(&IAP, sizeof(IAP) - 2) != IAP.crc) && (--err_cnt));

    IAP.FlashSize = ((*(uint16_t*) 0x1FFF7A22) * 1024);   // Flash大小

    if (err_cnt == 0) {
        IAP.Cmd                     = CMD_NormalStart;               // 正常启动
        IAP.Program[0].FlashAddress = SPI_FLASH_PROGRAM_1_ADDRESS;   // 程序存储地址
        IAP.Program[0].TotalSize    = 0x00000000;                    // 程序大小
        IAP.Program[0].crc          = 0x00000000;                    // 校验位
        IAP.Program[1].FlashAddress = SPI_FLASH_PROGRAM_2_ADDRESS;   // 程序存储地址
        IAP.Program[1].TotalSize    = 0x00000000;                    // 程序大小
        IAP.Program[1].crc          = 0x00000000;                    // 校验位
        IAP.HardVersions            = 100;                           // 硬件版本号
        /*保存IAP信息*/
        Save_IAP();
    }
}

/* 引导程序 */
void Bootloader(void) {
    printf("\r\n加载引导程序\r\n");
//    { /* 强制回退程序 */
//        uint32_t count = 0;
//        /* 按下按键上电后执行 */
//        while ((Key_Get() & (KEY_1)) == (KEY_1)) {
//            if (++count >= 0x2FFFFF) {
//                count--;
//                /* 引导到程序2 */
//                IAP.Cmd = CMD_BootProgram2;
//                Beep_Toggle();
//                for (uint32_t i = 0xFFFFF; i; i--) {
//                    asm("nop");
//                }
//            }
//        }
//        Beep_Off();
//    }
    /* 非特殊含义指令 */
    if ((IAP.Cmd & 0xFFFFFF00) != 0x5AA55A00) {
        /*保存IAP信息*/
        Save_IAP();
        printf("\r\n即将跳转到用户程序\r\n");
        /* 跳转到用户程序 */
        IAP_JumpApp();
    }
    /* 跳转到引导程序页 */
    DGUS_SetPage(33);
    DGUS_Clean();
    DGUS_Print(0, "<----------引导程序---------->");
    if ((IAP.Cmd & 0xFFFFFF00) != 0x5AA55A00) {
        /* 跳转用户程序失败 */
        DGUS_Print(1, "跳转用户程序失败");
        goto ex;
    }

    /*引导失败*/
    if ((IAP.Cmd & CMD_BootProgramError) == CMD_BootProgramError) {
        printf("\r\n引导失败\r\n");
        DGUS_Print(1, "引导失败");
        IAP.Cmd = CMD_NormalStart;
        /*保存IAP信息*/
        Save_IAP();
        while (1) {
        }
    }

    /*升级程序*/
    if ((IAP.Cmd & CMD_ProgramUpdate) == CMD_ProgramUpdate) {
        printf("升级程序\r\n");
        DGUS_Print(1, "开始系统升级");
    }
    /*备份程序*/
    if ((IAP.Cmd & CMD_ProgramBackup) == CMD_ProgramBackup) {
        printf("备份程序\r\n");
        DGUS_Print(1, "备份原始程序");
        DGUS_Print(1, "[CHIP] 0x%08X -> [SPI_FLASH] 0x%08X = %d Byte", CHIP_APP_START_ADDRESS, IAP.Program[1].FlashAddress, IAP.FlashSize - (CHIP_APP_START_ADDRESS & 0xFFFF));
        IAP.Program[1].TotalSize = 0;
        /* 将现有用户程序烧录到备份区 */
        if (Flash_2_SPI_Flash(CHIP_APP_START_ADDRESS,
                              IAP.Program[1].FlashAddress,
                              IAP.FlashSize - (CHIP_APP_START_ADDRESS & 0xFFFF),
                              &IAP.Program[1].crc) == 1) {
            DGUS_Print(1, "备份失败");
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
        /* 记录程序文件大小 */
        IAP.Program[1].TotalSize = IAP.FlashSize - (CHIP_APP_START_ADDRESS & 0xFFFF);
    }
    /*引导到主程序*/
    if ((IAP.Cmd & CMD_BootProgram1) == CMD_BootProgram1) {
        printf("引导到主程序\r\n");
        DGUS_Print(1, "加载主程序");
        DGUS_Print(1, "[SPI_FLASH] 0x%08X -> [CHIP] 0x%08X = %d Byte", IAP.Program[0].FlashAddress, CHIP_APP_START_ADDRESS, IAP.Program[0].TotalSize);
        if (SPI_Flash_2_Flash(IAP.Program[0].FlashAddress,
                              CHIP_APP_START_ADDRESS,
                              IAP.Program[0].TotalSize,
                              IAP.Program[0].crc) == 1) {
            printf("引导失败\r\n");
            DGUS_Print(1, "引导失败");
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
    /*引导到辅程序*/
    if ((IAP.Cmd & CMD_BootProgram2) == CMD_BootProgram2) {
        printf("引导到辅程序\r\n");
        DGUS_Print(1, "加载备份程序");
        DGUS_Print(1, "[SPI_FLASH] 0x%08X -> [CHIP] 0x%08X = %d Byte", IAP.Program[1].FlashAddress, CHIP_APP_START_ADDRESS, IAP.Program[1].TotalSize);
        if (SPI_Flash_2_Flash(IAP.Program[1].FlashAddress,
                              CHIP_APP_START_ADDRESS,
                              IAP.Program[1].TotalSize,
                              IAP.Program[1].crc) == 1) {
            printf("引导失败\r\n");
            DGUS_Print(1, "引导失败");
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
    DGUS_Print(1, "加载完成");

ex:
    printf("\r\n>-----即将复位-----<\r\n");
    DGUS_Print(1, "即将重启");
    /*保存IAP信息*/
    Save_IAP();
    /*延时*/
    for (uint32_t i = 0x1FFFFFF; i; i--) {
        asm("nop");
    }
    /*复位*/
    NVIC_SystemReset();
}

/* 跳转到用户程序 */
uint8_t IAP_JumpApp(void) {
    /*检查是否有应用程序*/
    if (((*(__IO uint32_t*) CHIP_APP_START_ADDRESS) & 0x2FFE0000) == 0x20000000) {
        /*设置中断向量表指向*/
        NVIC_SetVectorTable(NVIC_VectTab_FLASH, CHIP_APP_START_OFFSET);
        /*跳转函数指向用户应用程序地址*/
        Jump_To_Application = (pFunction) (*(__IO uint32_t*) (CHIP_APP_START_ADDRESS + 4));
        /*初始化用户程序堆栈指针*/
        __set_MSP(*(__IO uint32_t*) CHIP_APP_START_ADDRESS);
        /*跳转用户程序*/
        Jump_To_Application();
    }
    return 1;
}

/* 保存IAP信息 */
static uint8_t Save_IAP(void) {
    uint8_t  buffer[sizeof(IAP)];
    uint16_t crc = 0;
    /* 计算控制块的CRC校验码 */
    IAP.crc = CRC16_Modbus(&IAP, sizeof(IAP) - 2);
    /* 将IAP信息写入FRAM */
    FM24CL64_Write(CHIP_IAP_INFO_ADDRESS, &IAP, sizeof(IAP_StructTypedef));
    /* 将写入数据再次读出 */
    FM24CL64_Read(CHIP_IAP_INFO_ADDRESS, buffer, sizeof(IAP_StructTypedef));
    /* 获取存储器中的CRC */
    crc = *(uint16_t*) (&buffer[sizeof(IAP) - 2]);
    /* 计算控制块的CRC校验码 */
    if (CRC16_Modbus(buffer, sizeof(IAP) - 2) == crc) {
        return 1;
    }
    return 0;
}

static uint8_t buffer[4096];
/* 从SPI Flash烧录到内部Flash */
static uint8_t SPI_Flash_2_Flash(uint32_t spi_f_addr, uint32_t f_addr, uint32_t size, uint32_t crc) {
    uint32_t file_finish  = 0;        // 已完成大小
    uint32_t file_size    = size;     // 文件大小
    uint32_t data_crc32   = 0;        // 数据校验码
    uint32_t flashAddress = f_addr;   // 程序存储地址
    uint32_t r_cnt;                   // 读取计数
    printf("\r\n[SPI_FLASH->CHIP]:0x%08x->0x%08x=%dByte\r\n", spi_f_addr, f_addr, size);
    /*解锁内部Flash*/
    FLASH_Unlock();
    /*关闭Flash数据缓存*/
    FLASH_DataCacheCmd(DISABLE);
    DGUS_Print(1, "正在复制:");
    /*复制文件*/
    while (1) {
        /* 写道扇区首地址 */
        if (IS_FLASH_SECTOR_START(flashAddress)) {
            /*擦除扇区*/
            FLASH_EraseSector(FLASH_SECTOR(flashAddress), VoltageRange_3);   // 一定要判断是否擦除成功
        }
        /* 检查剩余字节数,若剩余字节大于缓存 */
        if ((file_size - file_finish) > sizeof(buffer)) {
            /*读取缓存大小文件*/
            r_cnt = sizeof(buffer);
        }
        /* 剩余字节数大于0小于缓存 */
        else if ((file_size - file_finish) > 0) {
            /* 读取剩余字节数 */
            r_cnt = (file_size - file_finish);
        }
        /*字节数为0,结束复制*/
        else
            break;
        LED_On(FATFS);
        W25QXX_Read(buffer, (spi_f_addr + file_finish), r_cnt);
        LED_Off(FATFS);
        /* 计算数据校验码 */
        data_crc32 = CRC32_Update(data_crc32, buffer, r_cnt);
        /* 将数据写入Flash */
        for (uint16_t i = 0; i < r_cnt; i++) {
            FLASH_ProgramByte(flashAddress++, buffer[i]);
        }
        /* 计数 */
        file_finish += r_cnt;
        DGUS_Print(0, "正在复制: %d%%  %d/%d Byte", (file_finish * 100 / file_size), file_finish, file_size);
    }
    /*开启Flash数据缓存*/
    FLASH_DataCacheCmd(ENABLE);
    /*Flash上锁*/
    FLASH_Lock();
    printf("\r\n烧录完成 校验码: %08X : %08X\r\n", data_crc32, crc);
    DGUS_Print(1, "复制完成 校验码: 0x%08X : 0x%08X", data_crc32, crc);
    /*检查校验码*/
    if (data_crc32 != crc && !(crc == 0x00000000 || crc == 0xFFFFFFFF)) {
        printf("\r\n校验失败\r\n");
        DGUS_Print(1, "校验失败: 0x%08X : 0x%08X", data_crc32, crc);
        /* 校验失败 */
        return 1;
    }

    file_finish  = 0;        // 已完成大小
    file_size    = size;     // 文件大小
    flashAddress = f_addr;   // 程序存储地址
    data_crc32   = 0;        // CRC校验
    printf("\r\n开始核对程序\r\n");
    DGUS_Print(1, "正在校验:");
    /*校验程序文件*/
    while (1) {
        /*检查剩余字节数,若剩余字节大于缓存*/
        if ((file_size - file_finish) > 0x800) {
            r_cnt = 0x800;
            /*读取缓存大小文件*/
            memcpy(buffer, (void*) (flashAddress + file_finish), r_cnt);
        }
        /*剩余字节数大于0小于缓存*/
        else if ((file_size - file_finish) > 0) {
            r_cnt = (file_size - file_finish);
            /*读取剩余字节数*/
            memcpy(buffer, (void*) (flashAddress + file_finish), r_cnt);
        }
        /*字节数为0,结束复制*/
        else
            break;
        /*计算数据校验码*/
        data_crc32 = CRC32_Update(data_crc32, buffer, r_cnt);
        /*计数*/
        file_finish += r_cnt;
        DGUS_Print(0, "正在校验: %d%%  %d/%d Byte", (file_finish * 100 / file_size), file_finish, file_size);
    }
    /*检查校验码*/
    if (data_crc32 != crc && !(crc == 0x00000000 || crc == 0xFFFFFFFF)) {
        printf("\r\n校验失败\r\n");
        DGUS_Print(0, "正在校验: 100%%  %d/%d Byte Error", file_finish, file_size);
        /* 校验失败 */
        return 1;
    }
    DGUS_Print(0, "正在校验: 100%%  %d/%d Byte OK", file_finish, file_size);
    printf("\r\n烧录成功\r\n");
    return 0;
}
/* 从内部Flash烧录到SPI Flash */
static uint8_t Flash_2_SPI_Flash(uint32_t f_addr, uint32_t spi_f_addr, uint32_t size, uint32_t* crc) {
    uint32_t file_finish = 0;      // 已完成大小
    uint32_t file_size   = size;   // 文件大小
    uint32_t data_crc32  = 0;      // 数据校验码
    uint32_t r_cnt;                // 读取计数
    printf("\r\n[CHIP->SPI_FLASH]:0x%08x->0x%08x=%dByte\r\n", f_addr, spi_f_addr, size);
    DGUS_Print(1, "正在复制;");
    /*复制文件*/
    while (1) {
        /*检查剩余字节数,若剩余字节大于缓存*/
        if ((file_size - file_finish) > 0x1000) {
            /*读取缓存大小文件*/
            r_cnt = 0x1000;
        }
        /*剩余字节数大于0小于缓存*/
        else if ((file_size - file_finish) > 0) {
            /*读取剩余字节数*/
            r_cnt = (file_size - file_finish);
        }
        /*字节数为0,结束复制*/
        else
            break;
        /* 读取数据 */
        memcpy(buffer, (uint8_t*) (f_addr + file_finish), r_cnt);
        /*计算数据校验码*/
        data_crc32 = CRC32_Update(data_crc32, buffer, r_cnt);
        LED_On(FATFS);
        /* 擦除Flash扇区 */
        W25QXX_EraseSector(spi_f_addr + file_finish);
        /* 写入数据 */
        W25QXX_Write(buffer, spi_f_addr + file_finish, r_cnt);
        LED_Off(FATFS);
        /*计数*/
        file_finish += r_cnt;
        DGUS_Print(0, "正在复制: %d%%  %d/%d Byte", (file_finish * 100 / file_size), file_finish, file_size);
    }
    printf("\r\n烧录完成 校验码: %08X\r\n", data_crc32);
    DGUS_Print(1, "复制完成 校验码: 0x%08X", data_crc32);
    file_size   = file_finish;   // 文件大小
    file_finish = 0;             // 已完成大小
    *crc        = data_crc32;    // 保存CRC数据
    data_crc32  = 0;             // 复位CRC
    printf("\r\n开始校验程序\r\n");
    DGUS_Print(1, "正在校验:");
    /*校验程序文件*/
    while (1) {
        /*检查剩余字节数,若剩余字节大于缓存*/
        if ((file_size - file_finish) > 0x1000) {
            /*读取缓存大小文件*/
            r_cnt = 0x1000;
        }
        /*剩余字节数大于0小于缓存*/
        else if ((file_size - file_finish) > 0) {
            /*读取剩余字节数*/
            r_cnt = (file_size - file_finish);
        }
        /*字节数为0,结束复制*/
        else
            break;
        LED_On(FATFS);
        /* 读取数据 */
        W25QXX_Read(buffer, (spi_f_addr + file_finish), r_cnt);
        LED_Off(FATFS);
        /*计算数据校验码*/
        data_crc32 = CRC32_Update(data_crc32, buffer, r_cnt);
        /* 计数 */
        file_finish += r_cnt;
        DGUS_Print(0, "正在校验: %d%%  %d/%d Byte", (file_finish * 100 / file_size), file_finish, file_size);
    }
    /*检查校验码*/
    if (data_crc32 != *crc) {
        *crc = 0;
        printf("\r\n校验失败\r\n");
        DGUS_Print(0, "正在校验: 100%%  %d/%d Byte Error", file_finish, file_size);
        return 1;
    }
    printf("\r\n烧录成功\r\n");
    DGUS_Print(0, "正在校验: 100%%  %d/%d Byte OK", file_finish, file_size);
    return 0;
}
