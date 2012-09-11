/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : linuxfile.c                                                      *
* Description : This file contains the functions needed to use efs for        *
*               accessing files under linux. This interface is meant          *
*               to be used for debugging purposes.                            *
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
#include "interfaces/linuxfile.h"
/*****************************************************************************/

/* ****************************************************************************  
 * short if_initInterface(hwInterface* file, char* fileName)
 * Description: This function should bring the hardware described in file in a
 * ready state to receive and retrieve data.
 * Return value: Return 0 on succes and -1 on failure.
*/
esint8 if_initInterface(hwInterface* file, eint8* fileName)
{
	eint32 sc;
	Fopen(&(file->imageFile),fileName);
	sc=getFileSize(file->imageFile);
	file->sectorCount=sc/512;
	file->readCount=file->writeCount=0;
	if(sc%512!=0)
		file->sectorCount--;
	return(0);
}
/*****************************************************************************/ 

/* ****************************************************************************  
 * short if_readBuf(hwInterface* file,unsigned long address,unsigned char* buf)
 * Description: This function should fill the characterpointer buf with 512 
 * bytes, offset by address*512 bytes. Adress is thus a LBA address.
 * Return value: Return 0 on success and -1 on failure.
*/
esint8 if_readBuf(hwInterface* file,euint32 address,euint8* buf)
{
	/*printf("READ  %li\n",address);*/
	if(if_setPos(file,address))return(-1);
	if( fread((void*)buf,512,1,file->imageFile) != 1) return(-1);
	file->readCount++;
	return(0);
}
/*****************************************************************************/ 

/* ****************************************************************************  
 * short if_writeBuf(hwInterface* file,unsigned long address,unsigned char* buf)
 * Description: This function writes 512 bytes from uchar* buf to the hardware
 * disc described in file. The write offset should be address sectors of 512 bytes.
 * Return value: Return 0 on success and -1 on failure.
*/
esint8 if_writeBuf(hwInterface* file,euint32 address,euint8* buf)
{
	/*printf("WRITE %li\n",address);*/
	if_setPos(file,address);
	if((fwrite((void*)buf,512,1,file->imageFile))!=1){
		perror("cf_writeBuf:");
		exit(-1);
	}
	fflush(file->imageFile);
	file->writeCount++;
	return(0);
}
/*****************************************************************************/ 

/* ****************************************************************************  
 * short if_setPos(hwInterface* file,unsigned long address)
 * Description: This function may or may not be required. It would set the write
 * or read buffer offset by 512*address bytes from the beginning of the disc.
 * Return value: Return 0 on success and -1 on failure.
*/
esint8 if_setPos(hwInterface* file,euint32 address)
{
	if(address>(file->sectorCount-1)){
		DBG((TXT("Illegal address\n")));
		exit(-1);
	}
	if((fseek(file->imageFile,512*address,SEEK_SET))!=0){
		perror("cf_setPos:");
		exit(-1);
	}
	return(0);
}
/*****************************************************************************/ 

