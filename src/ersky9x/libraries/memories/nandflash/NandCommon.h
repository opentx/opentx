/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2009, Atmel Corporation
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

#ifndef NANDCOMMON_H
#define NANDCOMMON_H

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/

/** 
 * \page NandFlash Maximum Supported Values
 * Since no memory allocation is available, limits have been set on various
 * characteristics of a NandFlash chip:
 * 
 * !Maximums
 * - NandCommon_MAXNUMBLOCKS
 * - NandCommon_MAXNUMPAGESPERBLOCK
 * - NandCommon_MAXPAGESIZE
 */
 
/// Maximum number of blocks in a device
#define NandCommon_MAXNUMBLOCKS             1024 //2048

/// Maximum number of pages in one block
#define NandCommon_MAXNUMPAGESPERBLOCK      256 //64

/// Maximum size of the data area of one page, in bytes.
#if !defined(NF_MAXPAGESIZE_SUPPORT_2K)
#define NandCommon_MAXPAGEDATASIZE          4096 //2048
#else
#define NandCommon_MAXPAGEDATASIZE          2048
#endif

/// Maximum size of the spare area of one page, in bytes.
#define NandCommon_MAXPAGESPARESIZE         128 //64

/// Maximum number of ecc bytes stored in the spare for one single page.
#define NandCommon_MAXSPAREECCBYTES         48 //24

/// Maximum number of extra free bytes inside the spare area of a page.
#define NandCommon_MAXSPAREEXTRABYTES       78 //38
//------------------------------------------------------------------------------

/** 
 * \page NandFlash Error Codes
 * NandFlash API methods return either 0 (indicating that there was no error),
 * or one of the following error codes:
 * 
 * !Codes                               
 * - NandCommon_ERROR_NOMOREBLOCKS      
 * - NandCommon_ERROR_BLOCKNOTMAPPED    
 * - NandCommon_ERROR_OUTOFBOUNDS       
 *- NandCommon_ERROR_MAPPINGNOTFOUND   
 *  - NandCommon_ERROR_CANNOTREAD        
 */

/// No more blocks can be allocated for a write operation.
#define NandCommon_ERROR_NOMOREBLOCKS       1

/// The desired logical block has no current physical mapping.
#define NandCommon_ERROR_BLOCKNOTMAPPED     2

/// Access if out-of-bounds.
#define NandCommon_ERROR_OUTOFBOUNDS        3

/// There are no block having the desired property.
#define NandCommon_ERROR_NOBLOCKFOUND       4

/// The nandflash device has no logical mapping information on it.
#define NandCommon_ERROR_MAPPINGNOTFOUND    5

/// A read operation cannot be carried out.
#define NandCommon_ERROR_CANNOTREAD         6

/// A write operation cannot be carried out.
#define NandCommon_ERROR_CANNOTWRITE        7

/// NandFlash chip model cannot be recognized.
#define NandCommon_ERROR_UNKNOWNMODEL       8

/// Page data is corrupted according to ECC
#define NandCommon_ERROR_CORRUPTEDDATA      9

/// Block is not in the required status.
#define NandCommon_ERROR_WRONGSTATUS        10

/// Device has no logical mapping stored in it
#define NandCommon_ERROR_NOMAPPING          11

/// The block being accessed is bad and must be replaced
#define NandCommon_ERROR_BADBLOCK           12

/// Failed to perform an erase operation
#define NandCommon_ERROR_CANNOTERASE        13

/// A hardware copyback operation failed.
#define NandCommon_ERROR_CANNOTCOPY         14

/// HW Ecc Not compatible with the Nand Model
#define NandCommon_ERROR_ECC_NOT_COMPATIBLE 15

#endif //#ifndef NANDCOMMON_H

