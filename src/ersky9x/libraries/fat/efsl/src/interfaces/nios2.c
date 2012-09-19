/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : nios2.c                                                          *
* Description : This file contains the functions needed to use efs for        *
*               accessing files on an SD-card connected to an Altera FPGA     *
*               running on NIOS2.                                             *
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
*                                                    (c)2005 Marcio Troccoli  *
\*****************************************************************************/

/*****************************************************************************/
#include "interfaces/nios2.h"
#include "interfaces/sd.h"

#include "altera_avalon_spi_regs.h"
#include "altera_avalon_spi.h"
#include "system.h"
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
	
	printf("\n spi ini");
}
/*****************************************************************************/

euint8 if_spiSend(hwInterface *iface, euint8 outgoing)
{
	euint8 incoming=0;
	alt_u8  SD_Data=0,status;
  
	IOWR_ALTERA_AVALON_SPI_SLAVE_SEL(SD_SPI_BASE, 1 );
	/* Set the SSO bit (force chipselect) */
	IOWR_ALTERA_AVALON_SPI_CONTROL(SD_SPI_BASE, 0x400);

	do{
    	    status = IORD_ALTERA_AVALON_SPI_STATUS(SD_SPI_BASE);//get status
	}while (((status & ALTERA_AVALON_SPI_STATUS_TRDY_MSK) == 0 ) &&
            (status & ALTERA_AVALON_SPI_STATUS_RRDY_MSK) == 0);
	/* wait till transmit and ready ok */

	IOWR_ALTERA_AVALON_SPI_TXDATA(SD_SPI_BASE, outgoing);
    
	/* Wait until the interface has finished transmitting */
	do{status = IORD_ALTERA_AVALON_SPI_STATUS(SD_SPI_BASE);}
	while ((status & ALTERA_AVALON_SPI_STATUS_TMT_MSK) == 0);

	/* reciver ready */
	if (((status & 0x80) != 0) ){
    	    SD_Data = IORD_ALTERA_AVALON_SPI_RXDATA(SD_SPI_BASE);
	}
	else{
		printf("\n no recive after transmit");
	}
 
	IOWR_ALTERA_AVALON_SPI_SLAVE_SEL(SD_SPI_BASE, 1);
	IOWR_ALTERA_AVALON_SPI_CONTROL(SD_SPI_BASE, 0);
 
	if( (status & 0x100) !=0)
    	printf("\n error in spi error in spi");
  
	return (SD_Data);
}
/*****************************************************************************/

