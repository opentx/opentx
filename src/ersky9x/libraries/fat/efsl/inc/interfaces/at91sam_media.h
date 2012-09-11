/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename :  at91sam_ramdisk.h                                               *
* Description : Headerfile for at91sam xxx.c                                  *
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

#ifndef __AT91SAM_RAMDISK_H_ 
#define __AT91SAM_RAMDISK_H_ 

#ifndef FALSE
#define FALSE	0x00
#define TRUE	0x01
#endif

#include "../debug.h"
#include "config.h"

#include "Media.h"

/*************************************************************\
              hwInterface
               ----------
* Media* 	pMedia		Pointer to emulation of Media interface.
* long		sectorCount		Number of sectors on the file.
\*************************************************************/

struct  hwInterface{
	Media*      pMedia;
	eint32  	sectorCount;
};

typedef struct hwInterface hwInterface;

esint8 if_initInterface(hwInterface* file,eint8* opts);
esint8 if_readBuf(hwInterface* file,euint32 address,euint8* buf);
esint8 if_writeBuf(hwInterface* file,euint32 address,euint8* buf);
esint8 if_setPos(hwInterface* file,euint32 address);

#endif // __AT91SAM_RAMDISK_H_
