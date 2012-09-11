/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------------------------------------------           *
*                                                                             *
* Description : This file is part of the linuxutils in the 0.2 branch         *
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

#include <stdio.h>
#include "efs.h"
#include "ls.h"

int main(int argc, char** argv)
{
	EmbeddedFileSystem efs;
	DirList *list;
	
	if(argc<3){
		fprintf(stderr,"Argument error : list <fs> <directory>\n");
		exit(-1);
	}

	if(efs_init(&efs,argv[1])!=0){
		printf("Could not open filesystem.\n");
		return(-1);
	}
	
	list = malloc(sizeof(*list));

	if((ls_openDir(list,&(efs.myFs),argv[2]))!=0){
	    printf("Unable to open directory for listing\n");
	    exit(-1);
	}

	while(ls_getNext(list)==0){
	    printf("%s\n",list->currentEntry.FileName);
	}
	
	fs_umount(&(efs.myFs));

	return(0);
}
