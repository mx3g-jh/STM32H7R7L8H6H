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

/* PWM捕获状态 */
uint8_t g_timx_chy_pwmin_sta = 0;

/* PWM高电平脉宽 */
uint32_t g_timx_chy_pwmin_hval;

/* PWM周期 */
uint32_t g_timx_chy_pwmin_cval;

/**
 * @brief   初始化高级定时器PWM输入
 * @note    TIM1时钟源为timg2_ck
 * @param   psc: 预分频系数
 * @retval  无
 */
void atim_timx_pwmin_chy_init(uint16_t psc)
{
    TIM_SlaveConfigTypeDef tim_slave_config_struct = {0};
    TIM_IC_InitTypeDef tim_ic_init_struct = {0};
    
    /* 初始化TIM输入捕获 */
    g_atimx_handle.Instance = ATIM_TIMX;
    g_atimx_handle.Init.Prescaler = psc;                                        /* 预分频器系数 */
    g_atimx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;                       /* 计数模式 */
    g_atimx_handle.Init.Period = 0xFFFF;                                        /* 自动重装载值 */
    g_atimx_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;                 /* 时钟分频 */
    g_atimx_handle.Init.RepetitionCounter = 0;                                  /* 重复计数数值 */
    g_atimx_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;     /* 自动重装载预加载模式 */
    HAL_TIM_IC_Init(&g_atimx_handle);
    
    /* 配置TIM从模式 */
    tim_slave_config_struct.SlaveMode = TIM_SLAVEMODE_RESET;                    /* 从模式 */
    tim_slave_config_struct.InputTrigger = TIM_TS_TI1FP1;                       /* 触发源 */
    tim_slave_config_struct.TriggerPolarity = TIM_TRIGGERPOLARITY_RISING;       /* 触发极性 */
    tim_slave_config_struct.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;       /* 触发分频 */
    tim_slave_config_struct.TriggerFilter = 0;                                  /* 触发滤波 */
    HAL_TIM_SlaveConfigSynchro(&g_atimx_handle, &tim_slave_config_struct);
    
    /* 配置TIM输入捕获通道1 */
    tim_ic_init_struct.ICPolarity = TIM_ICPOLARITY_RISING;                      /* 极性 */
    tim_ic_init_struct.ICSelection = TIM_ICSELECTION_DIRECTTI;                  /* 输入选择 */
    tim_ic_init_struct.ICPrescaler = TIM_ICPSC_DIV1;                            /* 输入分频 */
    tim_ic_init_struct.ICFilter = 0;                                            /* 输入滤波 */
    HAL_TIM_IC_ConfigChannel(&g_atimx_handle, &tim_ic_init_struct, TIM_CHANNEL_1);
    
    /* 配置TIM输入捕获通道2 */
    tim_ic_init_struct.ICPolarity = TIM_ICPOLARITY_FALLING;                     /* 极性 */
    tim_ic_init_struct.ICSelection = TIM_ICSELECTION_INDIRECTTI;                /* 输入选择 */
    tim_ic_init_struct.ICPrescaler = TIM_ICPSC_DIV1;                            /* 输入分频 */
    tim_ic_init_struct.ICFilter = 0;                                            /* 输入滤波 */
    HAL_TIM_IC_ConfigChannel(&g_atimx_handle, &tim_ic_init_struct, TIM_CHANNEL_2);
    
    /* 开启TIM中断模式输入捕获 */
    HAL_TIM_IC_Start_IT(&g_atimx_handle, TIM_CHANNEL_1);
    HAL_TIM_IC_Start_IT(&g_atimx_handle, TIM_CHANNEL_2);
}

/**
 * @brief   HAL库TIM输入捕获初始化MSP函数
 * @param   TIM句柄指针
 * @retval  无
 */
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    if (htim->Instance == ATIM_TIMX)
    {
        /* 使能时钟 */
        ATIM_TIMX_CLK_ENABLE();
        ATIM_TIMX_CHY_GPIO_CLK_ENABLE();
        
        /* 配置TIM PWM输入引脚 */
        gpio_init_struct.Pin = ATIM_TIMX_CHY_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP; 
        gpio_init_struct.Pull = GPIO_PULLDOWN;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
        gpio_init_struct.Alternate = ATIM_TIMX_CHY_GPIO_AF;
        HAL_GPIO_Init(ATIM_TIMX_CHY_GPIO_PORT, &gpio_init_struct);
        
        /* 配置中断优先级并使能中断 */
        HAL_NVIC_SetPriority(ATIM_TIMX_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(ATIM_TIMX_IRQn);
    }
}

/**
 * @brief   HAL库TIM输入捕获中断回调函数
 * @param   TIM句柄指针
 * @retval  无
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == ATIM_TIMX)
    {
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
        {
            /* 获取捕获到PWM的周期 */
            g_timx_chy_pwmin_cval = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1) + 1;
            g_timx_chy_pwmin_sta = 1;
        }
        
        if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
        {
            /* 获取捕获到PWM的高电平脉宽 */
            g_timx_chy_pwmin_hval = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2) + 1;
        }
    }
}


/**
 * @brief   TIM中断服务函数
 * @param   无
 * @retval  无
 */
void ATIM_TIMX_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&g_atimx_handle);
}
