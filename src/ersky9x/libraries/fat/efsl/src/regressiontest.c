/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename :  regressiontest.c                                                *
* Description : This file contains a regressiontest (obsolete)                *
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
#include "config.h"
#include "interfaces/helper.h"
//#include "interfaces/linuxfile.h"
#include "disc.h"
#include "partition.h"
#include "fs.h"
#include "file.h"
#include "mkfs.h"
#include "ioman.h"
#include "debug.h"
#include "extract.h"
#include "plibc.h"
#include <string.h>

/*****************************************************************************/

int main(int argc, char** argv)
{
	hwInterface *lfile=0;
	IOManager *ioman=0;
	Disc *disc=0;
	Partition *part=0;
	FileSystem *fs=0;
	File *fr=0;
	File *fw=0;
	FILE *out=0;
	unsigned char buf[4096];
	
	int c,r,ir;
	
	/*debug_init();*/
	
	switch(argv[1][0]){
		case '0':
			lfile = malloc(sizeof(*lfile));
			ioman = malloc(sizeof(*ioman));
			disc = malloc(sizeof(*disc));
			part = malloc(sizeof(*part));
			
			if_initInterface(lfile,"regtest.16");
			ioman_init(ioman,lfile,0);
			disc_initDisc(disc,ioman);
			memClr(disc->partitions,sizeof(PartitionField)*4);
			disc->partitions[0].type=0x0B;
			disc->partitions[0].LBA_begin=0;
			disc->partitions[0].numSectors=lfile->sectorCount;
			part_initPartition(part,disc);

			fs = malloc(sizeof(*fs));
			if( (fs_initFs(fs,part)) != 0){
				 printf("Unable to init the filesystem\n");
				 return(-1);
			}
			
			fr = malloc(sizeof(*fr));
			if ( (file_fopen(fr,fs,"file.r",MODE_READ)) != 0){
				printf("Unable to open the file file.r\n");
				return(-1);
			}
			
			out=fopen("REG_FILE_16_OUT","w");
			
			c=0;
			while( (r=file_fread(fr,c,4096,buf)) != 0){
				c+=r;
				fwrite(buf,r,1,out);
			}
			file_fclose(fr);
			fclose(out);
			fs_umount(fs);
			break;
		
		case '1':
			
			lfile = malloc(sizeof(*lfile));
			ioman = malloc(sizeof(*ioman));
			disc = malloc(sizeof(*disc));
			part = malloc(sizeof(*part));
			
			if_initInterface(lfile,"regtest.32");
			ioman_init(ioman,lfile,0);
			disc_initDisc(disc,ioman);
			memClr(disc->partitions,sizeof(PartitionField)*4);
			disc->partitions[0].type=0x0B;
			disc->partitions[0].LBA_begin=0;
			disc->partitions[0].numSectors=lfile->sectorCount;
			part_initPartition(part,disc);

			fs = malloc(sizeof(*fs));
			if( (fs_initFs(fs,part)) != 0){
				 printf("Unable to init the filesystem\n");
				 return(-1);
			}
			fr = malloc(sizeof(*fr));
			if ( (file_fopen(fr,fs,"file.r",MODE_READ)) != 0){
				printf("Unable to open the file file.r\n");
				return(-1);
			}
			
			out=fopen("REG_FILE_32_OUT","w");
			
			c=0;
			while( (r=file_fread(fr,c,4096,buf)) != 0){
				c+=r;
				fwrite(buf,r,1,out);
			}
			file_fclose(fr);
			fclose(out);
			fs_umount(fs);
			
			break;
		
		case '2':
				
			lfile = malloc(sizeof(*lfile));
			ioman = malloc(sizeof(*ioman));
			disc = malloc(sizeof(*disc));
			part = malloc(sizeof(*part));
			fs = malloc(sizeof(*fs));
			fr = malloc(sizeof(*fr));
			fw = malloc(sizeof(*fw));
			
			if_initInterface(lfile,"regtestrw.32");
			ioman_init(ioman,lfile,0);
			disc_initDisc(disc,ioman);
			memClr(disc->partitions,sizeof(PartitionField)*4);
			disc->partitions[0].type=0x0B;
			disc->partitions[0].LBA_begin=0;
			disc->partitions[0].numSectors=lfile->sectorCount;
			part_initPartition(part,disc);

			fs = malloc(sizeof(*fs));
			if( (fs_initFs(fs,part)) != 0){
				 printf("Unable to init the filesystem\n");
				 return(-1);
			}
						
			if((file_fopen(fr,fs,"file.r",'r'))!=0){
				printf("Unable to open %s for reading...\n","file.r");
				exit(0);
			}
			if((file_fopen(fw,fs,"file.w",'w'))!=0){
				printf("Unable to open %s for writing...\n","file.w");
				exit(0);
			}
			
			ir = atoi(argv[2]);
						
			while((r = file_read(fr,ir,buf))>0){
				file_write(fw,r,buf);
			}

			file_fclose(fr);
			file_fclose(fw);
			
			if((file_fopen(fr,fs,"file.w",'r'))!=0){
				printf("Unable to open %s for reading...\n","file.w");
				exit(0);
			}
			
			out=fopen("REG_FILE_RW32_OUT","w");

			while((r = file_read(fr,ir/2+3,buf))>0){
				fwrite(buf,1,r,out);
			}
			
			fclose(out);
			file_fclose(fr);
			fs_umount(fs);
			
			break;

		case '3':
			lfile = malloc(sizeof(*lfile));
			ioman = malloc(sizeof(*ioman));
			disc = malloc(sizeof(*disc));
			part = malloc(sizeof(*part));
			
			if_initInterface(lfile,"regtest.12");
			ioman_init(ioman,lfile,0);
			disc_initDisc(disc,ioman);
			memClr(disc->partitions,sizeof(PartitionField)*4);
			disc->partitions[0].type=0x0B;
			disc->partitions[0].LBA_begin=0;
			disc->partitions[0].numSectors=lfile->sectorCount;
			part_initPartition(part,disc);

			fs = malloc(sizeof(*fs));
			if( (fs_initFs(fs,part)) != 0){
				 printf("Unable to init the filesystem\n");
				 return(-1);
			}
			
			fr = malloc(sizeof(*fr));
			if ( (file_fopen(fr,fs,"file.r",MODE_READ)) != 0){
				printf("Unable to open the file file.r\n");
				return(-1);
			}
			
			out=fopen("REG_FILE_12_OUT","w");
			
			c=0;
			while( (r=file_fread(fr,c,4096,buf)) != 0){
				c+=r;
				fwrite(buf,r,1,out);
			}
			file_fclose(fr);
			fclose(out);
			fs_umount(fs);
			
			break;

		case '4':
				
			lfile = malloc(sizeof(*lfile));
			ioman = malloc(sizeof(*ioman));
			disc = malloc(sizeof(*disc));
			part = malloc(sizeof(*part));
			fs = malloc(sizeof(*fs));
			fr = malloc(sizeof(*fr));
			fw = malloc(sizeof(*fw));
			
			if_initInterface(lfile,"regtestrw.12");
			ioman_init(ioman,lfile,0);
			disc_initDisc(disc,ioman);
			memClr(disc->partitions,sizeof(PartitionField)*4);
			disc->partitions[0].type=0x0B;
			disc->partitions[0].LBA_begin=0;
			disc->partitions[0].numSectors=lfile->sectorCount;
			part_initPartition(part,disc);

			fs = malloc(sizeof(*fs));
			if( (fs_initFs(fs,part)) != 0){
				 printf("Unable to init the filesystem\n");
				 return(-1);
			}
						
			if((file_fopen(fr,fs,"file.r",'r'))!=0){
				printf("Unable to open %s for reading...\n","file.r");
				exit(0);
			}
			if((file_fopen(fw,fs,"file.w",'w'))!=0){
				printf("Unable to open %s for writing...\n","file.w");
				exit(0);
			}
			
			ir = atoi(argv[2]);
						
			while((r = file_read(fr,ir,buf))>0){
				file_write(fw,r,buf);
			}

			fflush(stdout);
			
			file_fclose(fr);
			file_fclose(fw);
			
			if((file_fopen(fr,fs,"file.w",'r'))!=0){
				printf("Unable to open %s for reading...\n","file.w");
				exit(0);
			}
			
			out=fopen("REG_FILE_RW12_OUT","w");

			while((r = file_read(fr,ir/2+3,buf))>0){
				fwrite(buf,1,r,out);
			}
			
			fclose(out);
			file_fclose(fr);
			fs_umount(fs);

			break;
		
			default:
				printf("Unknown test\n");

	}
	return(0);
}
/*****************************************************************************/

euint16 efsl_getYear(void)
{
	return(2005);
}
/*****************************************************************************/

euint8  efsl_getMonth(void)
{
	return(5);
}
/*****************************************************************************/

euint8  efsl_getDay(void)
{
	return(11);
}
/*****************************************************************************/

euint8  efsl_getHour(void)
{
	return(13);
}
/*****************************************************************************/

euint8  efsl_getMinute(void)
{
	return(14);
}
/*****************************************************************************/

euint8  efsl_getSecond(void)
{
	return(40);
}
/*****************************************************************************/


