#ifndef __BEEP_H__
#define __BEEP_H__

#include "stm32f4xx.h"

#define BEEP_RCCCLOCK RCC_AHB1Periph_GPIOF
#define BEEP_PORT     GPIOF
#define BEEP_PIN      GPIO_Pin_6

void Beep_Init(void);  //≥ı ºªØ

void Beep_On(void);
void Beep_Off(void);
void Beep_Toggle(void);

#endif
