/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2024-05-21
 * @brief       HyperRAM实验
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

static uint32_t buffer[0x02000000 / sizeof(uint32_t)] __attribute__((section(".bss.ARM.__at_0x70000000")));

/**
 * @brief   测试HyperRAM容量
 * @param   x: LCD上显示提示信息的起始X坐标
 * @param   y: LCD上显示提示信息的起始Y坐标
 * @retval  无
 */
void hyperram_test(uint16_t x, uint16_t y)
{
    uint32_t i;
    uint32_t temp = 0;
    uint32_t sval = 0;
    uint32_t size;
    
    size = hyperram_get_size();
    lcd_show_string(x, y, 180, y + 16, 16, "HyperRAM Test:      KB", RED);
    
    /* 每间隔16KB写入1字数据 */
    for (i = 0; i < size; i += 0x4000UL)
    {
        *(volatile uint32_t *)(HYPERRAM_BASE_ADDR + i) = temp++;
    }
    
    /* 每间隔16KB读取1字数据进行校验 */
    for (i = 0; i < size; i += 0x4000UL)
    {
        temp = *(volatile uint32_t *)(HYPERRAM_BASE_ADDR + i);
        
        if ((temp != 0) && (temp <= sval))
        {
            break;
        }
        else
        {
            sval = temp;
        }
        
        lcd_show_num(x + 15 * 8, y, (temp + 1) * 16, 5, 16, BLUE);
        printf("HyperRAM Capacity: %dKB\r\n", (temp + 1) * 16);
    }
}

int main(void)
{
    uint8_t t = 0;
    uint8_t key;
    uint32_t i;
    
    sys_mpu_config();                   /* 配置MPU */
    sys_cache_enable();                 /* 使能Cache */
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(300, 6, 2);    /* 配置时钟，600MHz */
    delay_init(600);                    /* 初始化延时 */
    usart_init(115200);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    key_init();                         /* 初始化按键 */
    lcd_init();                         /* 初始化LCD */
    hyperram_init();                    /* 初始化HyperRAM */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "HyperRAM TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "KEY0: Test HyperRAM", RED);
    lcd_show_string(30, 130, 200, 16, 16, "KEY1: Test Data", RED);
    
    /* 准备测试数据 */
    for (i = 0; i < (sizeof(buffer) / sizeof(uint32_t)); i++)
    {
        buffer[i] = i;
    }
    
    while (1)
    {
        key = key_scan(0);
        if (key == KEY0_PRES)
        {
            /* 测试HyperRAM容量 */
            hyperram_test(30, 170);
        }
        else if (key == KEY1_PRES)
        {
            /* 读取测试数据 */
            for (i = 0; i < (sizeof(buffer) / sizeof(uint32_t)); i++)
            {
                lcd_show_num(30, 190, buffer[i], 7, 16, BLUE);
                printf("buffer[%d]: %d\r\n", i, buffer[i]);
            }
        }
        
        if (++t == 20)
        {
            t = 0;
            LED0_TOGGLE();
        }
        
        delay_ms(10);
    }
}
