/**
 ****************************************************************************************************
 * @file        dma.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2024-05-21
 * @brief       DMA驱动代码
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

#include "./BSP/DMA/dma.h"

DMA_HandleTypeDef g_dma_handle = {0};   /* DMA句柄 */

extern UART_HandleTypeDef g_uart1_handle;

/**
 * @brief   初始化DMA
 * @param   无
 * @retval  无
 */
void dma_init(void)
{
    /* 使能时钟 */
    __HAL_RCC_GPDMA1_CLK_ENABLE();
    
    /* 初始化DMA */
    g_dma_handle.Instance = GPDMA1_Channel0;
    g_dma_handle.Init.Request = GPDMA1_REQUEST_USART1_TX;                                           /* 通道请求 */
    g_dma_handle.Init.BlkHWRequest = DMA_BREQ_SINGLE_BURST;                                         /* 块硬件请求模式 */
    g_dma_handle.Init.Direction = DMA_MEMORY_TO_PERIPH;                                             /* 传输方向 */
    g_dma_handle.Init.SrcInc = DMA_SINC_INCREMENTED;                                                /* 传输源地址增量模式 */
    g_dma_handle.Init.DestInc = DMA_DINC_FIXED;                                                     /* 传输目标地址增量模式 */
    g_dma_handle.Init.SrcDataWidth = DMA_SRC_DATAWIDTH_BYTE;                                        /* 传输源数据宽度 */
    g_dma_handle.Init.DestDataWidth = DMA_DEST_DATAWIDTH_BYTE;                                      /* 传输目标数据宽度 */
    g_dma_handle.Init.Priority = DMA_LOW_PRIORITY_LOW_WEIGHT;                                       /* 优先级 */
    g_dma_handle.Init.SrcBurstLength = 1;                                                           /* 传输源突发长度 */ 
    g_dma_handle.Init.DestBurstLength = 1;                                                          /* 传输目标突发长度 */
    g_dma_handle.Init.TransferAllocatedPort = DMA_SRC_ALLOCATED_PORT0 | DMA_DEST_ALLOCATED_PORT0;   /* 传输端口分配 */
    g_dma_handle.Init.TransferEventMode = DMA_TCEM_BLOCK_TRANSFER;                                  /* 传输事件模式 */
    g_dma_handle.Init.Mode = DMA_NORMAL;                                                            /* 传输模式 */
    HAL_DMA_Init(&g_dma_handle);
    
    /* 关联外设与DMA */
    __HAL_LINKDMA(&g_uart1_handle, hdmatx, g_dma_handle);
    
    /* 配置通道属性 */
    HAL_DMA_ConfigChannelAttributes(&g_dma_handle, DMA_CHANNEL_NPRIV);
    
    /* 配置中断优先级并使能中断 */
    HAL_NVIC_SetPriority(GPDMA1_Channel0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(GPDMA1_Channel0_IRQn);
}

/**
 * @brief   GPDMA1 Channel0中断服务函数
 * @param   无
 * @retval  无
 */
void GPDMA1_Channel0_IRQHandler(void)
{
    HAL_DMA_IRQHandler(&g_dma_handle);
}
