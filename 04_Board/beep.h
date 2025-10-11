#ifndef __BEEP_H__
#define __BEEP_H__

#include "stm32f4xx.h"

#define BEEP_RCCCLOCK RCC_AHB1Periph_GPIOG
#define BEEP_PORT     GPIOG
#define BEEP_PIN      GPIO_Pin_8

void Beep_Init(void);  //≥ı ºªØ

void Beep_On(void);
void Beep_Off(void);
void Beep_Toggle(void);

#endif
