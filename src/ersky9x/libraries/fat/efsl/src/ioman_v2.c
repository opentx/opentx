/*****************************************************************************\
*                     libfat - General purpose FAT library                    *
*                      ----------------------------------                     *
*                                                                             *
* Filename : ioman.c                                                          *
* Description : The IO Manager receives all requests for sectors in a central *
*               allowing it to make smart decision regarding caching.         *
*               The IOMAN_NUMBUFFER parameter determines how many sectors     *
*               ioman can cache. ioman also supports overallocating and       *
*               backtracking sectors                                          *
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
#include "ioman_v2.h"
/*****************************************************************************/

esint8 ioman_init(IOManager *ioman, hwInterface *iface, euint8* bufferarea)
{
	ioman->iface=iface;
	
	ioman->bufptr = ioman_getBuffer(ioman,bufferarea);
	ioman->numbuf = IOMAN_NUMBUFFER;
	ioman->numit  = IOMAN_NUMITERATIONS;
	
	ioman_reset(ioman);
	return(0);
}

void ioman_reset(IOManager *ioman)
{
	euint16 nb,ni;
	
	memClr(ioman->sector,sizeof(euint32)*ioman->numbuf);
	memClr(ioman->status,sizeof(euint8) *ioman->numbuf);
	memClr(ioman->usage ,sizeof(euint8) *ioman->numbuf);
	memClr(ioman->itptr ,sizeof(euint8) *ioman->numbuf);
	
	for(nb=0;nb<ioman->numbuf;nb++){
		for(ni=0;ni<ioman->numit;ni++){
			ioman->stack[nb][ni].sector=0;
			ioman->stack[nb][ni].status=0;
			ioman->stack[nb][ni].usage =0;
		}
	}
}

euint8* ioman_getBuffer(IOManager *ioman,euint8* bufferarea)
{
	static euint8 buf[512 * IOMAN_NUMBUFFER];
	return(buf);
}

void ioman_setAttr(IOManager *ioman,euint16 bufplace,euint8 attribute,euint8 val)
{
	if(bufplace>=ioman->numbuf)return; /* Out of bounds */
	
	if(val){
		ioman->status[bufplace]|=1<<attribute;
	}else{
		ioman->status[bufplace]&=~(1<<attribute);
	}
}

euint8 ioman_getAttr(IOManager *ioman,euint16 bufplace,euint8 attribute)
{
	if(bufplace>=ioman->numbuf)
		return(0xFF); /* Out of bounds */

	return(ioman->status[bufplace]&(1<<attribute));
}

euint8 ioman_getUseCnt(IOManager *ioman,euint16 bufplace)
{
	if(bufplace>=ioman->numbuf)return(0x00);
	return(ioman->usage[bufplace]);
}

void ioman_incUseCnt(IOManager *ioman,euint16 bufplace)
{
	if(bufplace>=ioman->numbuf)return;
	if(ioman->usage[bufplace]==0xFF)return;
	else ioman->usage[bufplace]++;
}

void ioman_decUseCnt(IOManager *ioman,euint16 bufplace)
{
	if(bufplace>=ioman->numbuf)return;
	if(ioman->usage[bufplace]==0x0)return;
	else ioman->usage[bufplace]--;
}

void ioman_resetUseCnt(IOManager *ioman,euint16 bufplace)
{
	if(bufplace>=ioman->numbuf)return;
	ioman->usage[bufplace]=0x00;
}

euint8 ioman_getRefCnt(IOManager *ioman,euint16 bufplace)
{
	if(bufplace>=ioman->numbuf)return(0x00);
	return(ioman->reference[bufplace]);
}

void ioman_incRefCnt(IOManager *ioman,euint16 bufplace)
{
	if(bufplace>=ioman->numbuf)return;
	if(ioman->reference[bufplace]==0xFF)return;
	else ioman->reference[bufplace]++;
}

void ioman_decRefCnt(IOManager *ioman,euint16 bufplace)
{
	if(bufplace>=ioman->numbuf)return;
	if(ioman->reference[bufplace]==0x00)return;
	else ioman->reference[bufplace]--;
}

void ioman_resetRefCnt(IOManager *ioman,euint16 bufplace)
{
	if(bufplace>=ioman->numbuf)return;
	ioman->reference[bufplace]=0x00;
}

