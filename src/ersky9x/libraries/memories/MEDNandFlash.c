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

#include "MEDNandFlash.h"
#include "nandflash/NandCommon.h"
#include "nandflash/TranslatedNandFlash.h"
#include "nandflash/NandFlashModel.h"
#include <utility/trace.h>
#include <utility/math.h>
#include <utility/assert.h>

#include <string.h>

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

/// Casts
#define MODEL(interface)        ((struct NandFlashModel *) interface)
#define TRANSLATED(interface)   ((struct TranslatedNandFlash *) interface)

//------------------------------------------------------------------------------
//         Internal variables
//------------------------------------------------------------------------------

static unsigned char pageWriteBuffer[NandCommon_MAXPAGEDATASIZE];
static signed short currentWriteBlock;
static signed short currentWritePage;

static unsigned char pageReadBuffer[NandCommon_MAXPAGEDATASIZE];
static signed short currentReadBlock;
static signed short currentReadPage;

//------------------------------------------------------------------------------
//         Internal functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Writes the current page of data on the NandFlash.
/// Returns 0 if successful; otherwise returns 1.
/// \param tnf  Pointer to a TranslatedNandFlash instance.
//------------------------------------------------------------------------------
static unsigned char FlushCurrentPage(Media *media)
{
    // Make sure there is a page to flush
    if (currentWritePage == -1) {

        return 0;
    }

    TRACE_DEBUG("FlushCurrentPage(B#%d:P#%d)\n\r",
              currentWriteBlock, currentWritePage);

    // Write page
    if (TranslatedNandFlash_WritePage(TRANSLATED(media->interface),
                                      currentWriteBlock,
                                      currentWritePage,
                                      pageWriteBuffer,
                                      0)) {

        TRACE_ERROR("FlushCurrentPage: Failed to write page.\n\r");
        return 1;
    }

    // No current write page & block
    currentWriteBlock = -1;
    currentWritePage = -1;

    return 0;
}

//------------------------------------------------------------------------------
/// Writes data at an unaligned (page-wise) address and size. The address is
/// provided as the block & page number plus an offset. The data to write MUST
/// NOT span more than one page.
/// Returns 0 if the data has been written; 1 otherwise.
/// \param media  Pointer to a nandflash Media instance.
/// \param block  Number of the block to write.
/// \param page  Number of the page to write.
/// \param offset  Write offset.
/// \param buffer  Data buffer.
/// \param size  Number of bytes to write.
//------------------------------------------------------------------------------
static unsigned char UnalignedWritePage(
    Media *media,
    unsigned short block,
    unsigned short page,
    unsigned short offset,
    unsigned char *buffer,
    unsigned int size)
{
    unsigned char error;
    unsigned short pageDataSize = NandFlashModel_GetPageDataSize(MODEL(media->interface));
    unsigned char writePage = ((size + offset) == pageDataSize);

    TRACE_DEBUG("UnalignedWritePage(B%d:P%d@%d, %d)\n\r",
              block, page, offset, size);
    ASSERT((size + offset) <= pageDataSize,
           "UnalignedWrite: Write size and offset exceed page data size\n\r");

    // If size is 0, return immediately
    if (size == 0) {

        return 0;
    }

    // If this is not the current page, flush the previous one
    if ((currentWriteBlock != block) || (currentWritePage != page)) {

        // Flush and make page the new current page
        FlushCurrentPage(media);
        TRACE_DEBUG("Current write page: B#%d:P#%d\n\r", block, page);
        currentWriteBlock = block;
        currentWritePage = page;

        // Read existing page data in a temporary buffer if the page is not
        // entirely written
        if (size != pageDataSize) {

            error = TranslatedNandFlash_ReadPage(TRANSLATED(media->interface),
                                                 block,
                                                 page,
                                                 pageWriteBuffer,
                                                 0);
            if (error) {
    
                TRACE_ERROR(
                          "UnalignedWrite: Could not read existing page data\n\r");
                return 1;
            }
        }
    }

    // Copy data in temporary buffer
    memcpy(&(pageWriteBuffer[offset]), buffer, size);
    // Update read buffer if necessary
    if ((currentReadPage == currentWritePage)
        && (currentReadBlock == currentWriteBlock)) {

        TRACE_DEBUG("Updating current read buffer\n\r");
        memcpy(&(pageReadBuffer[offset]), buffer, size);
    }

    // Flush page if it is complete
    if (writePage) {

        FlushCurrentPage(media);
    }

    return 0;
}

