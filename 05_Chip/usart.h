#ifndef __USART_H
#define __USART_H

#include "stdarg.h"
#include "stdio.h"
#include "stm32f4xx.h"
#include "string.h"

/* 缓冲定义 */
#define UART_TX_SIZE     2048
#define UART_RX_SIZE     2048
#define UART_RX_SEG_LEN  256
#define UART_RX_SEG_NUM  10

/* 485方向控制端口 */
#define UART_RD_PORT_RCC RCC_AHB1Periph_GPIOC
#define UART_RD_PORT     GPIOC
#define UART_RD_PIN      GPIO_Pin_1
#define UART_RD_RECV     Bit_SET
#define UART_RD_SEND     Bit_RESET

/* 接收结构体 */
typedef struct {
    uint8_t* start;
    uint8_t* end;
} UCB_URxBufptr;

typedef struct {
    uint16_t        URxCount;
    UCB_URxBufptr   URxDataPtr[UART_RX_SEG_NUM];
    UCB_URxBufptr*  URxDataIN;
    UCB_URxBufptr*  URxDataOUT;
    UCB_URxBufptr*  URxDataEND;
} UCB_ControlBlock;

/* 全局变量 */
extern uint8_t UART_RX_BUF[UART_RX_SIZE];
extern uint8_t UART_TX_BUF[UART_TX_SIZE];
extern UCB_ControlBlock U_CB;

/* 初始化与发送 */
void UART_Combo_Init(uint32_t baudrate);
void DMA_UART_TX_Init(void);
void DMA_UART_RX_Init(void);
void UART_Rx_Ptr_Init(void);

/* printf重映射支持 */
int fputc(int ch, FILE* f);

#endif
