/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : sd.c                                                             *
* Revision : Initial developement                                             *
* Description : This file contains the functions needed to use efs for        *
*               accessing files on an SD-card.                                *
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
#include "interfaces/sd.h"
/*****************************************************************************/

esint8 sd_Init(hwInterface *iface)
{
	esint16 i;
	euint8 resp;
	
	/* Try to send reset command up to 100 times */
	i=100;
	do{
		sd_Command(iface,0, 0, 0);
		resp=sd_Resp8b(iface);
	}
	while(resp!=1 && i--);
	
	if(resp!=1){
		if(resp==0xff){
			return(-1);
		}
		else{
			sd_Resp8bError(iface,resp);
			return(-2);
		}
	}

	/* Wait till card is ready initialising (returns 0 on CMD1) */
	/* Try up to 32000 times. */
	i=32000;
	do{
		sd_Command(iface,1, 0, 0);
		
		resp=sd_Resp8b(iface);
		if(resp!=0)
			sd_Resp8bError(iface,resp);
	}
	while(resp==1 && i--);
	
	if(resp!=0){
		sd_Resp8bError(iface,resp);
		return(-3);
	}
	
	return(0);
}
/*****************************************************************************/

void sd_Command(hwInterface *iface,euint8 cmd, euint16 paramx, euint16 paramy)
{
	if_spiSend(iface,0xff);

	if_spiSend(iface,0x40 | cmd);
	if_spiSend(iface,(euint8) (paramx >> 8)); /* MSB of parameter x */
	if_spiSend(iface,(euint8) (paramx)); /* LSB of parameter x */
	if_spiSend(iface,(euint8) (paramy >> 8)); /* MSB of parameter y */
	if_spiSend(iface,(euint8) (paramy)); /* LSB of parameter y */

	if_spiSend(iface,0x95); /* Checksum (should be only valid for first command (0) */

	if_spiSend(iface,0xff); /* eat empty command - response */
}
/*****************************************************************************/

euint8 sd_Resp8b(hwInterface *iface)
{
	euint8 i;
	euint8 resp;
	
	/* Respone will come after 1 - 8 pings */
	for(i=0;i<8;i++){
		resp = if_spiSend(iface,0xff);
		if(resp != 0xff)
			return(resp);
	}
		
	return(resp);
}
/*****************************************************************************/

euint16 sd_Resp16b(hwInterface *iface)
{
	euint16 resp;
	
	resp = ( sd_Resp8b(iface) << 8 ) & 0xff00;
	resp |= if_spiSend(iface,0xff);
	
	return(resp);
}
/*****************************************************************************/

void sd_Resp8bError(hwInterface *iface,euint8 value)
{
	switch(value)
	{
		case 0x40:
			DBG((TXT("Argument out of bounds.\n")));
			break;
		case 0x20:
			DBG((TXT("Address out of bounds.\n")));
			break;
		case 0x10:
			DBG((TXT("Error during erase sequence.\n")));
			break;
		case 0x08:
			DBG((TXT("CRC failed.\n")));
			break;
		case 0x04:
			DBG((TXT("Illegal command.\n")));
			break;
		case 0x02:
			DBG((TXT("Erase reset (see SanDisk docs p5-13).\n")));
			break;
		case 0x01:
			DBG((TXT("Card is initialising.\n")));
			break;
		default:
			DBG((TXT("Unknown error 0x%x (see SanDisk docs p5-13).\n"),value));
			break;
	}
}
/*****************************************************************************/

esint8 sd_State(hwInterface *iface)
{
	eint16 value;
	
	sd_Command(iface,13, 0, 0);
	value=sd_Resp16b(iface);

	switch(value)
	{
		case 0x000:
			return(1);
			break;
		case 0x0001:
			DBG((TXT("Card is Locked.\n")));
			break;
		case 0x0002:
			DBG((TXT("WP Erase Skip, Lock/Unlock Cmd Failed.\n")));
			break;
		case 0x0004:
			DBG((TXT("General / Unknown error -- card broken?.\n")));
			break;
		case 0x0008:
			DBG((TXT("Internal card controller error.\n")));
			break;
		case 0x0010:
			DBG((TXT("Card internal ECC was applied, but failed to correct the data.\n")));
			break;
		case 0x0020:
			DBG((TXT("Write protect violation.\n")));
			break;
		case 0x0040:
			DBG((TXT("An invalid selection, sectors for erase.\n")));
			break;
		case 0x0080:
			DBG((TXT("Out of Range, CSD_Overwrite.\n")));
			break;
		default:
			if(value>0x00FF)
				sd_Resp8bError(iface,(euint8) (value>>8));
			else
				DBG((TXT("Unknown error: 0x%x (see SanDisk docs p5-14).\n"),value));
			break;
	}
	return(-1);
}
/*****************************************************************************/

