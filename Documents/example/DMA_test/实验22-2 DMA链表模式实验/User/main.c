/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2024-05-21
 * @brief       DMA标准模式实验
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
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/LCD/lcd.h"
#include "./BSP/HYPERRAM/hyperram.h"
#include "./BSP/DMA/dma.h"

/* 传输数据 */
char *buffer[] = {
    "STM32\r\n",
    "DMA TEST\r\n",
    "正点原子 STM32 DMA\r\n",
    "\r\n",
};

/* 传输数据大小 */
uint32_t size[] = {
    sizeof("STM32\r\n") - 1,
    sizeof("DMA TEST\r\n") - 1,
    sizeof("正点原子 STM32 DMA\r\n") - 1,
    sizeof("\r\n") - 1,
};

/* 传输数据数量 */
uint32_t number = sizeof(buffer) / sizeof(buffer[0]);

int main(void)
{
    uint8_t t = 0;
    uint8_t key;
    uint16_t buf_index;
    uint8_t temp_index;
    
    sys_mpu_config();                           /* 配置MPU */
    sys_cache_enable();                         /* 使能Cache */
    HAL_Init();                                 /* 初始化HAL库 */
    sys_stm32_clock_init(300, 6, 2);            /* 配置时钟，600MHz */
    delay_init(600);                            /* 初始化延时 */
    usart_init(115200);                         /* 初始化串口 */
    led_init();                                 /* 初始化LED */
    key_init();                                 /* 初始化按键 */
    hyperram_init();                            /* 初始化HyperRAM */
    lcd_init();                                 /* 初始化LCD */
    dma_init((uint32_t *)buffer, size, number); /* 初始化DMA */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "DMA TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "KEY0:Start", RED);
    
    while (1)
    {
        key = key_scan(0);
        if (key == KEY0_PRES)
        {
            /* 开启DMA传输 */
            dma_start_transfer();
        }
        
        if (++t == 20)
        {
            t = 0;
            LED0_TOGGLE();
        }
        
        delay_ms(10);
    }
}
