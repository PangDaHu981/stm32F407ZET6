#include "sys_led.h"


void Task_SYS_Led(void);
//LED IO初始化
void SYS_LED_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(SYS_LED_RCCCLOCK,ENABLE);/*使能GPIOF时钟*/


    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_OUT;    /*复用*/
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;    /*推挽输出*/
    GPIO_InitStructure.GPIO_Pin     = SYS_LED_PIN;      /*PF7*/
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_UP;     /*上拉*/
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz; /**/
    GPIO_Init(SYS_LED_PORT,&GPIO_InitStructure);        /*初始化IO*/

}