esint8 ioman_pop(IOManager *ioman,euint16 bufplace)
{
	if(bufplace>=ioman->numbuf)return(-1);
	if(ioman->itptr[bufplace]==0 || ioman->itptr[bufplace]>IOMAN_NUMITERATIONS)return(-1);
	ioman->sector[bufplace] = ioman->stack[bufplace][ioman->itptr[bufplace]].sector;
	ioman->status[bufplace] = ioman->stack[bufplace][ioman->itptr[bufplace]].status;
	ioman->usage[bufplace]  = ioman->stack[bufplace][ioman->itptr[bufplace]].usage; 
	ioman->itptr[bufplace]--;
	return(0);
}

esint8 ioman_push(IOManager *ioman,euint16 bufplace)
{
	if(bufplace>=ioman->numbuf)return(-1);
	if(ioman->itptr[bufplace]>=IOMAN_NUMITERATIONS)return(-1);
	ioman->itptr[bufplace]++;
	ioman->stack[bufplace][ioman->itptr[bufplace]].sector = ioman->sector[bufplace];
	ioman->stack[bufplace][ioman->itptr[bufplace]].status = ioman->status[bufplace];
	ioman->stack[bufplace][ioman->itptr[bufplace]].usage  = ioman->usage[bufplace];
	return(0);
}

euint8* ioman_getPtr(IOManager *ioman,euint16 bufplace)
{
	if(bufplace>=ioman->numbuf)return(0);
	return(ioman->bufptr+bufplace*512);
}

esint16 ioman_getBp(IOManager *ioman,euint8* buf)
{
	if(buf<(ioman->bufptr) || buf>=( ioman->bufptr+(ioman->numbuf*512) )){
		return(-1);
	}
	return((buf-(ioman->bufptr))/512);
}

esint8 ioman_readSector(IOManager *ioman,euint32 address,euint8* buf)
{
	esint8 r;

	/*DBG((TXT("ioman_readSector::Requesting address %u to be put in %p.\n"),address,buf));*/
	if(buf==0){
		/*DBG((TXT("Refused to read sector, buf=0\n")));*/
		return(-1);
	}
	
	r=if_readBuf(ioman->iface,address,buf);
	
	if(r!=0){
		return(-1);
	}
	return(0);
}

esint8 ioman_writeSector(IOManager *ioman, euint32 address, euint8* buf)
{
	esint8 r;

	if(buf==0)return(-1);
	
	r=if_writeBuf(ioman->iface,address,buf);

	if(r<=0){
		return(-1);
	}
	return(0);
}

void ioman_resetCacheItem(IOManager *ioman,euint16 bufplace)
{
	ioman->sector[bufplace]    = 0;
	ioman->status[bufplace]    = 0;
	ioman->usage[bufplace]     = 0;
	ioman->reference[bufplace] = 0;
}

esint32 ioman_findSectorInCache(IOManager *ioman, euint32 address)
{
	euint16 c;
	
	for(c=0;c<ioman->numbuf;c++){
		if(ioman_isValid(c) && ioman->sector[c] == address)return(c);
	}
	return(-1);
}

esint32 ioman_findFreeSpot(IOManager *ioman)
{
	euint16 c;
	
	for(c=0;c<ioman->numbuf;c++){
		if(!ioman_isValid(c))return(c);
	}
	return(-1);
}

esint32 ioman_findUnusedSpot(IOManager *ioman)
{
	esint32 r=-1;
	euint16 c;
	euint8 fr=0,lr=0xFF;
	
	for(c=0;c<ioman->numbuf;c++){
		if(ioman_getUseCnt(ioman,c)==0){
			if(!ioman_isWritable(c) && !fr){
				fr=1;
				lr=0xFF;
				r=-1;
			}
			if(ioman_isWritable(c) && !fr){
				if(ioman_getRefCnt(ioman,c)<=lr){
					r=c;
					lr=ioman_getRefCnt(ioman,c);
				}
			}
			if(fr && !ioman_isWritable(c)){
				if(ioman_getRefCnt(ioman,c)<=lr){
					r=c;
					lr=ioman_getRefCnt(ioman,c);
				}
			}
		}
	}
	return(r);
}

