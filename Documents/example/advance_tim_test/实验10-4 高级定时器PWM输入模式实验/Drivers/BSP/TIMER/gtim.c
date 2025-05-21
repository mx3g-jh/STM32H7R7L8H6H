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

/* TIM句柄 */
TIM_HandleTypeDef g_gtimx_handle = {0};

/**
 * @brief   初始化通用定时器PWM输出
 * @note    TIM2、TIM3、TIM4、TIM5、TIM12、TIM13、TIM14时钟源为timg1_ck
 * @note    TIM9、TIM15、TIM16、TIM17时钟源为timg2_ck
 * @param   arr: 自动重装载值
 * @param   psc: 预分频系数
 * @retval  无
 */
void gtim_timx_pwm_chy_init(uint16_t arr, uint16_t psc)
{
    TIM_OC_InitTypeDef timx_oc_pwm_struct = {0};
    
    /* 初始化TIM PWM */
    g_gtimx_handle.Instance = GTIM_TIMX;
    g_gtimx_handle.Init.Prescaler = psc;                                            /* 预分频系数 */
    g_gtimx_handle.Init.CounterMode = TIM_COUNTERMODE_UP;                           /* 计数模式 */
    g_gtimx_handle.Init.Period = arr;                                               /* 重装载值 */
    g_gtimx_handle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;                     /* 时钟分频 */
    g_gtimx_handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;         /* 自动重装载预加载模式 */
    HAL_TIM_PWM_Init(&g_gtimx_handle);
    
    /* 配置TIM PWM通道 */
    timx_oc_pwm_struct.OCMode = TIM_OCMODE_PWM1;                                    /* PWM模式 */
    timx_oc_pwm_struct.Pulse = (arr + 1) >> 1;                                      /* 占空比 */
    timx_oc_pwm_struct.OCPolarity = TIM_OCPOLARITY_LOW;                             /* 有效电平 */
    HAL_TIM_PWM_ConfigChannel(&g_gtimx_handle, &timx_oc_pwm_struct, GTIM_TIMX_CHY);
    
    /* 开启TIM PWM输出 */
    HAL_TIM_PWM_Start(&g_gtimx_handle, GTIM_TIMX_CHY);
}

/**
 * @brief   HAL库TIM PWM初始化MSP函数
 * @param   TIM句柄指针
 * @retval  无
 */
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim)
{
    GPIO_InitTypeDef gpio_init_struct = {0};
    
    if (htim->Instance == GTIM_TIMX)
    {
        /* 使能时钟 */
        GTIM_TIMX_CHY_GPIO_CLK_ENABLE();
        GTIM_TIMX_CLK_ENABLE();
        
        /* 初始化TIM PWM输出引脚 */
        gpio_init_struct.Pin = GTIM_TIMX_CHY_GPIO_PIN;
        gpio_init_struct.Mode = GPIO_MODE_AF_PP;
        gpio_init_struct.Pull = GPIO_PULLUP;
        gpio_init_struct.Speed = GPIO_SPEED_FREQ_LOW;
        gpio_init_struct.Alternate = GTIM_TIMX_CHY_GPIO_AF;
        HAL_GPIO_Init(GTIM_TIMX_CHY_GPIO_PORT, &gpio_init_struct);
    }
}
