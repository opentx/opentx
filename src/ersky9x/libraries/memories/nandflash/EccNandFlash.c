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
//         Headers
//------------------------------------------------------------------------------

#include "EccNandFlash.h"
#include "NandCommon.h"
#include "NandSpareScheme.h"
#include <utility/trace.h>
#include <utility/hamming.h>
#include <utility/assert.h>
#ifdef HARDWARE_ECC
#include <hsmc4/hsmc4_ecc.h>
#endif 
#include <string.h>

//------------------------------------------------------------------------------
//         Internal definitions
//------------------------------------------------------------------------------

/// Casts
#define MODEL(ecc)  ((struct NandFlashModel *) ecc)
#define RAW(ecc)    ((struct RawNandFlash *) ecc)

    

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes an EccNandFlash instance.
/// \param ecc  Pointer to an EccNandFlash instance.
/// \param model  Pointer to the underlying nand chip model. Can be 0.
/// \param commandAddress  Address at which commands are sent.
/// \param addressAddress  Address at which addresses are sent.
/// \param dataAddress  Address at which data is sent.
/// \param pinChipEnable  Pin controlling the CE signal of the NandFlash.
/// \param pinReadyBusy  Pin used to monitor the ready/busy signal of the Nand.
//------------------------------------------------------------------------------
unsigned char EccNandFlash_Initialize(
    struct EccNandFlash *ecc,
    const struct NandFlashModel *model,
    unsigned int commandAddress,
    unsigned int addressAddress,
    unsigned int dataAddress,
    const Pin pinChipEnable,
    const Pin pinReadyBusy)
{
    unsigned char rc;
    rc = RawNandFlash_Initialize(RAW(ecc),
                                 model,
                                 commandAddress,
                                 addressAddress,
                                 dataAddress,
                                 pinChipEnable,
                                 pinReadyBusy);
#if defined(HARDWARE_ECC)
    {   unsigned int ecc_page;
        switch(NandFlashModel_GetPageDataSize(MODEL(ecc))) {
        case  512: ecc_page = AT91C_HSMC4_PAGESIZE_528_Bytes;  break;
        case 1024: ecc_page = AT91C_HSMC4_PAGESIZE_1056_Bytes; break;
        case 2048: ecc_page = AT91C_HSMC4_PAGESIZE_2112_Bytes; break;
        case 4096: ecc_page = AT91C_HSMC4_PAGESIZE_4224_Bytes; break;
        default:
            TRACE_ERROR("PageSize %d not compatible with ECC\n\r",
                        NandFlashModel_GetPageDataSize(MODEL(ecc)));
            return NandCommon_ERROR_ECC_NOT_COMPATIBLE;
        }
        HSMC4_EccConfigure(AT91C_ECC_TYPCORRECT_ONE_EVERY_256_BYTES,
                           ecc_page);
    }
#endif
    return rc;
}

//------------------------------------------------------------------------------
/// Reads the data and/or spare of a page of a nandflash chip, and verify that
/// the data is valid using the ECC information contained in the spare. If one
/// buffer pointer is 0, the corresponding area is not saved.
/// Returns 0 if the data has been read and is valid; otherwise returns either
/// NandCommon_ERROR_CORRUPTEDDATA or ...
/// \param ecc  Pointer to an EccNandFlash instance.
/// \param block  Number of block to read from.
/// \param page  Number of page to read inside given block.
/// \param data  Data area buffer.
/// \param spare  Spare area buffer.
//------------------------------------------------------------------------------
unsigned char EccNandFlash_ReadPage(
    struct EccNandFlash *ecc,
    unsigned short block,
    unsigned short page,
    void *data,
    void *spare)
{
    unsigned char error;
    unsigned short pageDataSize = NandFlashModel_GetPageDataSize(MODEL(ecc));
    unsigned char pageSpareSize = NandFlashModel_GetPageSpareSize(MODEL(ecc));
    unsigned char *pDataBuffer;
    unsigned char *pSpareBuffer;

    TRACE_DEBUG("EccNandFlash_ReadPage(B#%d:P#%d)\n\r", block, page);

    pDataBuffer = (data)   ? data  : RawNandFlash_GetDataBuffer(RAW(ecc));
    pSpareBuffer = (spare) ? spare : RawNandFlash_GetSpareBuffer(RAW(ecc));
    
#ifndef HARDWARE_ECC
    // Start by reading the spare and the data
    error = RawNandFlash_ReadPage(RAW(ecc), block, page, pDataBuffer, pSpareBuffer);
    if (error) {
       TRACE_ERROR("EccNandFlash_ReadPage: $page %d.%d\n\r",
                block, page);
        goto error;
    }

    // Retrieve ECC information from page and verify the data
    NandSpareScheme_ReadEcc(NandFlashModel_GetScheme(MODEL(ecc)), pSpareBuffer, ecc->hamming);
    error = Hamming_Verify256x(pDataBuffer, pageDataSize, ecc->hamming);
#else
    error = RawNandFlash_ReadPage(RAW(ecc), block, page, (unsigned char*)data, tmpSpare);
    if (error) {
        TRACE_ERROR("EccNandFlash_ReadPage: $page %d.%d\n\r",
                block, page);
        goto error;
    }

    // Retrieve ECC information from page
    NandSpareScheme_ReadEcc(NandFlashModel_GetScheme(MODEL(ecc)), tmpSpare, ecc->hsiaoInSpare);
    HSMC4_GetEccParity(pageDataSize, hsiao, NandFlashModel_GetDataBusWidth(MODEL(ecc)));
    // Verify the data
    error = HSMC4_VerifyHsiao((unsigned char*) data,
                              pageDataSize, 
                              ecc->hsiaoInSpare,
                              ecc->hsiao,
                              NandFlashModel_GetDataBusWidth(MODEL(ecc)));
#endif    
    if (error && (error != Hamming_ERROR_SINGLEBIT)) {
        error = NandCommon_ERROR_CORRUPTEDDATA;
        TRACE_ERROR("EccNandFlash_ReadPage: $page %d.%d\n\r",
                block, page);
        goto error;
    }
#ifndef HARDWARE_ECC
#else
     if (spare) {

        memcpy(spare, pSpareBuffer, pageSpareSize);
    }    
#endif
error: 
    if (data == (unsigned char *) 0)
      RawNandFlash_ReleaseDataBuffer(RAW(ecc));
    if (spare == (unsigned char *) 0)
        RawNandFlash_ReleaseSpareBuffer(RAW(ecc));
    return error;   
}

