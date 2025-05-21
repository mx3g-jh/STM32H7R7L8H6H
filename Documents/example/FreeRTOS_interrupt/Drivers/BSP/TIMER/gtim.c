/**
 ****************************************************************************************************
 * @file        gtim.c
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

#include "./BSP/TIMER/gtim.h"
#include "./BSP/LED/led.h"
#include "./SYSTEM/USART/usart.h"

TIM_HandleTypeDef g_tim3_handle;        /* 定时器3句柄 */
TIM_HandleTypeDef g_tim5_handle;        /* 定时器5句柄 */

/**
 * @brief       通用定时器TIMX定时中断初始化函数
 * @note
 *              通用定时器的时钟来自APB1,当PPRE1≥2分频的时候
 *              通用定时器的时钟为APB1时钟的2倍, 而APB1为150M, 所以定时器时钟 = 300Mhz
 *              定时器溢出时间计算方法: Tout = ((arr + 1) * (psc + 1)) / Ft us.
 *              Ft=定时器工作频率,单位:Mhz
 *
 * @param       arr: 自动重装值。
 * @param       psc: 时钟预分频数
 * @retval      无
 */
void gtim_tim3_int_init(uint16_t arr, uint16_t psc)
{
    GTIM_TIMX_INT_CLK_ENABLE();                                     /* 使能TIMx时钟 */
    
    g_tim3_handle.Instance = GTIM_TIMX_INT;                         /* 通用定时器x */
    g_tim3_handle.Init.Prescaler = psc;                             /* 预分频系数 */
    g_tim3_handle.Init.CounterMode = TIM_COUNTERMODE_UP;            /* 递增计数模式 */
    g_tim3_handle.Init.Period = arr;                                /* 自动装载值 */
    HAL_TIM_Base_Init(&g_tim3_handle);

    HAL_NVIC_SetPriority(GTIM_TIMX_INT_IRQn, 4, 0);                 /* 设置中断优先级，抢占优先级1，子优先级3 */
    HAL_NVIC_EnableIRQ(GTIM_TIMX_INT_IRQn);                         /* 开启ITMx中断 */

    HAL_TIM_Base_Start_IT(&g_tim3_handle);                          /* 使能定时器x和定时器x更新中断 */
}

/**
 * @brief   初始化基本定时器
 * @param   arr: 自动重装载值
 * @param   psc: 预分频系数
 * @retval  无
 */
void gtim_tim5_int_init(uint16_t arr, uint16_t psc)
{
    __HAL_RCC_TIM5_CLK_ENABLE();
    
    g_tim5_handle.Instance = TIM5;
    g_tim5_handle.Init.Prescaler = psc;
    g_tim5_handle.Init.Period = arr;
    HAL_TIM_Base_Init(&g_tim5_handle);
    HAL_TIM_Base_Start_IT(&g_tim5_handle);
    
    /* 配置中断优先级并使能中断 */
    HAL_NVIC_SetPriority(TIM5_IRQn, 6, 0);
    HAL_NVIC_EnableIRQ(TIM5_IRQn);
}

/**
 * @brief       定时器中断服务函数
 * @param       无
 * @retval      无
 */
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim3_handle);
}

void TIM5_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_tim5_handle);
}

/**
 * @brief   HAL库基本定时器超时中断回调函数
 * @param   无
 * @retval  无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3)
    {
        printf("TIM3输出\r\n");
    }
    else if (htim->Instance == TIM5)
    {
        printf("TIM5输出\r\n");
    }
}