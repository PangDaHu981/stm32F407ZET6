#include "led.h"

// LED IO��ʼ��
void LED_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHB1PeriphClockCmd(LED_RCCCLOCK, ENABLE);  //ʹ��ʱ��

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;    //��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  //�ٶ�
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;   //����

    GPIO_InitStructure.GPIO_Pin = COM_LED_PIN;
    GPIO_Init(COM_LED_PORT, &GPIO_InitStructure);  //��ʼ��
    GPIO_ResetBits(COM_LED_PORT, COM_LED_PIN);

    GPIO_InitStructure.GPIO_Pin = NET_LED_PIN;
    GPIO_Init(NET_LED_PORT, &GPIO_InitStructure);  //��ʼ��
    GPIO_ResetBits(NET_LED_PORT, NET_LED_PIN);

    GPIO_InitStructure.GPIO_Pin = FATFS_LED_PIN;
    GPIO_Init(FATFS_LED_PORT, &GPIO_InitStructure);  //��ʼ��
    GPIO_ResetBits(FATFS_LED_PORT, FATFS_LED_PIN);
}
