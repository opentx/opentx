/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : ui.c                                                             *
* Description : This file contains functions which will be presented to the   *
*               user of this library.                                         *
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
#include "ui.h"
/*****************************************************************************/

/*****************************************************************************
 * short listfiles(char *dir)
 *
 * Deschription: This function returns the number of files / directories
 * inside the given directory.
 *
 * Return value: number of files/directories in the given directory or -1
 * if directory does not exist.
\*****************************************************************************/

short listFiles(FileSystem *fs, char *dirname)
{
	unsigned long startCluster;
	unsigned char fileEntryCount;
	unsigned short counter=0;
	unsigned long offset=0;
	FileRecord fileEntry;
	FileLocation loc;
	unsigned char buf[512];
	File dir;
	unsigned short i;
	
	/* Find out if we are searching in the root dir or in */
	if(dirname[0]=='/' && dirname[1]=='\0')
	{
		if( (fs->type == FAT12) || (fs->type == FAT16) )
		{
			for(i=0;i<=(fs->volumeId.RootEntryCount/16);i++)
			{
				loc.Sector=fs->FirstSectorRootDir + i;
				part_readBuf(fs->part,loc.Sector,buf);
				/* I STOPPED HERE*/
				/* FIXME */
			}
		}
	}
	else /* Normal directory */
	{
		/* Check if path given is a directory */
		if(fs_findFile(fs,dirname,&loc,0)!=2)
		{
			FUNC_OUT((TXT("")));
			return(-1);
		}
	
		/* Find out what the startcluster of the directory is */
		part_readBuf(fs->part,loc.Sector, buf);
		fileEntry = *(((FileRecord*)buf) + loc.Offset);
		startCluster = (((unsigned long)fileEntry.FirstClusterHigh)<<16) 
			+ fileEntry.FirstClusterLow;

	    /* Init of dir */
		dir.fs=fs;
		dir.Cache.LogicCluster=-1;
		dir.Cache.FirstCluster=startCluster;
		dir.DirEntry.Attribute=ATTR_DIRECTORY;
	
		while((file_fread(&dir,offset,512,buf)))
		{
			DBG((TXT("Read 512 bytes from dir with offset %li.\n"),offset));
			for(fileEntryCount=0;fileEntryCount<16;fileEntryCount++)
			{
				fileEntry = *(((FileRecord*)buf) + fileEntryCount);
				if( !( (fileEntry.Attribute & 0x0F) == 0x0F ) )
				{
					if
					(
					 (fileEntry.FileName[0]>='A' && fileEntry.FileName[0]<='Z')
					 ||
					 (fileEntry.FileName[0]>='0' && fileEntry.FileName[0]<='9')
					)
					{
						DBG((TXT("Filename: %s\n"),fileEntry.FileName));
						counter++;
					}
				}
			}
			offset+=512;
		}
	}
	
	FUNC_OUT((TXT("")));
	return(counter);
	
	return(-1);
}
/*****************************************************************************/

/* ****************************************************************************  
 * esint16 rmfile(FileSystem *fs,euint8* filename)
 * Description: This function takes a filename as argument and deletes it,
 * by freeing it's clusterchain, and deleting it's entry from the directory.
 * Return value: 0 on success, -1 on errors, like file not found.
*/
esint16 rmfile(FileSystem *fs,euint8* filename)
{
	FileLocation loc;
	ClusterChain cache;
	euint8* buf;
	euint32 firstCluster=0;
	
	if((fs_findFile(fs,(eint8*)filename,&loc,0))==1){
		buf=part_getSect(fs->part,loc.Sector,IOM_MODE_READWRITE);
		firstCluster = ex_getb16(buf,loc.Offset*32+20);
		firstCluster <<= 16;
		firstCluster += ex_getb16(buf,loc.Offset*32+26);
		/* Bugfix:
		 * By clearing the entire structure, you mark end of directory.
		 * If this is not the case, files that are further away cannot
		* be opened anymore by implementations that follow the spec. */
		/*memClr(buf+(loc.Offset*32),32);*/
		*(buf+(loc.Offset*32)+0) = 0xE5; /* Mark file deleted */
		part_relSect(fs->part,buf);
		cache.DiscCluster = cache.LastCluster = cache.Linear = cache.LogicCluster = 0;
		cache.FirstCluster = firstCluster;
 		fat_unlinkClusterChain(fs,&cache);
		return(0);
	}
	return(-1);
}

/*****************************************************************************/
esint8 mkdir(FileSystem *fs,eint8* dirname)
{
	FileLocation loc;
	FileRecord direntry;
	euint32 nc,parentdir;
	euint8* buf;
	eint8 ffname[11];
	
	if( fs_findFile(fs,dirname,&loc,&parentdir) ){
		return(-1);
	}
	if(parentdir==0)return(-2);
	
	if(!fs_findFreeFile(fs,dirname,&loc,0))return(-3);
	
	/* You may never search for a free cluster, and the call
	 * functions that may cause changes to the FAT table, that
	 * is why getNextFreeCluster has to be called AFTER calling
	 * fs_findFreeFile, which may have to expand a directory in
	 * order to store the new filerecord !! 
	 */
	
	nc = fs_getNextFreeCluster(fs,fs_giveFreeClusterHint(fs));
	if(nc==0)return(0);
	
	fs_clearCluster(fs,nc);
	
	buf = part_getSect(fs->part,loc.Sector,IOM_MODE_READWRITE);

	dir_getFatFileName(dirname,ffname);
	memClr(&direntry,sizeof(direntry));
	memCpy(ffname,&direntry,11);
	direntry.FileSize = 0;
	direntry.FirstClusterHigh=nc>>16;
	direntry.FirstClusterLow=nc&0xFFFF;
	direntry.Attribute = ATTR_DIRECTORY;
	memCpy(&direntry,buf+(32*loc.Offset),32);
		
	part_relSect(fs->part,buf);
	
	buf = part_getSect(fs->part,fs_clusterToSector(fs,nc),IOM_MODE_READWRITE);
	
	memClr(&direntry,sizeof(direntry));
	memCpy(".          ",&direntry,11);
	direntry.Attribute = ATTR_DIRECTORY;
	direntry.FileSize = 0;
	direntry.FirstClusterHigh=nc>>16;
	direntry.FirstClusterLow=nc&0xFFFF;
	memCpy(&direntry,buf,32);
	
	if(fs->type == FAT32 && parentdir == fs->volumeId.RootCluster){
		parentdir = 0;
	}
	if(fs->type != FAT32 && parentdir<=1){
		parentdir = 0;
	} 
	
	memClr(&direntry,sizeof(direntry));
	memCpy("..         ",&direntry,11);
	direntry.Attribute = ATTR_DIRECTORY;
	direntry.FileSize = 0;
	direntry.FirstClusterHigh=parentdir>>16;
	direntry.FirstClusterLow=parentdir&0xFFFF;
	memCpy(&direntry,buf+32,32);

	part_relSect(fs->part,buf);
	
	fat_setNextClusterAddress(fs,nc,fat_giveEocMarker(fs));

	return(0);
}
