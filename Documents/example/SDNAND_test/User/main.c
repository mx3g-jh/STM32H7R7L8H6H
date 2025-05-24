/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2024-05-21
 * @brief       SD NAND实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 * 
 * 实验平台:正点原子 H7R7开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 * 
 ****************************************************************************************************
 */

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/delay/delay.h"
#include "./SYSTEM/usart/usart.h"
#include "./MALLOC/malloc.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/HYPERRAM/hyperram.h"
#include "./BSP/SDNAND/spi_sdnand.h"

/**
 * @brief   显示SD NAND信息
 * @param   无
 * @retval  无
 */
static void show_sdnand_info(void)
{
    printf("SD NAND Chip Size: %dMB\r\n", sdnand_info.chip_size >> 20);
    printf("SD NAND Block Size: %dB\r\n", sdnand_info.block_size);
    printf("SD NAND Block Number: %d\r\n", sdnand_info.block_num);
    printf("SD NAND Logic Block Size: %dB\r\n", sdnand_info.logic_block_size);
    printf("SD NAND Logic Block Number: %d\r\n", sdnand_info.logic_block_num);
    
    lcd_show_string(30, 130, 200, 16, 16, "SD Card Size:    MB", BLUE);
    lcd_show_num(30 + 14 * 8, 130, sdnand_info.chip_size >> 20, 3, 16, BLUE);
}

/**
 * @brief   SD NAND读测试
 * @param   无
 * @retval  无
 */
static void sdnand_read_test(void)
{
    uint8_t *buf;
    uint16_t i;
    
    buf = (uint8_t *)mymalloc(SRAMIN, sdnand_info.logic_block_size);
    if (buf == NULL)
    {
        return;
    }
    
    /* 读取并打印SD NAND第0个块的数据 */
    if (sdnand_read_disk(buf, 0, 1) == 0)
    {
        lcd_show_string(30, 150, 200, 16, 16, "USART1 Sending Data...", BLUE);
        printf("Block 0 Data:\r\n");
        for (i=0; i<sdnand_info.logic_block_size; i++)
        {
            printf("%02X ", buf[i]);
        }
        printf("\r\nData End\r\n");
        lcd_show_string(30, 150, 200, 16, 16, "USART1 Send Data Over!", BLUE);
    }
    else
    {
        printf("SD NAND read Failure!\r\n");
        lcd_show_string(30, 150, 200, 16, 16, "SD NAND read Failure!      ", BLUE);
    }
    
    myfree(SRAMIN, buf);
}

int main(void)
{
    uint8_t t = 0;
    uint8_t key;
    
    sys_mpu_config();                   /* 配置MPU */
    sys_cache_enable();                 /* 使能Cache */
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(300, 6, 2);    /* 配置时钟，600MHz */
    delay_init(600);                    /* 初始化延时 */
    usart_init(115200);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */
    hyperram_init();                    /* 初始化HyperRAM */
    lcd_init();                         /* 初始化LCD */
    my_mem_init(SRAMIN);                /* 初始化AXI-SRAM1~4内存池 */
    my_mem_init(SRAMEX);                /* 初始化XSPI2 HyperRAM内存池 */
    my_mem_init(SRAM12);                /* 初始化AHB-SRAM1~2内存池 */
    my_mem_init(SRAMDTCM);              /* 初始化DTCM内存池 */
    my_mem_init(SRAMITCM);              /* 初始化ITCM内存池 */
    sdnand_init();                      /* SD NAND初始化 */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "SD NAND TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    
    /* 显示SD NAND信息 */
    show_sdnand_info();
    
    while (1)
    {
        key = key_scan(0);
        if (key == KEY0_PRES)
        {
            /* SD NAND读测试 */
            sdnand_read_test();
        }
        
        if (++t == 20)
        {
            t = 0;
            LED0_TOGGLE();
        }
        
        delay_ms(10);
    }
}
