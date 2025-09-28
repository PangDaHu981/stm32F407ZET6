#include "delay.h"


void delay_Init(void){
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);
}

void delay_us(u16 us){
   SysTick->LOAD = (SystemCoreClock / 1000000) * us - 1; // SystemCoreClock为系统时钟频率
    SysTick->VAL = 0x00;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
    while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	
}

void delay_ms(u16 ms){
	while(ms--)
	{
		delay_us(1000);
	}
}

