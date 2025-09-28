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

OTA_InfoCB OTA_Info;            // ������24C02�ڵ�OTA��Ϣ��صĽṹ��
UpdateA_CB UpdateA;             // A�������õ��Ľṹ��
u32        BootStateFlag = 0;   // ��¼ȫ��״̬��־λ��ÿλ��ʾ1��״̬
uint8_t Flag  =  0;
uint8_t test = 0;
int main(void) { 
 //uint8_t i;
  //uint16_t i;
 //     /*�ж����ȼ�����*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    delay_Init();        // ��ʱ��ʼ��
    USART1_Init(115200);   // ����1��ʼ��
    /*��ʼ��Flash*/
    W25QXX_SPI_Init();
    /* ��ʼ������洢�� */
    FM24CL64_Init();
    u1_printf("\r\n����1��ʼ���ɹ�\r\n");
    Beep_Init();
    AT24C02_ReadOTAInfo();   // ��24C02��ȡ���ݵ�OTA_Info�ṹ��
    BootLoader_Branch();     // ��֧�ж�
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
// //// ���� slot2 ��ʼҳ
// //W25Q64_TestAddr(GetSlotBaseAddr(2));

// // ���� Flash �ʼ
// W25Q64_TestAddr(0x000000);

// // ���� Flash �м�
// W25Q64_TestAddr(0x400000);

// // ���� Flash ĩβ
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
        // ����1����
        if (U1_CB.URxDataOUT != U1_CB.URxDataIN) {                                                                      // ��鴮�ڽ��ջ������Ƿ���������
            Bootloader_Event_Process(U1_CB.URxDataOUT->start, (U1_CB.URxDataOUT->end - U1_CB.URxDataOUT->start + 1));   // ���������ݴ����Ӧ����
            U1_CB.URxDataOUT++;                                                                                         // ��ָ�����
            if (U1_CB.URxDataOUT == U1_CB.URxDataEND) {
                U1_CB.URxDataOUT = &U1_CB.URxDataPtr[0];   // ����Ѿ�������ĩβ�������»ص�������ͷ��
            }
        }
        /*����C*/
        if (BootStateFlag & IAP_XModem_C_FLAG) {   // ���ڷ���XModemЭ�����ʼC
            if (UpdateA.XModem_Timer_Count >= 100) {
                u1_printf("C");
                UpdateA.XModem_Timer_Count = 0;
            }
            UpdateA.XModem_Timer_Count++;
        }
if (BootStateFlag & UPDATA_A_FLAG) {
    u1_printf("������Ҫ���µĴ�С��%d�ֽ�\r\n", OTA_Info.FirmwareLen[UpdateA.Updata_A_from_W25Q64_Num]);

    if ((OTA_Info.FirmwareLen[UpdateA.Updata_A_from_W25Q64_Num] % 4) == 0) {
        STM32_EraseFlash(STM32_APP_START_SECTOR, STM32_APP_SECTOR_NUM);
        u1_printf("A���Ѳ���\r\n");

        uint32_t slot_base = GetSlotBaseAddr(UpdateA.Updata_A_from_W25Q64_Num);
        u1_printf("Addr = 0x%06X\r\n", slot_base);
        uint32_t fwlen     = OTA_Info.FirmwareLen[UpdateA.Updata_A_from_W25Q64_Num];
        uint32_t full_blk  = fwlen / STM32_PAGE_SIZE;   // ������ 1K ����
        uint32_t remain    = fwlen % STM32_PAGE_SIZE;   // ʣ�಻�� 1K �Ĳ���
        uint32_t flash_off = 0;

        // д������ 1KB ��
        for (uint32_t i = 0; i < full_blk; i++) {
            W25QXX_Read(UpdateA.Updata_A_Buff,
                        slot_base + i * STM32_PAGE_SIZE,
                        STM32_PAGE_SIZE);

            flash_off = i * STM32_PAGE_SIZE;

            STM32_WriteFlash(STM32_APP_START_ADDR + flash_off,
                             (u32*)UpdateA.Updata_A_Buff,
                             STM32_PAGE_SIZE);
        }

        // дʣ�಻�� 1KB ������
        if (remain != 0) {
            memset(UpdateA.Updata_A_Buff, 0xFF, STM32_PAGE_SIZE);  // δ���ֽ��� 0xFF������������

            W25QXX_Read(UpdateA.Updata_A_Buff,
                        slot_base + full_blk * STM32_PAGE_SIZE,
                        remain);

            flash_off = full_blk * STM32_PAGE_SIZE;

            // Flash д����Ҫ�� 4 �ֽڶ���
            uint32_t padded = (remain + 3) & ~3U;

            STM32_WriteFlash(STM32_APP_START_ADDR + flash_off,
                             (u32*)UpdateA.Updata_A_Buff,
                             padded);
        }

        if (UpdateA.Updata_A_from_W25Q64_Num == 0) {
            OTA_Info.OTA_flag = 0;
            AT24C02_WriteOTAInfo();
        }

        u1_printf("A��������ɣ���������ϵͳ��\r\n");
        delay_ms(100);
        NVIC_SystemReset();
    } else {
        u1_printf("������APP���ȴ���\r\n");
        BootStateFlag &= ~(UPDATA_A_FLAG);
    }
}
    };
}