esint32 ioman_findOverallocableSpot(IOManager *ioman)
{
	euint8 points,lp=0xFF;
	euint16 c;
	esint32 r=-1;
	
	for(c=0;c<ioman->numbuf;c++){
		if(ioman->itptr[c]<ioman->numit){
			points = 0;
			if(ioman_isWritable(c))points+=0x7F;
			points += ((euint16)(ioman->itptr[c]*0x4D))/(ioman->numit);
			points += ((euint16)(ioman_getRefCnt(ioman,c)*0x33))/0xFF;
			if(points<lp){
				lp=points;
				r=c;
			}
		}
	}
	return(r);
}

esint8 ioman_putSectorInCache(IOManager *ioman, euint32 address, euint16 bufplace)
{
	euint8* buf;
	
	if((buf = ioman_getPtr(ioman,bufplace))==0)return(-1);
	if((ioman_readSector(ioman,address,buf)))return(-1);
	ioman_setValid(bufplace);
	ioman->sector[bufplace]=address;
	return(0);
}

esint8 ioman_flushSector(IOManager *ioman, euint16 bufplace)
{
	euint8* buf;
	
	if((buf = ioman_getPtr(ioman,bufplace))==0)return(-1);
	if(!ioman_isWritable(bufplace))return(-1);
	if(!(ioman_writeSector(ioman,ioman->sector[bufplace],buf)))return(-1);
	return(0);
}

euint8* ioman_getSector(IOManager *ioman,euint32 address, euint8 mode)
{
	esint32 bp;
	
	if((bp=ioman_findSectorInCache(ioman,address))!=-1){
		if(mode==IOM_MODE_READWRITE){
			ioman_setWritable(bp);
		}
		ioman_incUseCnt(ioman,bp);
		ioman_incRefCnt(ioman,bp);
		return(ioman_getPtr(ioman,bp));
	}
	
	if((bp=ioman_findFreeSpot(ioman))==-1){
		if(((bp=ioman_findUnusedSpot(ioman))!=-1)&&(ioman_isWritable(bp))){
			ioman_flushSector(ioman,bp);
		}
	}
	
	if(bp!=-1){
		ioman_resetCacheItem(ioman,bp);
		if((ioman_putSectorInCache(ioman,address,bp))){
			return(0);
		}
		if(mode==IOM_MODE_READWRITE){
			ioman_setWritable(bp);
		}
		ioman_incUseCnt(ioman,bp);
		ioman_incRefCnt(ioman,bp);
		return(ioman_getPtr(ioman,bp));
	}
	
	if((bp=ioman_findOverallocableSpot(ioman))!=-1){
		if(ioman_isWritable(bp)){
			ioman_flushSector(ioman,bp);
		}
		if(ioman_push(ioman,bp)){
			return(0);
		}
		ioman_resetCacheItem(ioman,bp);
		if((ioman_putSectorInCache(ioman,address,bp))){
			return(0);
		}
		if(mode==IOM_MODE_READWRITE){
			ioman_setWritable(bp);
		}
		ioman_incUseCnt(ioman,bp);
		ioman_incRefCnt(ioman,bp);
		return(ioman_getPtr(ioman,bp));
	}
	return(0);
}

esint8 ioman_releaseSector(IOManager *ioman,euint8* buf)
{
	euint16 bp;
	
	bp=ioman_getBp(ioman,buf);
	ioman_decUseCnt(ioman,bp);
	
	if(ioman_getUseCnt(ioman,bp)==0 && ioman->itptr!=0){
		if(ioman_isWritable(bp)){
			ioman_flushSector(ioman,bp);
		}
		ioman_pop(ioman,bp);
		ioman_putSectorInCache(ioman,ioman->sector[bp],bp);
	}
	return(0);
}


void ioman_printStatus(IOManager *ioman)
{
	euint16 c;
	
	DBG((TXT("IO-Manager -- Report\n====================\n\n")));
	DBG((TXT("Buffer is %i sectors, from %p to %p\n"),
	          ioman->numbuf,ioman->bufptr,ioman->bufptr+(ioman->numbuf*512)));
	for(c=0;c<ioman->numbuf;c++){
		if(ioman_isValid(c)){
			DBG((TXT("BP %3i\t SC %i\t\t US %i\t RF %i\t %s %s\n"),
			c,ioman->sector[c],ioman_getUseCnt(ioman,c),ioman_getRefCnt(ioman,c),
			ioman_isUserBuf(c) ? "USRBUF" : "      ",
			ioman_isWritable(c) ? "WRITABLE" : "READONLY"));
		}
	}
}
