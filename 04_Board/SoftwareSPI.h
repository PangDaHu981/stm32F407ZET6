#ifndef __SOFTWARESPI_H__
#define __SOFTWARESPI_H__

#include "stm32f4xx.h"

#define W25QXX_SPI 0
#define WK2124_SPI 1

#define SPI_MODE_0 0x00   // SPI模式0 空闲电平为低电平，第一跳变沿（上升沿）数据采样
#define SPI_MODE_1 0x01   // SPI模式1 空闲电平为低电平，第二跳变沿（下降沿）数据采样
#define SPI_MODE_2 0x02   // SPI模式2 空闲电平为高电平，第一跳变沿（下降沿）数据采样
#define SPI_MODE_3 0x03   // SPI模式3 空闲电平为高电平，第二跳变沿（上升沿）数据采样

/* SPI初始化 */
void SoftwareSPI_Init(uint8_t port, uint8_t mode, uint8_t firstBit);
/* SPI 读写一个字节 */
uint8_t SoftwareSPI_ReadWriteByte(uint8_t port, uint8_t TxData);
/*获取SPI1端口使用权*/
void SoftwareSPI_Take(uint8_t port);
/*释放SPI1端口使用权*/
void SoftwareSPI_Give(uint8_t port);

#endif
