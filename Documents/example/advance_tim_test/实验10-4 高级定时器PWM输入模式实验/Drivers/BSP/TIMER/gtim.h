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
#define GTIM_TIMX                       TIM4
#define GTIM_TIMX_CLK_ENABLE()          do { __HAL_RCC_TIM4_CLK_ENABLE(); } while (0)
#define GTIM_TIMX_CHY                   TIM_CHANNEL_3
#define GTIM_TIMX_CHY_GPIO_PORT         GPIOD
#define GTIM_TIMX_CHY_GPIO_PIN          GPIO_PIN_14
#define GTIM_TIMX_CHY_GPIO_AF           GPIO_AF2_TIM4
#define GTIM_TIMX_CHY_GPIO_CLK_ENABLE() do { __HAL_RCC_GPIOD_CLK_ENABLE(); } while (0)

/* 函数声明 */
void gtim_timx_pwm_chy_init(uint16_t arr, uint16_t psc);    /* 初始化通用定时器PWM输出 */

#endif
