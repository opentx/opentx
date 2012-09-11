/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#ifndef _COMPILER_H_
#define _COMPILER_H_

typedef float               Float16;
typedef unsigned char       U8 ;
typedef unsigned short      U16;
typedef unsigned long       U32;
typedef unsigned char       Bool;

typedef unsigned char       Byte;

typedef char     bit;

#define MSB0(u32)       (((U8* )&u32)[3])
#define MSB1(u32)       (((U8* )&u32)[2])
#define MSB2(u32)       (((U8* )&u32)[1])
#define MSB3(u32)       (((U8* )&u32)[0])
#define LSB0(u32)       MSB3(u32)
#define LSB1(u32)       MSB2(u32)
#define LSB2(u32)       MSB1(u32)
#define LSB3(u32)       MSB0(u32)
#define MSB(u16)        (((U8* )&u16)[1])
#define LSB(u16)        (((U8* )&u16)[0])
#define MSW(u32)        (((U16*)&u32)[1])
#define LSW(u32)        (((U16*)&u32)[0])


// Constants
#define DISABLE  0
#define ENABLE   1

#define TRUE  (1 == 1)
#define FALSE (0 == 1)

#endif /* _COMPILER_H_ */

