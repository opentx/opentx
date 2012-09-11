/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------- -----------------------------------          *
*                                                                             *
* Filename : efs.h                                                            *
* Description : Headerfile for efs.c                                          *
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

#ifndef __EFS_H__
#define __EFS_H__

/*****************************************************************************/
#include "types.h"
#include "config.h"
#include "interface.h"
#include "disc.h"
#include "partition.h"
#include "fs.h"
#include "file.h"
#include "time.h"
#include "ui.h"
/*****************************************************************************/

typedef File EmbeddedFile;

struct EmbeddedFileSystem{
	hwInterface myCard;
	IOManager myIOman;
	Disc myDisc;
	Partition myPart;
	FileSystem myFs;
};
typedef struct  EmbeddedFileSystem EmbeddedFileSystem;

esint8 efs_init(EmbeddedFileSystem * efs,eint8 * opts);


#endif

