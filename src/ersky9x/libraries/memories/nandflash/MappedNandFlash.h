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
/// \page "MappedNandFlash"
///
/// !!!Purpose
/// 
/// MappedNandFlash layer will do operations on logical blocks of nandflash, it is called by 
/// TranslatedNandFlash layer
//------------------------------------------------------------------------------

#ifndef MAPPEDNANDFLASH_H
#define MAPPEDNANDFLASH_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "ManagedNandFlash.h"

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

struct MappedNandFlash {

    struct ManagedNandFlash managed;
    signed short logicalMapping[NandCommon_MAXNUMBLOCKS];
    signed short logicalMappingBlock;
    unsigned char mappingModified;
    unsigned char reserved;
};

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

extern unsigned char MappedNandFlash_Initialize(
    struct MappedNandFlash *mapped,
    const struct NandFlashModel *model,
    unsigned int commandAddress,
    unsigned int addressAddress,
    unsigned int dataAddress,
    const Pin pinChipEnable,
    const Pin pinReadyBusy,
    unsigned short baseBlock,
    unsigned short sizeInBlocks);

extern unsigned char MappedNandFlash_ReadPage(
    const struct MappedNandFlash *mapped,
    unsigned short block,
    unsigned short page,
    void *data,
    void *spare);

extern unsigned char MappedNandFlash_WritePage(
    const struct MappedNandFlash *mapped,
    unsigned short block,
    unsigned short page,
    void *data,
    void *spare);

extern unsigned char MappedNandFlash_Map(
    struct MappedNandFlash *mapped,
    unsigned short logicalBlock,
    unsigned short physicalBlock);

extern unsigned char MappedNandFlash_Unmap(
    struct MappedNandFlash *mapped,
    unsigned short logicalBlock);

extern signed short MappedNandFlash_LogicalToPhysical(
    const struct MappedNandFlash *mapped,
    unsigned short logicalBlock);

extern signed short MappedNandFlash_PhysicalToLogical(
    const struct MappedNandFlash *mapped,
    unsigned short physicalBlock);

extern unsigned char MappedNandFlash_SaveLogicalMapping(
    struct MappedNandFlash *mapped,
    unsigned short physicalBlock);

extern unsigned char MappedNandFlash_EraseAll(
    struct MappedNandFlash *mapped,
    unsigned char level);

#endif //#ifndef MAPPEDNANDFLASH_H

