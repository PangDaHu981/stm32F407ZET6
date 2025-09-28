#include "sys_led.h"


void Task_SYS_Led(void);
//LED IO��ʼ��
void SYS_LED_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(SYS_LED_RCCCLOCK,ENABLE);/*ʹ��GPIOFʱ��*/


    GPIO_InitStructure.GPIO_Mode    = GPIO_Mode_OUT;    /*����*/
    GPIO_InitStructure.GPIO_OType   = GPIO_OType_PP;    /*�������*/
    GPIO_InitStructure.GPIO_Pin     = SYS_LED_PIN;      /*PF7*/
    GPIO_InitStructure.GPIO_PuPd    = GPIO_PuPd_UP;     /*����*/
    GPIO_InitStructure.GPIO_Speed   = GPIO_Speed_50MHz; /**/
    GPIO_Init(SYS_LED_PORT,&GPIO_InitStructure);        /*��ʼ��IO*/

}

