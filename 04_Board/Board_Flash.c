#include "Board_Flash.h"
#include "stm32f4xx_flash.h"
const uint32_t sector_table[] = {
    FLASH_Sector_0, FLASH_Sector_1, FLASH_Sector_2, FLASH_Sector_3,
    FLASH_Sector_4, FLASH_Sector_5, FLASH_Sector_6, FLASH_Sector_7,
    FLASH_Sector_8, FLASH_Sector_9, FLASH_Sector_10, FLASH_Sector_11
};

void STM32_EraseFlash(u16 start_sector, u16 num)
{
    u16 i;
     if(start_sector + num > 12) return; // 防止越界
    FLASH_Unlock();
    for(i = 0; i < num; i++) {
        FLASH_EraseSector(sector_table[start_sector + i], VoltageRange_3);
        // FLASH_EraseSector参数：扇区号，电压范围
    }
    FLASH_Lock();
}

// void STM32_WriteFlash(u32 startaddr, u32 *wdata, u32 wnum)	//startaddr表示写入的起始地址，wdata表示待写入数据数组，num表示待写入的字节个数
// {
// 	FLASH_Unlock();
// 	while(wnum){
// 		FLASH_ProgramWord(startaddr, *wdata);
// 		wnum -= 4;
// 		startaddr += 4;
// 		wdata++;
// 	}
	
// 	FLASH_Lock();
// }
void STM32_WriteFlash(u32 startaddr, u32 *wdata, u32 wnum)
{
    FLASH_Unlock();
    while(wnum) {
        FLASH_ProgramWord(startaddr, *wdata);
        wnum -= 4;
        startaddr += 4;
        wdata++;
    }
    FLASH_Lock();
}


