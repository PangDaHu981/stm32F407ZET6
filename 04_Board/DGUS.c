#include "DGUS.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"

/* 屏幕初始化 */
void DGUS_Init(void) {
    /* 使能时钟 */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);   //使能GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);  //使能USART3时钟
    {                                                       /* 串口引脚初始化 */
        GPIO_InitTypeDef GPIO_InitStructure;

        GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11;  // USART TX/RX
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;               //复用输出模式
        GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;              //推挽输出
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;          //输出速率100MHz
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;               //上拉
        GPIO_Init(GPIOB, &GPIO_InitStructure);                      //初始化
        /*指定复用通道*/
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_USART3);  //复用为USART
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_USART3);  //复用为USART
    }

    { /* 串口初始化 */
        USART_InitTypeDef USART_InitStructure;
        USART_InitStructure.USART_BaudRate            = 460800;                          //设置波特率
        USART_InitStructure.USART_WordLength          = USART_WordLength_8b;             //位数据格式
        USART_InitStructure.USART_StopBits            = USART_StopBits_1;                //停止位
        USART_InitStructure.USART_Parity              = USART_Parity_No;                 //奇偶校验位
        USART_InitStructure.USART_Mode                = USART_Mode_Tx;                   //工作模式设置为收发模式
        USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  //无硬件数据流控制
        USART_Init(USART3, &USART_InitStructure);                                        //根据指定的参数初始化串口
    }
    /*使能串口*/
    USART_Cmd(USART3, ENABLE);  //使能串口
}
/* 传输数据 */
void DGUS_Transmit(uint8_t* data, uint16_t length) {
    while (length--) {
        /* 等待发送完毕 */
        while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET) {
        }
        /* 发送一个字节数据到串口 */
        USART_SendData(USART3, (uint8_t) *data++);
    }
}
/* 数据发送缓存 */
uint8_t DGUS_SendBuffer[256];
/* 设定当前页 */
void DGUS_SetPage(uint16_t page) {
    /* 获取发送缓存 */
    uint8_t* msg = DGUS_SendBuffer;
    /*报文指针*/
    uint8_t* msg_len;
    /* 帧头 */
    msg[0] = 0x5A;
    msg[1] = 0xA5;
    /* 数据长度 */
    msg_len  = &msg[2];
    *msg_len = 0;
    /* 指令*/
    msg[3 + (*msg_len)++] = 0x82;
    msg[3 + (*msg_len)++] = 0x00;
    msg[3 + (*msg_len)++] = 0x84;
    msg[3 + (*msg_len)++] = 0x5A;
    msg[3 + (*msg_len)++] = 0x01;
    msg[3 + (*msg_len)++] = page >> 8;
    msg[3 + (*msg_len)++] = page;
    /*发送报文*/
    DGUS_Transmit(msg, (*msg_len) + 3);
}

/* 向屏幕打印字 */
void DGUS_Print(uint8_t new, const char* message, ...) {
    /* 当前打印行 */
    static uint8_t line = 0;
    /* 获取发送缓存 */
    uint8_t* msg = DGUS_SendBuffer;
    /*报文指针*/
    uint8_t* msg_len;
    va_list  ap;
    /* 需要新行 */
    if (new != 0) {
        /* 超过最大行数 */
        if (++line >= 24) {
            /* 行数复位 */
            line = 1;
            DGUS_Clean();
        }
    }
    /* 帧头 */
    msg[0] = 0x5A;
    msg[1] = 0xA5;
    /* 数据长度 */
    msg_len  = &msg[2];
    *msg_len = 0;
    /* 指令*/
    msg[3 + (*msg_len)++] = 0x82;
    msg[3 + (*msg_len)++] = (uint8_t) ((0x4300 + (line * 0x0040)) >> 8);
    msg[3 + (*msg_len)++] = (uint8_t) ((0x4300 + (line * 0x0040)) & 0xFF);
    /* 将文字打印到消息中 */
    va_start(ap, message);
    vsprintf((char*) (&(msg[3 + (*msg_len)])), message, ap);
    va_end(ap);
    /* 计算长度 */
    *msg_len += strlen((char*) (&(msg[3 + (*msg_len)])));
    /* 写入结束符 */
    msg[3 + (*msg_len)++] = 0xFF;
    msg[3 + (*msg_len)++] = 0xFF;
    /*发送报文*/
    DGUS_Transmit(msg, (*msg_len) + 3);
}

/* 清屏 */
void DGUS_Clean(void) {
    /* 获取发送缓存 */
    uint8_t* msg = DGUS_SendBuffer;
    /*报文指针*/
    uint8_t* msg_len;
    /* 清空页面 */
    for (uint8_t i = 1; i < 24; i++) {
        /* 帧头 */
        msg[0] = 0x5A;
        msg[1] = 0xA5;
        /* 数据长度 */
        msg_len  = &msg[2];
        *msg_len = 0;
        /* 指令*/
        msg[3 + (*msg_len)++] = 0x82;
        msg[3 + (*msg_len)++] = (uint8_t) ((0x4300 + (i * 0x0040)) >> 8);
        msg[3 + (*msg_len)++] = (uint8_t) ((0x4300 + (i * 0x0040)) & 0xFF);
        /* 写入结束符 */
        msg[3 + (*msg_len)++] = 0xFF;
        msg[3 + (*msg_len)++] = 0xFF;
        /*发送报文*/
        DGUS_Transmit(msg, (*msg_len) + 3);
    }
}
