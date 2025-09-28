#include "led.h"

// LED IO初始化
void LED_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(LED_RCCCLOCK, ENABLE);  //使能时钟

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;    //普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  //速度
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;   //上拉

    GPIO_InitStructure.GPIO_Pin = COM_LED_PIN;
    GPIO_Init(COM_LED_PORT, &GPIO_InitStructure);  //初始化
    GPIO_ResetBits(COM_LED_PORT, COM_LED_PIN);

    GPIO_InitStructure.GPIO_Pin = NET_LED_PIN;
    GPIO_Init(NET_LED_PORT, &GPIO_InitStructure);  //初始化
    GPIO_ResetBits(NET_LED_PORT, NET_LED_PIN);

    GPIO_InitStructure.GPIO_Pin = FATFS_LED_PIN;
    GPIO_Init(FATFS_LED_PORT, &GPIO_InitStructure);  //初始化
    GPIO_ResetBits(FATFS_LED_PORT, FATFS_LED_PIN);
}
