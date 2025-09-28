#ifndef __RS706_H__
#define __RS706_H__

#include "stm32f4xx.h"
#include "time.h"
#define RS706_RCCCLOCK RCC_AHB1Periph_GPIOF
#define RS706_PORT     GPIOF
#define RS706_PIN      GPIO_Pin_9

// 定义定时器
#define RS706_TIM           TIM2
#define RS706_TIM_RCC       RCC_APB1Periph_TIM2
#define RS706_TIM_IRQn      TIM2_IRQn
#define RS706_TIM_HANDLER   TIM2_IRQHandler
// 喂狗时间（以毫秒为单位）
#define RS706_FEED_TIME_MS  500  // RS706 需要 MCU 在这个周期内翻转一次
/* RS706初始化 */
void RS706_Init(void);
///* RS706事件 */
//static void RS706_Event(void);

void RS706_Event(void);
void RS706_TIM_Init(void);
#endif
