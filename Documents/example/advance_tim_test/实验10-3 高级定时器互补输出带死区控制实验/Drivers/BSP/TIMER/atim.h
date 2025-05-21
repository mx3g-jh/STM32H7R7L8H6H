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
#define ATIM_TIMX                           TIM1
#define ATIM_TIMX_CLK_ENABLE()              do { __HAL_RCC_TIM1_CLK_ENABLE(); } while (0)
#define ATIM_TIMX_CHY                       TIM_CHANNEL_1
#define ATIM_TIMX_CHY_GPIO_PORT             GPIOC
#define ATIM_TIMX_CHY_GPIO_PIN              GPIO_PIN_2
#define ATIM_TIMX_CHY_GPIO_AF               GPIO_AF1_TIM1
#define ATIM_TIMX_CHY_GPIO_CLK_ENABLE()     do { __HAL_RCC_GPIOC_CLK_ENABLE(); } while (0)
#define ATIM_TIMX_CHYN_GPIO_PORT            GPIOE
#define ATIM_TIMX_CHYN_GPIO_PIN             GPIO_PIN_8
#define ATIM_TIMX_CHYN_GPIO_AF              GPIO_AF1_TIM1
#define ATIM_TIMX_CHYN_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOE_CLK_ENABLE(); } while (0)
#define ATIM_TIMX_BKIN_GPIO_PORT            GPIOA
#define ATIM_TIMX_BKIN_GPIO_PIN             GPIO_PIN_6
#define ATIM_TIMX_BKIN_GPIO_AF              GPIO_AF1_TIM1
#define ATIM_TIMX_BKIN_GPIO_CLK_ENABLE()    do { __HAL_RCC_GPIOA_CLK_ENABLE(); } while (0)

/* 函数声明 */
void atim_timx_cplm_init(uint16_t arr, uint16_t psc);   /* 初始化高级定时器互补输出带死区控制 */
void atim_timx_cplm_set(uint16_t ccr, uint8_t dtg);     /* 设置高级定时器互补输出带死区控制 */

#endif
