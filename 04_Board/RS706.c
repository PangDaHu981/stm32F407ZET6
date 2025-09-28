#include "rs706.h"
//#include "FreeRTOS.h"
//#include "task.h"
//#include "timers.h"

/**
 * @brief  RS706��ʼ��
 * @note
 * @retval None
 */
void RS706_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RS706_RCCCLOCK, ENABLE);   // ʹ��PC�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin   = RS706_PIN;          // LED0�˿�����
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;   // ��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   // �������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;   // IO���ٶ�Ϊ2MHz
    GPIO_Init(RS706_PORT, &GPIO_InitStructure);         // �����趨������ʼ��GPIO

//    xTimerStart(
//        xTimerCreate((const char*) "RS706_Event",
//                     (TickType_t) 200,     /* ��ʱ������ 1000(tick) */
//                     (UBaseType_t) pdTRUE, /* ����ģʽ */
//                     (void*) 1,            /* Ϊÿ����ʱ������һ��������ΨһID */
//                     (TimerCallbackFunction_t) RS706_Event),
//        0);   // �������ڶ�ʱ��
}
// ��ʼ����ʱ�� TIM2
void RS706_TIM_Init(void) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // ���� TIM2 ʱ��
    RCC_APB1PeriphClockCmd(RS706_TIM_RCC, ENABLE);
    
    // ��ʱ�� TIM2 ����������
    TIM_TimeBaseStructure.TIM_Period = (RS706_FEED_TIME_MS * 1000) - 1;  // ���㶨ʱ����
    TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;  // 1MHz ����Ƶ�� (������Ƶ 72MHz)
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(RS706_TIM, &TIM_TimeBaseStructure);
    
    // ʹ�ܶ�ʱ�������ж�
    TIM_ITConfig(RS706_TIM, TIM_IT_Update, ENABLE);
    
    // ���� NVIC
    NVIC_InitStructure.NVIC_IRQChannel = RS706_TIM_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // ������ʱ��
    TIM_Cmd(RS706_TIM, ENABLE);
}

// TIM2 �жϷ����������ڷ�ת RS706 ι�����ţ�
void RS706_TIM_HANDLER(void) {
    if (TIM_GetITStatus(RS706_TIM, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(RS706_TIM, TIM_IT_Update);
        
        // ��ת RS706 ι������
        GPIO_WriteBit(RS706_PORT, RS706_PIN, 
                      (BitAction)(!GPIO_ReadInputDataBit(RS706_PORT, RS706_PIN)));
    }
}
/**
 * @brief  RS706�¼�
 * @note
 * @retval None
 */
//static void RS706_Event(void) {
//    /* ���Ϸ�תι��IO */
//    if (GPIO_ReadInputDataBit(RS706_PORT, RS706_PIN) != 0) {
//        GPIO_ResetBits(RS706_PORT, RS706_PIN);
//    } else {
//        GPIO_SetBits(RS706_PORT, RS706_PIN);
//    }
//}

 void RS706_Event(void) {
    /* ���Ϸ�תι��IO */
    if (GPIO_ReadInputDataBit(RS706_PORT, RS706_PIN) != 0) {
        GPIO_ResetBits(RS706_PORT, RS706_PIN);
    } else {
        GPIO_SetBits(RS706_PORT, RS706_PIN);
    }
}

