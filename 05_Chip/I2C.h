#ifndef __I2C_H__
#define __I2C_H__

#include "stm32f4xx.h"

#define FM24CL64_I2C 1

#define _I2C_SCL_1(port)    GPIO_SetBits(_I2C_Port[port].SCL.GPIOx,_I2C_Port[port].SCL.GPIO_Pin)
#define _I2C_SDA_1(port)    GPIO_SetBits(_I2C_Port[port].SDA.GPIOx,_I2C_Port[port].SDA.GPIO_Pin)
#define _I2C_SCL_0(port)    GPIO_ResetBits(_I2C_Port[port].SCL.GPIOx,_I2C_Port[port].SCL.GPIO_Pin)
#define _I2C_SDA_0(port)    GPIO_ResetBits(_I2C_Port[port].SDA.GPIOx,_I2C_Port[port].SDA.GPIO_Pin)
#define _I2C_SDA_READ(port) GPIO_ReadInputDataBit(_I2C_Port[port].SDA.GPIOx,_I2C_Port[port].SDA.GPIO_Pin)
#define _I2C_DELAY(us)      for(uint32_t i=0; i< us*10; i++)asm("nop");
// struct _I2C_PORT_STRUCT{
//     struct {
//     GPIO_TypeDef * GPIOx;
//     uint16_t GPIO_Pin;
//     }SDA;
//     struct {
//     GPIO_TypeDef * GPIOx;
//     uint16_t GPIO_Pin;
//     }SCL;
// }_I2C_Port[]={
//     {{GPIOG, GPIO_Pin_7}, {GPIOG, GPIO_Pin_6}}, //  PCF8563
//     {{GPIOG, GPIO_Pin_8}, {GPIOG, GPIO_Pin_7}}, //  FM24CL04
// };

typedef struct {
    struct {
    GPIO_TypeDef * GPIOx;
    uint16_t GPIO_Pin;
    }SDA;
    struct {
    GPIO_TypeDef * GPIOx;
    uint16_t GPIO_Pin;
    }SCL;
}_I2C_PORT_STRUCT;

extern _I2C_PORT_STRUCT _I2C_Port[2];

void _I2C_Init(uint8_t port);
void _I2C_SendByte(uint8_t port,uint8_t data);
uint8_t _I2C_ReadByte(uint8_t port,uint8_t ack);
void _I2C_Start(uint8_t port);
void _I2C_Stop(uint8_t port);

static uint8_t _I2C_SDA_Wait_Ack(uint8_t port);
static void _I2C_DeInit(uint8_t port);
static void _I2C_SDA_Ack(uint8_t port);
static void _I2C_SDA_NAck(uint8_t port);

#endif