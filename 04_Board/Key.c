/***
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* �ļ���:     Key.c
* ����:       �������������ļ�
* �汾��:     v0.1
* ����:       ����
* ��������:   2021/4/18
*
* @�ൺ��������Դ�Ƽ����޹�˾
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
**/

#include "Key.h"

/*��������ʱ��*/
#define KEY_SWITCK_RCCCLOCK RCC_AHB1Periph_GPIOG

/*��������IO����*/
static struct {
    GPIO_TypeDef* GPIOx;
    uint16_t      GPIO_Pin;
    uint8_t       H_State;
} Key_Gpio[] = {
    {GPIOG, GPIO_Pin_12, 1},
};

/***
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* ��������:   Key_Init()
* �������:   ��
* ���ز���:   ��
* ����:       ����
* ��������:   2021/4/18
* ����:       �������س�ʼ��
*
* @�ൺ��������Դ�Ƽ����޹�˾
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
**/
void Key_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(KEY_SWITCK_RCCCLOCK, ENABLE);  //ʹ��GPIOʱ��

    /*GPIO��ʼ������*/
    for (uint8_t i = 0; i < ArraySize(Key_Gpio); i++) {
        GPIO_InitStructure.GPIO_Pin  = Key_Gpio[i].GPIO_Pin;  //��������
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;          //����
        GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;      //����
        GPIO_Init(Key_Gpio[i].GPIOx, &GPIO_InitStructure);    //��ʼ������
    }

    return;
}

/***
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
* ��������:   Key_Get()
* �������:   ��
* ���ز���:   �������صļ�ֵ
* ����:       ����
* ��������:   2021/4/18
* ����:       ��ȡ�������صļ�ֵ
*
* @�ൺ��������Դ�Ƽ����޹�˾
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
**/
uint8_t Key_Get(void) {
    uint8_t key = 0;

    /*ѭ����ȡ��ֵ*/
    for (uint8_t i = 0; i < ArraySize(Key_Gpio); i++) {
        uint8_t val = GPIO_ReadInputDataBit(Key_Gpio[i].GPIOx, Key_Gpio[i].GPIO_Pin) ? Key_Gpio[i].H_State : !Key_Gpio[i].H_State;
        key |= (val << i);
    }

    return key;
}
