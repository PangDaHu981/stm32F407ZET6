#include "W25Q64.h"

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