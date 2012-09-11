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
/// \page "TranslatedNandFlash"
///
/// !!!Purpose
/// 
/// Translate a nandflash physical block to logical block, it will call lower layer such as MappedNandFlash
//------------------------------------------------------------------------------

#ifndef TRANSLATEDNANDFLASH_H
#define TRANSLATEDNANDFLASH_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "MappedNandFlash.h"

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

struct TranslatedNandFlash {

    struct MappedNandFlash mapped;
    signed short currentLogicalBlock;
    signed short previousPhysicalBlock;
    unsigned char currentBlockPageStatuses[NandCommon_MAXNUMPAGESPERBLOCK / 8];
};

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

extern unsigned char TranslatedNandFlash_Initialize(
    struct TranslatedNandFlash *translated,
    const struct NandFlashModel *model,
    unsigned int commandAddress,
    unsigned int addressAddress,
    unsigned int dataAddress,
    const Pin pinChipEnable,
    const Pin pinReadyBusy,
    unsigned short baseBlock,
    unsigned short sizeInBlocks);

extern unsigned char TranslatedNandFlash_ReadPage(
    const struct TranslatedNandFlash *translated,
    unsigned short block,
    unsigned short page,
    void *data,
    void *spare);

extern unsigned char TranslatedNandFlash_WritePage(
    struct TranslatedNandFlash *translated,
    unsigned short block,
    unsigned short page,
    void *data,
    void *spare);

extern unsigned char TranslatedNandFlash_Flush(
    struct TranslatedNandFlash *translated);

extern unsigned char TranslatedNandFlash_EraseAll(
    struct TranslatedNandFlash *translated,
    unsigned char level);

extern unsigned char TranslatedNandFlash_SaveLogicalMapping(
    struct TranslatedNandFlash *translated);

extern unsigned short TranslatedNandFlash_GetDeviceSizeInBlocks(
   const struct TranslatedNandFlash *translated);

extern unsigned int TranslatedNandFlash_GetDeviceSizeInPages(
   const struct TranslatedNandFlash *translated);

extern unsigned long long TranslatedNandFlash_GetDeviceSizeInBytes(
   const struct TranslatedNandFlash *translated);

#endif //#ifndef TRANSLATEDNANDFLASH_H

