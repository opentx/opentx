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
#include "mmc.h"

/*****************************************************************************/

esint8 if_initInterface(hwInterface* file, eint8* opts)
{
pDiskCtrlBlk_t pStatus;
  MmcStatusUpdate();
  pStatus = MmcDiskStatus();
  switch(pStatus->DiskStatus)
  {
  case DiskCommandPass:
    file->sectorCount = pStatus->BlockNumb;
    return(0);
  case DiskNotPresent:
    return(-1);
  case DiskNotReady:
    return(-2);
  }
  // Unknown error
  return(-3);
}
/*****************************************************************************/

esint8 if_readBuf(hwInterface* file,euint32 address,euint8* buf)
{
  if(MmcDiskIO(buf,address,1,DiskRead) == DiskCommandPass)
  {
    return(0);
  }
  return(-1);
}
/*****************************************************************************/

esint8 if_writeBuf(hwInterface* file,euint32 address,euint8* buf)
{
  if(MmcDiskIO(buf,address,1,DiskWrite) == DiskCommandPass)
  {
    return(0);
  }
  return(-1);
}
/*****************************************************************************/

esint8 if_setPos(hwInterface* file,euint32 address)
{
	return(0);
}
/*****************************************************************************/

