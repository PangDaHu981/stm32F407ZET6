#include "I2C.h"

#define _I2C_RCCCLOCK RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOG



_I2C_PORT_STRUCT _I2C_Port[2] = {
    {GPIOG, GPIO_Pin_7, GPIOC, GPIO_Pin_6},
    {GPIOG, GPIO_Pin_8, GPIOG, GPIO_Pin_7}
};
void _I2C_Init(uint8_t port)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(_I2C_RCCCLOCK, ENABLE);

    GPIO_InitStructure.GPIO_Pin     = _I2C_Port[port].SDA.GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_OUT;    //普通输出模式
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_OD;    //推挽输出
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_UP;     //上拉
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_100MHz;
    GPIO_Init(_I2C_Port[port].SDA.GPIOx,&GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin     = _I2C_Port[port].SCL.GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_OUT;    //普通输出模式
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_OD;    //推挽输出
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_UP;     //上拉
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_100MHz;
    GPIO_Init(_I2C_Port[port].SCL.GPIOx,&GPIO_InitStructure);

    _I2C_SCL_1(port);
    _I2C_SDA_1(port);

}

void _I2C_SendByte(uint8_t port,uint8_t data){

    for(uint8_t i=0; i<8; i++){
        if(data & 0x80)
            _I2C_SDA_1(port);
        else
            _I2C_SDA_0(port);
        data <<= 1;
        _I2C_DELAY(2);
        _I2C_SCL_1(port);
        _I2C_DELAY(2);
        _I2C_SCL_0(port);
        _I2C_DELAY(2);
    }
    if(_I2C_SDA_Wait_Ack(port) == 0)
        asm("nop");

}




uint8_t _I2C_ReadByte(uint8_t port,uint8_t ack){

    uint8_t receive = 0;
    _I2C_SDA_1(port);
    for(uint8_t i=0; i<8; i++){
        _I2C_DELAY(2);
        _I2C_SCL_1(port);
        _I2C_DELAY(1);
        receive <<= 1;
        if(_I2C_SDA_READ(port)) receive |= 0x01;
        _I2C_DELAY(1);
        _I2C_SCL_0(port);
        _I2C_DELAY(2);
    }
    if(ack)_I2C_SDA_Ack(port);
    else _I2C_SDA_NAck(port);

    return receive;

}

void _I2C_Start(uint8_t port){

    _I2C_SDA_1(port);
    _I2C_SCL_1(port);
    _I2C_DELAY(2);
    _I2C_SDA_0(port);
    _I2C_DELAY(2);
    _I2C_SCL_0(port);
    _I2C_DELAY(2);

}

void _I2C_Stop(uint8_t port){

    _I2C_SDA_0(port);
    _I2C_DELAY(2);
    _I2C_SCL_1(port);
    _I2C_DELAY(2);
    _I2C_SDA_1(port);
    _I2C_DELAY(6);

}

static uint8_t _I2C_SDA_Wait_Ack(uint8_t port){

    uint8_t ack = 0;
    _I2C_SDA_1(port);
    _I2C_DELAY(2);
    _I2C_SCL_1(port);
    _I2C_DELAY(1);
    ack = _I2C_SDA_READ(port);
    _I2C_DELAY(1);
    _I2C_SCL_0(port);
    _I2C_DELAY(2);

    return ack?0:1;

}

static void _I2C_SDA_Ack(uint8_t port){

    _I2C_SDA_0(port);
    _I2C_DELAY(2);
    _I2C_SCL_1(port);
    _I2C_DELAY(2);
    _I2C_SCL_0(port);
    _I2C_DELAY(2);

}

static void _I2C_SDA_NAck(uint8_t port){

    _I2C_SDA_1(port);
    _I2C_DELAY(2);
    _I2C_SCL_1(port);
    _I2C_DELAY(2);
    _I2C_SCL_0(port);
    _I2C_DELAY(2);

}