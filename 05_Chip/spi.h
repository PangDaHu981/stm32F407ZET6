#ifndef __SPI_H__
#define __SPI_H__
#include "stm32f4xx.h"

/* SPI1初始化 */
void SPI1_Init(void);
/* 设置波特率 */
void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler);
/* SPI1 读写一个字节 */
uint8_t SPI1_ReadWriteByte(uint8_t TxData);
/*获取SPI1端口使用权*/
void SPI1_Take(void);
/*释放SPI1端口使用权*/
void SPI1_Give(void);

/* SPI 初始化 */
void SPI2_Init(void);
/* SPI2 设置波特率 */
void SPI2_SetSpeed(uint8_t SPI_BaudRatePrescaler);
/* SPI2 读写一个字节 */
uint8_t SPI2_ReadWriteByte(uint8_t TxData);
/*获取SPI2端口使用权*/
void SPI2_Take(void);
/*释放SPI2端口使用权*/
void SPI2_Give(void);

#endif
