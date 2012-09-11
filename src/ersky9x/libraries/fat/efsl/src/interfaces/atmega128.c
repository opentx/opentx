/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : atmega128.c                                                      *
* Description : This file contains the functions needed to use efs for        *
*               accessing files on an SD-card connected to an ATMega128.      *
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
#include "interfaces/atmega128.h"
#include "interfaces/sd.h"
/*****************************************************************************/

esint8 if_initInterface(hwInterface* file, eint8* opts)
{
	if_spiInit(file);
	if(sd_Init(file)<0)	{
		DBG((TXT("Card failed to init, breaking up...\n")));
		return(-1);
	}
	if(sd_State(file)<0){
		DBG((TXT("Card didn't return the ready state, breaking up...\n")));
		return(-2);
	}
	file->sectorCount=4; /* FIXME ASAP!! */
	DBG((TXT("Init done...\n")));
	return(0);
}
/*****************************************************************************/ 

esint8 if_readBuf(hwInterface* file,euint32 address,euint8* buf)
{
	return(sd_readSector(file,address,buf,512));
}
/*****************************************************************************/

esint8 if_writeBuf(hwInterface* file,euint32 address,euint8* buf)
{
	return(sd_writeSector(file,address, buf));
}
/*****************************************************************************/ 

esint8 if_setPos(hwInterface* file,euint32 address)
{
	return(0);
}
/*****************************************************************************/ 

void if_spiInit(hwInterface *iface)
{
	euint8 i;
	
	/* Unselect card */
	PORTB |= 0x01;
	
	/* Set as master, clock and chip select output */
	DDR_SPI = (1<<DD_MOSI) | (1<<DD_SCK) | 1;

	/* Enable SPI, master, set clock rate to fck/2 */
	SPCR = (1<<SPE) | (1<<MSTR); /* fsck / 4 */ 
	SPSR = 1; /* fsck / 2 */

	/* Send 10 spi commands with card not selected */
	for(i=0;i<10;i++)
		if_spiSend(iface,0xff);

	/* Select card */
	PORTB &= 0xFE;
}
/*****************************************************************************/

euint8 if_spiSend(hwInterface *iface, euint8 outgoing)
{
	euint8 incoming=0;
	
	PORTB &= 0xFE;
	SPDR = outgoing;
	while(!(SPSR & (1<<SPIF)));
	incoming = SPDR;
	PORTB |= 0x01;

	return(incoming);
}
/*****************************************************************************/

