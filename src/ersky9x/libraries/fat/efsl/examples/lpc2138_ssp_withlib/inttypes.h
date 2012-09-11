/* Copyright (c) 2002, Marek Michalkiewicz
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE. */

/* $Id: inttypes.h,v 1.1 2005/11/16 17:33:36 lennartyseboodt Exp $ */

/*
   inttypes.h

   Contributors:
     Created by Marek Michalkiewicz <marekm@linux.org.pl>
 */

#ifndef __INTTYPES_H_
#define __INTTYPES_H_

/** \defgroup avr_inttypes Integer Types
    \code #include <inttypes.h> \endcode

    Use [u]intN_t if you need exactly N bits.

    Since these typedefs are mandated by the C99 standard, they are preferred
    over rolling your own typedefs.

    \note If avr-gcc's \c -mint8 option is used, no 32-bit types will be
    available.

    \todo There is a pending patch that may go into gcc to change the
    behaviour of the -mint8 option.  The current (2003-09-17) situation for
    -mint8 is sizeof(int) == 1, sizeof(long) == 2 and sizeof(long long) == 8.
    Note the absence of a 4-byte, 32-bit type.  The patch proposes to change
    sozeof(long long) to be 4 bytes (32 bits).  When and if the patch is
    included in gcc, we will need to change avr-libc accordingly.  */

#if __INT_MAX__ == 127
# define __USING_MINT8 1
#endif

/** \name 8-bit types. */

/*@{*/

/** \ingroup avr_inttypes
    8-bit signed type. */

typedef signed char int8_t;

/** \ingroup avr_inttypes
    8-bit unsigned type. */

typedef unsigned char uint8_t;

#if __USING_MINT8

typedef long int16_t;
typedef unsigned long uint16_t;

#else /* no -mint8 */

/*@}*/

/** \name 16-bit types. */

/*@{*/

/** \ingroup avr_inttypes
    16-bit signed type. */

typedef int int16_t;

/** \ingroup avr_inttypes
    16-bit unsigned type. */

typedef unsigned int uint16_t;

/*@}*/

/** \name 32-bit types. */

/*@{*/

/** \ingroup avr_inttypes
    32-bit signed type. */

typedef long int32_t;

/** \ingroup avr_inttypes
    32-bit unsigned type. */

typedef unsigned long uint32_t;

#endif

/*@}*/

/** \name 64-bit types. */

/*@{*/

/** \ingroup avr_inttypes
    64-bit signed type. */

typedef long long int64_t;

/** \ingroup avr_inttypes
    64-bit unsigned type. */

typedef unsigned long long uint64_t;

/*@}*/

/** \name Pointer types.
    These allow you to declare variables of the same size as a pointer. */

/*@{*/

/** \ingroup avr_inttypes
    Signed pointer compatible type. */

typedef int16_t intptr_t;

/** \ingroup avr_inttypes
    Unsigned pointer compatible type. */

typedef uint16_t uintptr_t;

/*@}*/

#endif
