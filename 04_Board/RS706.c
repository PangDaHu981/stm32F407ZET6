#include "rs706.h"
//#include "FreeRTOS.h"
//#include "task.h"
//#include "timers.h"

/**
 * @brief  RS706初始化
 * @note
 * @retval None
 */
void RS706_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RS706_RCCCLOCK, ENABLE);   // 使能PC端口时钟

    GPIO_InitStructure.GPIO_Pin   = RS706_PIN;          // LED0端口配置
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;   // 普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // IO口速度为2MHz
    GPIO_Init(RS706_PORT, &GPIO_InitStructure);         // 根据设定参数初始化GPIO

//    xTimerStart(
//        xTimerCreate((const char*) "RS706_Event",
//                     (TickType_t) 200,     /* 定时器周期 1000(tick) */
//                     (UBaseType_t) pdTRUE, /* 周期模式 */
//                     (void*) 1,            /* 为每个计时器分配一个索引的唯一ID */
//                     (TimerCallbackFunction_t) RS706_Event),
//        0);   // 开启周期定时器
}
// 初始化定时器 TIM2
void RS706_TIM_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 开启 TIM2 时钟
    RCC_APB1PeriphClockCmd(RS706_TIM_RCC, ENABLE);
    
    // 定时器 TIM2 计数器配置
    TIM_TimeBaseStructure.TIM_Period = (RS706_FEED_TIME_MS * 1000) - 1;  // 计算定时周期
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;  // 1MHz 计数频率 (假设主频 72MHz)
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(RS706_TIM, &TIM_TimeBaseStructure);
    
    // 使能定时器更新中断
    TIM_ITConfig(RS706_TIM, TIM_IT_Update, ENABLE);
    
    // 配置 NVIC
    NVIC_InitStructure.NVIC_IRQChannel = RS706_TIM_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 启动定时器
    TIM_Cmd(RS706_TIM, ENABLE);
}

// TIM2 中断服务函数（定期翻转 RS706 喂狗引脚）
void RS706_TIM_HANDLER(void) {
    if (TIM_GetITStatus(RS706_TIM, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(RS706_TIM, TIM_IT_Update);
        
        // 翻转 RS706 喂狗引脚
        GPIO_WriteBit(RS706_PORT, RS706_PIN, 
                      (BitAction)(!GPIO_ReadInputDataBit(RS706_PORT, RS706_PIN)));
    }
}
/**
 * @brief  RS706事件
 * @note
 * @retval None
 */
//static void RS706_Event(void) {
//    /* 不断反转喂狗IO */
//    if (GPIO_ReadInputDataBit(RS706_PORT, RS706_PIN) != 0) {
//        GPIO_ResetBits(RS706_PORT, RS706_PIN);
//    } else {
//        GPIO_SetBits(RS706_PORT, RS706_PIN);
//    }
//}

 void RS706_Event(void) {
    /* 不断反转喂狗IO */
    if (GPIO_ReadInputDataBit(RS706_PORT, RS706_PIN) != 0) {
        GPIO_ResetBits(RS706_PORT, RS706_PIN);
    } else {
        GPIO_SetBits(RS706_PORT, RS706_PIN);
    }
}

