#include "usart.h"
#include "delay.h"
uint8_t               USART1_RX_BUF[USART1_RX_SIZE];   // 接收缓冲
uint8_t               USART1_TX_BUF[USART1_TX_SIZE];   // 发送缓冲
UCB_ControlBlock U1_CB;

void USART1_Init(u32 baudrate) {
    GPIO_InitTypeDef  GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef  NVIC_InitStruct;

    // 1. 使能时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); // GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); // USART1时钟
    RCC_AHB1PeriphClockCmd(USART1_RD_PORT_RCC, ENABLE);   // 使能时钟
    // 2. 配置GPIO
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);  // TX
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1); // RX

    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

            GPIO_InitTypeDef GPIO_InitStructure;
        GPIO_InitStructure.GPIO_Pin   = USART1_RD_PIN;                   // USART TX/RX
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;                   // 复用输出模式
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                   // 推挽输出
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;               // 输出速率100MHz
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;                    // 上拉
        GPIO_Init(USART1_RD_PORT, &GPIO_InitStructure);                  // 初始化
        GPIO_WriteBit(USART1_RD_PORT, USART1_RD_PIN, USART1_RD_RECV);   // 设置为接收模式

    // 3. 配置USART
    USART_InitStruct.USART_BaudRate            = baudrate;
    USART_InitStruct.USART_WordLength          = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits            = USART_StopBits_1;
    USART_InitStruct.USART_Parity              = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStruct);

    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);

    // 4. NVIC配置
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    /* 切换为非接收模式 */
    //GPIO_WriteBit(USART1_RD_PORT, USART1_RD_PIN, USART1_RD_SEND);
     GPIO_WriteBit(USART1_RD_PORT, USART1_RD_PIN, USART1_RD_RECV);
    U1Rx_Ptr_Init();
    DMA_USART1_RX_Init();
    USART_Cmd(USART1, ENABLE);
}

void DMA_USART1_RX_Init(void) {
    DMA_InitTypeDef DMA_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); // F407的USART1 RX对应DMA2, Stream2, Channel4

    DMA_DeInit(DMA2_Stream2);
    while (DMA_GetCmdStatus(DMA2_Stream2) != DISABLE);

    DMA_InitStruct.DMA_Channel = DMA_Channel_4;
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)USART1_RX_BUF;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStruct.DMA_BufferSize = USART1_REC_LEN + 1;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream2, &DMA_InitStruct);

    DMA_Cmd(DMA2_Stream2, ENABLE);
}


void U1Rx_Ptr_Init(void) {
    U1_CB.URxCount         = 0;
    U1_CB.URxDataIN        = &U1_CB.URxDataPtr[0];
    U1_CB.URxDataOUT       = &U1_CB.URxDataPtr[0];
    U1_CB.URxDataEND       = &U1_CB.URxDataPtr[NUM - 1];
    U1_CB.URxDataIN->start = USART1_RX_BUF;
}

void u1_printf(char* format, ...) {
    u16     i;
    va_list list_data;
    va_start(list_data, format);
    vsprintf((char*) USART1_TX_BUF, format, list_data);
    va_end(list_data);

    GPIO_WriteBit(USART1_RD_PORT, USART1_RD_PIN, USART1_RD_SEND); // 只在发送前切换一次

    for (i = 0; i < strlen((const char*) USART1_TX_BUF); i++) {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) != 1);
        USART_SendData(USART1, USART1_TX_BUF[i]);
    }
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != 1);
     delay_us(10);
    GPIO_WriteBit(USART1_RD_PORT, USART1_RD_PIN, USART1_RD_RECV); // 发送完后切回接收
}

void USART1_IRQHandler(void)
{
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET) { // 判断是否为串口空闲中断
        volatile uint32_t tmp;
        tmp = USART1->SR; // 清除空闲中断标志步骤1：读取SR
        tmp = USART1->DR; // 清除空闲中断标志步骤2：读取DR

        // 计算本次接收的数据长度
        uint16_t len = (USART1_REC_LEN + 1) - DMA_GetCurrDataCounter(DMA2_Stream2);
        U1_CB.URxCount += len; // 累计接收数量

        U1_CB.URxDataIN->end = &USART1_RX_BUF[U1_CB.URxCount - 1]; // 记录本次接收结束位置
        U1_CB.URxDataIN++; // IN指针后移
        if (U1_CB.URxDataIN == U1_CB.URxDataEND) { // 回卷
            U1_CB.URxDataIN = &U1_CB.URxDataPtr[0];
        }
        if ((USART1_RX_SIZE - U1_CB.URxCount) >= USART1_REC_LEN) { // 剩余空间足够
            U1_CB.URxDataIN->start = &USART1_RX_BUF[U1_CB.URxCount];
        } else { // 剩余空间不足，回卷
            U1_CB.URxDataIN->start = USART1_RX_BUF;
            U1_CB.URxCount = 0;
        }

        DMA_Cmd(DMA2_Stream2, DISABLE); // 关闭DMA流
        DMA_SetCurrDataCounter(DMA2_Stream2, USART1_REC_LEN + 1); // 重新设置接收量
        DMA2_Stream2->M0AR = (uint32_t)U1_CB.URxDataIN->start;    // 重新设置接收位置
        DMA_ClearFlag(DMA2_Stream2, DMA_FLAG_TCIF2 | DMA_FLAG_HTIF2 | DMA_FLAG_TEIF2 | DMA_FLAG_DMEIF2 | DMA_FLAG_FEIF2); // 清除所有相关DMA中断标志
        DMA_Cmd(DMA2_Stream2, ENABLE); // 重新开启DMA流
    }
}
