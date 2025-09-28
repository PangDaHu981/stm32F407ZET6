#ifndef __DGUS_H__
#define __DGUS_H__

#include "stm32f4xx.h"

#ifndef NULL
#define NULL 0
#endif
/*
 *ָ��ṹ
 * ָ��֡ͷ ָ��� ָ�� ��ʼ��ַ [���ݳ���] [��������] [CRCУ����]
 *
 *   ÿ��ָ�������ָ��֡ͷ��ָ��ȡ�ָ���ʼ��ַ�ĸ����֡�
 *   [���ݳ���][��������][CRCУ���� ]��������������ÿ��ָ��С�
 *
 */

/* ��Ļ��ʼ�� */
void DGUS_Init(void);
/* �������� */
void DGUS_Transmit(uint8_t* data, uint16_t length);
/* �趨��ǰҳ */
void DGUS_SetPage(uint16_t page);
/* ����Ļ��ӡ�� */
void DGUS_Print(uint8_t new, const char* message, ...);
/* ���� */
void DGUS_Clean(void);
#endif
