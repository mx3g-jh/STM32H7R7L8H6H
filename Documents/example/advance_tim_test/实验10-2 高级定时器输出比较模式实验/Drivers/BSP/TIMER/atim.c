/**
 ****************************************************************************************************
 * @file        atim.c
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

#include "./BSP/TIMER/atim.h"

/* TIM句柄 */
TIM_HandleTypeDef g_atimx_handle = {0};

/**
 * @brief   初始化高级定时器输出比较
 * @note    TIM1时钟源为timg2_ck
 * @param   arr: 自动重装载值
 * @param   psc: 预分频系数
 * @retval  无
 */
void atim_timx_comp_init(uint16_t arr, uint16_t psc)
{
    TIM_OC_InitTypeDef timx_oc_init_struct = {0};
    
    /* 初始化TIM输出比较 */
    g_atimx_handle.Instance = ATIM_TIMX;
    g_atimx_handle.Init.Prescaler = psc;                                            /* 预分频系数 */
    g_atimx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;                           /* 计数模式 */
    g_atimx_handle.Init.Period = arr;                                               /* 重装载值 */
    g_atimx_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;                     /* 时钟分频 */
    g_atimx_handle.Init.RepetitionCounter = 0;                                      /* 重复计数数值 */
    g_atimx_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;         /* 自动重装载预加载模式 */
    HAL_TIM_OC_Init(&g_atimx_handle);
    
    /* 配置TIM输出比较通道 */
    timx_oc_init_struct.OCMode = TIM_OCMODE_TOGGLE;                                 /* 输出比较模式 */
    timx_oc_init_struct.Pulse = (((arr + 1) >> 2) * 1) - 1;                         /* 比较值 */
    timx_oc_init_struct.OCPolarity = TIM_OCPOLARITY_HIGH;                           /* 输出比较极性 */
    HAL_TIM_OC_ConfigChannel(&g_atimx_handle, &timx_oc_init_struct, TIM_CHANNEL_1);
    
    /* 配置TIM输出比较通道 */
    timx_oc_init_struct.OCMode = TIM_OCMODE_TOGGLE;                                 /* 输出比较模式 */
    timx_oc_init_struct.Pulse = (((arr + 1) >> 2) * 3) - 1;                         /* 比较值 */
    timx_oc_init_struct.OCPolarity = TIM_OCPOLARITY_HIGH;                           /* 输出比较极性 */
    HAL_TIM_OC_ConfigChannel(&g_atimx_handle, &timx_oc_init_struct, TIM_CHANNEL_2);
    
    /* 开启TIM输出比较输出 */
    HAL_TIM_OC_Start(&g_atimx_handle, TIM_CHANNEL_1);
    HAL_TIM_OC_Start(&g_atimx_handle, TIM_CHANNEL_2);
}

/**
 * @brief   HAL库TIM输出比较初始化MSP函数
 * @param   TIM句柄指针
 * @retval  无
 */
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    if (htim->Instance == ATIM_TIMX)
    {
        /* 使能时钟 */
        ATIM_TIMX_CH1_GPIO_CLK_ENABLE();
        ATIM_TIMX_CH2_GPIO_CLK_ENABLE();
        ATIM_TIMX_CLK_ENABLE();
        
        /* 配置TIM输出比较通道1输出引脚 */
        gpio_init_struct.Pin = ATIM_TIMX_CH1_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
        gpio_init_struct.Alternate = ATIM_TIMX_CH1_GPIO_AF;
        HAL_GPIO_Init(ATIM_TIMX_CH1_GPIO_PORT, &gpio_init_struct);
        
        /* 配置TIM输出比较通道2输出引脚 */
        gpio_init_struct.Pin = ATIM_TIMX_CH2_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
        gpio_init_struct.Alternate = ATIM_TIMX_CH2_GPIO_AF;
        HAL_GPIO_Init(ATIM_TIMX_CH2_GPIO_PORT, &gpio_init_struct);
    }
}
