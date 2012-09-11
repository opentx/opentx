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

/**
 * \file
 *
 * \par Purpose
 *
 * Type and methods for manipulating NandFlash models.
 *
 * \par Usage
 *
 * -# Find the model of a NandFlash using its device ID with the NandFlashModel_Find function.
 *   
 * -# Retrieve parameters of a NandFlash model using the following functions:
 *      - NandFlashModel_GetDeviceId
 *      - NandFlashModel_GetDeviceSizeInBlocks
 *      - NandFlashModel_GetDeviceSizeInPages
 *      - NandFlashModel_GetDeviceSizeInBytes
 *      - NandFlashModel_GetBlockSizeInPages
 *      - NandFlashModel_GetBlockSizeInBytes
 *      - NandFlashModel_GetPageDataSize
 *      - NandFlashModel_GetPageSpareSize
 *      - NandFlashModel_GetDataBusWidth
 *      - NandFlashModel_UsesSmallBlocksRead
 *      - NandFlashModel_UsesSmallBlocksWrite
 */

#ifndef NANDFLASHMODEL_H
#define NANDFLASHMODEL_H

/*----------------------------------------------------------------------------
 *        Forward declarations
 *----------------------------------------------------------------------------*/
struct NandSpareScheme;

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/

/**
  * \page NandFlashModel options
  * This page lists the possible options for a NandFlash chip.
  * 
  * !Options
  * - NandFlashModel_DATABUS8
  * - NandFlashModel_DATABUS16
  * - NandFlashModel_COPYBACK
*/

/// Indicates the Nand uses an 8-bit databus.
#define NandFlashModel_DATABUS8     (0 << 0)

/// Indicates the Nand uses a 16-bit databus.
#define NandFlashModel_DATABUS16    (1 << 0)

/// The Nand supports the copy-back function (internal page-to-page copy).
#define NandFlashModel_COPYBACK     (1 << 1)


/*----------------------------------------------------------------------------
 *        Types
 *----------------------------------------------------------------------------*/

/// Describes a particular model of NandFlash device.
struct NandFlashModel {

    /// Identifier for the device.
    unsigned char deviceId;
    /// Special options for the NandFlash.
    unsigned char options;
    /// Size of the data area of a page, in bytes.
    unsigned short pageSizeInBytes;
    /// Size of the device in MB.
    unsigned short deviceSizeInMegaBytes;
    /// Size of one block in kilobytes.
    unsigned short blockSizeInKBytes;
    /// Spare area placement scheme
    const struct NandSpareScheme *scheme;
};

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
extern unsigned char NandFlashModel_Find(
    const struct NandFlashModel *modelList,
    unsigned int size,
    unsigned int id,
    struct NandFlashModel *model);

extern unsigned char NandFlashModel_TranslateAccess(
    const struct NandFlashModel *model,
    unsigned int address,
    unsigned int size,
    unsigned short *block,
    unsigned short *page,
    unsigned short *offset);

extern const struct NandSpareScheme * NandFlashModel_GetScheme(
    const struct NandFlashModel *model);

extern unsigned char NandFlashModel_GetDeviceId(
    const struct NandFlashModel *model);

extern unsigned short NandFlashModel_GetDeviceSizeInBlocks(
    const struct NandFlashModel *model);

extern unsigned int NandFlashModel_GetDeviceSizeInPages(
    const struct NandFlashModel *model);

extern unsigned long long NandFlashModel_GetDeviceSizeInBytes(
    const struct NandFlashModel *model);

extern unsigned int NandFlashModel_GetDeviceSizeInMBytes(
    const struct NandFlashModel *model);

extern unsigned short NandFlashModel_GetBlockSizeInPages(
    const struct NandFlashModel *model);

extern unsigned int NandFlashModel_GetBlockSizeInBytes(
    const struct NandFlashModel *model);

extern unsigned short NandFlashModel_GetPageDataSize(
    const struct NandFlashModel *model);

extern unsigned char NandFlashModel_GetPageSpareSize(
    const struct NandFlashModel *model);

extern unsigned char NandFlashModel_GetDataBusWidth(
    const struct NandFlashModel *model);

extern unsigned char NandFlashModel_HasSmallBlocks(
    const struct NandFlashModel *model);

extern unsigned char NandFlashModel_SupportsCopyBack(
    const struct NandFlashModel *model);

#endif //#ifndef NANDFLASHMODEL_H

