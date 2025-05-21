/**
 ****************************************************************************************************
 * @file        diskio.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-05-23
 * @brief       FATFS底层(diskio) 驱动代码
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
 * 修改说明
 * V1.0 20220523
 * 第一次发布
 *
 ****************************************************************************************************
 */

#include "./MALLOC/malloc.h"
#include "./SYSTEM/usart/usart.h"
#include "./FATFS/source/ff.h"
#include "./FATFS/source/diskio.h"
#include "./BSP/NORFLASH/norflash_ex.h"
#include "./BSP/SDNAND/spi_sdnand.h"
#include "./BSP/SDMMC/sdmmc_sdcard.h"


#define SD_CARD     0   /* SD卡,卷标为0 */
#define NOR_FLASH   1   /* NOR Flash,卷标为1 */
#define SD_NAND     2   /* SD NAND,卷标为2 */

/**
 * NOR Flash区域划分（最大扇区大小扇区对齐）
 *    名称      起始地址    大小
 *     代码区 0x00000000 0x00100000
 * 文件系统区 0x00100000 0x01800000
 *     字库区 0x01900000 0x00604000
 *     用户区 0x01F04000 0x000FC000
 */
#define NORFLASH_SECTOR_SIZE    (512)
#define NORFLASH_SECTOR_COUNT   (0x01800000 / NORFLASH_SECTOR_SIZE)
#define NORFLASH_BLOCK_SIZE     (8)
#define NORFLASH_FATFS_BASE     (0x00100000)

extern SD_HandleTypeDef g_sd_handle;

/**
 * @brief       获得磁盘状态
 * @param       pdrv : 磁盘编号0~9
 * @retval      执行结果(参见FATFS, DSTATUS的定义)
 */
DSTATUS disk_status (
    BYTE pdrv       /* Physical drive nmuber to identify the drive */
)
{
    return RES_OK;
}

/**
 * @brief       初始化磁盘
 * @param       pdrv : 磁盘编号0~9
 * @retval      执行结果(参见FATFS, DSTATUS的定义)
 */
DSTATUS disk_initialize (
    BYTE pdrv       /* Physical drive nmuber to identify the drive */
)
{
    uint8_t res = 0;

    switch (pdrv)
    {
        case SD_CARD:           /* SD卡 */
            res = sd_init();    /* SD卡初始化 */
            break;

        case NOR_FLASH:         /* NOR Flash */
            norflash_ex_init();
            break;

        case SD_NAND:           /* SD NAND */
            sdnand_init();      /* 初始化SD NAND */
            break;

        default:
            res = 1;
    }

    if (res)
    {
        return  STA_NOINIT;
    }
    else
    {
        return 0; /* 初始化成功*/
    }
}

/**
 * @brief       读扇区
 * @param       pdrv   : 磁盘编号0~9
 * @param       buff   : 数据接收缓冲首地址
 * @param       sector : 扇区地址
 * @param       count  : 需要读取的扇区数
 * @retval      执行结果(参见FATFS, DRESULT的定义)
 */
DRESULT disk_read (
    BYTE pdrv,      /* Physical drive nmuber to identify the drive */
    BYTE *buff,     /* Data buffer to store read data */
    DWORD sector,   /* Sector address in LBA */
    UINT count      /* Number of sectors to read */
)
{
    uint8_t res = 0;

    if (!count) return RES_PARERR;  /* count不能等于0，否则返回参数错误 */

    switch (pdrv)
    {
        case SD_CARD:   /* SD卡 */
            res = sd_read_disk(buff, sector, count);

            while (res) /* 读出错 */
            {
                if (res != 2) sd_init();    /* 重新初始化SD卡 */

                res = sd_read_disk(buff, sector, count);
                //printf("sd rd error:%d\r\n", res);
            }

            break;

        case NOR_FLASH:     /* NOR Flash */
            for (; count > 0; count--)
            {
                norflash_ex_read(NORFLASH_FATFS_BASE + sector * NORFLASH_SECTOR_SIZE, buff, NORFLASH_SECTOR_SIZE);
                sector++;
                buff += NORFLASH_SECTOR_SIZE;
            }

            res = 0;
            break;

        case SD_NAND:      /* SD NAND */
            res = sdnand_read_disk(buff, sector, count);
            break;

        default:
            res = 1;
    }

    /* 处理返回值，将返回值转成ff.c的返回值 */
    if (res == 0x00)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR; 
    }
}

