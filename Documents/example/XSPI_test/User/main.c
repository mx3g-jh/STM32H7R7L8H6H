/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2024-05-21
 * @brief       XSPI实验
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
#include "./BSP/NORFLASH/norflash.h"

static uint8_t g_text_buf[] = {"STM32 XSPI TEST"};
#define TEXT_SIZE (sizeof(g_text_buf))

int main(void)
{
    uint8_t t = 0;
    uint8_t key;
    norflash_type_t norflash_type;
    uint32_t flashsize;
    uint8_t data[TEXT_SIZE];
    
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
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "XSPI TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "WK_UP:Write KEY0:Read", RED);
    
    /* 初始化NOR Flash */
    norflash_type = norflash_init();
    /* 开启NOR Flash内存映射 */
    norflash_memory_mapped();
    if (norflash_type == NORFlash_Unknow)
    {
        while (1)
        {
            lcd_show_string(30, 130, 200, 16, 16, "NOR Flash Check Failed!", RED);
            delay_ms(500);
            lcd_show_string(30, 130, 200, 16, 16, "Please Check!          ", RED);
            delay_ms(500);
            LED0_TOGGLE();
        }
    }
    lcd_show_string(30, 130, 200, 16, 16, "NOR Flash Ready!", RED);
    
    /* 获取NOR Flash片大小 */
    flashsize = norflash_get_chip_size();
    
    while (1)
    {
        key = key_scan(0);
        if (key == WKUP_PRES)
        {
            /* 往NOR Flash写入数据 */
            lcd_fill(0, 150, 239, 319, WHITE);
            lcd_show_string(30, 150, 200, 16, 16, "Start Write Flash....", BLUE);
            norflash_init();
            norflash_write(flashsize - TEXT_SIZE, g_text_buf, TEXT_SIZE);
            norflash_memory_mapped();
            lcd_show_string(30, 150, 200, 16, 16, "Flash Write Finished!", BLUE);
        }
        else if (key == KEY0_PRES)
        {
            /* 从NOR Flash读取数据 */
            lcd_show_string(30, 150, 200, 16, 16, "Start Read Flash.... ", BLUE);
            norflash_read(flashsize - TEXT_SIZE, data, TEXT_SIZE);
            lcd_show_string(30, 150, 200, 16, 16, "The Data Readed Is:  ", BLUE);
            lcd_show_string(30, 170, 200, 16, 16, (char *)data, BLUE);
        }
        
        if (++t == 20)
        {
            t = 0;
            LED0_TOGGLE();
        }
        
        delay_ms(10);
    }
}
