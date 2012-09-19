/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          ---------------------------------------------------------          *
*                                                                             *
* Filename :  types.h                                                         *
* Description : This file contains the crossplatform data types               *
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

/* Contributions
 *                                                    (c)2005 Marcio Troccoli *
 */

#ifndef __EFS_TYPES_H__
#define __EFS_TYPES_H__

/*****************************************************************************/
#include "config.h"
/*****************************************************************************/

#if defined(HW_ENDPOINT_LINUX)
	typedef char eint8;
	typedef signed char esint8;
	typedef unsigned char euint8;
	typedef short eint16;
	typedef signed short esint16;
	typedef unsigned short euint16; 
	typedef long eint32; 
	typedef signed long esint32;
	typedef unsigned long euint32;
#elif defined(HW_ENDPOINT_LINUX64)
	typedef char eint8;
	typedef signed char esint8;
	typedef unsigned char euint8;
	typedef short eint16;
	typedef signed short esint16;
	typedef unsigned short euint16; 
	typedef int eint32; 
	typedef signed int esint32;
	typedef unsigned int euint32;
#elif defined (HW_ENDPOINT_ATMEGA128_SD)
	typedef char eint8;
	typedef signed char esint8;
	typedef unsigned char euint8;
	typedef short eint16;
	typedef signed short esint16;
	typedef unsigned short euint16; 
	typedef long eint32; 
	typedef signed long esint32;
	typedef unsigned long euint32; 
#elif defined(HW_ENDPOINT_DSP_TI6713_SD)
	typedef char eint8;
	typedef signed char esint8;
	typedef unsigned char euint8;
	typedef short eint16;
	typedef signed short esint16;
	typedef unsigned short euint16; 
	typedef int eint32; 
	typedef signed int esint32;
	typedef unsigned int euint32;
#elif defined(NIOS_2)
	typedef char eint8;
	typedef signed char esint8;
	typedef unsigned char euint8;
	typedef short eint16;
	typedef signed short esint16;
	typedef unsigned short euint16;
	typedef int eint32;
	typedef signed int esint32;
	typedef unsigned int euint32; 
#else
	typedef char eint8;
	typedef signed char esint8;
	typedef unsigned char euint8;
	typedef short eint16;
	typedef signed short esint16;
	typedef unsigned short euint16; 
	typedef long eint32; 
	typedef signed long esint32;
	typedef unsigned long euint32; 
#endif

#endif
