#include "beep.h"

// BEEP IO初始化
void Beep_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(BEEP_RCCCLOCK, ENABLE);  //使能PC端口时钟

    GPIO_InitStructure.GPIO_Pin   = BEEP_PIN;         // LED0端口配置
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;   //
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;    //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  // IO口速度为2MHz
    GPIO_Init(BEEP_PORT, &GPIO_InitStructure);        //根据设定参数初始化GPIO
}

void Beep_Off(void) {
    GPIO_ResetBits(BEEP_PORT, BEEP_PIN);  // 输出低
}

void Beep_On(void) {
    GPIO_SetBits(BEEP_PORT, BEEP_PIN);  // 输出高
}

void Beep_Toggle(void) {
    GPIO_ToggleBits(BEEP_PORT, BEEP_PIN);  // 切换输出
}
