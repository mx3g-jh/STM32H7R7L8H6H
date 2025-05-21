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

/* PWM剩余个数计数器 */
uint32_t g_timx_chy_npwm_remain = 0;

/**
 * @brief   初始化高级定时器输出指定个数PWM
 * @note    TIM1时钟源为timg2_ck
 * @param   arr: 自动重装载值
 * @param   psc: 预分频系数
 * @retval  无
 */
void atim_timx_chy_npwm_init(uint16_t arr, uint16_t psc)
{
    TIM_OC_InitTypeDef timx_oc_init_struct = {0};
    
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
    timx_oc_init_struct.OCMode = TIM_OCMODE_PWM1;                                   /* PWM模式 */
    timx_oc_init_struct.Pulse = (arr + 1) >> 1;                                     /* 占空比 */
    timx_oc_init_struct.OCPolarity = TIM_OCPOLARITY_HIGH;                           /* 有效电平 */
    HAL_TIM_PWM_ConfigChannel(&g_atimx_handle, &timx_oc_init_struct, ATIM_TIMX_CHY);
    
    /* 使能TIM更新中断 */
    __HAL_TIM_ENABLE_IT(&g_atimx_handle, TIM_IT_UPDATE);
    
    /* 开启TIM PWM输出 */
    HAL_TIM_PWM_Start(&g_atimx_handle, ATIM_TIMX_CHY);
    
    /* 配置中断优先级并使能中断 */
    HAL_NVIC_SetPriority(ATIM_TIMX_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ATIM_TIMX_IRQn);
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
        ATIM_TIMX_CLK_ENABLE();
        
        /* 初始化TIM PWM输出引脚 */
        gpio_init_struct.Pin = ATIM_TIMX_CHY_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
        gpio_init_struct.Alternate = ATIM_TIMX_CHY_GPIO_AF;
        HAL_GPIO_Init(ATIM_TIMX_CHY_GPIO_PORT, &gpio_init_struct);
    }
}

/**
 * @brief   设置高级定时器输出指定个数PWM
 * @param   npwm: 指定PWM个数
 * @retval  无
 */
void atim_timx_chy_npwm_set(uint32_t npwm)
{
    if (npwm == 0)
    {
        return;
    }
    
    g_timx_chy_npwm_remain = npwm;
    HAL_TIM_GenerateEvent(&g_atimx_handle, TIM_EVENTSOURCE_UPDATE);
}

/**
 * @brief   HAL库TIM超时中断回调函数
 * @param   TIM句柄指针
 * @retval  无
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    uint32_t npwm = 0;
    
    if (htim->Instance == ATIM_TIMX)
    {
        /* 关闭TIM更新中断 */
        __HAL_TIM_DISABLE_IT(htim, TIM_IT_UPDATE);
        
        /* 每次最多输出0x10000个PWM（TIMx_RCR只有[15:0]有效） */
        if (g_timx_chy_npwm_remain > 0x10000)
        {
            g_timx_chy_npwm_remain -= 0x10000;
            npwm = 0x10000;
        }
        else if ((g_timx_chy_npwm_remain % 0x10000) != 0)
        {
            npwm = g_timx_chy_npwm_remain % 0x10000;
            g_timx_chy_npwm_remain = 0;
        }
        
        if (npwm != 0)
        {
            /* 利用高级定时器的重复计数器来输出PWM */
            htim->Instance->RCR = npwm - 1;
            htim->Instance->EGR = TIM_EGR_UG;
            __HAL_TIM_ENABLE(htim);
        }
        else
        {
            ATIM_TIMX->CR1 &= ~(TIM_CR1_CEN);
        }
        
        /* 清除更新事件标志 */
        __HAL_TIM_CLEAR_IT(htim, TIM_IT_UPDATE);
        
        /* 使能TIM更新中断 */
        __HAL_TIM_ENABLE_IT(htim, TIM_IT_UPDATE);
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
