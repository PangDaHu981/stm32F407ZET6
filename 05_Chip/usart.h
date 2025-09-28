#ifndef __USART_H
#define __USART_H
#include "stdarg.h"
#include "stdio.h"
#include "stm32f4xx.h"
#include "string.h"

#define USART1_RX_SIZE 2048   // ������ջ������ܴ�С
#define USART1_TX_SIZE 2048   // ���巢�ͻ������ܴ�С
#define USART1_REC_LEN 256    // ���嵥���������ֽ��� 256
#define NUM            10     // ������ջ���θ���

#define USART1_RD_PORT_RCC RCC_AHB1Periph_GPIOC /* �����շ�ʹ�ܶ˿�ʱ�� */
#define USART1_RD_PORT     GPIOC                /* �����շ�ʹ�ܶ˿� */
#define USART1_RD_PIN      GPIO_Pin_1           /* �����շ�ʹ������ */
#define USART1_RD_RECV     Bit_SET              /* �����շ�ʹ�� ����ģʽ �ߵ�ƽ */
#define USART1_RD_SEND     Bit_RESET            /* �����շ�ʹ�� ����ģʽ �͵�ƽ */

typedef struct {
    uint8_t* start;
    uint8_t* end;
} UCB_URxBufptr;   // ������¼һ�λ���������β��ַ

typedef struct {
    u16            URxCount;          // ��¼���ջ�������������
    UCB_URxBufptr  URxDataPtr[NUM];   // ��������飬��������Ϊÿһ�����ݵ���β��ַ
    UCB_URxBufptr* URxDataIN;         // дλ��
    UCB_URxBufptr* URxDataOUT;        // ��λ��
    UCB_URxBufptr* URxDataEND;        // ���һ�λ������ĵ�ַ�������ж��Ƿ���Ҫ��ͷ
} UCB_ControlBlock;                   // USART Control Block	������¼���յ���ÿһ�������ݵ���βλ�úͶ�дλ��

void USART1_Init(u32 baudrate);      // ����1��ʼ����baudrate���ò�����
void DMA_USART1_RX_Init(void);       // ����1��DMA��ʼ��
void U1Rx_Ptr_Init(void);            // USART Control Block�и����ʼ��
void u1_printf(char* format, ...);   // ����1��ӡ����



extern UCB_ControlBlock U1_CB;                           // ����Ϊ�ⲿ��������stm32f10x_it.c�е�USART1_IRQHandler�������õ�
extern uint8_t               USART1_RX_BUF[USART1_RX_SIZE];   // ����Ϊ�ⲿ��������stm32f10x_it.c�е�USART1_IRQHandler�������õ�


#endif
