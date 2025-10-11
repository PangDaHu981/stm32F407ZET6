#include "usart.h"
#include "delay.h"

/* 全局缓冲 */
uint8_t UART_RX_BUF[UART_RX_SIZE];
uint8_t UART_TX_BUF[UART_TX_SIZE];
UCB_ControlBlock U_CB;

/* === 初始化函数 === */
void UART_Combo_Init(uint32_t baudrate) {
    GPIO_InitTypeDef  GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef  NVIC_InitStruct;

    /* 开启时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4 | RCC_APB1Periph_USART2, ENABLE);
    //RCC_AHB1PeriphClockCmd(UART_RD_PORT_RCC, ENABLE);

    /* UART4_TX -> PC10 */
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* USART2_RX -> PA3 */
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // /* 485方向控制 */
    // GPIO_InitStruct.GPIO_Pin   = UART_RD_PIN;
    // GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;
    // GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    // GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    // GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
    // GPIO_Init(UART_RD_PORT, &GPIO_InitStruct);
    // GPIO_WriteBit(UART_RD_PORT, UART_RD_PIN, UART_RD_RECV);

    /* === 初始化 UART4 (TX端) === */
    USART_InitStruct.USART_BaudRate            = baudrate;
    USART_InitStruct.USART_WordLength          = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits            = USART_StopBits_1;
    USART_InitStruct.USART_Parity              = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode                = USART_Mode_Tx;
    USART_Init(UART4, &USART_InitStruct);
    USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE);
    USART_Cmd(UART4, ENABLE);

    /* === 初始化 USART2 (RX端) === */
    USART_InitStruct.USART_Mode = USART_Mode_Rx;
    USART_Init(USART2, &USART_InitStruct);
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);
    USART_Cmd(USART2, ENABLE);

    /* NVIC配置 */
    NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    /* 初始化DMA与控制块 */
    UART_Rx_Ptr_Init();
    DMA_UART_TX_Init();
    DMA_UART_RX_Init();
}

/* === DMA TX 初始化 (UART4) === */
void DMA_UART_TX_Init(void) {
    DMA_InitTypeDef DMA_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); // UART4用DMA1_Stream4_Channel4

    DMA_DeInit(DMA1_Stream4);
    while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE);

    DMA_InitStruct.DMA_Channel = DMA_Channel_4;
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&UART4->DR;
    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)UART_TX_BUF;
    DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStruct.DMA_BufferSize = 0; // 动态配置
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream4, &DMA_InitStruct);

    DMA_ITConfig(DMA1_Stream4, DMA_IT_TC, ENABLE);
    NVIC_EnableIRQ(DMA1_Stream4_IRQn);
}

/* === DMA RX 初始化 (USART2) === */
void DMA_UART_RX_Init(void) {
    DMA_InitTypeDef DMA_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); // USART2_RX: DMA1_Stream5_Channel4

    DMA_DeInit(DMA1_Stream5);
    while (DMA_GetCmdStatus(DMA1_Stream5) != DISABLE);

    DMA_InitStruct.DMA_Channel = DMA_Channel_4;
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)UART_RX_BUF;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStruct.DMA_BufferSize = UART_RX_SEG_LEN;
    DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStruct.DMA_Priority = DMA_Priority_High;
    DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA1_Stream5, &DMA_InitStruct);

    DMA_Cmd(DMA1_Stream5, ENABLE);
}

/* === 接收控制块初始化 === */
void UART_Rx_Ptr_Init(void) {
    U_CB.URxCount         = 0;
    U_CB.URxDataIN        = &U_CB.URxDataPtr[0];
    U_CB.URxDataOUT       = &U_CB.URxDataPtr[0];
    U_CB.URxDataEND       = &U_CB.URxDataPtr[UART_RX_SEG_NUM - 1];
    U_CB.URxDataIN->start = UART_RX_BUF;
}

/* === USART2 空闲中断接收 === */
void USART2_IRQHandler(void) {
    if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) {
        volatile uint32_t tmp;
        tmp = USART2->SR;
        tmp = USART2->DR;

        uint16_t len = UART_RX_SEG_LEN - DMA_GetCurrDataCounter(DMA1_Stream5);
        U_CB.URxCount += len;
        U_CB.URxDataIN->end = &UART_RX_BUF[U_CB.URxCount - 1];

        U_CB.URxDataIN++;
        if (U_CB.URxDataIN > U_CB.URxDataEND) {
            U_CB.URxDataIN = &U_CB.URxDataPtr[0];
        }

        if ((UART_RX_SIZE - U_CB.URxCount) >= UART_RX_SEG_LEN) {
            U_CB.URxDataIN->start = &UART_RX_BUF[U_CB.URxCount];
        } else {
            U_CB.URxDataIN->start = UART_RX_BUF;
            U_CB.URxCount = 0;
        }

        DMA_Cmd(DMA1_Stream5, DISABLE);
        DMA_SetCurrDataCounter(DMA1_Stream5, UART_RX_SEG_LEN);
        DMA1_Stream5->M0AR = (uint32_t)U_CB.URxDataIN->start;
        DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5 | DMA_FLAG_HTIF5);
        DMA_Cmd(DMA1_Stream5, ENABLE);
    }
}

/* === DMA1_Stream4 (UART4 TX完成中断) === */
void DMA1_Stream4_IRQHandler(void) {
    if (DMA_GetITStatus(DMA1_Stream4, DMA_IT_TCIF4) != RESET) {
        DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF4);
        while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
        //GPIO_WriteBit(UART_RD_PORT, UART_RD_PIN, UART_RD_RECV); // 发送完切回接收
    }
}

/* === printf重映射 === */
int fputc(int ch, FILE* f) {
    static uint16_t tx_len = 0;

    UART_TX_BUF[0] = (uint8_t)ch;
    //GPIO_WriteBit(UART_RD_PORT, UART_RD_PIN, UART_RD_SEND);
    while (DMA_GetCmdStatus(DMA1_Stream4) != DISABLE);
    DMA_Cmd(DMA1_Stream4, DISABLE);
    DMA1_Stream4->M0AR = (uint32_t)UART_TX_BUF;
    DMA_SetCurrDataCounter(DMA1_Stream4, 1);
    DMA_ClearFlag(DMA1_Stream4, DMA_FLAG_TCIF4);
    DMA_Cmd(DMA1_Stream4, ENABLE);
    return ch;
}
