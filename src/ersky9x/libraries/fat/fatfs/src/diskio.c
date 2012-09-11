/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2007        */
/*-----------------------------------------------------------------------*/
/* This is a stub disk I/O module that acts as front end of the existing */
/* disk I/O modules and attach it to FatFs module with common interface. */
/*-----------------------------------------------------------------------*/

#include <board.h>
#include <board_memories.h>
#include <utility/trace.h>
#include <utility/assert.h>

#include <diskio.h>
#include <ftldrv.h>

#include <string.h>
#include <stdio.h>
#include "integer.h"
#include "fatfs_config.h"

//------------------------------------------------------------------------------
//         Internal variables
//------------------------------------------------------------------------------

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/


DSTATUS disk_initialize (
    BYTE drv                /* Physical drive number (0..) */
)
{
    DSTATUS stat = STA_NOINIT;

    switch (drv) {
        case DRV_SDRAM :
            stat = 0;
            break;

        case DRV_MMC :
            stat = 0;
            break;

        case DRV_NAND:
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
    DSTATUS stat=STA_NOINIT;

    switch (drv) {
        case DRV_SDRAM :
            stat = 0;  // ok
            break;

        case DRV_MMC :
            stat = 0;  // ok
            break;
        case DRV_NAND:
            stat = 0;
            break;
    }

    return stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE drv,        /* Physical drive number (0..) */
    BYTE *buff,        /* Data buffer to store read data */
    DWORD sector,    /* Sector number (LBA) */
    BYTE count        /* Sector count (1..255) */
)
{
    unsigned char result;
    DRESULT res = RES_ERROR;

    unsigned int addr, len;
    RtMedia *pRtMedia;
    Media *pMedia;

    if (drv == DRV_NAND)
      pRtMedia = &(gRtNandMedias);
    else {
      TRACE_ERROR("disk_read: Unknown drive\n\r");
      return RES_ERROR;
    }

    pMedia = &(pRtMedia->media);

    if (pMedia->blockSize < SECTOR_SIZE_DEFAULT) {
        addr = sector * (SECTOR_SIZE_DEFAULT / pMedia->blockSize);
        len  = count * (SECTOR_SIZE_DEFAULT / pMedia->blockSize);
    }
    else {
        addr = sector;
        len  = count;
    }

    result = RTMEDIA_Read(pRtMedia,
                      addr,               // address
                      (void*)buff,          // data
                      len,                // data size
                      NULL,
                      NULL);

    if( result == MED_STATUS_SUCCESS ) {
        res = RES_OK;
    }
    else {
        TRACE_ERROR("RTMEDIA_Read pb: 0x%X\n\r", result);
        res = RES_ERROR;
    }

    return res;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0

DRESULT disk_write (
    BYTE drv,            /* Physical drive number (0..) */
    const BYTE *buff,    /* Data to be written */
    DWORD sector,        /* Sector number (LBA) */
    BYTE count            /* Sector count (1..255) */
)
{
    DRESULT res=RES_PARERR;
    unsigned int result;
    void * tmp;
    tmp = (void *) buff;
    RtMedia *pRtMedia;
    Media *pMedia;
    unsigned int addr, len;

    if (drv == DRV_NAND)
      pRtMedia = &(gRtNandMedias);
    else {
      TRACE_ERROR("disk_read: Unknown drive\n\r");
      return RES_ERROR;
    }

    pMedia = &(pRtMedia->media);

    if (pMedia->blockSize < SECTOR_SIZE_DEFAULT) {
        addr = sector * (SECTOR_SIZE_DEFAULT / pMedia->blockSize);
        len  = count * (SECTOR_SIZE_DEFAULT / pMedia->blockSize);
    }
    else {
        addr = sector;
        len  = count;
    }

    result = RTMEDIA_Write(pRtMedia,
                       addr,              // address
                       (void*)tmp,         // data
                       len,               // data size
                       NULL,
                       NULL);

    if( result == MED_STATUS_SUCCESS ) {

        res = RES_OK;
    }
    else {

        TRACE_ERROR("RTMEDIA_Write pb: 0x%X\n\r", result);
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
    RtMedia *pRtMedia;
    Media *pMedia;

    if (drv == DRV_NAND)
      pRtMedia = &(gRtNandMedias);
    else {
      TRACE_ERROR("disk_read: Unknown drive\n\r");
      return RES_ERROR;
    }

    pMedia = &(pRtMedia->media);

    switch (drv) {
        case DRV_SDRAM :
        switch (ctrl) {

            case GET_BLOCK_SIZE:
                *(WORD*)buff = 1;
                res = RES_OK;
                break;

            case GET_SECTOR_COUNT :   /* Get number of sectors on the disk (DWORD) */
                //*(DWORD*)buff = (DWORD)(medias[DRV_SDRAM].size);
                if (pMedia->blockSize < SECTOR_SIZE_DEFAULT)
                    *(DWORD*)buff = (DWORD)(pMedia->size /
                                            (SECTOR_SIZE_DEFAULT /
                                            pMedia->blockSize));
                else
                    *(DWORD*)buff = (DWORD)(pMedia->size);

                res = RES_OK;
                break;

            case GET_SECTOR_SIZE :   /* Get sectors on the disk (WORD) */
                //*(WORD*)buff = medias[DRV_SDRAM].blockSize;
                if (pMedia->blockSize < SECTOR_SIZE_DEFAULT)
                    *(WORD*)buff = SECTOR_SIZE_DEFAULT;
                else
                    *(WORD*)buff = pMedia->blockSize;
                res = RES_OK;
                break;

            case CTRL_SYNC :   /* Make sure that data has been written */
                res = RES_OK;
                break;

            default:
                res = RES_PARERR;
        }
        break;

        case DRV_MMC :
        switch (ctrl) {

            case GET_BLOCK_SIZE:
                *(WORD*)buff = 1;
                res = RES_OK;
                break;

            case GET_SECTOR_COUNT :   /* Get number of sectors on the disk (DWORD) */
                //*(DWORD*)buff = (DWORD)(medias[DRV_MMC].size);
                if (pMedia->blockSize < SECTOR_SIZE_DEFAULT)
                        *(DWORD*)buff = (DWORD)(pMedia->size /
                                                (SECTOR_SIZE_DEFAULT /
                                                pMedia->blockSize));
                else
                    *(DWORD*)buff = (DWORD)(pMedia->size);
                res = RES_OK;
                break;

            case GET_SECTOR_SIZE :   /* Get sectors on the disk (WORD) */
                //*(WORD*)buff = medias[DRV_MMC].blockSize;
                if (pMedia->blockSize < SECTOR_SIZE_DEFAULT)
                    *(WORD*)buff = SECTOR_SIZE_DEFAULT;
                else
                    *(WORD*)buff = pMedia->blockSize;
                res = RES_OK;
                break;

            case CTRL_SYNC :   /* Make sure that data has been written */
                res = RES_OK;
                break;

            default:
                res = RES_PARERR;
        }
        break;

        case DRV_NAND :
            switch (ctrl) {

                case GET_BLOCK_SIZE:
                    *(WORD*)buff = 1;
                    res = RES_OK;
                    break;

                case GET_SECTOR_COUNT :   /* Get number of sectors on the disk (DWORD) */
                    if (pMedia->blockSize < SECTOR_SIZE_DEFAULT)
                        *(DWORD*)buff = (DWORD)(pMedia->size /
                                                (SECTOR_SIZE_DEFAULT /
                                                pMedia->blockSize));
                    else
                        *(DWORD*)buff = (DWORD)(pMedia->size);
                    res = RES_OK;
                    break;

                case GET_SECTOR_SIZE :	 /* Get sectors on the disk (WORD) */
                    //*(WORD*)buff = medias[DRV_MMC].blockSize;
                    if (pMedia->blockSize < SECTOR_SIZE_DEFAULT)
                        *(WORD*)buff = SECTOR_SIZE_DEFAULT;
                    else
                        *(WORD*)buff = pMedia->blockSize;
                    res = RES_OK;
                    break;

                case CTRL_SYNC :   /* Make sure that data has been written */
                    MED_Flush(pMedia);
                    res = RES_OK;
                    break;

                default:
                    res = RES_PARERR;
        }

    }

   return res;
}

//------------------------------------------------------------------------------
/// Currnet time is returned with packed into a DWORD value.
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

    time =  ((2009-1980)<<25)
          | ( 9<<21)
          | (15<<16)
          | (17<<11)
          | (45<<5)
          | ((59/2)<<0);

    return time;
}



