/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2024-05-21
 * @brief       FreeRTOS 移植实验
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
#include "./BSP/LCD/lcd.h"
#include "./MALLOC/malloc.h"
#include "./BSP/LED/led.h"
#include "./BSP/KEY/key.h"
#include "./BSP/HYPERRAM/hyperram.h"
#include "freertos_demo.h"


int main(void)
{
    sys_mpu_config();                   /* 配置MPU */
    sys_cache_enable();                 /* 使能Cache */
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(300, 6, 2);    /* 配置时钟，600MHz */
    delay_init(600);                    /* 初始化延时 */
    usart_init(115200);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    hyperram_init();                    /* HyperRAM初始化 */
    lcd_init();                         /* LCD初始化 */
    my_mem_init(SRAMIN);                /* 内部SRAM内存池初始化 */
    my_mem_init(SRAMEX);                /* 外部hyperram内存池初始化 */
    
    
    freertos_demo();                    /* 运行FreeRTOS例程 */
}
