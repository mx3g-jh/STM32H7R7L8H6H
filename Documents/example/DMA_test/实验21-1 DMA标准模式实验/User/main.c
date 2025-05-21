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

extern UART_HandleTypeDef g_uart1_handle;

char temp[] = {"正点原子 STM32 DMA实验\r\n"};
uint8_t buf[(sizeof(temp) - 1) * 200] = {0};
uint8_t uart_ready = 1;

/**
 * @brief   HAL库UART传输完成回调函数
 * @param   无
 * @retval  无
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    /* 标记传输完成，可以进行下一次传输 */
    uart_ready = 1;
}

int main(void)
{
    uint8_t t = 0;
    uint8_t key;
    uint16_t buf_index;
    uint8_t temp_index;
    
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
    dma_init();                         /* 初始化DMA */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "DMA TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    lcd_show_string(30, 110, 200, 16, 16, "KEY0:Start", RED);
    
    /* 准备数据 */
    for (buf_index = 0; buf_index < (sizeof(buf) / (sizeof(temp) - 1)); buf_index++)
    {
        for (temp_index = 0; temp_index < (sizeof(temp) - 1); temp_index++)
        {
            buf[buf_index * (sizeof(temp) - 1) + temp_index] = temp[temp_index];
        }
    }
    SCB_CleanDCache();
    
    while (1)
    {
        key = key_scan(0);
        if (key == KEY0_PRES)
        {
            if (uart_ready == 1)
            {
                uart_ready = 0;
                /* DMA传输串口数据 */
                HAL_UART_Transmit_DMA(&g_uart1_handle, buf, sizeof(buf));
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
