/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2024-05-21
 * @brief       USB虚拟串口（Slave）实验
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
#include "./BSP/LCD/lcd.h"
#include "./BSP/HYPERRAM/hyperram.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_cdc.h"
#include "usbd_cdc_if.h"

/* USBD句柄 */
USBD_HandleTypeDef g_usbd_handle = {0};

int main(void)
{
    uint16_t len;
    uint16_t times = 0;
    uint8_t conn_sta;
    
    sys_mpu_config();                   /* 配置MPU */
    sys_cache_enable();                 /* 使能Cache */
    HAL_Init();                         /* 初始化HAL库 */
    sys_stm32_clock_init(300, 6, 2);    /* 配置时钟，600MHz */
    delay_init(600);                    /* 初始化延时 */
    usart_init(115200);                 /* 初始化串口 */
    led_init();                         /* 初始化LED */
    hyperram_init();                    /* 初始化HyperRAM */
    lcd_init();                         /* 初始化LCD */
    my_mem_init(SRAMIN);                /* 初始化AXI-SRAM1~4内存池 */
    my_mem_init(SRAMEX);                /* 初始化XSPI2 HyperRAM内存池 */
    my_mem_init(SRAM12);                /* 初始化AHB-SRAM1~2内存池 */
    my_mem_init(SRAMDTCM);              /* 初始化DTCM内存池 */
    my_mem_init(SRAMITCM);              /* 初始化ITCM内存池 */
    
    lcd_show_string(30, 50, 200, 16, 16, "STM32", RED);
    lcd_show_string(30, 70, 200, 16, 16, "USB VCP TEST", RED);
    lcd_show_string(30, 90, 200, 16, 16, "ATOM@ALIENTEK", RED);
    
    USBD_Init(&g_usbd_handle, &CDC_Desc, DEVICE_HS);
    USBD_RegisterClass(&g_usbd_handle, USBD_CDC_CLASS);
    USBD_CDC_RegisterInterface(&g_usbd_handle, &USBD_CDC_fops);
    USBD_Start(&g_usbd_handle);
    delay_ms(1000);
    
    while (1)
    {
        if (conn_sta != g_usb_conn_state)
        {
            conn_sta = g_usb_conn_state;
            
            if (g_usb_conn_state != 0)
            {
                lcd_show_string(30, 190, 200, 16, 16, "USB Connected    ", RED);
            }
            else
            {
                lcd_show_string(30, 190, 200, 16, 16, "USB Disconnected ", RED);
            }
        }
        
        if ((g_usbd_cdc_rx_sta & (1 << 15)) == (1 << 15))
        {
            len = g_usbd_cdc_rx_sta & 0x3FFF;
            usb_printf("\r\n您发送的消息为：\r\n");
            usbd_cdc_transmit(g_usbd_cdc_rx_buffer, len);
            usb_printf("\r\n\r\n");
            g_usbd_cdc_rx_sta = 0;
        }
        else
        {
            if ((times % 5000) == 0)
            {
                usb_printf("\r\n正点原子 H7R7开发板 USB虚拟串口（Slave）实验\r\n");
                usb_printf("正点原子@ALIENTEK\r\n\r\n\r\n");
            }
            if ((times % 200) == 0)
            {
                usb_printf("请输入数据，以回车键结束\r\n");
            }
            if ((times % 30) == 0)
            {
                LED0_TOGGLE();
            }
            times++;
            delay_ms(10);
        }
    }
}
