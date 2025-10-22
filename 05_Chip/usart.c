#include "usart.h"
#include "delay.h"

// USART2接收缓冲区
uint8_t USART2_RX_BUF[USART2_RX_SIZE];
UCB_ControlBlock U2_CB;

// UART4发送缓冲区
uint8_t UART4_TX_BUF[UART4_TX_SIZE];

// ==================== USART2配置(接收) ====================
void UART_Combo_Init(u32 baudrate) {
    GPIO_InitTypeDef  GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef  NVIC_InitStruct;

    // 1. 使能时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_AHB1PeriphClockCmd(USART_RD_PORT_RCC, ENABLE);

    // 2. GPIO配置 - PA3作为USART2_RX
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); // USART2_RX (PA3)

    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_3;  // 只配置RX引脚
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 收发使能引脚配置
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin   = USART_RD_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(USART_RD_PORT, &GPIO_InitStructure);
    GPIO_WriteBit(USART_RD_PORT, USART_RD_PIN, USART_RD_RECV);

    // 3. USART2配置(只接收)
    USART_InitStruct.USART_BaudRate            = baudrate;
    USART_InitStruct.USART_WordLength          = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits            = USART_StopBits_1;
    USART_InitStruct.USART_Parity              = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode                = USART_Mode_Rx;  // 只接收
    USART_Init(USART2, &USART_InitStruct);

    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
    USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);

    // 4. NVIC配置
    NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    
    GPIO_WriteBit(USART_RD_PORT, USART_RD_PIN, USART_RD_RECV);
    U2Rx_Ptr_Init();
    DMA_USART2_RX_Init();
    USART_Cmd(USART2, ENABLE);
    UART4_Init(baudrate);
}

void DMA_USART2_RX_Init(void) {
    DMA_InitTypeDef DMA_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Stream5);
    while (DMA_GetCmdStatus(DMA1_Stream5) != DISABLE);

    DMA_InitStruct.DMA_Channel = DMA_Channel_4;
    DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
    DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)USART2_RX_BUF;
    DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
    DMA_InitStruct.DMA_BufferSize = USART2_REC_LEN + 1;
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
    DMA_Init(DMA1_Stream5, &DMA_InitStruct);

    DMA_Cmd(DMA1_Stream5, ENABLE);
}

void U2Rx_Ptr_Init(void) {
    U2_CB.URxCount         = 0;
    U2_CB.URxDataIN        = &U2_CB.URxDataPtr[0];
    U2_CB.URxDataOUT       = &U2_CB.URxDataPtr[0];
    U2_CB.URxDataEND       = &U2_CB.URxDataPtr[NUM - 1];
    U2_CB.URxDataIN->start = USART2_RX_BUF;
}

void USART2_IRQHandler(void) {
    if (USART_GetITStatus(USART2, USART_IT_IDLE) != RESET) {
        volatile uint32_t tmp;
        tmp = USART2->SR;
        tmp = USART2->DR;

        uint16_t len = (USART2_REC_LEN + 1) - DMA_GetCurrDataCounter(DMA1_Stream5);
        U2_CB.URxCount += len;

        U2_CB.URxDataIN->end = &USART2_RX_BUF[U2_CB.URxCount - 1];
        U2_CB.URxDataIN++;
        if (U2_CB.URxDataIN == U2_CB.URxDataEND) {
            U2_CB.URxDataIN = &U2_CB.URxDataPtr[0];
        }
        if ((USART2_RX_SIZE - U2_CB.URxCount) >= USART2_REC_LEN) {
            U2_CB.URxDataIN->start = &USART2_RX_BUF[U2_CB.URxCount];
        } else {
            U2_CB.URxDataIN->start = USART2_RX_BUF;
            U2_CB.URxCount = 0;
        }

        DMA_Cmd(DMA1_Stream5, DISABLE);
        DMA_SetCurrDataCounter(DMA1_Stream5, USART2_REC_LEN + 1);
        DMA1_Stream5->M0AR = (uint32_t)U2_CB.URxDataIN->start;
        DMA_ClearFlag(DMA1_Stream5, DMA_FLAG_TCIF5 | DMA_FLAG_HTIF5 | DMA_FLAG_TEIF5 | DMA_FLAG_DMEIF5 | DMA_FLAG_FEIF5);
        DMA_Cmd(DMA1_Stream5, ENABLE);
    }
}

// ==================== UART4配置(发送) ====================
void UART4_Init(u32 baudrate) {
    GPIO_InitTypeDef  GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;

    // 1. 使能时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

    // 2. GPIO配置 - PA0作为UART4_TX
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource0, GPIO_AF_UART4); // UART4_TX (PA0)

    GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_0;  // 只配置TX引脚
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    // 3. UART4配置(只发送)
    USART_InitStruct.USART_BaudRate            = baudrate;
    USART_InitStruct.USART_WordLength          = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits            = USART_StopBits_1;
    USART_InitStruct.USART_Parity              = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode                = USART_Mode_Tx;  // 只发送
    USART_Init(UART4, &USART_InitStruct);

    USART_Cmd(UART4, ENABLE);
}

void u4_printf(char* format, ...) {
    u16     i;
    va_list list_data;
    va_start(list_data, format);
    vsprintf((char*) UART4_TX_BUF, format, list_data);
    va_end(list_data);

    // 切换到发送模式
    GPIO_WriteBit(USART_RD_PORT, USART_RD_PIN, USART_RD_SEND);

    for (i = 0; i < strlen((const char*) UART4_TX_BUF); i++) {
        while (USART_GetFlagStatus(UART4, USART_FLAG_TXE) != 1);
        USART_SendData(UART4, UART4_TX_BUF[i]);
    }
    while (USART_GetFlagStatus(UART4, USART_FLAG_TC) != 1);
    delay_us(10);
    
    // 切换回接收模式
    GPIO_WriteBit(USART_RD_PORT, USART_RD_PIN, USART_RD_RECV);
}