/**
 * @brief       写扇区
 * @param       pdrv   : 磁盘编号0~9
 * @param       buff   : 发送数据缓存区首地址
 * @param       sector : 扇区地址
 * @param       count  : 需要写入的扇区数
 * @retval      执行结果(参见FATFS, DRESULT的定义)
 */
DRESULT disk_write (
    BYTE pdrv,          /* Physical drive nmuber to identify the drive */
    const BYTE *buff,   /* Data to be written */
    DWORD sector,       /* Sector address in LBA */
    UINT count          /* Number of sectors to write */
)
{
    uint8_t res = 0;

    if (!count) return RES_PARERR;  /* count不能等于0，否则返回参数错误 */

    switch (pdrv)
    {
        case SD_CARD:       /* SD卡 */
            res = sd_write_disk((uint8_t *)buff, sector, count);

            while (res)     /* 写出错 */
            {
                sd_init();  /* 重新初始化SD卡 */
                res = sd_write_disk((uint8_t *)buff, sector, count);
                //printf("sd wr error:%d\r\n", res);
            }

            break;

        case NOR_FLASH:     /* NOR Flash */
            for (; count > 0; count--)
            {
                norflash_ex_write(NORFLASH_FATFS_BASE + sector * NORFLASH_SECTOR_SIZE, (uint8_t *)buff, NORFLASH_SECTOR_SIZE);
                sector++;
                buff += NORFLASH_SECTOR_SIZE;
            }

            res = 0;
            break;

        case SD_NAND:      /* SD NAND */
            res = sdnand_write_disk((uint8_t *)buff, sector, count);
            break;

        default:
            res = 1;
    }

    /* 处理返回值，将返回值转成ff.c的返回值 */
    if (res == 0x00)
    {
        return RES_OK;
    }
    else
    {
        return RES_ERROR; 
    }
}

/**
 * @brief       获取其他控制参数
 * @param       pdrv   : 磁盘编号0~9
 * @param       ctrl   : 控制代码
 * @param       buff   : 发送/接收缓冲区指针
 * @retval      执行结果(参见FATFS, DRESULT的定义)
 */
DRESULT disk_ioctl (
    BYTE pdrv,      /* Physical drive nmuber (0..) */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
)
{
    DRESULT res;

    if (pdrv == SD_CARD)    /* SD卡 */
    {
        HAL_SD_CardInfoTypeDef sd_card_info_struct;
        
        HAL_SD_GetCardInfo(&g_sd_handle, &sd_card_info_struct);
        
        switch (cmd)
        {
            case CTRL_SYNC:
                res = RES_OK;
                break;

            case GET_SECTOR_SIZE:
                *(DWORD *)buff = 512;
                res = RES_OK;
                break;

            case GET_BLOCK_SIZE:
                *(WORD *)buff = sd_card_info_struct.LogBlockSize;
                res = RES_OK;
                break;

            case GET_SECTOR_COUNT:
                *(DWORD *)buff = sd_card_info_struct.LogBlockNbr;
                res = RES_OK;
                break;

            default:
                res = RES_PARERR;
                break;
        }
    }
    else if (pdrv == NOR_FLASH) /* NOR Flash */
    {
        switch (cmd)
        {
            case CTRL_SYNC:
                res = RES_OK;
                break;

            case GET_SECTOR_SIZE:
                *(WORD *)buff = NORFLASH_SECTOR_SIZE;
                res = RES_OK;
                break;

            case GET_BLOCK_SIZE:
                *(WORD *)buff = NORFLASH_BLOCK_SIZE;
                res = RES_OK;
                break;

            case GET_SECTOR_COUNT:
                *(DWORD *)buff = NORFLASH_SECTOR_COUNT;
                res = RES_OK;
                break;

            default:
                res = RES_PARERR;
                break;
        }
    }
    else if (pdrv == SD_NAND)  /* SDNAND */
    {
        switch (cmd)
        {
            case CTRL_SYNC:
                res = RES_OK;
                break;

            case GET_SECTOR_SIZE:
                *(WORD *)buff = 512;
                res = RES_OK;
                break;

            case GET_BLOCK_SIZE:
                *(WORD *)buff = sdnand_info.logic_block_size;
                res = RES_OK;
                break;

            case GET_SECTOR_COUNT:
                *(DWORD *)buff = sdnand_info.logic_block_num;
                res = RES_OK;
                break;

            default:
                res = RES_PARERR;
                break;
        }
    }
    else
    {
        res = RES_ERROR;    /* 其他的不支持 */
    }
    
    return res;
}




















