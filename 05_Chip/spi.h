#ifndef __SPI_H__
#define __SPI_H__
#include "stm32f4xx.h"

/* SPI1��ʼ�� */
void SPI1_Init(void);
/* ���ò����� */
void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler);
/* SPI1 ��дһ���ֽ� */
uint8_t SPI1_ReadWriteByte(uint8_t TxData);
/*��ȡSPI1�˿�ʹ��Ȩ*/
void SPI1_Take(void);
/*�ͷ�SPI1�˿�ʹ��Ȩ*/
void SPI1_Give(void);

/* SPI ��ʼ�� */
void SPI2_Init(void);
/* SPI2 ���ò����� */
void SPI2_SetSpeed(uint8_t SPI_BaudRatePrescaler);
/* SPI2 ��дһ���ֽ� */
uint8_t SPI2_ReadWriteByte(uint8_t TxData);
/*��ȡSPI2�˿�ʹ��Ȩ*/
void SPI2_Take(void);
/*�ͷ�SPI2�˿�ʹ��Ȩ*/
void SPI2_Give(void);

#endif