//------------------------------------------------------------------------------
/// Writes a data buffer at the specified address on a NandFlash media. An
/// optional callback can be triggered after the transfer is completed.
/// Returns MED_STATUS_SUCCESS if the transfer has been started successfully;
/// otherwise returns MED_STATUS_ERROR.
/// \param media  Pointer to the NandFlash Media instance.
/// \param address  Address where the data shall be written.
/// \param data  Data buffer.
/// \param length  Number of bytes to write.
/// \param callback  Optional callback to call when the write is finished.
/// \param argument  Optional argument to the callback function.
//------------------------------------------------------------------------------
static unsigned char MEDNandFlash_Write(
    Media *media,
    unsigned int address,
    void *data,
    unsigned int length,
    MediaCallback callback,
    void *argument)
{
    unsigned short pageDataSize =
                NandFlashModel_GetPageDataSize(MODEL(media->interface));
    unsigned short blockSize =
                NandFlashModel_GetBlockSizeInPages(MODEL(media->interface));
    unsigned short block, page, offset;
    unsigned int writeSize;
    unsigned char *buffer = (unsigned char *) data;
    unsigned int remainingLength;
    unsigned char status;

    TRACE_INFO("MEDNandFlash_Write(0x%08X, %d)\n\r", address, length);

    // Translate access
    if (NandFlashModel_TranslateAccess(MODEL(media->interface),
                                       address,
                                       length,
                                       &block,
                                       &page,
                                       &offset)) {

        TRACE_ERROR("MEDNandFlash_Write: Could not start write.\n\r");
        return MED_STATUS_ERROR;
    }

    TRACE_DEBUG("MEDNandFlash_Write(B#%d:P#%d@%d, %d)\n\r",
              block, page, offset, length);

    // Write pages
    remainingLength = length;
    status = MED_STATUS_SUCCESS;
    while ((status == MED_STATUS_SUCCESS) && (remainingLength > 0)) {

        // Write one page
        writeSize = min(pageDataSize-offset, remainingLength);
        if (UnalignedWritePage(media, block, page, offset, buffer, writeSize)) {

            TRACE_ERROR("MEDNandFlash_Write: Failed to write page\n\r");
            status = MED_STATUS_ERROR;
        }
        else {

            // Update addresses
            remainingLength -= writeSize;
            buffer += writeSize;
            offset = 0;
            page++;
            if (page == blockSize) {
    
                page = 0;
                block++;
            }
        }
    }

    // Trigger callback
    if (callback) {

        callback(argument, status, length - remainingLength, remainingLength);
    }

    // Reset flush timer
    //AT91C_BASE_NANDFLUSHTIMER->TC_CCR = AT91C_TC_CLKEN | AT91C_TC_SWTRG;

    return status;
}

