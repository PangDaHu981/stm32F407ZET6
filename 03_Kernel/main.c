#include "stm32f4xx.h"

#include "DGUS.h"
#include "IAP.h"
#include "Key.h"
#include "beep.h"
#include "led.h"
#include "sys_led.h"
#include "usart.h"
#include "RS706.h"
#include "main.h"
#include "delay.h"
#include "FM24CL64.h"
#include "w25q64.h"
#include "boot.h"

OTA_InfoCB OTA_Info;            // 保存在24C02内的OTA信息相关的结构体
UpdateA_CB UpdateA;             // A区更新用到的结构体
u32        BootStateFlag = 0;   // 记录全局状态标志位，每位表示1种状态
uint8_t Flag  =  0;
uint8_t test = 0;
int main(void) { 
 //uint8_t i;
  //uint16_t i;
 //     /*中断优先级分组*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    delay_Init();        // 延时初始化
    USART1_Init(115200);   // 串口1初始化
    /*初始化Flash*/
    W25QXX_SPI_Init();
    /* 初始化铁电存储器 */
    FM24CL64_Init();
    u1_printf("\r\n串口1初始化成功\r\n");
    Beep_Init();
    AT24C02_ReadOTAInfo();   // 从24C02读取数据到OTA_Info结构体
    BootLoader_Branch();     // 分支判断
    while (1) {
        delay_ms(10);
     if (GPIO_ReadInputDataBit(RS706_PORT, RS706_PIN) != 0) {
        GPIO_ResetBits(RS706_PORT, RS706_PIN);
    } else {
        GPIO_SetBits(RS706_PORT, RS706_PIN);
    }
    
//     if(Flag == 1){
//         Flag =0;
// //        W25Q64_TestAddr(GetSlotBaseAddr(1));
// //
// //// 测试 slot2 起始页
// //W25Q64_TestAddr(GetSlotBaseAddr(2));

// // 测试 Flash 最开始
// W25Q64_TestAddr(0x000000);

// // 测试 Flash 中间
// W25Q64_TestAddr(0x400000);

// // 测试 Flash 末尾
// W25Q64_TestAddr(0x7F0000);
//         //W25Q64_TestSlot(test);
//         // AT24C02_WriteOTAInfo();
//         // test = 1;
//         // FM24CL64_Write(0x00, &test, 1);
//         // test = 0;
//     }else if(Flag == 2)
//     {
//         Flag = 0;
//         FM24CL64_Read(0x00, &test, 1);
        
//         AT24C02_ReadOTAInfo();
//     }
        // 串口1接收
        if (U1_CB.URxDataOUT != U1_CB.URxDataIN) {                                                                      // 检查串口接收缓冲区是否有新数据
            Bootloader_Event_Process(U1_CB.URxDataOUT->start, (U1_CB.URxDataOUT->end - U1_CB.URxDataOUT->start + 1));   // 根据新数据处理对应任务
            U1_CB.URxDataOUT++;                                                                                         // 读指针后移
            if (U1_CB.URxDataOUT == U1_CB.URxDataEND) {
                U1_CB.URxDataOUT = &U1_CB.URxDataPtr[0];   // 如果已经到缓冲末尾，则重新回到缓冲区头部
            }
        }
        /*发送C*/
        if (BootStateFlag & IAP_XModem_C_FLAG) {   // 串口发送XModem协议的起始C
            if (UpdateA.XModem_Timer_Count >= 100) {
                u1_printf("C");
                UpdateA.XModem_Timer_Count = 0;
            }
            UpdateA.XModem_Timer_Count++;
        }
if (BootStateFlag & UPDATA_A_FLAG) {
    u1_printf("本次需要更新的大小：%d字节\r\n", OTA_Info.FirmwareLen[UpdateA.Updata_A_from_W25Q64_Num]);

    if ((OTA_Info.FirmwareLen[UpdateA.Updata_A_from_W25Q64_Num] % 4) == 0) {
        STM32_EraseFlash(STM32_APP_START_SECTOR, STM32_APP_SECTOR_NUM);
        u1_printf("A区已擦除\r\n");

        uint32_t slot_base = GetSlotBaseAddr(UpdateA.Updata_A_from_W25Q64_Num);
        u1_printf("Addr = 0x%06X\r\n", slot_base);
        uint32_t fwlen     = OTA_Info.FirmwareLen[UpdateA.Updata_A_from_W25Q64_Num];
        uint32_t full_blk  = fwlen / STM32_PAGE_SIZE;   // 完整的 1K 块数
        uint32_t remain    = fwlen % STM32_PAGE_SIZE;   // 剩余不足 1K 的部分
        uint32_t flash_off = 0;

        // 写完整的 1KB 块
        for (uint32_t i = 0; i < full_blk; i++) {
            W25QXX_Read(UpdateA.Updata_A_Buff,
                        slot_base + i * STM32_PAGE_SIZE,
                        STM32_PAGE_SIZE);

            flash_off = i * STM32_PAGE_SIZE;

            STM32_WriteFlash(STM32_APP_START_ADDR + flash_off,
                             (u32*)UpdateA.Updata_A_Buff,
                             STM32_PAGE_SIZE);
        }

        // 写剩余不足 1KB 的内容
        if (remain != 0) {
            memset(UpdateA.Updata_A_Buff, 0xFF, STM32_PAGE_SIZE);  // 未用字节填 0xFF，避免脏数据

            W25QXX_Read(UpdateA.Updata_A_Buff,
                        slot_base + full_blk * STM32_PAGE_SIZE,
                        remain);

            flash_off = full_blk * STM32_PAGE_SIZE;

            // Flash 写入需要按 4 字节对齐
            uint32_t padded = (remain + 3) & ~3U;

            STM32_WriteFlash(STM32_APP_START_ADDR + flash_off,
                             (u32*)UpdateA.Updata_A_Buff,
                             padded);
        }

        if (UpdateA.Updata_A_from_W25Q64_Num == 0) {
            OTA_Info.OTA_flag = 0;
            AT24C02_WriteOTAInfo();
        }

        u1_printf("A区更新完成，马上重启系统！\r\n");
        delay_ms(100);
        NVIC_SystemReset();
    } else {
        u1_printf("待更新APP长度错误！\r\n");
        BootStateFlag &= ~(UPDATA_A_FLAG);
    }
}
    };
}
