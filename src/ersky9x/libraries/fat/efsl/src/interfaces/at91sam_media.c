/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : at91sam_ramdisk.c                                                *
* Description : This file contains the functions needed to use efs for        *
*               accessing files on an SD-card connected to an At91SAM.        *
*                                                                             *
* This program is free software; you can redistribute it and/or               *
* modify it under the terms of the GNU General Public License                 *
* as published by the Free Software Foundation; version 2                     *
* of the License.                                                             *
                                                                              *
* This program is distributed in the hope that it will be useful,             *
* but WITHOUT ANY WARRANTY; without even the implied warranty of              *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
* GNU General Public License for more details.                                *
*                                                                             *
* As a special exception, if other files instantiate templates or             *
* use macros or inline functions from this file, or you compile this          *
* file and link it with other works to produce a work based on this file,     *
* this file does not by itself cause the resulting work to be covered         *
* by the GNU General Public License. However the source code for this         *
* file must still be made available in accordance with section (3) of         *
* the GNU General Public License.                                             *
*                                                                             *
* This exception does not invalidate any other reasons why a work based       *
* on this file might be covered by the GNU General Public License.            *
*                                                                             *
*                          AT91SAM7S ARM7 Interface (c)2007 Stanimir Bonev    *
\*****************************************************************************/

/*****************************************************************************/

#include <board.h>
#include <utility/trace.h>
#include <utility/assert.h>
#include <memories/Media.h>
#include <string.h>
#include <stdio.h>

#include "at91sam_media.h"

#define SECTOR_SIZE_DISK  512

//------------------------------------------------------------------------------
//         Internal variables
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//         Internal functions
//------------------------------------------------------------------------------

static void translate_media(Media *pMedia,
                            euint32 address,
                            euint32 *pAddr, euint32 *pLen)
{
    eint32 addr, len;

    if (pMedia->blockSize >= SECTOR_SIZE_DISK) {
        addr = address / (pMedia->blockSize / SECTOR_SIZE_DISK);
        len  = 1;
    }
    else {
        addr = address * (SECTOR_SIZE_DISK / pMedia->blockSize);
        len  = SECTOR_SIZE_DISK / pMedia->blockSize;
    }
    if (pAddr) *pAddr = addr;
    if (pLen)  *pLen  = len;
}

/*****************************************************************************/

esint8 if_initInterface(hwInterface* file, eint8* opts)
{
    unsigned int value;
    
    file->pMedia = (Media *)opts;

    if(file->pMedia->blockSize > SECTOR_SIZE_DISK) {
        TRACE_FATAL("EFSL sector size < %d!\n\r", SECTOR_SIZE_DISK);
        return -1;
    }

    if (file->pMedia->blockSize >= SECTOR_SIZE_DISK)
        file->sectorCount = file->pMedia->size
                              * (file->pMedia->blockSize / SECTOR_SIZE_DISK);
    else
        file->sectorCount = file->pMedia->size
                              / (SECTOR_SIZE_DISK / file->pMedia->blockSize);

    return(0);
}
/*****************************************************************************/

esint8 if_readBuf(hwInterface* file,euint32 address,euint8* buf)
{
    unsigned int result;
    euint32 addr, len;

    translate_media(file->pMedia, address, &addr, &len);
    result = MED_Read(file->pMedia,
                      addr,
                      (void*)buf,
                      len,
                      NULL,
                      NULL);

    if( result != MED_STATUS_SUCCESS ) {
        TRACE_INFO("MED_Read pb: 0x%X\n\r", result);
    }
    return(0);
}
/*****************************************************************************/

esint8 if_writeBuf(hwInterface* file,euint32 address,euint8* buf)
{
    unsigned int result;
    euint32 addr, len;

    translate_media(file->pMedia, address, &addr, &len);
    result = MED_Write(file->pMedia,
                       addr,
                       (void*)buf,
                       len,
                       NULL,
                       NULL);

    if( result != MED_STATUS_SUCCESS ) {
        TRACE_INFO("MED_WRITE pb: 0x%X\n\r", result);
    }
    return(0);
}
/*****************************************************************************/

esint8 if_setPos(hwInterface* file,euint32 address)
{
	return(0);
}
/*****************************************************************************/

