/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : partition.c                                                      *
* Description : These functions are partition specific. Searching FAT type    *
*               partitions and read/write functions to partitions.            *
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
#include "partition.h"
/*****************************************************************************/

/* ****************************************************************************  
 * void part_initPartition(Partition *part,Disc* refDisc)
 * Description: This function searches the 4 partitions for a FAT class partition
 * and marks the first one found as the active to be used partition.
*/
void part_initPartition(Partition *part,Disc* refDisc)
{
	eint16 c;
	
	part->disc=refDisc;
	part->activePartition=-1; /* No partition selected */
	part_setError(part,PART_NOERROR);
	for(c=3;c>=0;c--){
		if(part_isFatPart(part->disc->partitions[c].type))
			part->activePartition=c;
	} 
}
/*****************************************************************************/ 


/* ****************************************************************************  
 * eint16 part_isFatPart(euint8 type)
 * Description: This functions checks if a partitiontype (eint8) is of the FAT
 * type in the broadest sense. I
 * Return value: If it is FAT, returns 1, otherwise 0.
*/
eint16 part_isFatPart(euint8 type)
{
	if(type == PT_FAT12  ||
	   type == PT_FAT16A ||
	   type == PT_FAT16  ||
	   type == PT_FAT32  ||
	   type == PT_FAT32A ||
	   type == PT_FAT16B   )
	{
		return(1);
	}
	return(0);
}
/*****************************************************************************/ 

esint8 part_readBuf(Partition *part, euint32 address, euint8* buf)
{
	return(if_readBuf(part->disc->ioman->iface,part_getRealLBA(part,address), buf));
}

/* ****************************************************************************  
 * eint16 part_writeBuf(Partition *part,euint32 address,euint8* buf)
 * Description: This function writes 512 bytes, from buf. It's offset is address
 * sectors from the beginning of the partition.
 * Return value: It returns whatever the hardware function returns. (-1=error)
*/
eint16 part_writeBuf(Partition *part,euint32 address,euint8* buf)
{
	/*DBG((TXT("part_writeBuf :: %li\n"),address));*/
	return(if_writeBuf(part->disc->ioman->iface,part_getRealLBA(part,address),buf));
}
/*****************************************************************************/ 


/* ****************************************************************************  
 * euint32 part_getRealLBA(Partition *part,euint32 address)
 * Description: This function calculates what the partition offset for
 * a partition is + the address.
 * Return value: Sector address.
*/
euint32 part_getRealLBA(Partition *part,euint32 address)
{
	return(part->disc->partitions[part->activePartition].LBA_begin+address);
}
/*****************************************************************************/ 

/* ****************************************************************************  
 * euint8* part_getSect(Partition *part, euint32 address, euint8 mode)
 * Description: This function calls ioman_getSector, but recalculates the sector
 * address to be partition relative.
 * Return value: Whatever getSector returns. (pointer or 0)
*/
euint8* part_getSect(Partition *part, euint32 address, euint8 mode)
{
	return(ioman_getSector(part->disc->ioman,part_getRealLBA(part,address),mode));
}

/* ****************************************************************************  
 * esint8 part_relSect(Partition *part, euint8* buf)
 * Description: This function calls ioman_releaseSector.
 * Return value: Whatever releaseSector returns.
*/
esint8 part_relSect(Partition *part, euint8* buf)
{
	return(ioman_releaseSector(part->disc->ioman,buf));
}

esint8 part_flushPart(Partition *part,euint32 addr_l, euint32 addr_h)
{
	return( 
		ioman_flushRange(part->disc->ioman,part_getRealLBA(part,addr_l),part_getRealLBA(part,addr_h)) 
	);	
}

esint8 part_directSectorRead(Partition *part,euint32 address, euint8* buf)
{
	return(
		ioman_directSectorRead(part->disc->ioman,part_getRealLBA(part,address),buf)
	);
}

esint8 part_directSectorWrite(Partition *part,euint32 address, euint8* buf)
{
	return(
		ioman_directSectorWrite(part->disc->ioman,part_getRealLBA(part,address),buf)
	);
}


