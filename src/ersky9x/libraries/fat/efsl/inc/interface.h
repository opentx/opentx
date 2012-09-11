/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          --------------------------------------------------------           *
*                                                                             *
* Filename : interface.h                                                      *
* Description : This headerfile includes the right interface headerfile       *
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

#ifndef __TYPES_H__
#define __TYPES_H__

/*****************************************************************************/
#include "types.h"
#include "config.h"
/*****************************************************************************/

#if defined(HW_ENDPOINT_LINUX) || defined(HW_ENDPOINT_LINUX64)
	#include "interfaces/linuxfile.h"
#elif defined(HW_ENDPOINT_ATMEGA128_SD)
	#include "interfaces/atmega128.h"
#elif defined(HW_ENDPOINT_DSP_TI6713_SD)
	#include "interfaces/dsp67xx.h"
#elif defined(HW_ENDPOINT_LPC2000_SD)
	#include "interfaces/lpc2000_spi.h"
#elif defined(HW_ENDPOINT_AT91SAM)
	#include "interfaces/at91sam7s_spi.h"
#else
	#error "NO INTERFACE DEFINED - see interface.h"
#endif

#endif

