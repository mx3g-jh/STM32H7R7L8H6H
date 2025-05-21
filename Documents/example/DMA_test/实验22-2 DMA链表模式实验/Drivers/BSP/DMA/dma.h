/**
 ****************************************************************************************************
 * @file        dma.h
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

#ifndef __DMA_H
#define __DMA_H

#include "./SYSTEM/sys/sys.h"

/* 功能定义 */
#define DMA_MAX_NODE    10  /* DMA链表最大节点数量 */

/* 函数声明 */
void dma_init(uint32_t *bufaddr, uint32_t *bufsize, uint32_t bufnum);   /* 初始化DMA */
void dma_start_transfer(void);                                          /* 开启DMA传输 */

#endif
