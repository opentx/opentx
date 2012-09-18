/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include <string.h>
#include <stdio.h>
#include "board/board.h"
#include "../FatFs/diskio.h"
#include "../FatFs/ff.h"
#include "memories/Media.h"
#include "debug.h"

#define SECTOR_SIZE_DEFAULT 512

/*-----------------------------------------------------------------------*/
/* Lock / unlock functions                                               */
/*-----------------------------------------------------------------------*/
int ff_cre_syncobj (BYTE vol, _SYNC_t *mutex)
{
  *mutex = CoCreateMutex();
  return 1;
}

int ff_req_grant (_SYNC_t mutex)
{
  CoEnterMutexSection(mutex);
  return 1;
}

void ff_rel_grant (_SYNC_t mutex)
{
  CoLeaveMutexSection(mutex);
}

int ff_del_syncobj (_SYNC_t mutex)
{
  return 1;
}

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/


DSTATUS disk_initialize (
	BYTE drv				/* Physical drive number (0..) */
)
{
    DSTATUS stat = STA_NOINIT;

    switch (drv)
    {
        default:
            stat = 0;
            break;
    }

    return stat;
}

/*-----------------------------------------------------------------------*/
/* Return Disk Status                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE drv        /* Physical drive number (0..) */
)
{
    DSTATUS stat = STA_NOINIT;

    switch (drv)
    {
        default:
            stat = 0;  // ok
            break;
    }

    return stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE drv,		/* Physical drive number (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	BYTE count		/* Number of sectors to read (1..255) */
)
{
    unsigned char result;
    DRESULT res = RES_ERROR;

    TRACE_ERROR("MED_Read drv=%d sector=%d count=%d\n\r", drv, sector, count);

    unsigned int addr, len;
    if (medias[drv].blockSize < SECTOR_SIZE_DEFAULT)
    {
        addr = sector * (SECTOR_SIZE_DEFAULT / medias[drv].blockSize);
        len  = count * (SECTOR_SIZE_DEFAULT / medias[drv].blockSize);
    }
    else
    {
        addr = sector;
        len  = count;
    }

    result = MED_Read(&medias[drv], addr, (void*)buff, len, NULL, NULL);

    if( result == MED_STATUS_SUCCESS )
    {
        res = RES_OK;
    }
    else
    {
        TRACE_ERROR("MED_Read pb: 0x%X\n\r", result);
        res = RES_ERROR;
    }
   return res;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
/* The FatFs module will issue multiple sector transfer request
/  (count > 1) to the disk I/O layer. The disk function should process
/  the multiple sector transfer properly Do. not translate it into
/  multiple single sector transfers to the media, or the data read/write
/  performance may be drasticaly decreased. */

#if _READONLY == 0
DRESULT disk_write (
	BYTE drv,			/* Physical drive number (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	BYTE count			/* Number of sectors to write (1..255) */
)
{
    DRESULT res=RES_PARERR;
    unsigned int result;
    void * tmp;
    tmp = (void *) buff;

    unsigned int addr, len;
    if (medias[drv].blockSize < SECTOR_SIZE_DEFAULT)
    {
        addr = sector * (SECTOR_SIZE_DEFAULT / medias[drv].blockSize);
        len  = count * (SECTOR_SIZE_DEFAULT / medias[drv].blockSize);
    }
    else
    {
        addr = sector;
        len  = count;
    }

    result = MED_Write(&medias[drv], addr, (void*)tmp, len, NULL, NULL);

    if( result == MED_STATUS_SUCCESS )
    {
        res = RES_OK;
    }
    else
    {
        TRACE_ERROR("MED_Write pb: 0x%X\n\r", result);
        res = RES_ERROR;
    }

    return res;
}
#endif /* _READONLY */

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
// Command    Description
//
//CTRL_SYNC    Make sure that the disk drive has finished pending write process.
// When the disk I/O module has a write back cache, flush the dirty sector immediately.
// In read-only configuration, this command is not needed.
//
//GET_SECTOR_COUNT    Returns total sectors on the drive into the DWORD variable pointed by Buffer.
// This command is used in only f_mkfs function.
//
//GET_BLOCK_SIZE    Returns erase block size of the memory array in unit
// of sector into the DWORD variable pointed by Buffer.
// When the erase block size is unknown or magnetic disk device, return 1.
// This command is used in only f_mkfs function.
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE drv,        /* Physical drive number (0..) */
    BYTE ctrl,        /* Control code */
    void *buff        /* Buffer to send/receive control data */
)
{
    DRESULT res=RES_PARERR;

    switch (drv)
    {
#if 0
        case DRV_SDRAM :
        switch (ctrl)
        {
            case GET_BLOCK_SIZE:
                *(DWORD*)buff = 1;
                res = RES_OK;
                break;

            case GET_SECTOR_COUNT :   /* Get number of sectors on the disk (DWORD) */
                if (medias[DRV_SDRAM].blockSize < SECTOR_SIZE_DEFAULT)
                {
                    *(DWORD*)buff = (DWORD)(medias[DRV_SDRAM].size /
                                            (SECTOR_SIZE_DEFAULT /
                                            medias[DRV_SDRAM].blockSize));
                }
                else
                {
                    *(DWORD*)buff = (DWORD)(medias[DRV_SDRAM].size);
                }
                res = RES_OK;
                break;

            case GET_SECTOR_SIZE :   /* Get sectors on the disk (WORD) */
                if (medias[DRV_SDRAM].blockSize < SECTOR_SIZE_DEFAULT)
                {
                    *(WORD*)buff = SECTOR_SIZE_DEFAULT;
                }
                else
                {
                    *(WORD*)buff = medias[DRV_SDRAM].blockSize;
                }
                res = RES_OK;
                break;

            case CTRL_SYNC :   /* Make sure that data has been written */
                res = RES_OK;
                break;

            default:
                res = RES_PARERR;
        }
        break;
#endif
        default:
        // case DRV_MMC :
        switch (ctrl)
        {
            case GET_BLOCK_SIZE:
                *(DWORD*)buff = 1;
                res = RES_OK;
                break;

            case GET_SECTOR_COUNT :   /* Get number of sectors on the disk (DWORD) */
                if (medias[DRV_SDMMC].blockSize < SECTOR_SIZE_DEFAULT)
                {
                    *(DWORD*)buff = (DWORD)(medias[DRV_SDMMC].size /
                                                (SECTOR_SIZE_DEFAULT /
                                                medias[DRV_SDMMC].blockSize));
                }
                else
                {
                    *(DWORD*)buff = (DWORD)(medias[DRV_SDMMC].size);
                }
                res = RES_OK;
                break;

            case GET_SECTOR_SIZE :   /* Get sectors on the disk (WORD) */
                if (medias[DRV_SDMMC].blockSize < SECTOR_SIZE_DEFAULT)
                {
                    *(WORD*)buff = SECTOR_SIZE_DEFAULT;
                }
                else
                {
                    *(WORD*)buff = medias[DRV_SDMMC].blockSize;
                }
                res = RES_OK;
                break;

            case CTRL_SYNC :   /* Make sure that data has been written */
                res = RES_OK;
                break;

            default:
                res = RES_PARERR;
                break;
        }
        break;
#if 0
        case DRV_NAND :
            switch (ctrl)
            {
                case GET_BLOCK_SIZE:
                    *(DWORD*)buff = 1;
                    res = RES_OK;
                    break;

                case GET_SECTOR_COUNT :   /* Get number of sectors on the disk (DWORD) */
                    if (medias[DRV_NAND].blockSize < SECTOR_SIZE_DEFAULT)
                    {
                        *(DWORD*)buff = (DWORD)(medias[DRV_NAND].size /
                                                (SECTOR_SIZE_DEFAULT /
                                                medias[DRV_NAND].blockSize));
                    }
                    else
                    {
                        *(DWORD*)buff = (DWORD)(medias[DRV_NAND].size);
                    }
                    res = RES_OK;
                    break;

                case GET_SECTOR_SIZE :	 /* Get sectors on the disk (WORD) */
                    if (medias[DRV_NAND].blockSize < SECTOR_SIZE_DEFAULT)
                    {
                        *(WORD*)buff = SECTOR_SIZE_DEFAULT;
                    }
                    else
                    {
                        *(WORD*)buff = medias[DRV_NAND].blockSize;
                    }
                    res = RES_OK;
                    break;

                case CTRL_SYNC :   /* Make sure that data has been written */
                    MED_Flush(&medias[DRV_NAND]);
                    res = RES_OK;
                    break;

                default:
                    res = RES_PARERR;
        }
#endif
    }
   return res;
}

#if 0
//------------------------------------------------------------------------------
/// Current time is returned with packed into a DWORD value.
/// The bit field is as follows:
///   bit31:25  Year from 1980 (0..127)
///   bit24:21  Month (1..12)
///   bit20:16  Day in month(1..31)
///   bit15:11  Hour (0..23)
///   bit10:5   Minute (0..59)
///   bit4:0    Second / 2 (0..29)
//------------------------------------------------------------------------------
DWORD get_fattime (void)
{
    unsigned int time;

    time =  ((2010-1980)<<25)
          | ( 9<<21)
          | (15<<16)
          | (17<<11)
          | (45<<5)
          | ((59/2)<<0);

    return time;
}
#endif
