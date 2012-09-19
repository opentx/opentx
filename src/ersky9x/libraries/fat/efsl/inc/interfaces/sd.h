/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          ---------------------------------------------------------          *
*                                                                             *
* Filename :  sd.h                                                            *
* Revision :  Initial developement                                            *
* Description : Headerfile for sd.c                                           *
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
*                                                    (c)2006 Lennart Yseboodt *
*                                                    (c)2006 Michael De Nil   *
\*****************************************************************************/

#ifndef __SD_H_ 
#define __SD_H_ 

#include "config.h"
#include "types.h"
#include "../debug.h"

#ifdef HW_ENDPOINT_ATMEGA128_SD
	#include "interfaces/atmega128.h"
#endif
#ifdef HW_ENDPOINT_DSP_TI6713_SD
	#include "interfaces/dsp67xx.h"
#endif
#ifdef HW_ENDPOINT_LPC2000_SD
	#include "interfaces/lpc2000_spi.h"
#endif

#define	CMDREAD		17
#define	CMDWRITE	24
#define	CMDREADCSD       9

esint8  sd_Init(hwInterface *iface);
void sd_Command(hwInterface *iface,euint8 cmd, euint16 paramx, euint16 paramy);
euint8 sd_Resp8b(hwInterface *iface);
void sd_Resp8bError(hwInterface *iface,euint8 value);
euint16 sd_Resp16b(hwInterface *iface);
esint8 sd_State(hwInterface *iface);

esint8 sd_readSector(hwInterface *iface,euint32 address,euint8* buf, euint16 len);
esint8 sd_writeSector(hwInterface *iface,euint32 address, euint8* buf);
esint8 sd_getDriveSize(hwInterface *iface, euint32* drive_size );

#endif
