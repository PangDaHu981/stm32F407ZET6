#include "DGUS.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"

/* ��Ļ��ʼ�� */
void DGUS_Init(void) {
    /* ʹ��ʱ�� */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);   //ʹ��GPIOAʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);  //ʹ��USART3ʱ��
    {                                                       /* �������ų�ʼ�� */
        GPIO_InitTypeDef GPIO_InitStructure;

        GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11;  // USART TX/RX
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;               //�������ģʽ
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              //�������
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;          //�������100MHz
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;               //����
        GPIO_Init(GPIOB, &GPIO_InitStructure);                      //��ʼ��
        /*ָ������ͨ��*/
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);  //����ΪUSART
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);  //����ΪUSART
    }

    { /* ���ڳ�ʼ�� */
        USART_InitTypeDef USART_InitStructure;
        USART_InitStructure.USART_BaudRate            = 460800;                          //���ò�����
        USART_InitStructure.USART_WordLength          = USART_WordLength_8b;             //λ���ݸ�ʽ
        USART_InitStructure.USART_StopBits            = USART_StopBits_1;                //ֹͣλ
        USART_InitStructure.USART_Parity              = USART_Parity_No;                 //��żУ��λ
        USART_InitStructure.USART_Mode                = USART_Mode_Tx;                   //����ģʽ����Ϊ�շ�ģʽ
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //��Ӳ������������
        USART_Init(USART3, &USART_InitStructure);                                        //����ָ���Ĳ�����ʼ������
    }
    /*ʹ�ܴ���*/
    USART_Cmd(USART3, ENABLE);  //ʹ�ܴ���
}
/* �������� */
void DGUS_Transmit(uint8_t* data, uint16_t length) {
    while (length--) {
        /* �ȴ�������� */
        while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET) {
        }
        /* ����һ���ֽ����ݵ����� */
        USART_SendData(USART3, (uint8_t) *data++);
    }
}
/* ���ݷ��ͻ��� */
uint8_t DGUS_SendBuffer[256];
/* �趨��ǰҳ */
void DGUS_SetPage(uint16_t page) {
    /* ��ȡ���ͻ��� */
    uint8_t* msg = DGUS_SendBuffer;
    /*����ָ��*/
    uint8_t* msg_len;
    /* ֡ͷ */
    msg[0] = 0x5A;
    msg[1] = 0xA5;
    /* ���ݳ��� */
    msg_len  = &msg[2];
    *msg_len = 0;
    /* ָ��*/
    msg[3 + (*msg_len)++] = 0x82;
    msg[3 + (*msg_len)++] = 0x00;
    msg[3 + (*msg_len)++] = 0x84;
    msg[3 + (*msg_len)++] = 0x5A;
    msg[3 + (*msg_len)++] = 0x01;
    msg[3 + (*msg_len)++] = page >> 8;
    msg[3 + (*msg_len)++] = page;
    /*���ͱ���*/
    DGUS_Transmit(msg, (*msg_len) + 3);
}

/* ����Ļ��ӡ�� */
void DGUS_Print(uint8_t new, const char* message, ...) {
    /* ��ǰ��ӡ�� */
    static uint8_t line = 0;
    /* ��ȡ���ͻ��� */
    uint8_t* msg = DGUS_SendBuffer;
    /*����ָ��*/
    uint8_t* msg_len;
    va_list  ap;
    /* ��Ҫ���� */
    if (new != 0) {
        /* ����������� */
        if (++line >= 24) {
            /* ������λ */
            line = 1;
            DGUS_Clean();
        }
    }
    /* ֡ͷ */
    msg[0] = 0x5A;
    msg[1] = 0xA5;
    /* ���ݳ��� */
    msg_len  = &msg[2];
    *msg_len = 0;
    /* ָ��*/
    msg[3 + (*msg_len)++] = 0x82;
    msg[3 + (*msg_len)++] = (uint8_t) ((0x4300 + (line * 0x0040)) >> 8);
    msg[3 + (*msg_len)++] = (uint8_t) ((0x4300 + (line * 0x0040)) & 0xFF);
    /* �����ִ�ӡ����Ϣ�� */
    va_start(ap, message);
    vsprintf((char*) (&(msg[3 + (*msg_len)])), message, ap);
    va_end(ap);
    /* ���㳤�� */
    *msg_len += strlen((char*) (&(msg[3 + (*msg_len)])));
    /* д������� */
    msg[3 + (*msg_len)++] = 0xFF;
    msg[3 + (*msg_len)++] = 0xFF;
    /*���ͱ���*/
    DGUS_Transmit(msg, (*msg_len) + 3);
}

/* ���� */
void DGUS_Clean(void) {
    /* ��ȡ���ͻ��� */
    uint8_t* msg = DGUS_SendBuffer;
    /*����ָ��*/
    uint8_t* msg_len;
    /* ���ҳ�� */
    for (uint8_t i = 1; i < 24; i++) {
        /* ֡ͷ */
        msg[0] = 0x5A;
        msg[1] = 0xA5;
        /* ���ݳ��� */
        msg_len  = &msg[2];
        *msg_len = 0;
        /* ָ��*/
        msg[3 + (*msg_len)++] = 0x82;
        msg[3 + (*msg_len)++] = (uint8_t) ((0x4300 + (i * 0x0040)) >> 8);
        msg[3 + (*msg_len)++] = (uint8_t) ((0x4300 + (i * 0x0040)) & 0xFF);
        /* д������� */
        msg[3 + (*msg_len)++] = 0xFF;
        msg[3 + (*msg_len)++] = 0xFF;
        /*���ͱ���*/
        DGUS_Transmit(msg, (*msg_len) + 3);
    }
}
