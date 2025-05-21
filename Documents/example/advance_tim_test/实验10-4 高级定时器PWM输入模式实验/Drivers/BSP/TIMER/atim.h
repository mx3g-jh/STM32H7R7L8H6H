/**
 ****************************************************************************************************
 * @file        atim.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2024-05-21
 * @brief       高级定时器驱动代码
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

#ifndef __ATIM_H
#define __ATIM_H

#include "./SYSTEM/sys/sys.h"

/* 高级定时器定义 */
#define ATIM_TIMX                       TIM1
#define ATIM_TIMX_IRQn                  TIM1_CC_IRQn
#define ATIM_TIMX_IRQHandler            TIM1_CC_IRQHandler
#define ATIM_TIMX_CLK_ENABLE()          do { __HAL_RCC_TIM1_CLK_ENABLE(); } while (0)
#define ATIM_TIMX_CHY                   TIM_CHANNEL_1
#define ATIM_TIMX_CHY_GPIO_PORT         GPIOC
#define ATIM_TIMX_CHY_GPIO_PIN          GPIO_PIN_2
#define ATIM_TIMX_CHY_GPIO_AF           GPIO_AF1_TIM1
#define ATIM_TIMX_CHY_GPIO_CLK_ENABLE() do { __HAL_RCC_GPIOC_CLK_ENABLE(); } while (0)

/* 函数声明 */
void atim_timx_pwmin_chy_init(uint16_t psc);    /* 初始化高级定时器PWM输入 */

#endif
