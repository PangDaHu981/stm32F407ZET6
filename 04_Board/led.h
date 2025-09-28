#ifndef __LED_H__
#define __LED_H__

#include "stm32f4xx.h"
#define LED_RCCCLOCK RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOF

#define COM_LED_PORT GPIOB
#define COM_LED_PIN  GPIO_Pin_3

#define NET_LED_PORT GPIOB
#define NET_LED_PIN  GPIO_Pin_4

#define FATFS_LED_PORT GPIOF
#define FATFS_LED_PIN  GPIO_Pin_11

#define LED_On(led)    GPIO_SetBits(led##_LED_PORT, led##_LED_PIN)     //����
#define LED_Off(led)   GPIO_ResetBits(led##_LED_PORT, led##_LED_PIN)   //�ص�
#define LED_OnOff(led) GPIO_ToggleBits(led##_LED_PORT, led##_LED_PIN)  //ת����

void LED_Init(void);  //��ʼ��

#endif