//------------------------------------------------------------------------------
/// Reads data at an unaligned address and/or size. The address is provided as
/// the block & page numbers plus an offset.
/// Returns 0 if the data has been read; otherwise returns 1.
/// \param media  Pointer to a nandflash Media instance.
/// \param block  Number of the block to read.
/// \param page  Number of the page to read.
/// \param offset  Read offset.
/// \param buffer  Buffer for storing data.
/// \param size  Number of bytes to read.
//------------------------------------------------------------------------------
static unsigned char UnalignedReadPage(
    Media *media,
    unsigned short block,
    unsigned short page,
    unsigned short offset,
    unsigned char *buffer,
    unsigned int size)
{
    unsigned char error;
    unsigned short pageDataSize = NandFlashModel_GetPageDataSize(MODEL(media->interface));

    TRACE_DEBUG("UnalignedReadPage(B%d:P%d@%d, %d)\n\r", block, page, offset, size);

    // Check that one page is read at most
    ASSERT((size + offset) <= pageDataSize,
           "UnalignedReadPage: Read size & offset exceed page data size\n\r");

    // Check if this is not the current read page
    if ((block != currentReadBlock) || (page != currentReadPage)) {

        TRACE_DEBUG("Current read page: B#%d:P#%d\n\r", block, page);
        currentReadBlock = block;
        currentReadPage = page;

        // Check if this is the current write page
        if ((currentReadBlock == currentWriteBlock)
            && (currentReadPage == currentWritePage)) {

            TRACE_DEBUG("Reading current write page\n\r");
            memcpy(pageReadBuffer, pageWriteBuffer, NandCommon_MAXPAGEDATASIZE);
        }
        else {

            // Read whole page into a temporary buffer
            error = TranslatedNandFlash_ReadPage(TRANSLATED(media->interface),
                                                 block,
                                                 page,
                                                 pageReadBuffer,
                                                 0);
            if (error) {

                TRACE_ERROR("UnalignedRead: Could not read page\n\r");
                return 1;
            }
        }
    }

    // Copy data into buffer
    memcpy(buffer, &(pageReadBuffer[offset]), size);

    return 0;
}

//------------------------------------------------------------------------------
/// Reads data at the specified address of a NandFlash media. An optional
/// callback is invoked when the transfer completes.
/// Returns 1 if the transfer has been started; otherwise returns 0.
/// \param media  Pointer to the NandFlash Media to read.
/// \param address  Address at which the data shall be read.
/// \param data  Data buffer.
/// \param length  Number of bytes to read.
/// \param callback  Optional callback function.
/// \param argument  Optional argument to the callback function.
//------------------------------------------------------------------------------
static unsigned char MEDNandFlash_Read(
    Media *media,
    unsigned int address,
    void *data,
    unsigned int length,
    MediaCallback callback,
    void *argument)
{
    unsigned short block, page, offset;
    unsigned short pageDataSize = NandFlashModel_GetPageDataSize(MODEL(media->interface));
    unsigned short blockSizeInPages = NandFlashModel_GetBlockSizeInPages(MODEL(media->interface));
    unsigned int remainingLength;
    unsigned int readSize;
    unsigned char *buffer = (unsigned char *) data;
    unsigned char status;

    TRACE_INFO("MEDNandFlash_Read(0x%08X, %d)\n\r", address, length);

    // Translate access into block, page and offset
    if (NandFlashModel_TranslateAccess(MODEL(media->interface),
                                       address,
                                       length,
                                       &block,
                                       &page,
                                       &offset)) {

        TRACE_ERROR("MEDNandFlash_Read: Cannot perform access\n\r");
        return MED_STATUS_ERROR;
    }

    // Read
    remainingLength = length;
    status = MED_STATUS_SUCCESS;
    while ((status == MED_STATUS_SUCCESS) && (remainingLength > 0)) {

        // Read page
        readSize = min(pageDataSize-offset, remainingLength);
        if (UnalignedReadPage(media, block, page, offset, buffer, readSize)) {

            TRACE_ERROR("MEDNandFlash_Read: Could not read page\n\r");
            status = MED_STATUS_ERROR;
        }
        else {
        
            // Update values
            remainingLength -= readSize;
            buffer += readSize;
            offset = 0;
            page++;
            if (page == blockSizeInPages) {
    
                page = 0;
                block++;
            }
        }
    }

    // Trigger callback
    if (callback) {

        callback(argument, status, length - remainingLength, remainingLength);
    }

    return status;
}

