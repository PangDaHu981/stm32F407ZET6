#ifndef __RS706_H__
#define __RS706_H__

#include "stm32f4xx.h"
#include "time.h"
#define RS706_RCCCLOCK RCC_AHB1Periph_GPIOF
#define RS706_PORT     GPIOF
#define RS706_PIN      GPIO_Pin_9

// ���嶨ʱ��
#define RS706_TIM           TIM2
#define RS706_TIM_RCC       RCC_APB1Periph_TIM2
#define RS706_TIM_IRQn      TIM2_IRQn
#define RS706_TIM_HANDLER   TIM2_IRQHandler
// ι��ʱ�䣨�Ժ���Ϊ��λ��
#define RS706_FEED_TIME_MS  500  // RS706 ��Ҫ MCU ����������ڷ�תһ��
/* RS706��ʼ�� */
void RS706_Init(void);
///* RS706�¼� */
//static void RS706_Event(void);

void RS706_Event(void);
void RS706_TIM_Init(void);
#endif
