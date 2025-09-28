#ifndef __SYS_LED_H__
#define __SYS_LED_H__

#include "stm32f4xx.h"

#define SYS_LED_RCCCLOCK    RCC_AHB1Periph_GPIOE
#define SYS_LED_PORT        GPIOE
#define SYS_LED_PIN         GPIO_Pin_4

#define SYS_LED_On()        GPIO_SetBits(SYS_LED_PORT,SYS_LED_PIN)      //开灯
#define SYS_LED_Off()       GPIO_ResetBits(SYS_LED_PORT,SYS_LED_PIN)    //关灯
#define SYS_LED_OnOff()     GPIO_ToggleBits(SYS_LED_PORT,SYS_LED_PIN)   //转换灯


void SYS_LED_Init(void);//初始化


#endif


