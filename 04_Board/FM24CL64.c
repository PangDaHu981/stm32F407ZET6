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
    FM24CL64_I2C_SendByte((uint8_t) (addr >> 8));    //数据地址高位
    FM24CL64_I2C_SendByte((uint8_t) (addr & 0xFF));  //数据地址低位
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
    FM24CL64_I2C_SendByte((uint8_t) (addr >> 8));    //数据地址高位
    FM24CL64_I2C_SendByte((uint8_t) (addr & 0xFF));  //数据地址低位
    while (lens--) {
        FM24CL64_I2C_SendByte(*wbuf++);
    }
    FM24CL64_I2C_Stop();
}
u8 FM24CL64_I2C_WaitAck(int timeout)
{
    // 假设FM24CL64使用的I2C端口号为1，如有不同请自行调整
    uint8_t port = 1;
    _I2C_SDA_1(port); // 释放SDA
    while (_I2C_SDA_READ(port)) { // 等待从机拉低SDA
        timeout--;
        delay_us(2);
        if (timeout <= 0) {
            _I2C_Stop(port); // 超时后发送停止信号
            return 1;        // 接收应答失败
        }
    }
    _I2C_SCL_1(port); // 拉高SCL，准备接收应答信号
    delay_us(2);
    if (_I2C_SDA_READ(port) != 0) { // 检测应答信号是否有效
        _I2C_Stop(port);            // 如果无效，发送停止信号
        return 2;                   // 接收应答失败
    }
    _I2C_SCL_0(port); // 拉低SCL，准备下一次通信
    return 0;         // 接收应答成功
}
/**
*   @param  addr:  待写入的起始地址，取值区间0~255，应由待写入的页号*8得到。at24c02支持最大8字节一页写入
*   @param  wdata: 待写数组，应不超过8字节，超出部分无效
*   @retval 0：正常 其他：异常
*/
u8 AT24C02_WritePage(u8 addr, u8* wdata)
{
    u8 i;
    FM24CL64_I2C_Start();
    FM24CL64_I2C_SendByte(FM24CL64_ADDRESS); // 写设备地址
    if (FM24CL64_I2C_WaitAck(100) != 0) {
        FM24CL64_I2C_Stop();
        return 1;
    }
    FM24CL64_I2C_SendByte(addr); // 写存储地址
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
