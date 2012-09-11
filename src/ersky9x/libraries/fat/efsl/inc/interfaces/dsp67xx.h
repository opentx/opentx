/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename :  dsp67xx.h                                                       *
* Revision :  Initial developement                                            *
* Description : Headerfile for dsp67xx.h                                      *
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

#ifndef __DSP67XX_H_ 
#define __DSP67XX_H_ 

#include "config.h"
#include "mcbsp/mcbsp.h"

/*************************************************************\
              hwInterface
               ----------
* FILE* 	imagefile		File emulation of hw interface.
* long		sectorCount		Number of sectors on the file.
\*************************************************************/
struct hwInterface{
	/*FILE 	*imageFile;*/
	long  	sectorCount;
	BspPort *port;
};
typedef struct hwInterface hwInterface;

short if_initInterface(hwInterface* file,char* opts);
short if_readBuf(hwInterface* file,euint32 address,unsigned char* buf);
short if_writeBuf(hwInterface* file,unsigned long address,unsigned char* buf);
short if_setPos(hwInterface* file,unsigned long address);

signed char if_spiInit(hwInterface *iface);
unsigned char if_spiSend(hwInterface *iface,euint8 outgoing);

#endif
