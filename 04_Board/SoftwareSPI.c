#include "SoftwareSPI.h"

#define _I2C_RCCCLOCK RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOE

struct _SPI_Port_STRUCT {
    struct {
        GPIO_TypeDef* GPIOx;
        uint16_t      GPIO_Pin;
    } MOSI;
    struct {
        GPIO_TypeDef* GPIOx;
        uint16_t      GPIO_Pin;
    } MISO;
    struct {
        GPIO_TypeDef* GPIOx;
        uint16_t      GPIO_Pin;
    } SCK;
    //SemaphoreHandle_t Lock;           // 权限锁
    uint8_t           Lock;
    uint8_t           CPOL     : 1;   // 空闲状态电平 0 低电平 1 高电平
    uint8_t           CPHA     : 1;   // 数据采样边沿 0 第一跳变沿 1 第二跳变沿
    uint8_t           FirstBit : 1;   // 数据传输起始位 0 LSB 1 MSB
} _SPI_Port[] = {

    {
        /* W25Q64 */
        {GPIOF, GPIO_Pin_9},
        {GPIOF, GPIO_Pin_8},
        {GPIOF, GPIO_Pin_7},
    },
    {
        /* WK2124 */
        {GPIOE, GPIO_Pin_4},
        {GPIOE, GPIO_Pin_5},
        {GPIOE, GPIO_Pin_3},
    },
};

#define _SPI_SCK_1(port)           GPIO_SetBits(_SPI_Port[port].SCK.GPIOx, _SPI_Port[port].SCK.GPIO_Pin)
#define _SPI_SCK_0(port)           GPIO_ResetBits(_SPI_Port[port].SCK.GPIOx, _SPI_Port[port].SCK.GPIO_Pin)
#define _SPI_SCK_T(port)           GPIO_ToggleBits(_SPI_Port[port].SCK.GPIOx, _SPI_Port[port].SCK.GPIO_Pin)
#define _SPI_SCK_IDLE(port)        GPIO_WriteBit(_SPI_Port[port].SCK.GPIOx, _SPI_Port[port].SCK.GPIO_Pin, (BitAction) _SPI_Port[port].CPOL)
#define _SPI_MOSI_1(port)          GPIO_SetBits(_SPI_Port[port].MOSI.GPIOx, _SPI_Port[port].MOSI.GPIO_Pin)
#define _SPI_MOSI_0(port)          GPIO_ResetBits(_SPI_Port[port].MOSI.GPIOx, _SPI_Port[port].MOSI.GPIO_Pin)
#define _SPI_MOSI_Write(port, bit) GPIO_WriteBit(_SPI_Port[port].MOSI.GPIOx, _SPI_Port[port].MOSI.GPIO_Pin, (BitAction) bit)
#define _SPI_MISO_READ(port)       GPIO_ReadInputDataBit(_SPI_Port[port].MISO.GPIOx, _SPI_Port[port].MISO.GPIO_Pin)
#define _SPI_DELAY(us)                                       \
    for (volatile uint32_t delay = us * 5; delay; delay--) { \
    }

void SoftwareSPI_Init(uint8_t port,      // 端口号
                      uint8_t mode,      // SPI模式
                      uint8_t firstBit   // 数据传输起始位 0 LSB 1 MSB
) {
    GPIO_InitTypeDef GPIO_InitStructure;

    _SPI_Port[port].CPOL     = ((mode & 0x02) == 0x02);
    _SPI_Port[port].CPHA     = ((mode & 0x01) == 0x01);
    _SPI_Port[port].FirstBit = firstBit;

    RCC_AHB1PeriphClockCmd(_I2C_RCCCLOCK, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = _SPI_Port[port].MOSI.GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;   // 普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   // 开漏输出
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;    // 上拉
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(_SPI_Port[port].MOSI.GPIOx, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = _SPI_Port[port].MISO.GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;   // 普通输入模式
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;   // 上拉
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(_SPI_Port[port].MISO.GPIOx, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = _SPI_Port[port].SCK.GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;   // 普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   // 开漏输出
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;    // 上拉
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(_SPI_Port[port].SCK.GPIOx, &GPIO_InitStructure);
    /* 总线空闲 */
    _SPI_SCK_IDLE(port);

    // if (_SPI_Port[port].Lock == NULL) {
    //     _SPI_Port[port].Lock = xSemaphoreCreateMutex();
    //     SoftwareSPI_Give(port);
    // }
}

/* SPI 读写一个字节 */
uint8_t SoftwareSPI_ReadWriteByte(uint8_t port, uint8_t TxData) {
    uint8_t rec = 0;
    uint8_t bit = 0;

    _SPI_SCK_IDLE(port);
    for (uint8_t i = 0; i < 8; i++) {
        if (_SPI_Port[port].FirstBit == 0) {   // 低位在前
            bit = (TxData >> i) & 0x01;        // 待发送位为数据低位
        } else {                               // 高位在前
            bit = (TxData << i) & 0x80;        // 待发送位为数据高位
        }
        _SPI_MOSI_Write(port, bit);        // 发送数据
        _SPI_DELAY(1);                     // 等待时钟周期
        _SPI_SCK_T(port);                  // 发出第一个时钟跳变沿信号
        _SPI_DELAY(1);                     // 等待时钟周期
        if (_SPI_Port[port].CPHA == 0) {   // 如果第一跳变沿数据被采样
            bit = _SPI_MISO_READ(port);    // 数据采样
        }
        _SPI_SCK_T(port);                  // 发出第二个时钟跳变沿信号
        if (_SPI_Port[port].CPHA != 0) {   // 如果第二跳变沿数据被采样
            bit = _SPI_MISO_READ(port);    // 数据采样
        }
        if (_SPI_Port[port].FirstBit == 0) {   // 低位在前
            rec |= (bit << i);                 // 数据放在高位
        } else {                               // 高位在前
            rec <<= 1;                         // 整合数据
            rec |= bit;                        // 数据放在低位
        }
    }
    return rec;   // 返回通过SPIx最近接收的数据
}

/*获取SPI1端口使用权*/
void SoftwareSPI_Take(uint8_t port) {
    // if (_SPI_Port[port].Lock != NULL) {
    //     //xSemaphoreTake(_SPI_Port[port].Lock, portMAX_DELAY);
    // }
}
/*释放SPI1端口使用权*/
void SoftwareSPI_Give(uint8_t port) {
    // if (_SPI_Port[port].Lock != NULL) {
    //     //xSemaphoreGive(_SPI_Port[port].Lock);
    // }
}
