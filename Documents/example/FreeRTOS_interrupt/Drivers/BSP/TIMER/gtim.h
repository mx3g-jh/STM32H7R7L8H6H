/**
 ****************************************************************************************************
 * @file        gtim.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2024-05-21
 * @brief       通用定时器驱动代码
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

#ifndef __GTIM_H
#define __GTIM_H

#include "./SYSTEM/sys/sys.h"

/* 通用定时器定义 */
#define GTIM_TIMX_INT                    TIM3
#define GTIM_TIMX_INT_IRQn               TIM3_IRQn
#define GTIM_TIMX_INT_IRQHandler         TIM3_IRQHandler
#define GTIM_TIMX_INT_CLK_ENABLE()       do{ __HAL_RCC_TIM3_CLK_ENABLE(); }while(0)  /* TIM3 时钟使能 */

/******************************************************************************************************/

void gtim_tim3_int_init(uint16_t arr, uint16_t psc);      /* 通用定时器 定时中断初始化函数 */
void gtim_tim5_int_init(uint16_t arr, uint16_t psc);

#endif
