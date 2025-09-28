#include "W25Q64.h"
#include "boot.h"
struct SPI_Flash_Struct {
    uint32_t Capacity;  // Flash����

} SPI_Flash;

// SD����ʼ����ʱ��,��Ҫ����
void W25QXX_SPI_SpeedLow(void) {
    SPI1_SetSpeed(SPI_BaudRatePrescaler_256);   // ���õ�����ģʽ
}
// SD������������ʱ��,���Ը�����
void W25QXX_SPI_SpeedHigh(void) {
    SPI1_SetSpeed(SPI_BaudRatePrescaler_2);   // ���õ�����ģʽ
}
void W25QXX_SPI_Init(void) {

    GPIO_InitTypeDef GPIO_InitStructure;   // ����Ӳ������SD��������Ŀ����������
    RCC_AHB1PeriphClockCmd(W25QXX_CS_RCCCLOCK, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = W25QXX_CS_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;       // ��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       // �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   // 100MHz
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;        // ����
    GPIO_Init(W25QXX_CS_PORT, &GPIO_InitStructure);
    GPIO_SetBits(W25QXX_CS_PORT, W25QXX_CS_PIN);
    SPI1_Init();
    SPI_Flash.Capacity = W25QXX_ReadCapacity();
}
// ȡ��ѡ��,�ͷ�SPI����
void W25QXX_DisSelect(void) {
    /* ����Ѿ�Ƭѡ */
    if (GPIO_ReadOutputDataBit(W25QXX_CS_PORT, W25QXX_CS_PIN) == RESET) {
        GPIO_SetBits(W25QXX_CS_PORT, W25QXX_CS_PIN);
        SPI1_ReadWriteByte(0xff);   // �ṩ�����8��ʱ��
        SPI2_Give();                  /* �ͷŴ���ʹ��Ȩ */
    }
}
/*
 *��SR�Ĵ���
 */
uint8_t W25QXX_ReadSR(void) {
    uint8_t sr = 0;
    /*Ƭѡ����*/
    W25QXX_CS_0;
    /*��������:��ȡSR״̬�Ĵ���*/
    SPI1_ReadWriteByte(W25QX_ReadStatusReg);
    /*��һ���ֽ�*/
    sr = SPI1_ReadWriteByte(0Xff);
    /*ȡ��Ƭѡ*/
    W25QXX_CS_1;
    /*���ؼĴ�����ֵ*/
    return sr;
}
/*
 *W25Qxx дʹ��
 */
void W25QXX_Write_Enable(void) {
    /*Ƭѡ����*/
    W25QXX_CS_0;
    /*����дʹ��*/
    SPI1_ReadWriteByte(W25QX_WriteEnable);
    /*ȡ��Ƭѡ*/
    W25QXX_CS_1;
}

/*
 *W25Qxx д��ֹ
 */
void W25QXX_Write_Disable(void) {
    /*Ƭѡ����*/
    W25QXX_CS_0;
    /*����д��ֹ*/
    SPI1_ReadWriteByte(W25QX_WriteDisable);
    /*ȡ��Ƭѡ*/
    W25QXX_CS_1;
}

/*
 *W25Qxx ��ȡоƬID
 */
uint16_t W25QXX_ReadID(void) {
    uint16_t id = 0;
    /*Ƭѡ����*/
    W25QXX_CS_0;
    /*���Ͷ�ȡID����*/
    SPI1_ReadWriteByte(W25QX_ManufactDeviceID);
    /*�ȴ�����ʱ������*/
    SPI1_ReadWriteByte(0x00);
    SPI1_ReadWriteByte(0x00);
    SPI1_ReadWriteByte(0x00);
    /*��ȡ�����ֽ�*/
    id |= SPI1_ReadWriteByte(0xFF) << 8;
    id |= SPI1_ReadWriteByte(0xFF);
    /*ȡ��Ƭѡ*/
    W25QXX_CS_1;
    /*����оƬID*/
    return id;
}
/*
 *��ȡоƬ����
 */
uint32_t W25QXX_ReadCapacity(void) {
    uint16_t id;
    uint32_t capacity = 1;
    /*��ȡоƬ����ID*/
    id = W25QXX_ReadID();
    /*��ȡ������Ϣ(BCD��)*/
    id &= 0x00FF;
    /*ת������*/
    id = ((id >> 4) * 10) + (id & 0x0F);
    /*��������*/
    for (; id; id--) {
        capacity *= 2;
    }
    capacity *= 1024 / 8;
    /*��������*/
    return capacity;
}
/*ֱ�Ӷ�ȡ����*/
    void W25QXX_Read(void* r_bf, uint32_t r_addr, uint16_t count) {
    uint8_t* r_poi = r_bf;
    /*�ȴ�д�����*/
    W25QXX_WaitBusy();
    /*Ƭѡ����*/
    W25QXX_CS_0;
    /*���Ͷ�ȡ��������*/
    SPI1_ReadWriteByte(W25QX_ReadData);
    /*�������ݵ�ַ*/
    SPI1_ReadWriteByte((uint8_t) (r_addr >> 16));
    SPI1_ReadWriteByte((uint8_t) (r_addr >> 8));
    SPI1_ReadWriteByte((uint8_t) (r_addr));
    /*ѭ����ȡ����*/
    while (count--) {
        *r_poi++ = SPI1_ReadWriteByte(0XFF);
    }
    /*ȡ��Ƭѡ*/
    W25QXX_CS_1;
}

/*
 *дҳ ���256�ֽ�
 */
void W25QXX_WritePage(void* w_bf, uint32_t w_addr, uint16_t count) {
    uint8_t* w_poi = w_bf;
    /*�ȴ�д�����*/
    W25QXX_WaitBusy();
    /*дʹ��*/
    W25QXX_Write_Enable();
    /*Ƭѡ����*/
    W25QXX_CS_0;
    /*����ҳ�������*/
    SPI1_ReadWriteByte(W25QX_PageProgram);
    /*�������ݵ�ַ*/
    SPI1_ReadWriteByte((uint8_t) (w_addr >> 16));
    SPI1_ReadWriteByte((uint8_t) (w_addr >> 8));
    SPI1_ReadWriteByte((uint8_t) (w_addr));
    /*ѭ��д��*/
    while (count--) {
        SPI1_ReadWriteByte(*w_poi++);
    }
    /*ȡ��Ƭѡ*/
    W25QXX_CS_1;
    /*�ȴ�д�����*/
    W25QXX_WaitBusy();
}

/*
 *ֱ��д������ �Զ���ҳ ��У��
 */
void W25QXX_Write(void* w_bf, uint32_t w_addr, uint16_t count) {
    uint8_t* w_poi = w_bf;
    uint16_t page_byte;
    /*д��*/
    while (count) {
        /*���㵱ǰҳʣ��ռ�*/
        page_byte = 256 - w_addr % 256;
        /*���ʣ��д���ֽڴ��ڵ�ǰҳʣ��ռ�*/
        if (count > page_byte) {
            /*д��ʣ��ҳ*/
            W25QXX_WritePage(w_poi, w_addr, page_byte);
            /*�������*/
            w_poi += page_byte;
            w_addr += page_byte;
            count -= page_byte;
        }
        /*���ʣ��д���ֽ�С�ڵ�ǰҳʣ��ռ�*/
        else if (count <= page_byte) {
            /*д��ʣ������*/
            W25QXX_WritePage(w_poi, w_addr, count);
            /*�������*/
            w_poi += count;
            w_addr += count;
            count -= count;
        }
    }
    
}

/*
 *��������
 */
void W25QXX_EraseSector(uint32_t address) {
    /*��������������ַ*/
    address /= 4096;
    address *= 4096;
    /*�ȴ�æλ*/
    W25QXX_WaitBusy();
    /*дʹ��*/
    W25QXX_Write_Enable();
    /*Ƭѡ����*/
    W25QXX_CS_0;
    /*���Ͳ�������ָ��*/
    SPI1_ReadWriteByte(W25QX_SectorErase);
    /*���Ͳ�����ַ*/
    SPI1_ReadWriteByte((uint8_t) (address >> 16));
    SPI1_ReadWriteByte((uint8_t) (address >> 8));
    SPI1_ReadWriteByte((uint8_t) (address));
    /*ȡ��Ƭѡ*/
    W25QXX_CS_1;
    /*�ȴ�д�����*/
    W25QXX_WaitBusy();
}
// void W25Q64_Erase64K(uint8_t blockNB)
// {
//     uint8_t wdata[4];
//     uint32_t addr = blockNB * 64 * 1024; // ������׵�ַ

//     wdata[0] = 0xD8;                    // 64K�����ָ��
//     wdata[1] = (addr >> 16) & 0xFF;     // ��ַ��8λ
//     wdata[2] = (addr >> 8) & 0xFF;      // ��ַ��8λ
//     wdata[3] = addr & 0xFF;             // ��ַ��8λ

//     W25QXX_WaitBusy();                  // �ȴ�����
//     W25QXX_Write_Enable();               // ʹ��д��
//     W25QXX_CS_1;
//     for(uint8_t i = 0; i < 4; i++)
//     {
//         SPI1_ReadWriteByte(wdata[i]);
//     }
//     //SPI1_Write(wdata, 4);               // ���Ͳ�������͵�ַ
//     W25QXX_CS_0;
//     W25QXX_WaitBusy();                  // �ȴ��������
// }
// void W25Q64_Erase64K(uint8_t slot_index)
// {
//     if (slot_index == 0 || slot_index > SLOT_NUM) return;

//     uint32_t addr = GetSlotBaseAddr(slot_index);
//     uint8_t wdata[4];

//     wdata[0] = 0xD8;                    // 64K block erase
//     wdata[1] = (addr >> 16) & 0xFF;
//     wdata[2] = (addr >> 8) & 0xFF;
//     wdata[3] = addr & 0xFF;

//     W25QXX_WaitBusy();
//     W25QXX_Write_Enable();
//     W25QXX_CS_1;
//     for (uint8_t i = 0; i < 4; i++)
//     {
//         SPI1_ReadWriteByte(wdata[i]);
//     }
//     W25QXX_CS_0;
//     W25QXX_WaitBusy();
// }
// void W25Q64_EraseSlot(uint8_t slot_index) {
//     uint32_t base = GetSlotBaseAddr(slot_index);
//     if (base == 0xFFFFFFFF) return;

//     // һ�� slot ��С 512KB��ÿ�β� 64KB���ܹ� 8 ����
//     for (uint32_t i = 0; i < (SLOT_SIZE / (64 * 1024)); i++) {
//         uint32_t addr = base + i * 64 * 1024;
//         W25Q64_Erase64K(addr / (64 * 1024));
//     }
// }


// ����һ�� 64KB block
void W25Q64_Erase64K(uint32_t addr) 
{
    W25QXX_WaitBusy();
    W25QXX_Write_Enable();

    W25QXX_CS_0;                          // ? CS ����
    SPI1_ReadWriteByte(0xD8);             // 64K block erase
    SPI1_ReadWriteByte((addr >> 16) & 0xFF);
    SPI1_ReadWriteByte((addr >> 8) & 0xFF);
    SPI1_ReadWriteByte(addr & 0xFF);
    W25QXX_CS_1;                          // ? CS ����

    W25QXX_WaitBusy();
}

// ����һ�� slot��512KB = 8 �� 64KB block��
void W25Q64_EraseSlot(uint8_t slot_index) 
{
    uint32_t base = GetSlotBaseAddr(slot_index);
    if (base == 0xFFFFFFFF) return;

    for (uint32_t i = 0; i < (SLOT_SIZE / (64 * 1024)); i++) {
        uint32_t addr = base + i * (64 * 1024);
        W25Q64_Erase64K(addr);   // �����ַ�������� index
    }
}
/*
 *æλ�ȴ�
 */
void W25QXX_WaitBusy(void) {
    while ((W25QXX_ReadSR() & 0x01) == 0x01)
        ;
}

/*
 *�������ģʽ
 */
void W25QXX_PowerDown(void) {
    /*Ƭѡ����*/
    W25QXX_CS_0;
    /*���͵�������*/
    SPI1_ReadWriteByte(W25QX_PowerDown);
    /*ȡ��Ƭѡ*/
    W25QXX_CS_1;
}

/*
 *����
 */
void W25QXX_WAKEUP(void) {
    /*Ƭѡ����*/
    W25QXX_CS_0;
    /*����ȡ������*/
    SPI1_ReadWriteByte(W25QX_ReleasePowerDown);
    /*ȡ��Ƭѡ*/
    W25QXX_CS_1;
}

/*	���һ��д��256�ֽ�	*/
void W25Q64_PageWrite(uint8_t *wbuff, uint16_t pageNB){
        uint8_t* w_poi = wbuff;
	uint8_t wdata[4];
	
	wdata[0] = 0x02;
	wdata[1] = (pageNB*256)>>16;
	wdata[2] = (pageNB*256)>>8;
	wdata[3] = (pageNB*256)>>0;
	
	W25QXX_WaitBusy();
	W25QXX_Write_Enable();
	W25QXX_CS_1;
    for(uint8_t i=0;i<4;i++)
    {
        SPI1_ReadWriteByte(wdata[i]);
    }
    for(uint16_t i=0;i<256;i++)
    {
        SPI1_ReadWriteByte(*w_poi++);
    }
	// SPI1_Write(wdata,4);
	// SPI1_Write(wbuff,256);
	W25QXX_CS_0;
}
// #define TEST_BLOCK_SIZE   1024   // ÿ��д/�� 1KB
// #define TEST_BLOCK_COUNT  512    // 512 * 1KB = 512KB

// void W25Q64_TestSlot(uint8_t slot_index)
// {
//     uint32_t base = GetSlotBaseAddr(slot_index);
//     if (base == 0xFFFFFFFF) {
//         u1_printf("Invalid slot index: %d\r\n", slot_index);
//         return;
//     }

//     uint8_t writeBuf[TEST_BLOCK_SIZE];
//     uint8_t readBuf[TEST_BLOCK_SIZE];

//     // ��ʼ��д����
//     for (uint32_t i = 0; i < TEST_BLOCK_SIZE; i++) {
//         writeBuf[i] = i & 0xFF;  // 0~255 ѭ�����
//     }

//     u1_printf("Erase slot %d ...\r\n", slot_index);
//     W25Q64_EraseSlot(slot_index); // �������� 512KB

//     // ���д������У��
//     for (uint32_t blk = 0; blk < TEST_BLOCK_COUNT; blk++) {
//         uint32_t addr = base + blk * TEST_BLOCK_SIZE;

//         // д
//         W25QXX_Write(writeBuf, addr, TEST_BLOCK_SIZE);

//         // ��
//         W25QXX_Read(readBuf, addr, TEST_BLOCK_SIZE);

//         // �Ƚ�
//         for (uint32_t i = 0; i < TEST_BLOCK_SIZE; i++) {
//             if (readBuf[i] != writeBuf[i]) {
//                 u1_printf("Slot %d Test FAIL at 0x%08X: W=0x%02X, R=0x%02X\r\n",
//                           slot_index, addr + i, writeBuf[i], readBuf[i]);
//                 return;  // ����ֱ�ӷ���
//             }
//         }
//     }

//     u1_printf("Slot %d Test PASS\r\n", slot_index);
// }
#define PAGE_SIZE         256              // W25Q64 ҳ��С
//#define SLOT_SIZE         (512 * 1024)     // 512KB
#define SLOT_PAGE_COUNT   (SLOT_SIZE / PAGE_SIZE)  // ÿ��Slot��2048ҳ

// ����ָ�� Slot �Ķ�д�Ƿ����� (ǿ�ư�ҳд��)
void W25Q64_TestSlot(uint8_t slot_index)
{
    uint32_t base = GetSlotBaseAddr(slot_index);
    if (base == 0xFFFFFFFF) {
        u1_printf("Invalid slot index: %d\r\n", slot_index);
        return;
    }

    uint8_t writeBuf[PAGE_SIZE];
    uint8_t readBuf[PAGE_SIZE];

    // ����������
    for (uint32_t i = 0; i < PAGE_SIZE; i++) {
        writeBuf[i] = i;   // 0x00, 0x01, 0x02 ...
    }

    u1_printf("Erase slot %d ...\r\n", slot_index);
    W25Q64_EraseSlot(slot_index); // �������� 512KB

    u1_printf("Write + Read test on slot %d ...\r\n", slot_index);

    for (uint32_t page = 0; page < SLOT_PAGE_COUNT; page++) {
        uint32_t addr = base + page * PAGE_SIZE;

        // д��һҳ
        W25QXX_Write(writeBuf, addr, PAGE_SIZE);

        // ����һҳ
        W25QXX_Read(readBuf, addr, PAGE_SIZE);

        // У��һҳ
        for (uint32_t i = 0; i < PAGE_SIZE; i++) {
            if (readBuf[i] != writeBuf[i]) {
                u1_printf("Slot %d Test FAIL at 0x%08X: W=0x%02X, R=0x%02X\r\n",
                          slot_index, addr + i, writeBuf[i], readBuf[i]);
                return; // ���ִ���ֱ�ӷ���
            }
        }
    }

    u1_printf("Slot %d Test PASS!\r\n", slot_index);
}

void W25Q64_TestAddr(uint32_t addr)
{
    uint8_t writeBuf[PAGE_SIZE];
    uint8_t readBuf[PAGE_SIZE];

    for (uint32_t i = 0; i < PAGE_SIZE; i++) {
        writeBuf[i] = i;
    }

    u1_printf("Erase 64K block at 0x%08X ...\r\n", addr & ~(64*1024-1));
    W25Q64_Erase64K(addr & ~(64*1024-1));

    u1_printf("Write + Read test at 0x%08X ...\r\n", addr);

    W25QXX_Write(writeBuf, addr, PAGE_SIZE);
    W25QXX_Read(readBuf, addr, PAGE_SIZE);

    for (uint32_t i = 0; i < PAGE_SIZE; i++) {
        if (readBuf[i] != writeBuf[i]) {
            u1_printf("Test FAIL at 0x%08X: W=0x%02X, R=0x%02X\r\n",
                      addr + i, writeBuf[i], readBuf[i]);
            return;
        }
    }

    u1_printf("Test PASS at 0x%08X\r\n", addr);
}