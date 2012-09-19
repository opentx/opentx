/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename :  extract.c                                                       *
* Description : This file contains functions to copy structures that get      *
*               corrupted when using direct memory copy                       *
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
#include "extract.h"
/*****************************************************************************/

/*****************************************************************************/
#ifdef BYTE_ALIGNMENT
/*****************************************************************************/

euint16 ex_getb16(euint8* buf,euint32 offset)
{
	return(ltb_end16(*((euint16*)(buf+offset))));
}
/*****************************************************************************/

euint32 ex_getb32(euint8* buf,euint32 offset)
{
	return(ltb_end32(*((euint32*)(buf+offset))));
}
/*****************************************************************************/

void ex_setb16(euint8* buf,euint32 offset,euint16 data)
{
	*((euint16*)(buf+offset)) = btl_end16(data);
}
/*****************************************************************************/

void ex_setb32(euint8* buf,euint32 offset,euint32 data)
{
	*((euint32*)(buf+offset)) = btl_end32(data);
}
/*****************************************************************************/

void ex_getPartitionField(euint8* buf,PartitionField* pf, euint32 offset)
{
	*pf=*((PartitionField*)(buf+offset));
}
/*****************************************************************************/

void ex_setPartitionField(euint8* buf,PartitionField* pf, euint32 offset)
{

}
/*****************************************************************************/




/*****************************************************************************/
/*****************************************************************************/
#else
/*****************************************************************************/
/*****************************************************************************/



euint16 ex_getb16(euint8* buf,euint32 offset)
{
	return(ltb_end16(((*(buf+offset+1))<<8) + ((*(buf+offset+0))<<0)));
}
/*****************************************************************************/

euint32 ex_getb32(euint8* buf,euint32 offset)
{
	return(ltb_end32(((euint32)buf[offset+3]<<24)+
	      ((euint32)buf[offset+2]<<16)+
	      ((euint32)buf[offset+1]<<8)+
	      ((euint32)buf[offset+0]<<0)));
}
/*****************************************************************************/

void ex_setb16(euint8* buf,euint32 offset,euint16 data)
{
#ifdef BIG_ENDIAN
	*(buf+offset+1) = data>>0;
	*(buf+offset+0) = data>>8;
#else
	*(buf+offset+0) = data>>0;
	*(buf+offset+1) = data>>8;
#endif
}
/*****************************************************************************/

void ex_setb32(euint8* buf,euint32 offset,euint32 data)
{
#ifdef BIG_ENDIAN
	*(buf+offset+3) = data>> 0;
	*(buf+offset+2) = data>> 8;
	*(buf+offset+1) = data>>16;
	*(buf+offset+0) = data>>24;
#else
	*(buf+offset+0) = data>> 0;
	*(buf+offset+1) = data>> 8;
	*(buf+offset+2) = data>>16;
	*(buf+offset+3) = data>>24;
#endif
}
/*****************************************************************************/

void ex_getPartitionField(euint8* buf,PartitionField* pf, euint32 offset)
{
	pf->bootFlag       = *(buf + offset);
	pf->CHS_begin[0]   = *(buf + offset + 1);
	pf->CHS_begin[1]   = *(buf + offset + 2);
	pf->CHS_begin[2]   = *(buf + offset + 3);
	pf->type           = *(buf + offset + 4);
	pf->CHS_end[0]     = *(buf + offset + 5);
	pf->CHS_end[1]     = *(buf + offset + 6);
	pf->CHS_end[2]     = *(buf + offset + 7);
	pf->LBA_begin      = ex_getb32(buf + offset,8);
	pf->numSectors     = ex_getb32(buf + offset,12);
}
/*****************************************************************************/

void ex_setPartitionField(euint8* buf,PartitionField* pf, euint32 offset)
{

}
/*****************************************************************************/



/*****************************************************************************/
#endif
/*****************************************************************************/

