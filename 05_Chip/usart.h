#ifndef __USART_H
#define __USART_H
#include "stdarg.h"
#include "stdio.h"
#include "stm32f4xx.h"
#include "string.h"

// USART2接收配置
#define USART2_RX_SIZE 2048
#define USART2_REC_LEN 256
#define NUM            10

// UART4发送配置  
#define UART4_TX_SIZE 2048

// 收发使能引脚配置
#define USART_RD_PORT_RCC RCC_AHB1Periph_GPIOC
#define USART_RD_PORT     GPIOC
#define USART_RD_PIN      GPIO_Pin_1
#define USART_RD_RECV     Bit_SET
#define USART_RD_SEND     Bit_RESET

typedef struct {
    uint8_t* start;
    uint8_t* end;
} UCB_URxBufptr;

typedef struct {
    u16            URxCount;
    UCB_URxBufptr  URxDataPtr[NUM];
    UCB_URxBufptr* URxDataIN;
    UCB_URxBufptr* URxDataOUT;
    UCB_URxBufptr* URxDataEND;
} UCB_ControlBlock;

// USART2函数声明
void UART_Combo_Init(u32 baudrate);
void DMA_USART2_RX_Init(void);
void U2Rx_Ptr_Init(void);
void USART2_IRQHandler(void);

// UART4函数声明
void UART4_Init(u32 baudrate);
void u4_printf(char* format, ...);

extern UCB_ControlBlock U2_CB;
extern uint8_t USART2_RX_BUF[USART2_RX_SIZE];
extern uint8_t UART4_TX_BUF[UART4_TX_SIZE];

#endif