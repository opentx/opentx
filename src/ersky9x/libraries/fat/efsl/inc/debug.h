/*****************************************************************************\
*              efs - General purpose Embedded Filesystem library              *
*          ---------------------------------------------------------          *
*                                                                             *
* Filename : debug.h                                                          *
* Description : Headerfile for debug.c                                        *
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
 *                               LPC2000 ARM7 Interface (c)2005 Martin Thomas *
 */

#ifndef __DEBUG_H__
#define __DEBUG_H__

/*****************************************************************************/
#include "types.h"
#include "config.h"
/*****************************************************************************/

#ifndef DEBUG
	#define TXT(x) ;
	#define DBG(x) ;
	#define FUNC_IN(x) ;
	#define FUNC_OUT(x) ;
#endif

#ifdef DEBUG
	#if defined(HW_ENDPOINT_LINUX) || defined(HW_ENDPOINT_LINUX64)
	    #define HW_ENDPOINT_LINUX_ALL	
	#endif

	#ifdef HW_ENDPOINT_ATMEGA128_SD
		#include <avr/io.h>
		#include <compat/ina90.h>
		#include <avr/pgmspace.h>
		#include <stdio.h>

		#define TXT(x) PSTR(x)
		#define DBG(x) debug x
		#define FUNC_IN(x) ;
		#define FUNC_OUT(x) ;
	#endif

	#ifdef HW_ENDPOINT_LINUX_ALL
		#include <stdio.h>
		#include <stdarg.h>
	
		#define TXT(x) x
		#define DBG(x) debug x
		#define FUNC_IN(x) debug_funcin(x)
		#define FUNC_OUT(x) debug_funcout(x)
	#endif

	#ifdef HW_ENDPOINT_DSP_TI6713_SD
		#include <stdio.h>
		#include <stdarg.h>

		#define TXT(x) x
		#define DBG(x) printf x
		#define FUNC_IN(x) ;
		#define FUNC_OUT(x) ;
	#endif
	#ifdef HW_ENDPOINT_NIOS_2_SD
		#include <stdio.h>
		#include <stdarg.h>

		#define TXT(x) x
		#define DBG(x) printf x
		#define FUNC_IN(x) ;
		#define FUNC_OUT(x) ;
	#endif
	#ifdef HW_ENDPOINT_LPC2000_SD
		#include "interfaces/lpc2000_dbg_printf.h"
		
		#define TXT(x) x
		#define DBG(x) debug x
		#define FUNC_IN(x) ;
		#define FUNC_OUT(x) ;
		#define debug lpc2000_debug_printf
	#endif

	#ifdef HW_ENDPOINT_AT91SAM
		#include <stdio.h>
		#include <stdarg.h>

		#define TXT(x) x
#ifndef NDEBUG
		#define DBG(x) printf x
#else
		#define DBG(x)
#endif
		#define FUNC_IN(x) ;
		#define FUNC_OUT(x) ;
	#else
	void debug(const eint8 *format, ...); /* This is messy FIXME */
	#endif

	void debug_init();
	void debug_end();

	#ifdef HW_ENDPOINT_LINUX_ALL
	FILE* debugfile;
	volatile euint8 tw;
	void debug_funcin(const eint8 *format, ...);
	void debug_funcout(const eint8 *format, ...);
	euint8 debug_getByte();
	euint8 debug_getString(euint8 *data,euint16 length);
	#endif

	#ifdef HW_ENDPOINT_ATMEGA128_SD
	void debug_initUART(euint16 baudrate );
	void debug_sendByte(euint8 data );
	#endif

#endif

#endif
