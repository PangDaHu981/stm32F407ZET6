#include "spi.h"


#ifdef __FREERTOS__
static SemaphoreHandle_t SPI1_Lock = NULL;
#endif
/* SPI1��ʼ�� */
void SPI1_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);  //ʹ��GPIOBʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);   //ʹ��SPI1ʱ��

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;  //
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;             //���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;            //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;         // 100MHz
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;             //����
    GPIO_Init(GPIOA, &GPIO_InitStructure);                    //��ʼ��

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;        //
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;      //���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;     //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 100MHz
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;    //����
    GPIO_Init(GPIOA, &GPIO_InitStructure);             //��ʼ��

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);  // PB3����Ϊ SPI1
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);  // PB4����Ϊ SPI1
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);  // PB5����Ϊ SPI1

    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;                  //����SPI����ģʽ:����Ϊ��SPI
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;                  //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_Low;                     //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_1Edge;                   //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;                     // NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;        //���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;                 //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial     = 7;                                // CRCֵ����Ķ���ʽ
    SPI_Init(SPI1, &SPI_InitStructure);                                         //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

    SPI_Cmd(SPI1, ENABLE);     //ʹ��SPI����
    SPI1_ReadWriteByte(0xff);  //��������

#ifdef __FREERTOS__
    if (SPI1_Lock == NULL) {
        SPI1_Lock = xSemaphoreCreateMutex();
        SPI1_Give();
    }
#endif
}

/* ���ò����� */
void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler) {
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));  //�ж���Ч��
    SPI1->CR1 &= 0XFFC7;                                             //λ3-5���㣬�������ò�����
    SPI1->CR1 |= SPI_BaudRatePrescaler;                              //����SPI1�ٶ�
    SPI_Cmd(SPI1, ENABLE);                                           //ʹ��SPI1
    SPI1_ReadWriteByte(0xff);                                        //��������
}

/* SPI1 ��дһ���ֽ� */
uint8_t SPI1_ReadWriteByte(uint8_t TxData) {
    SPI_I2S_SendData(SPI1, TxData);  //ͨ������SPIx����һ��byte  ����
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
        ;  //�ȴ���������
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
        ;                              //�ȴ�������һ��byte
    return SPI_I2S_ReceiveData(SPI1);  //����ͨ��SPIx������յ�����
}

/*��ȡSPI1�˿�ʹ��Ȩ*/
void SPI1_Take(void) {
#ifdef __FREERTOS__
    if (SPI1_Lock != NULL)
        xSemaphoreTake(SPI1_Lock, portMAX_DELAY);
#endif
}
/*�ͷ�SPI1�˿�ʹ��Ȩ*/
void SPI1_Give(void) {
#ifdef __FREERTOS__
    if (SPI1_Lock != NULL)
        xSemaphoreGive(SPI1_Lock);
#endif
}

#ifdef __FREERTOS__
static SemaphoreHandle_t SPI2_Lock = NULL;
#endif
/* SPI ��ʼ�� */
void SPI2_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);  //ʹ��GPIOBʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);   //ʹ��SPI2ʱ��

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;  // PB3~5���ù������
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;                             //���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                            //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;                        // 100MHz
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;                             //����
    GPIO_Init(GPIOB, &GPIO_InitStructure);                                    //��ʼ��

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_SPI2);  // PB3����Ϊ SPI2
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_SPI2);  // PB4����Ϊ SPI2
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_SPI2);  // PB5����Ϊ SPI2

    SPI_InitStructure.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;  //����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
    SPI_InitStructure.SPI_Mode              = SPI_Mode_Master;                  //����SPI����ģʽ:����Ϊ��SPI
    SPI_InitStructure.SPI_DataSize          = SPI_DataSize_8b;                  //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI_InitStructure.SPI_CPOL              = SPI_CPOL_High;                    //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
    SPI_InitStructure.SPI_CPHA              = SPI_CPHA_2Edge;                   //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    SPI_InitStructure.SPI_NSS               = SPI_NSS_Soft;                     // NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;        //���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
    SPI_InitStructure.SPI_FirstBit          = SPI_FirstBit_MSB;                 //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    SPI_InitStructure.SPI_CRCPolynomial     = 7;                                // CRCֵ����Ķ���ʽ
    SPI_Init(SPI2, &SPI_InitStructure);                                         //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���

    SPI_Cmd(SPI2, ENABLE);     //ʹ��SPI����
    SPI2_ReadWriteByte(0xff);  //��������

#ifdef __FREERTOS__
    if (SPI2_Lock == NULL) {
        SPI2_Lock = xSemaphoreCreateMutex();
        SPI2_Give();
    }
#endif
}

/* SPI2 ���ò����� */
void SPI2_SetSpeed(uint8_t SPI_BaudRatePrescaler) {
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));  //�ж���Ч��
    SPI2->CR1 &= 0XFFC7;                                             //λ3-5���㣬�������ò�����
    SPI2->CR1 |= SPI_BaudRatePrescaler;                              //����SPI2�ٶ�
    SPI_Cmd(SPI2, ENABLE);                                           //ʹ��SPI2
    SPI2_ReadWriteByte(0xff);                                        //��������
}

/* SPI2 ��дһ���ֽ� */
uint8_t SPI2_ReadWriteByte(uint8_t TxData) {
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET)
        ;                            //�ȴ���������
    SPI_I2S_SendData(SPI2, TxData);  //ͨ������SPIx����һ��byte  ����
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
        ;                              //�ȴ�������һ��byte
    return SPI_I2S_ReceiveData(SPI2);  //����ͨ��SPIx������յ�����
}

/*��ȡSPI2�˿�ʹ��Ȩ*/
void SPI2_Take(void) {
#ifdef __FREERTOS__
    if (SPI2_Lock != NULL)
        xSemaphoreTake(SPI2_Lock, portMAX_DELAY);
#endif
}
/*�ͷ�SPI2�˿�ʹ��Ȩ*/
void SPI2_Give(void) {
#ifdef __FREERTOS__
    if (SPI2_Lock != NULL)
        xSemaphoreGive(SPI2_Lock);
#endif
}
