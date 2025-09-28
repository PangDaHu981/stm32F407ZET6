#include "W25Q64.h"
#include "boot.h"
struct SPI_Flash_Struct {
    uint32_t Capacity;  // Flash容量

} SPI_Flash;

// SD卡初始化的时候,需要低速
void W25QXX_SPI_SpeedLow(void) {
    SPI1_SetSpeed(SPI_BaudRatePrescaler_256);   // 设置到低速模式
}
// SD卡正常工作的时候,可以高速了
void W25QXX_SPI_SpeedHigh(void) {
    SPI1_SetSpeed(SPI_BaudRatePrescaler_2);   // 设置到高速模式
}
void W25QXX_SPI_Init(void) {

    GPIO_InitTypeDef GPIO_InitStructure;   // 设置硬件上与SD卡相关联的控制引脚输出
    RCC_AHB1PeriphClockCmd(W25QXX_CS_RCCCLOCK, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = W25QXX_CS_PIN;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;       // 普通输出模式
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;   // 100MHz
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;        // 上拉
    GPIO_Init(W25QXX_CS_PORT, &GPIO_InitStructure);
    GPIO_SetBits(W25QXX_CS_PORT, W25QXX_CS_PIN);
    SPI1_Init();
    SPI_Flash.Capacity = W25QXX_ReadCapacity();
}
// 取消选择,释放SPI总线
void W25QXX_DisSelect(void) {
    /* 如果已经片选 */
    if (GPIO_ReadOutputDataBit(W25QXX_CS_PORT, W25QXX_CS_PIN) == RESET) {
        GPIO_SetBits(W25QXX_CS_PORT, W25QXX_CS_PIN);
        SPI1_ReadWriteByte(0xff);   // 提供额外的8个时钟
        SPI2_Give();                  /* 释放串口使用权 */
    }
}
/*
 *读SR寄存器
 */
uint8_t W25QXX_ReadSR(void) {
    uint8_t sr = 0;
    /*片选器件*/
    W25QXX_CS_0;
    /*发送命令:读取SR状态寄存器*/
    SPI1_ReadWriteByte(W25QX_ReadStatusReg);
    /*读一个字节*/
    sr = SPI1_ReadWriteByte(0Xff);
    /*取消片选*/
    W25QXX_CS_1;
    /*返回寄存器的值*/
    return sr;
}
/*
 *W25Qxx 写使能
 */
void W25QXX_Write_Enable(void) {
    /*片选器件*/
    W25QXX_CS_0;
    /*发送写使能*/
    SPI1_ReadWriteByte(W25QX_WriteEnable);
    /*取消片选*/
    W25QXX_CS_1;
}

/*
 *W25Qxx 写禁止
 */
void W25QXX_Write_Disable(void) {
    /*片选器件*/
    W25QXX_CS_0;
    /*发送写禁止*/
    SPI1_ReadWriteByte(W25QX_WriteDisable);
    /*取消片选*/
    W25QXX_CS_1;
}

/*
 *W25Qxx 读取芯片ID
 */
uint16_t W25QXX_ReadID(void) {
    uint16_t id = 0;
    /*片选器件*/
    W25QXX_CS_0;
    /*发送读取ID命令*/
    SPI1_ReadWriteByte(W25QX_ManufactDeviceID);
    /*等待三个时钟周期*/
    SPI1_ReadWriteByte(0x00);
    SPI1_ReadWriteByte(0x00);
    SPI1_ReadWriteByte(0x00);
    /*读取两个字节*/
    id |= SPI1_ReadWriteByte(0xFF) << 8;
    id |= SPI1_ReadWriteByte(0xFF);
    /*取消片选*/
    W25QXX_CS_1;
    /*返回芯片ID*/
    return id;
}
/*
 *读取芯片容量
 */
uint32_t W25QXX_ReadCapacity(void) {
    uint16_t id;
    uint32_t capacity = 1;
    /*读取芯片制造ID*/
    id = W25QXX_ReadID();
    /*获取容量信息(BCD码)*/
    id &= 0x00FF;
    /*转换次幂*/
    id = ((id >> 4) * 10) + (id & 0x0F);
    /*计算容量*/
    for (; id; id--) {
        capacity *= 2;
    }
    capacity *= 1024 / 8;
    /*返回容量*/
    return capacity;
}
/*直接读取数据*/
    void W25QXX_Read(void* r_bf, uint32_t r_addr, uint16_t count) {
    uint8_t* r_poi = r_bf;
    /*等待写入结束*/
    W25QXX_WaitBusy();
    /*片选器件*/
    W25QXX_CS_0;
    /*发送读取数据命令*/
    SPI1_ReadWriteByte(W25QX_ReadData);
    /*发送数据地址*/
    SPI1_ReadWriteByte((uint8_t) (r_addr >> 16));
    SPI1_ReadWriteByte((uint8_t) (r_addr >> 8));
    SPI1_ReadWriteByte((uint8_t) (r_addr));
    /*循环读取数据*/
    while (count--) {
        *r_poi++ = SPI1_ReadWriteByte(0XFF);
    }
    /*取消片选*/
    W25QXX_CS_1;
}

/*
 *写页 最大256字节
 */
void W25QXX_WritePage(void* w_bf, uint32_t w_addr, uint16_t count) {
    uint8_t* w_poi = w_bf;
    /*等待写入结束*/
    W25QXX_WaitBusy();
    /*写使能*/
    W25QXX_Write_Enable();
    /*片选器件*/
    W25QXX_CS_0;
    /*发送页编程命令*/
    SPI1_ReadWriteByte(W25QX_PageProgram);
    /*发送数据地址*/
    SPI1_ReadWriteByte((uint8_t) (w_addr >> 16));
    SPI1_ReadWriteByte((uint8_t) (w_addr >> 8));
    SPI1_ReadWriteByte((uint8_t) (w_addr));
    /*循环写入*/
    while (count--) {
        SPI1_ReadWriteByte(*w_poi++);
    }
    /*取消片选*/
    W25QXX_CS_1;
    /*等待写入结束*/
    W25QXX_WaitBusy();
}

/*
 *直接写入数据 自动换页 无校验
 */
void W25QXX_Write(void* w_bf, uint32_t w_addr, uint16_t count) {
    uint8_t* w_poi = w_bf;
    uint16_t page_byte;
    /*写入*/
    while (count) {
        /*计算当前页剩余空间*/
        page_byte = 256 - w_addr % 256;
        /*如果剩余写入字节大于当前页剩余空间*/
        if (count > page_byte) {
            /*写完剩余页*/
            W25QXX_WritePage(w_poi, w_addr, page_byte);
            /*计算参量*/
            w_poi += page_byte;
            w_addr += page_byte;
            count -= page_byte;
        }
        /*如果剩余写入字节小于当前页剩余空间*/
        else if (count <= page_byte) {
            /*写完剩余数据*/
            W25QXX_WritePage(w_poi, w_addr, count);
            /*计算参量*/
            w_poi += count;
            w_addr += count;
            count -= count;
        }
    }
    
}

/*
 *擦除扇区
 */
void W25QXX_EraseSector(uint32_t address) {
    /*整数对齐扇区地址*/
    address /= 4096;
    address *= 4096;
    /*等待忙位*/
    W25QXX_WaitBusy();
    /*写使能*/
    W25QXX_Write_Enable();
    /*片选器件*/
    W25QXX_CS_0;
    /*发送擦除扇区指令*/
    SPI1_ReadWriteByte(W25QX_SectorErase);
    /*发送擦除地址*/
    SPI1_ReadWriteByte((uint8_t) (address >> 16));
    SPI1_ReadWriteByte((uint8_t) (address >> 8));
    SPI1_ReadWriteByte((uint8_t) (address));
    /*取消片选*/
    W25QXX_CS_1;
    /*等待写入结束*/
    W25QXX_WaitBusy();
}
// void W25Q64_Erase64K(uint8_t blockNB)
// {
//     uint8_t wdata[4];
//     uint32_t addr = blockNB * 64 * 1024; // 计算块首地址

//     wdata[0] = 0xD8;                    // 64K块擦除指令
//     wdata[1] = (addr >> 16) & 0xFF;     // 地址高8位
//     wdata[2] = (addr >> 8) & 0xFF;      // 地址中8位
//     wdata[3] = addr & 0xFF;             // 地址低8位

//     W25QXX_WaitBusy();                  // 等待空闲
//     W25QXX_Write_Enable();               // 使能写入
//     W25QXX_CS_1;
//     for(uint8_t i = 0; i < 4; i++)
//     {
//         SPI1_ReadWriteByte(wdata[i]);
//     }
//     //SPI1_Write(wdata, 4);               // 发送擦除命令和地址
//     W25QXX_CS_0;
//     W25QXX_WaitBusy();                  // 等待擦除完成
// }
// void W25Q64_Erase64K(uint8_t slot_index)
// {
//     if (slot_index == 0 || slot_index > SLOT_NUM) return;

//     uint32_t addr = GetSlotBaseAddr(slot_index);
//     uint8_t wdata[4];

//     wdata[0] = 0xD8;                    // 64K block erase
//     wdata[1] = (addr >> 16) & 0xFF;
//     wdata[2] = (addr >> 8) & 0xFF;
//     wdata[3] = addr & 0xFF;

//     W25QXX_WaitBusy();
//     W25QXX_Write_Enable();
//     W25QXX_CS_1;
//     for (uint8_t i = 0; i < 4; i++)
//     {
//         SPI1_ReadWriteByte(wdata[i]);
//     }
//     W25QXX_CS_0;
//     W25QXX_WaitBusy();
// }
// void W25Q64_EraseSlot(uint8_t slot_index) {
//     uint32_t base = GetSlotBaseAddr(slot_index);
//     if (base == 0xFFFFFFFF) return;

//     // 一个 slot 大小 512KB，每次擦 64KB，总共 8 个块
//     for (uint32_t i = 0; i < (SLOT_SIZE / (64 * 1024)); i++) {
//         uint32_t addr = base + i * 64 * 1024;
//         W25Q64_Erase64K(addr / (64 * 1024));
//     }
// }


// 擦除一个 64KB block
void W25Q64_Erase64K(uint32_t addr) 
{
    W25QXX_WaitBusy();
    W25QXX_Write_Enable();

    W25QXX_CS_0;                          // ? CS 拉低
    SPI1_ReadWriteByte(0xD8);             // 64K block erase
    SPI1_ReadWriteByte((addr >> 16) & 0xFF);
    SPI1_ReadWriteByte((addr >> 8) & 0xFF);
    SPI1_ReadWriteByte(addr & 0xFF);
    W25QXX_CS_1;                          // ? CS 拉高

    W25QXX_WaitBusy();
}

// 擦除一个 slot（512KB = 8 个 64KB block）
void W25Q64_EraseSlot(uint8_t slot_index) 
{
    uint32_t base = GetSlotBaseAddr(slot_index);
    if (base == 0xFFFFFFFF) return;

    for (uint32_t i = 0; i < (SLOT_SIZE / (64 * 1024)); i++) {
        uint32_t addr = base + i * (64 * 1024);
        W25Q64_Erase64K(addr);   // 传入地址，而不是 index
    }
}
/*
 *忙位等待
 */
void W25QXX_WaitBusy(void) {
    while ((W25QXX_ReadSR() & 0x01) == 0x01)
        ;
}

/*
 *进入掉电模式
 */
void W25QXX_PowerDown(void) {
    /*片选器件*/
    W25QXX_CS_0;
    /*发送掉电命令*/
    SPI1_ReadWriteByte(W25QX_PowerDown);
    /*取消片选*/
    W25QXX_CS_1;
}

/*
 *唤醒
 */
void W25QXX_WAKEUP(void) {
    /*片选器件*/
    W25QXX_CS_0;
    /*发送取消掉电*/
    SPI1_ReadWriteByte(W25QX_ReleasePowerDown);
    /*取消片选*/
    W25QXX_CS_1;
}

/*	最大一次写入256字节	*/
void W25Q64_PageWrite(uint8_t *wbuff, uint16_t pageNB){
        uint8_t* w_poi = wbuff;
	uint8_t wdata[4];
	
	wdata[0] = 0x02;
	wdata[1] = (pageNB*256)>>16;
	wdata[2] = (pageNB*256)>>8;
	wdata[3] = (pageNB*256)>>0;
	
	W25QXX_WaitBusy();
	W25QXX_Write_Enable();
	W25QXX_CS_1;
    for(uint8_t i=0;i<4;i++)
    {
        SPI1_ReadWriteByte(wdata[i]);
    }
    for(uint16_t i=0;i<256;i++)
    {
        SPI1_ReadWriteByte(*w_poi++);
    }
	// SPI1_Write(wdata,4);
	// SPI1_Write(wbuff,256);
	W25QXX_CS_0;
}
// #define TEST_BLOCK_SIZE   1024   // 每次写/读 1KB
// #define TEST_BLOCK_COUNT  512    // 512 * 1KB = 512KB

// void W25Q64_TestSlot(uint8_t slot_index)
// {
//     uint32_t base = GetSlotBaseAddr(slot_index);
//     if (base == 0xFFFFFFFF) {
//         u1_printf("Invalid slot index: %d\r\n", slot_index);
//         return;
//     }

//     uint8_t writeBuf[TEST_BLOCK_SIZE];
//     uint8_t readBuf[TEST_BLOCK_SIZE];

//     // 初始化写数据
//     for (uint32_t i = 0; i < TEST_BLOCK_SIZE; i++) {
//         writeBuf[i] = i & 0xFF;  // 0~255 循环填充
//     }

//     u1_printf("Erase slot %d ...\r\n", slot_index);
//     W25Q64_EraseSlot(slot_index); // 擦除整个 512KB

//     // 逐块写、读、校验
//     for (uint32_t blk = 0; blk < TEST_BLOCK_COUNT; blk++) {
//         uint32_t addr = base + blk * TEST_BLOCK_SIZE;

//         // 写
//         W25QXX_Write(writeBuf, addr, TEST_BLOCK_SIZE);

//         // 读
//         W25QXX_Read(readBuf, addr, TEST_BLOCK_SIZE);

//         // 比较
//         for (uint32_t i = 0; i < TEST_BLOCK_SIZE; i++) {
//             if (readBuf[i] != writeBuf[i]) {
//                 u1_printf("Slot %d Test FAIL at 0x%08X: W=0x%02X, R=0x%02X\r\n",
//                           slot_index, addr + i, writeBuf[i], readBuf[i]);
//                 return;  // 出错直接返回
//             }
//         }
//     }

//     u1_printf("Slot %d Test PASS\r\n", slot_index);
// }
#define PAGE_SIZE         256              // W25Q64 页大小
//#define SLOT_SIZE         (512 * 1024)     // 512KB
#define SLOT_PAGE_COUNT   (SLOT_SIZE / PAGE_SIZE)  // 每个Slot共2048页

// 测试指定 Slot 的读写是否正常 (强制按页写入)
void W25Q64_TestSlot(uint8_t slot_index)
{
    uint32_t base = GetSlotBaseAddr(slot_index);
    if (base == 0xFFFFFFFF) {
        u1_printf("Invalid slot index: %d\r\n", slot_index);
        return;
    }

    uint8_t writeBuf[PAGE_SIZE];
    uint8_t readBuf[PAGE_SIZE];

    // 填充测试数据
    for (uint32_t i = 0; i < PAGE_SIZE; i++) {
        writeBuf[i] = i;   // 0x00, 0x01, 0x02 ...
    }

    u1_printf("Erase slot %d ...\r\n", slot_index);
    W25Q64_EraseSlot(slot_index); // 擦除整个 512KB

    u1_printf("Write + Read test on slot %d ...\r\n", slot_index);

    for (uint32_t page = 0; page < SLOT_PAGE_COUNT; page++) {
        uint32_t addr = base + page * PAGE_SIZE;

        // 写入一页
        W25QXX_Write(writeBuf, addr, PAGE_SIZE);

        // 读回一页
        W25QXX_Read(readBuf, addr, PAGE_SIZE);

        // 校验一页
        for (uint32_t i = 0; i < PAGE_SIZE; i++) {
            if (readBuf[i] != writeBuf[i]) {
                u1_printf("Slot %d Test FAIL at 0x%08X: W=0x%02X, R=0x%02X\r\n",
                          slot_index, addr + i, writeBuf[i], readBuf[i]);
                return; // 发现错误直接返回
            }
        }
    }

    u1_printf("Slot %d Test PASS!\r\n", slot_index);
}

void W25Q64_TestAddr(uint32_t addr)
{
    uint8_t writeBuf[PAGE_SIZE];
    uint8_t readBuf[PAGE_SIZE];

    for (uint32_t i = 0; i < PAGE_SIZE; i++) {
        writeBuf[i] = i;
    }

    u1_printf("Erase 64K block at 0x%08X ...\r\n", addr & ~(64*1024-1));
    W25Q64_Erase64K(addr & ~(64*1024-1));

    u1_printf("Write + Read test at 0x%08X ...\r\n", addr);

    W25QXX_Write(writeBuf, addr, PAGE_SIZE);
    W25QXX_Read(readBuf, addr, PAGE_SIZE);

    for (uint32_t i = 0; i < PAGE_SIZE; i++) {
        if (readBuf[i] != writeBuf[i]) {
            u1_printf("Test FAIL at 0x%08X: W=0x%02X, R=0x%02X\r\n",
                      addr + i, writeBuf[i], readBuf[i]);
            return;
        }
    }

    u1_printf("Test PASS at 0x%08X\r\n", addr);
}