/* ****************************************************************************
 * WAIT ?? -- FIXME
 * CMDWRITE
 * WAIT
 * CARD RESP
 * WAIT
 * DATA BLOCK OUT
 *      START BLOCK
 *      DATA
 *      CHKS (2B)
 * BUSY...
 */

esint8 sd_writeSector(hwInterface *iface,euint32 address, euint8* buf)
{
	euint32 place;
	euint16 i;
	euint16 t=0;
	
	/*DBG((TXT("Trying to write %u to sector %u.\n"),(void *)&buf,address));*/
	place=512*address;
	sd_Command(iface,CMDWRITE, (euint16) (place >> 16), (euint16) place);

	sd_Resp8b(iface); /* Card response */

	if_spiSend(iface,0xfe); /* Start block */
	for(i=0;i<512;i++) 
		if_spiSend(iface,buf[i]); /* Send data */
	if_spiSend(iface,0xff); /* Checksum part 1 */
	if_spiSend(iface,0xff); /* Checksum part 2 */

	if_spiSend(iface,0xff);

	while(if_spiSend(iface,0xff)!=0xff){
		t++;
		/* Removed NOP */
	}
	/*DBG((TXT("Nopp'ed %u times.\n"),t));*/

	return(0);
}
/*****************************************************************************/

/* ****************************************************************************
 * WAIT ?? -- FIXME
 * CMDCMD
 * WAIT
 * CARD RESP
 * WAIT
 * DATA BLOCK IN
 * 		START BLOCK
 * 		DATA
 * 		CHKS (2B)
 */

esint8 sd_readSector(hwInterface *iface,euint32 address, euint8* buf, euint16 len)
{
	euint8 cardresp;
	euint8 firstblock;
	euint8 c;
	euint16 fb_timeout=0xffff;
	euint32 i;
	euint32 place;

	/*DBG((TXT("sd_readSector::Trying to read sector %u and store it at %p.\n"),address,&buf[0]));*/
	place=512*address;
	sd_Command(iface,CMDREAD, (euint16) (place >> 16), (euint16) place);
	
	cardresp=sd_Resp8b(iface); /* Card response */ 

	/* Wait for startblock */
	do
		firstblock=sd_Resp8b(iface); 
	while(firstblock==0xff && fb_timeout--);

	if(cardresp!=0x00 || firstblock!=0xfe){
		sd_Resp8bError(iface,firstblock);
		return(-1);
	}
	
	for(i=0;i<512;i++){
		c = if_spiSend(iface,0xff);
		if(i<len)
			buf[i] = c;
	}

	/* Checksum (2 byte) - ignore for now */
	if_spiSend(iface,0xff);
	if_spiSend(iface,0xff);

	return(0);
}
/*****************************************************************************/

/* ****************************************************************************
 calculates size of card from CSD 
 (extension by Martin Thomas, inspired by code from Holger Klabunde)
 */
esint8 sd_getDriveSize(hwInterface *iface, euint32* drive_size )
{
	euint8 cardresp, i, by;
	euint8 iob[16];
	euint16 c_size, c_size_mult, read_bl_len;
	
	sd_Command(iface, CMDREADCSD, 0, 0);
	
	do {
		cardresp = sd_Resp8b(iface);
	} while ( cardresp != 0xFE );

	DBG((TXT("CSD:")));
	for( i=0; i<16; i++) {
		iob[i] = sd_Resp8b(iface);
		DBG((TXT(" %02x"), iob[i]));
	}
	DBG((TXT("\n")));

	if_spiSend(iface,0xff);
	if_spiSend(iface,0xff);
	
	c_size = iob[6] & 0x03; // bits 1..0
	c_size <<= 10;
	c_size += (euint16)iob[7]<<2;
	c_size += iob[8]>>6;

	by= iob[5] & 0x0F;
	read_bl_len = 1;
	read_bl_len <<= by;

	by=iob[9] & 0x03;
	by <<= 1;
	by += iob[10] >> 7;
	
	c_size_mult = 1;
	c_size_mult <<= (2+by);
	
	*drive_size = (euint32)(c_size+1) * (euint32)c_size_mult * (euint32)read_bl_len;
	
	return 0;
}