//------------------------------------------------------------------------------
/// Carries out all pending operations. Returns MED_STATUS_SUCCESS if
/// succesful; otherwise, returns MED_STATUS_ERROR.
/// \param media  Pointer to a NandFlash Media instance.
//------------------------------------------------------------------------------
static unsigned char MEDNandFlash_Flush(Media *media)
{
    TRACE_INFO("MEDNandFlash_Flush()\n\r");

    if (FlushCurrentPage(media)) {

        TRACE_ERROR("MEDNandFlash_Flush: Could not flush current page\n\r");
        return MED_STATUS_ERROR;
    }

    if (TranslatedNandFlash_Flush(TRANSLATED(media->interface))) {

        TRACE_ERROR("MEDNandFlash_Flush: Could not flush translated nand\n\r");
        return MED_STATUS_ERROR;
    }

    if (TranslatedNandFlash_SaveLogicalMapping(TRANSLATED(media->interface))) {

        TRACE_ERROR("MEDNandFlash_Flush: Could not save the logical mapping\n\r");
        return MED_STATUS_ERROR;
    }

    return MED_STATUS_SUCCESS;
}

//------------------------------------------------------------------------------
/// Interrupt handler for the nandflash media. Triggered when the flush timer
/// expires, initiating a MEDNandFlash_Flush().
/// \param media  Pointer to a nandflash Media instance.
//------------------------------------------------------------------------------
static void MEDNandFlash_InterruptHandler(Media *media)
{
    //volatile unsigned int dummy;

    TRACE_DEBUG("Flush timer expired\n\r");
    MEDNandFlash_Flush(media);

    // Acknowledge interrupt
    //dummy = AT91C_BASE_NANDFLUSHTIMER->TC_SR;
}

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes a media instance to operate on the given NandFlash device.
/// \param media  Pointer to a Media instance.
/// \param tnf  Pointer to the TranslatedNandFlash to use.
//------------------------------------------------------------------------------
void MEDNandFlash_Initialize(Media *media,
                             struct TranslatedNandFlash *translated)
{
    TRACE_INFO("MEDNandFlash_Initialize()\n\r");

    media->write = MEDNandFlash_Write;
    media->read = MEDNandFlash_Read;
    media->lock = 0;
    media->unlock = 0;
    media->flush = MEDNandFlash_Flush;
    media->handler = MEDNandFlash_InterruptHandler;

    media->interface = translated;

    media->baseAddress = 0;
    media->blockSize   = 1;
    media->size = TranslatedNandFlash_GetDeviceSizeInBytes(translated);
    
    TRACE_INFO("NF Size: %d\n\r", media->size);

    media->mappedRD  = 0;
    media->mappedWR  = 0;
    media->protected = 0;
    media->removable = 0;
    media->state = MED_STATE_READY;

    currentWriteBlock = -1;
    currentWritePage = -1;
    currentReadBlock = -1;
    currentReadPage = -1;

    // Configure flush timer
    /*
    AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_NANDFLUSHTIMER);
    AT91C_BASE_NANDFLUSHTIMER->TC_CCR = AT91C_TC_CLKDIS;
    AT91C_BASE_NANDFLUSHTIMER->TC_IDR = 0xFFFFFFFF;
    AT91C_BASE_NANDFLUSHTIMER->TC_CMR = AT91C_TC_CLKS_TIMER_DIV5_CLOCK
                                        | AT91C_TC_CPCSTOP
                                        | AT91C_TC_CPCDIS
                                        | AT91C_TC_WAVESEL_UP_AUTO
                                        | AT91C_TC_WAVE;
    AT91C_BASE_NANDFLUSHTIMER->TC_RC = 0xFFFF;
    AT91C_BASE_NANDFLUSHTIMER->TC_IER = AT91C_TC_CPCS;
    */
}

