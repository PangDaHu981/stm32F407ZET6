#include "beep.h"

// BEEP IO��ʼ��
void Beep_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(BEEP_RCCCLOCK, ENABLE);  //ʹ��PC�˿�ʱ��

    GPIO_InitStructure.GPIO_Pin   = BEEP_PIN;         // LED0�˿�����
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;   //
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;    //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  // IO���ٶ�Ϊ2MHz
    GPIO_Init(BEEP_PORT, &GPIO_InitStructure);        //�����趨������ʼ��GPIO
}

void Beep_Off(void) {
    GPIO_ResetBits(BEEP_PORT, BEEP_PIN);  // �����
}

void Beep_On(void) {
    GPIO_SetBits(BEEP_PORT, BEEP_PIN);  // �����
}

void Beep_Toggle(void) {
    GPIO_ToggleBits(BEEP_PORT, BEEP_PIN);  // �л����
}
