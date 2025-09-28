/***
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* 文件名:     Key.c
* 描述:       按键开关驱动文件
* 版本号:     v0.1
* 作者:       孙威
* 创建日期:   2021/4/18
*
* @青岛程宁新能源科技有限公司
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
**/

#include "Key.h"

/*按键开关时钟*/
#define KEY_SWITCK_RCCCLOCK RCC_AHB1Periph_GPIOG

/*按键开关IO定义*/
static struct {
    GPIO_TypeDef* GPIOx;
    uint16_t      GPIO_Pin;
    uint8_t       H_State;
} Key_Gpio[] = {
    {GPIOG, GPIO_Pin_12, 1},
};

/***
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* 函数名称:   Key_Init()
* 输入参数:   无
* 返回参数:   无
* 作者:       孙威
* 创建日期:   2021/4/18
* 功能:       按键开关初始化
*
* @青岛程宁新能源科技有限公司
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
**/
void Key_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(KEY_SWITCK_RCCCLOCK, ENABLE);  //使能GPIO时钟

    /*GPIO初始化设置*/
    for (uint8_t i = 0; i < ArraySize(Key_Gpio); i++) {
        GPIO_InitStructure.GPIO_Pin  = Key_Gpio[i].GPIO_Pin;  //引脚配置
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;          //输入
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;      //浮空
        GPIO_Init(Key_Gpio[i].GPIOx, &GPIO_InitStructure);    //初始化引脚
    }

    return;
}

/***
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* 函数名称:   Key_Get()
* 输入参数:   无
* 返回参数:   按键开关的键值
* 作者:       孙威
* 创建日期:   2021/4/18
* 功能:       获取按键开关的键值
*
* @青岛程宁新能源科技有限公司
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
**/
uint8_t Key_Get(void) {
    uint8_t key = 0;

    /*循环获取键值*/
    for (uint8_t i = 0; i < ArraySize(Key_Gpio); i++) {
        uint8_t val = GPIO_ReadInputDataBit(Key_Gpio[i].GPIOx, Key_Gpio[i].GPIO_Pin) ? Key_Gpio[i].H_State : !Key_Gpio[i].H_State;
        key |= (val << i);
    }

    return key;
}
