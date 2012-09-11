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

//------------------------------------------------------------------------------
/// \page "ManagedNandFlash"
///
/// !!!Purpose
/// 
/// The lower layer of nandflash block management, it is called by MappedNandFlash layer, and 
/// it will call EccNandFlash layer.
//------------------------------------------------------------------------------

#ifndef MANAGEDNANDFLASH_H
#define MANAGEDNANDFLASH_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "NandCommon.h"
#include "EccNandFlash.h"

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

#define NandBlockStatus_DEFAULT         0xF
#define NandBlockStatus_FREE            0xE
#define NandBlockStatus_LIVE            0xC
#define NandBlockStatus_DIRTY           0x8
#define NandBlockStatus_BAD             0x0

#define NandEraseDIRTY                  0   // Erase dirty blocks only
#define NandEraseDATA                   1   // Erase all data, calculate count
#define NandEraseFULL                   2   // Erase all, reset erase count

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

struct NandBlockStatus {

    unsigned int status:4,
                 eraseCount:28;
};

struct ManagedNandFlash {

    struct EccNandFlash ecc;
    struct NandBlockStatus blockStatuses[NandCommon_MAXNUMBLOCKS];
    unsigned short baseBlock;
    unsigned short sizeInBlocks;
};

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

extern unsigned char ManagedNandFlash_Initialize(
    struct ManagedNandFlash *managed,
    const struct NandFlashModel *model,
    unsigned int commandAddress,
    unsigned int addressAddress,
    unsigned int dataAddress,
    const Pin pinChipEnable,
    const Pin pinReadyBusy,
    unsigned short baseBlock,
    unsigned short sizeInBlocks);

extern unsigned char ManagedNandFlash_AllocateBlock(
    struct ManagedNandFlash *managed,
    unsigned short block);

extern unsigned char ManagedNandFlash_ReleaseBlock(
    struct ManagedNandFlash *managed,
    unsigned short block);

extern unsigned char ManagedNandFlash_EraseBlock(
    struct ManagedNandFlash *managed,
    unsigned short block);

extern unsigned char ManagedNandFlash_ReadPage(
    const struct ManagedNandFlash *managed,
    unsigned short block,
    unsigned short page,
    void *data,
    void *spare);

extern unsigned char ManagedNandFlash_WritePage(
    const struct ManagedNandFlash *managed,
    unsigned short block,
    unsigned short page,
    void *data,
    void *spare);

extern unsigned char ManagedNandFlash_CopyPage(
    const struct ManagedNandFlash *managed,
    unsigned short sourceBlock,
    unsigned short sourcePage,
    unsigned short destBlock,
    unsigned short destPage);

extern unsigned char ManagedNandFlash_CopyBlock(
    const struct ManagedNandFlash *managed,
    unsigned short sourceBlock,
    unsigned short destBlock);

extern unsigned char ManagedNandFlash_EraseDirtyBlocks(
    struct ManagedNandFlash *managed);

extern unsigned char ManagedNandFlash_FindYoungestBlock(
    const struct ManagedNandFlash *managed,
    unsigned char status,
    unsigned short *block);

extern unsigned short ManagedNandFlash_CountBlocks(
    const struct ManagedNandFlash *managed,
    unsigned char status);
    
extern unsigned short ManagedNandFlash_GetDeviceSizeInBlocks(
    const struct ManagedNandFlash *managed);

extern unsigned char ManagedNandFlash_EraseAll(
    struct ManagedNandFlash *managed,
    unsigned char level);

#endif //#ifndef MANAGEDNANDFLASH_H

