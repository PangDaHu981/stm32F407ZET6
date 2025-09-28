#include "FM24CL64.h"
#include "main.h"
#include "I2C.h"
#include "delay.h"
#include <string.h> 
void FM24CL64_Init(void) {
    FM24CL64_I2C_Init();
}

void FM24CL64_Read(uint16_t addr, void* buff, uint16_t lens) {
    uint8_t* rbuf = buff;

    FM24CL64_I2C_Start();
    FM24CL64_I2C_SendByte(FM24CL64_ADDRESS);
    FM24CL64_I2C_SendByte((uint8_t) (addr >> 8));    //���ݵ�ַ��λ
    FM24CL64_I2C_SendByte((uint8_t) (addr & 0xFF));  //���ݵ�ַ��λ
    FM24CL64_I2C_Stop();

    FM24CL64_I2C_Start();
    FM24CL64_I2C_SendByte(FM24CL64_ADDRESS | 0x01);
    while (--lens) {
        *rbuf++ = FM24CL64_I2C_ReadByte(1);
    }

    *rbuf = FM24CL64_I2C_ReadByte(0);

    FM24CL64_I2C_Stop();
}

void FM24CL64_Write(uint16_t addr, void* buff, uint16_t lens) {
    uint8_t* wbuf = buff;

    FM24CL64_I2C_Start();
    FM24CL64_I2C_SendByte(FM24CL64_ADDRESS);
    FM24CL64_I2C_SendByte((uint8_t) (addr >> 8));    //���ݵ�ַ��λ
    FM24CL64_I2C_SendByte((uint8_t) (addr & 0xFF));  //���ݵ�ַ��λ
    while (lens--) {
        FM24CL64_I2C_SendByte(*wbuf++);
    }
    FM24CL64_I2C_Stop();
}
u8 FM24CL64_I2C_WaitAck(int timeout)
{
    // ����FM24CL64ʹ�õ�I2C�˿ں�Ϊ1�����в�ͬ�����е���
    uint8_t port = 1;
    _I2C_SDA_1(port); // �ͷ�SDA
    while (_I2C_SDA_READ(port)) { // �ȴ��ӻ�����SDA
        timeout--;
        delay_us(2);
        if (timeout <= 0) {
            _I2C_Stop(port); // ��ʱ����ֹͣ�ź�
            return 1;        // ����Ӧ��ʧ��
        }
    }
    _I2C_SCL_1(port); // ����SCL��׼������Ӧ���ź�
    delay_us(2);
    if (_I2C_SDA_READ(port) != 0) { // ���Ӧ���ź��Ƿ���Ч
        _I2C_Stop(port);            // �����Ч������ֹͣ�ź�
        return 2;                   // ����Ӧ��ʧ��
    }
    _I2C_SCL_0(port); // ����SCL��׼����һ��ͨ��
    return 0;         // ����Ӧ��ɹ�
}
/**
*   @param  addr:  ��д�����ʼ��ַ��ȡֵ����0~255��Ӧ�ɴ�д���ҳ��*8�õ���at24c02֧�����8�ֽ�һҳд��
*   @param  wdata: ��д���飬Ӧ������8�ֽڣ�����������Ч
*   @retval 0������ �������쳣
*/
u8 AT24C02_WritePage(u8 addr, u8* wdata)
{
    u8 i;
    FM24CL64_I2C_Start();
    FM24CL64_I2C_SendByte(FM24CL64_ADDRESS); // д�豸��ַ
    if (FM24CL64_I2C_WaitAck(100) != 0) {
        FM24CL64_I2C_Stop();
        return 1;
    }
    FM24CL64_I2C_SendByte(addr); // д�洢��ַ
    if (FM24CL64_I2C_WaitAck(100) != 0) {
        FM24CL64_I2C_Stop();
        return 2;
    }
    for (i = 0; i < 8; i++) {
        FM24CL64_I2C_SendByte(wdata[i]);
        if (FM24CL64_I2C_WaitAck(100) != 0) {
            FM24CL64_I2C_Stop();
            return 3 + i;
        }
    }
    FM24CL64_I2C_Stop();
    return 0;
}
void AT24C02_ReadOTAInfo(void){
    memset(&OTA_Info, 0, OTA_INFOCB_SIZE);
    //FM24CL64_Read(0, (u8*) &OTA_Info, OTA_INFOCB_SIZE);
    FM24CL64_Read(0x161D ,&OTA_Info,sizeof(OTA_InfoCB) );
}

void AT24C02_WriteOTAInfo(void) {
    // for (u8 i = 0; i < OTA_INFOCB_SIZE / 8; i++) {
    //     //FM24CL64_Write(i * 8, ((u8*) &OTA_Info) + i * 8, 8);
    //     AT24C02_WritePage(i * 8, ((u8*) &OTA_Info) + i * 8);
    //     delay_ms(5);
    // }
    FM24CL64_Write(0x161D ,&OTA_Info,sizeof(OTA_InfoCB) );
}