//------------------------------------------------------------------------------
/// Writes the data and/or spare area of a nandflash page, after calculating an
/// ECC for the data area and storing it in the spare. If no data buffer is
/// provided, the ECC is read from the existing page spare. If no spare buffer
/// is provided, the spare area is still written with the ECC information
/// calculated on the data buffer.
/// Returns 0 if successful; otherwise returns an error code.
/// \param ecc Pointer to an EccNandFlash instance.
/// \param block  Number of the block to write in.
/// \param page  Number of the page to write inside the given block.
/// \param data  Data area buffer, can be 0.
/// \param spare  Spare area buffer, can be 0.
//------------------------------------------------------------------------------
unsigned char EccNandFlash_WritePage(
    struct EccNandFlash *ecc,
    unsigned short block,
    unsigned short page,
    void *data,
    void *spare)
{
    unsigned char error;
    unsigned short pageDataSize = NandFlashModel_GetPageDataSize(MODEL(ecc));
    unsigned short pageSpareSize = NandFlashModel_GetPageSpareSize(MODEL(ecc));
    unsigned char *pSpareBuffer = RawNandFlash_GetSpareBuffer(RAW(ecc));

    ASSERT(data || spare, "EccNandFlash_WritePage: At least one area must be written\n\r");
    TRACE_DEBUG("EccNandFlash_WritePage(B#%d:P#%d)\n\r", block, page);
#ifndef HARDWARE_ECC
    // Compute ECC on the new data, if provided
    // If not provided, hamming code set to 0xFFFF.. to keep existing bytes
    memset(ecc->hamming, 0xFF, NandCommon_MAXSPAREECCBYTES);
    if (data) {

        // Compute hamming code on data
        Hamming_Compute256x(data, pageDataSize, ecc->hamming);
    }

    // Store code in spare buffer (if no buffer provided, use a temp. one)
    if (!spare) {

        spare = pSpareBuffer;
        memset(spare, 0xFF, pageSpareSize);
    }
    NandSpareScheme_WriteEcc(NandFlashModel_GetScheme(MODEL(ecc)), spare, ecc->hamming);

    // Perform write operation
    error = RawNandFlash_WritePage(RAW(ecc), block, page, data, spare);
    if (error)
        goto error;

#else
    // Store code in spare buffer (if no buffer provided, use a temp. one)
    if (!spare) {
        spare = pSpareBuffer;
        memset(spare, 0xFF, pageSpareSize);
    }
    // Perform write operation
    error = RawNandFlash_WritePage(RAW(ecc), block, page, data, spare);
    if (error)
      goto error;

    HSMC4_GetEccParity(pageDataSize, hsiao, NandFlashModel_GetDataBusWidth(MODEL(ecc)));
    // Perform write operation
    NandSpareScheme_WriteEcc(NandFlashModel_GetScheme(MODEL(ecc)), spare, hsiao);
    error = RawNandFlash_WritePage(RAW(ecc), block, page, 0, spare);
    if (error) 
      goto error;
#endif        
    RawNandFlash_ReleaseSpareBuffer(RAW(ecc));
    return 0;
    
error:
      RawNandFlash_ReleaseSpareBuffer(RAW(ecc));
      TRACE_ERROR("EccNandFlash_WritePage: Failed to write page\n\r");
      return error;
}


