/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : dsp67xx.c                                                        *
* Revision : Initial developement                                             *
* Description : This file contains the functions needed to use efs for        *
*               accessing files on an SD-card connected to a TI 67            *
*               series DSP.                                                   *
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

/*****************************************************************************/
#include "interfaces/dsp67xx.h"
#include "interfaces/sd.h"
#include <stdio.h>
/*****************************************************************************/

short if_initInterface(hwInterface* file, char* opts)
{
    file->sectorCount=0;
    return(if_spiInit(file));
}
/*****************************************************************************/ 

short if_writeBuf(hwInterface* file,unsigned long address,unsigned char* buf)
{
	return(0);
}
/*****************************************************************************/ 

short if_setPos(hwInterface* file,unsigned long address)
{
	return(0);
}
/*****************************************************************************/ 

signed char if_spiInit(hwInterface *iface)
{
	return(sd_Init(iface));
}
/*****************************************************************************/

unsigned char if_spiSend(hwInterface *iface, euint8 outgoing)
{
	unsigned char r;

/*	while((*(unsigned volatile long*)McBSP0_SPCR & 0x20000)==0);
	*(unsigned volatile char*)McBSP0_DXR=outgoing;
	while(((*(unsigned volatile long*)McBSP0_SPCR & 0x2)==0));
	r=*(unsigned volatile char*)McBSP0_DRR; */
	
	while(!MCBSP_xrdy(iface->port->hBsp));
	MCBSP_write(iface->port->hBsp,outgoing);
	while(!MCBSP_rrdy(iface->port->hBsp));
	r=MCBSP_read(iface->port->hBsp);
	
	return(r);
}
/*****************************************************************************/

short if_readBuf(hwInterface* file,euint32 address,unsigned char* buf)
{
	short r;
	r=sd_readSector(file,address,buf,512);
	if(r!=0)printf("ERROR READING SECTOR %i\n",address);
	return(r);
}
/*****************************************************************************/

