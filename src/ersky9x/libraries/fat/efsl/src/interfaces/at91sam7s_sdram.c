/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : at91sam7s_spi.c                                                  *
* Description : This file contains the functions needed to use efs for        *
*               accessing files on an SD-card connected to an At91SAM7S.      *
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
#include "at91sam7s_spi.h"
#include <board.h>
#include <board_memories.h>
#include <utility/trace.h>
#include <utility/assert.h>
#include <memories/Media.h>
#if defined(BOARD_SDRAM_SIZE)
#include <memories/MEDFlash.h>
#include <memories/MEDSdram.h>
#include <memories/MEDDdram.h>
#elif defined(BOARD_PSRAM_SIZE)
#include <memories/MEDRamDisk.h>
#endif
#include <string.h>
#include <stdio.h>


/// Maximum number of LUNs which can be defined.
#define MAX_LUNS             1

#define SECTOR_SIZE_SDRAM  512

#if defined(sdram) || defined(ddram) || defined(psram)
// The code is launch in external ram
#define CODE_SIZE           30*1024
#else
// Entire sdram is reserved for the FAT
#define CODE_SIZE           0
#endif

//------------------------------------------------------------------------------
//         Internal variables
//------------------------------------------------------------------------------

/// Available medias.
extern Media medias[MAX_LUNS];
extern unsigned int numMedias;

//------------------------------------------------------------------------------
//         Internal functions
//------------------------------------------------------------------------------

static void translate_media(Media *pMedia,
                            euint32 address,
                            euint32 *pAddr, euint32 *pLen)
{
    eint32 addr, len;

    if (pMedia->blockSize >= SECTOR_SIZE_SDRAM) {
        addr = address / (pMedia->blockSize / SECTOR_SIZE_SDRAM);
        len  = 1;
    }
    else {
        addr = address * (SECTOR_SIZE_SDRAM / pMedia->blockSize);
        len  = SECTOR_SIZE_SDRAM / pMedia->blockSize;
    }
    if (pAddr) *pAddr = addr;
    if (pLen)  *pLen  = len;
}

/*****************************************************************************/

esint8 if_initInterface(hwInterface* file, eint8* opts)
{
    unsigned int value;

#if defined(AT91C_BASE_DDR2C)
    MEDDdram_Initialize(&(medias[numMedias]),
                        SECTOR_SIZE_SDRAM, 
                        (unsigned int) (AT91C_DDR2 + CODE_SIZE)/SECTOR_SIZE_SDRAM,
                        BOARD_DDRAM_SIZE/SECTOR_SIZE_SDRAM );
    file->sectorCount = (BOARD_DDRAM_SIZE)/SECTOR_SIZE_SDRAM;
#elif defined(BOARD_SDRAM_SIZE)
    MEDSdram_Initialize(&(medias[numMedias]),
                        SECTOR_SIZE_SDRAM,
                        (unsigned int) (AT91C_EBI_SDRAM + CODE_SIZE)/SECTOR_SIZE_SDRAM,
                        (BOARD_SDRAM_SIZE-CODE_SIZE)/SECTOR_SIZE_SDRAM);
    file->sectorCount = (BOARD_SDRAM_SIZE-CODE_SIZE)/SECTOR_SIZE_SDRAM;
#elif defined (BOARD_PSRAM_SIZE)
    BOARD_ConfigurePsram();
    MEDRamDisk_Initialize(&(medias[numMedias]),
                          SECTOR_SIZE_SDRAM,
                          (unsigned int)(BOARD_EBI_PSRAM + CODE_SIZE) / SECTOR_SIZE_SDRAM,
                          (BOARD_PSRAM_SIZE-CODE_SIZE) / SECTOR_SIZE_SDRAM);
    file->sectorCount = (BOARD_PSRAM_SIZE-CODE_SIZE)/SECTOR_SIZE_SDRAM;
#else
    #error NO SDRAM or DDR or PSRAM DRIVE
#endif

    return(0);
}
/*****************************************************************************/

esint8 if_readBuf(hwInterface* file,euint32 address,euint8* buf)
{
    unsigned int result;
    euint32 addr, len;

    translate_media(&medias[numMedias], address, &addr, &len);
    result = MED_Read(&medias[numMedias],
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

    translate_media(&medias[numMedias], address, &addr, &len);
    result = MED_Write(&medias[numMedias],
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

