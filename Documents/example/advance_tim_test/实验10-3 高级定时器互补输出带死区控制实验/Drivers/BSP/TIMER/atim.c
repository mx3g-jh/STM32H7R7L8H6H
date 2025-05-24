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

/* 死区控制参数配置句柄 */
TIM_BreakDeadTimeConfigTypeDef g_break_dead_time_config_struct = {0};

/**
 * @brief   初始化高级定时器互补输出带死区控制
 * @note    TIM1时钟源为timg2_ck
 * @param   arr: 自动重装载值
 * @param   psc: 预分频系数
 * @retval  无
 */
void atim_timx_cplm_init(uint16_t arr, uint16_t psc)
{
    TIM_OC_InitTypeDef timx_oc_struct = {0};
    
    /* 初始化TIM PWM */
    g_atimx_handle.Instance = ATIM_TIMX;
    g_atimx_handle.Init.Prescaler = psc;                                            /* 预分频系数 */
    g_atimx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;                           /* 计数模式 */
    g_atimx_handle.Init.Period = arr;                                               /* 重装载值 */
    g_atimx_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;                     /* 时钟分频 */
    g_atimx_handle.Init.RepetitionCounter = 0;                                      /* 重复计数数值 */
    g_atimx_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;         /* 自动重装载预加载模式 */
    HAL_TIM_PWM_Init(&g_atimx_handle);
    
    /* 配置TIM PWM通道 */
    timx_oc_struct.OCMode = TIM_OCMODE_PWM1;                                        /* PWM模式 */
    timx_oc_struct.Pulse = (arr + 1) >> 1;                                          /* 占空比 */
    timx_oc_struct.OCPolarity = TIM_OCPOLARITY_LOW;                                 /* 有效电平 */
    timx_oc_struct.OCNPolarity = TIM_OCNPOLARITY_LOW;                               /* 互补通道有效电平 */
    timx_oc_struct.OCIdleState = TIM_OCIDLESTATE_SET;                               /* 空闲有效电平 */
    timx_oc_struct.OCNIdleState = TIM_OCNIDLESTATE_SET;                             /* 互补通道空闲有效电平 */
    HAL_TIM_PWM_ConfigChannel(&g_atimx_handle, &timx_oc_struct, ATIM_TIMX_CHY);
    
    /* 配置TIM死区控制参数 */
    g_break_dead_time_config_struct.OffStateRunMode = TIM_OSSR_DISABLE;             /* 运行模式的关闭输出状态 */
    g_break_dead_time_config_struct.OffStateIDLEMode = TIM_OSSI_DISABLE;            /* 空闲模式的关闭输出状态 */
    g_break_dead_time_config_struct.LockLevel = TIM_LOCKLEVEL_OFF;                  /* 寄存器锁 */
    g_break_dead_time_config_struct.BreakState = TIM_BREAK_ENABLE;                  /* 刹车输入使能 */
    g_break_dead_time_config_struct.BreakPolarity = TIM_BREAKPOLARITY_HIGH;         /* 刹车输入有效极性 */
    g_break_dead_time_config_struct.AutomaticOutput = TIM_AUTOMATICOUTPUT_ENABLE;   /* 自动使能输出 */
    HAL_TIMEx_ConfigBreakDeadTime(&g_atimx_handle, &g_break_dead_time_config_struct);
    
    /* 开启TIM PWM输出 */
    HAL_TIM_PWM_Start(&g_atimx_handle, ATIM_TIMX_CHY);
    HAL_TIMEx_PWMN_Start(&g_atimx_handle, ATIM_TIMX_CHY);
}

/**
 * @brief   设置高级定时器互补输出带死区控制
 * @param   ccr: 输出比较值
 * @param   dtg: 死区时间
 * @retval  无
 */
void atim_timx_cplm_set(uint16_t ccr, uint8_t dtg)
{
    g_break_dead_time_config_struct.DeadTime = dtg;                                     /* 死区时间 */
    HAL_TIMEx_ConfigBreakDeadTime(&g_atimx_handle, &g_break_dead_time_config_struct);   /* 配置死区时间参数 */
    __HAL_TIM_MOE_ENABLE(&g_atimx_handle);                                              /* 使能主输出 */
    __HAL_TIM_SET_COMPARE(&g_atimx_handle, ATIM_TIMX_CHY, ccr);                         /* 设置比较寄存器 */
}

/**
 * @brief   HAL库TIM PWM初始化MSP函数
 * @param   TIM句柄指针
 * @retval  无
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    if (htim->Instance == ATIM_TIMX)
    {
        /* 使能时钟 */
        ATIM_TIMX_CHY_GPIO_CLK_ENABLE();
        ATIM_TIMX_CHYN_GPIO_CLK_ENABLE();
        ATIM_TIMX_BKIN_GPIO_CLK_ENABLE();
        ATIM_TIMX_CLK_ENABLE();
        
        /* 配置输出比较通道1输出引脚 */
        gpio_init_struct.Pin = ATIM_TIMX_CHY_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
        gpio_init_struct.Alternate = ATIM_TIMX_CHY_GPIO_AF;
        HAL_GPIO_Init(ATIM_TIMX_CHY_GPIO_PORT, &gpio_init_struct);
        
        /* 配置输出比较通道1互补输出引脚 */
        gpio_init_struct.Pin = ATIM_TIMX_CHYN_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
        gpio_init_struct.Alternate = ATIM_TIMX_CHYN_GPIO_AF;
        HAL_GPIO_Init(ATIM_TIMX_CHYN_GPIO_PORT, &gpio_init_struct);
        
        /* 配置输出比较通道刹车输入引脚 */
        gpio_init_struct.Pin = ATIM_TIMX_BKIN_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLDOWN;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
        gpio_init_struct.Alternate = ATIM_TIMX_BKIN_GPIO_AF;
        HAL_GPIO_Init(ATIM_TIMX_BKIN_GPIO_PORT, &gpio_init_struct);
    }
}
