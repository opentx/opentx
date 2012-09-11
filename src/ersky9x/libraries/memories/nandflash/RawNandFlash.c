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

/**
 * \file
 *
 * SkipBlockNandFlash layer supplies application a set of interface to operate nandflash.
 *
 */
 
/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "RawNandFlash.h"
#include "NandCommon.h"
#include "NandFlashModelList.h"
#include <pio/pio_it.h>
#include <utility/trace.h>
#include <utility/assert.h>

#include <string.h>

#if !defined(CHIP_NAND_CTRL)
/*----------------------------------------------------------------------------
 *        Internal definitions
 *----------------------------------------------------------------------------*/

/// Nand flash chip status codes
#define STATUS_READY                    (1 << 6)
#define STATUS_ERROR                    (1 << 0)

/// Nand flash commands
#define COMMAND_READ_1                  0x00
#define COMMAND_READ_2                  0x30
#define COMMAND_COPYBACK_READ_1         0x00
#define COMMAND_COPYBACK_READ_2         0x35
#define COMMAND_COPYBACK_PROGRAM_1      0x85
#define COMMAND_COPYBACK_PROGRAM_2      0x10
#define COMMAND_RANDOM_OUT              0x05
#define COMMAND_RANDOM_OUT_2            0xE0
#define COMMAND_RANDOM_IN               0x85
#define COMMAND_READID                  0x90
#define COMMAND_WRITE_1                 0x80
#define COMMAND_WRITE_2                 0x10
#define COMMAND_ERASE_1                 0x60
#define COMMAND_ERASE_2                 0xD0
#define COMMAND_STATUS                  0x70
#define COMMAND_RESET                   0xFF


/// Nand flash commands (small blocks)
#define COMMAND_READ_A                  0x00
#define COMMAND_READ_C                  0x50


/*----------------------------------------------------------------------------
 *        Internal Macros
 *----------------------------------------------------------------------------*/

#define ENABLE_CE(raw)        PIO_Clear(&(raw->pinChipEnable))
#define DISABLE_CE(raw)       PIO_Set(&(raw->pinChipEnable))

#define WRITE_COMMAND(raw, command) \
    {*((volatile unsigned char *) raw->commandAddress) = (unsigned char) command;}
#define WRITE_COMMAND16(raw, command) \
    {*((volatile unsigned short *) raw->commandAddress) = (unsigned short) command;}
#define WRITE_ADDRESS(raw, address) \
    {*((volatile unsigned char *) raw->addressAddress) = (unsigned char) address;}
#define WRITE_ADDRESS16(raw, address) \
    {*((volatile unsigned short *) raw->addressAddress) = (unsigned short) address;}
#define WRITE_DATA8(raw, data) \
    {*((volatile unsigned char *) raw->dataAddress) = (unsigned char) data;}
#define READ_DATA8(raw) \
    (*((volatile unsigned char *) raw->dataAddress))
#define WRITE_DATA16(raw, data) \
    {*((volatile unsigned short *) raw->dataAddress) = (unsigned short) data;}
#define READ_DATA16(raw) \
    (*((volatile unsigned short *) raw->dataAddress))

/// Internal cast macros
#define MODEL(raw)  ((struct NandFlashModel *) raw)

/// Number of tries for erasing a block
#define NUMERASETRIES           2
/// Number of tries for writing a block
#define NUMWRITETRIES           2
/// Number of tries for copying a block
#define NUMCOPYTRIES            2

/*----------------------------------------------------------------------------
 *        Internal functions
 *----------------------------------------------------------------------------*/
 
/**
 * \brief Sends the column address to the NandFlash chip.
 *
 * \param raw  Pointer to a RawNandFlash instance.
 * \param columnAddress  Column address to send.
 */
static void WriteColumnAddress(
    const struct RawNandFlash *raw,
    unsigned short columnAddress)
{
    unsigned short pageDataSize = NandFlashModel_GetPageDataSize(MODEL(raw));

    /* Check the data bus width of the NandFlash */
    if (NandFlashModel_GetDataBusWidth(MODEL(raw)) == 16) {
        /* Div 2 is because we address in word and not in byte */
        columnAddress >>= 1;
    }

    while (pageDataSize > 0) {
    
        if (NandFlashModel_GetDataBusWidth(MODEL(raw)) == 16) {
            WRITE_ADDRESS16(raw, columnAddress & 0xFF);
        }
        else {
            WRITE_ADDRESS(raw, columnAddress & 0xFF);
        }
        pageDataSize >>= 8;
        columnAddress >>= 8;
    }
}

/**
 * \brief Sends the row address to the NandFlash chip.
 *
 * \param raw  Pointer to a RawNandFlash instance.
 * \param rowAddress  Row address to send.
 */
static void WriteRowAddress(
    const struct RawNandFlash *raw,
    unsigned int rowAddress)
{
    unsigned int numPages = NandFlashModel_GetDeviceSizeInPages(MODEL(raw));

    while (numPages > 0) {
    
        if (NandFlashModel_GetDataBusWidth(MODEL(raw)) == 16) {
            WRITE_ADDRESS16(raw, rowAddress & 0xFF);
        }
        else {
            WRITE_ADDRESS(raw, rowAddress & 0xFF);
        }
        numPages >>= 8;
        rowAddress >>= 8;
    }
}

/**
 * \brief Waiting for the completion of a page program, erase and random read completion.
 *
 * \param raw  Pointer to a RawNandFlash instance.
 */
static void WaitReady(const struct RawNandFlash *raw)
{
    if (raw->pinReadyBusy.mask) {
        PIO_EnableIt(&(raw->pinReadyBusy));
        
        while (!PIO_Get(&(raw->pinReadyBusy)));
        //if (!PIO_Get(&(raw->pinReadyBusy)) {
            
        //}
    }
    else {
        WRITE_COMMAND(raw, COMMAND_STATUS);
        while ((READ_DATA8(raw) & STATUS_READY) != STATUS_READY);
    }
}

/**
 * \brief Check for program or erase operation is completed.
 *
 * \param raw  Pointer to a RawNandFlash instance.
 * \return 1 operation is completed; otherwise, return 0.
 */
static unsigned char IsOperationComplete(const struct RawNandFlash *raw)
{
    unsigned char status;

    WRITE_COMMAND(raw, COMMAND_STATUS);

    status = READ_DATA8(raw);

    if (((status & STATUS_READY) != STATUS_READY) || ((status & STATUS_ERROR) != 0)) {
        return 0;
    }
    return 1;
}

/**
 * \brief Sends data to the NandFlash chip from the provided buffer.
 *
 * \param raw  Pointer to a RawNandFlash instance.
 * \param size  Number of bytes that will be written.
 */
static void WriteData(
    const struct RawNandFlash *raw,
    unsigned char *buffer,
    unsigned int size)
{
    unsigned int i;

    // Check the data bus width of the NandFlash
    if (NandFlashModel_GetDataBusWidth(MODEL(raw)) == 16) {

        unsigned short *buffer16 = (unsigned short *) buffer;
        size >>= 1;

        for(i=0; i < size; i++) {

            WRITE_DATA16(raw, buffer16[i]);
        }
    }
    else {

        for(i=0; i < size; i++) {

            WRITE_DATA8(raw, buffer[i]);
        }
    }
}

/**
 * \brief Reads data from the NandFlash chip into the provided buffer.
 *
 * \param raw  Pointer to a RawNandFlash instance.
 * \param buffer  Buffer where the data will be stored.
 * \param size  Number of bytes that will be written.
 */
static void ReadData(
    const struct RawNandFlash *raw,
    unsigned char *buffer,
    unsigned int size)
{
    unsigned int i;

    // Check the chip data bus width
    if (NandFlashModel_GetDataBusWidth(MODEL(raw)) == 16) {

        unsigned short *buffer16 = (unsigned short *) buffer;
        size >>= 1;

        for (i=0; i < size; i++) {

            buffer16[i] = READ_DATA16(raw);
        }
    }
    else {

        for (i=0; i < size; i++) {

            buffer[i] = READ_DATA8(raw);
        }
    }
}

/**
 * \brief Erases the specified block of the device.
 *
 * \param raw  Pointer to a RawNandFlash instance.
 * \param block  Number of the physical block to erase.
 * \return 0 if successful; otherwise returns an error code.
 */
static unsigned char EraseBlock(
    const struct RawNandFlash *raw,
    unsigned short block)
{
    unsigned char error = 0;
    unsigned int rowAddress;

    TRACE_DEBUG("EraseBlock(%d)\r\n", block);

    // Calculate address used for erase
    rowAddress = block * NandFlashModel_GetBlockSizeInPages(MODEL(raw));

    // Start erase
    ENABLE_CE(raw);
    WRITE_COMMAND(raw, COMMAND_ERASE_1);
    WriteRowAddress(raw, rowAddress);
    WRITE_COMMAND(raw, COMMAND_ERASE_2);

    WaitReady(raw);
	#if !defined (OP_BOOTSTRAP_on)
    if (!IsOperationComplete(raw)) {
        TRACE_ERROR(
                 "EraseBlock: Could not erase block %d.\n\r",
                 block);
        error = NandCommon_ERROR_CANNOTERASE;
    }
	#endif

    DISABLE_CE(raw);

    return error;
}

/**
 * \brief Writes the data and/or the spare area of a page on a NandFlash chip.
 *
 * \param raw  Pointer to a RawNandFlash instance.
 * \param block  Number of the physical block to erase.
 * \param page  Number of the page to write inside the given block.
 * \param data  Buffer containing the data area.
 * \param spare  Buffer containing the spare area.
 * \return 0 if successful; otherwise returns an error code.
 * \note If one of the buffer pointer is 0, the corresponding area is not written.
 */
static unsigned char WritePage(
    const struct RawNandFlash *raw,
    unsigned short block,
    unsigned short page,
    void *data,
    void *spare)
{
    unsigned char error = 0;
    unsigned int pageDataSize = NandFlashModel_GetPageDataSize(MODEL(raw));
    unsigned int spareDataSize = NandFlashModel_GetPageSpareSize(MODEL(raw));
    unsigned short dummyByte;
    unsigned int rowAddress;

    TRACE_DEBUG("WritePage(B#%d:P#%d)\r\n", block, page);
    // Calculate physical address of the page
    rowAddress = block * NandFlashModel_GetBlockSizeInPages(MODEL(raw)) + page;

    // Start write operation
    ENABLE_CE(raw);

    // Write data area if needed
    if (data) {

        WRITE_COMMAND(raw, COMMAND_WRITE_1);
        WriteColumnAddress(raw, 0);
        WriteRowAddress(raw, rowAddress);
        WriteData(raw, (unsigned char *) data, pageDataSize);

        // Spare is written here as well since it is more efficient
        if (spare) {

            WriteData(raw, (unsigned char *) spare, spareDataSize);
        }
        else {
            // Note: special case when ECC parity generation. 
            // ECC results are available as soon as the counter reaches the end of the main area.
            // But when reach PageSize for an example, it could not generate last ECC_PR, The 
            // workaround is to receive PageSize+1 word.
            ReadData(raw, (unsigned char *) (&dummyByte), 2);
        }
        WRITE_COMMAND(raw, COMMAND_WRITE_2);

        WaitReady(raw);
        if (!IsOperationComplete(raw)) {
            TRACE_ERROR("WritePage: Failed writing data area.\n\r");
            error = NandCommon_ERROR_CANNOTWRITE;
        }
    }

    // Write spare area alone if needed
    if (spare && !data) {

        WRITE_COMMAND(raw, COMMAND_WRITE_1);
        WriteColumnAddress(raw, pageDataSize);
        WriteRowAddress(raw, rowAddress);
        WriteData(raw, (unsigned char *) spare, spareDataSize);
        WRITE_COMMAND(raw, COMMAND_WRITE_2);

        WaitReady(raw);
        if (!IsOperationComplete(raw)) {
            TRACE_ERROR("WritePage: Failed writing data area.\n\r");
            error = NandCommon_ERROR_CANNOTWRITE;
        }
    }

    // Disable chip
    DISABLE_CE(raw);

    return error;
}

/**
 * \brief Copies the data in a page of the NandFlash device to an other page on that same chip.
 *
 * \param raw  Pointer to a RawNandFlash instance.
 * \param sourceBlock  Source block number.
 * \param sourcePage  Source page number inside the source block.
 * \param destBlock  Destination block number.
 * \param destPage  Destination page number inside the destination block.
 * \return 0 if successful; otherwise returns an error code.
 * \note Both pages must have be even or odd; it is not possible to copy
 * and even page to an odd page and vice-versa.
 */
static unsigned char CopyPage(
    struct RawNandFlash *raw,
    unsigned short sourceBlock,
    unsigned short sourcePage,
    unsigned short destBlock,
    unsigned short destPage)
{
    unsigned short numPages = NandFlashModel_GetBlockSizeInPages(MODEL(raw));
    unsigned int sourceRow = sourceBlock * numPages + sourcePage;
    unsigned int destRow = destBlock * numPages + destPage;
    unsigned char error = 0;

    ASSERT((sourcePage & 1) == (destPage & 1),
           "CopyPage: Source and destination page must have the same parity.\n\r");

    TRACE_DEBUG("CopyPage(B#%d:P#%d -> B#%d:P#%d)\n\r",
              sourceBlock, sourcePage, destBlock, destPage);

    // Use the copy-back facility if available
    if (NandFlashModel_SupportsCopyBack(MODEL(raw))) {

        // Start operation
        ENABLE_CE(raw);

        // Start copy-back read
        WRITE_COMMAND(raw, COMMAND_COPYBACK_READ_1);
        WriteColumnAddress(raw, 0);
        WriteRowAddress(raw, sourceRow);
        WRITE_COMMAND(raw, COMMAND_COPYBACK_READ_2);
        WaitReady(raw);

        // Start copy-back write
        WRITE_COMMAND(raw, COMMAND_COPYBACK_PROGRAM_1);
        WriteColumnAddress(raw, 0);
        WriteRowAddress(raw, destRow);
        WRITE_COMMAND(raw, COMMAND_COPYBACK_PROGRAM_2);
        WaitReady(raw);

        // Check status
        if (!IsOperationComplete(raw)) {
            TRACE_ERROR("CopyPage: Failed to copy page.\n\r");
            error = NandCommon_ERROR_CANNOTCOPY;
        }

        // Finish operation
        DISABLE_CE(raw);
    }
    else {
        // Software copy
        if (RawNandFlash_ReadPage(raw, sourceBlock, sourcePage, RawNandFlash_GetDataBuffer(raw), RawNandFlash_GetSpareBuffer(raw))) {

            TRACE_ERROR("CopyPage: Failed to read page to copy\n\r");
            error = NandCommon_ERROR_CANNOTREAD;
        }
        else if (RawNandFlash_WritePage(raw, destBlock, destPage, RawNandFlash_GetDataBuffer(raw), RawNandFlash_GetSpareBuffer(raw))) {

            TRACE_ERROR("CopyPage: Failed to write dest. page\n\r");
            error = NandCommon_ERROR_CANNOTWRITE;
        }
    }

    return error;
}

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
 
/**
 * \brief Initializes a RawNandFlash instance based on the given model and physical interface.
 *
 * \param raw  Pointer to a RawNandFlash instance.
 * \param model  Pointer to the underlying nand chip model. Can be 0.
 * \param commandAddress  Address at which commands are sent.
 * \param addressAddress  Address at which addresses are sent.
 * \param dataAddress  Address at which data is sent.
 * \param pinChipEnable  Pin controlling the CE signal of the NandFlash.
 * \param pinReadyBusy  Pin used to monitor the ready/busy signal of the Nand.
 * \return 0 if successful; otherwise returns NandCommon_ERROR_UNKNOWNMODEL.
 */
unsigned char RawNandFlash_Initialize(
    struct RawNandFlash *raw,
    const struct NandFlashModel *model,
    unsigned int commandAddress,
    unsigned int addressAddress,
    unsigned int dataAddress,
    const Pin pinChipEnable,
    const Pin pinReadyBusy)
{
    TRACE_DEBUG("RawNandFlash_Initialize()\r\n");

    // Initialize fields
    raw->commandAddress = commandAddress;
    raw->addressAddress = addressAddress;
    raw->dataAddress = dataAddress;
    raw->pinChipEnable = pinChipEnable;
    raw->pinReadyBusy = pinReadyBusy;

    // Reset
    RawNandFlash_Reset(raw);

    // If model is not provided, autodetect it
    if (!model) {

        TRACE_DEBUG("No model provided, trying autodetection ...\n\r");
        if (NandFlashModel_Find(nandFlashModelList,
                                NandFlashModelList_SIZE,
                                RawNandFlash_ReadId(raw),
                                &(raw->model))) {

            TRACE_ERROR(
                      "RawNandFlash_Initialize: Could not autodetect chip.\n\r");
            return NandCommon_ERROR_UNKNOWNMODEL;
        }
    }
    else {

        // Copy provided model
        raw->model = *model;
    }

    raw->dataLock  = 0;
    raw->spareLock = 0;
    
    return 0;
}

/**
 * \brief Resets a NandFlash device.
 *
 * \param raw  Pointer to a RawNandFlash instance.
 */
void RawNandFlash_Reset(const struct RawNandFlash *raw)
{
    TRACE_DEBUG("RawNandFlash_Reset()\n\r");

    ENABLE_CE(raw);
    WRITE_COMMAND(raw, COMMAND_RESET);
    //WRITE_COMMAND16(raw, COMMAND_RESET);
    WaitReady(raw);
    DISABLE_CE(raw);
}

/**
 * \brief Reads and returns the identifiers of a NandFlash chip.
 *
 * \param raw  Pointer to a RawNandFlash instance.
 * \return id1|(id2<<8)|(id3<<16)|(id4<<24)
 */
unsigned int RawNandFlash_ReadId(const struct RawNandFlash *raw)
{
    unsigned int chipId;

    TRACE_DEBUG("RawNandFlash_ReadId()\n\r");

    ENABLE_CE(raw);
    //WRITE_COMMAND16(raw, COMMAND_READID);
    WRITE_COMMAND(raw, COMMAND_READID);
    WRITE_ADDRESS(raw, 0);
    chipId  = READ_DATA8(raw);
    chipId |= READ_DATA8(raw) << 8;
    chipId |= READ_DATA8(raw) << 16;
    chipId |= READ_DATA8(raw) << 24;
    DISABLE_CE(raw);

    return chipId;
}

 
/**
 * \brief Erases the specified block of the device, retrying several time if it fails.
 *
 * \param raw  Pointer to a RawNandFlash instance.
 * \param block  Number of the physical block to erase.
 * \return 0 if successful; otherwise returns NandCommon_ERROR_UNKNOWNMODEL.
 */
unsigned char RawNandFlash_EraseBlock(
    const struct RawNandFlash *raw,
    unsigned short block)
{
	#if !defined(OP_BOOTSTRAP_on)
    unsigned char numTries = NUMERASETRIES;

    TRACE_DEBUG("RawNandFlash_EraseBlock(B#%d)\n\r", block);

    while (numTries > 0) {

        if (!EraseBlock(raw, block)) {

            return 0;
        }
        numTries--;
    }

    TRACE_ERROR("RawNandFlash_EraseBlock: Failed to erase %d after %d tries\n\r",
                block, NUMERASETRIES);
    return NandCommon_ERROR_BADBLOCK;
	#else
	return EraseBlock(raw, block);
	#endif
}

/**
 * \brief Reads the data and/or the spare areas of a page of a NandFlash into the  provided buffers.
 *
 * \param raw  Pointer to a RawNandFlash instance.
 * \param block  Number of the physical block to read.
 * \param page  Number of the page to read inside the given block.
 * \param data  Buffer where the data area will be read.
 * \param spare  Buffer where the spare area will be read.
 * \return 0 if successful; otherwise returns 1.
 * \note If one of the buffer pointer is 0, the corresponding area is not read.
 */
unsigned char RawNandFlash_ReadPage(
    const struct RawNandFlash *raw,
    unsigned short block,
    unsigned short page,
    void *data,
    void *spare)
{
    unsigned char hasSmallBlocks = NandFlashModel_HasSmallBlocks(MODEL(raw));
    unsigned int pageDataSize = NandFlashModel_GetPageDataSize(MODEL(raw));
    unsigned int pageSpareSize = NandFlashModel_GetPageSpareSize(MODEL(raw));
    unsigned int colAddress;
    unsigned int rowAddress;

    ASSERT(data || spare, "RawNandFlash_ReadPage: At least one area must be read\n\r");
    TRACE_DEBUG("RawNandFlash_ReadPage(B#%d:P#%d)\r\n", block, page);

    // Calculate actual address of the page
    rowAddress = block * NandFlashModel_GetBlockSizeInPages(MODEL(raw)) + page;

    // Start operation
    ENABLE_CE(raw);

    if (data) {
        colAddress = 0;
    }
    else {
        // to read spare area in sequential access
        colAddress = pageDataSize;
    }

    // Use either small blocks or large blocks data area read
    if (hasSmallBlocks) {

        WRITE_COMMAND(raw, COMMAND_READ_A);
        WriteColumnAddress(raw, colAddress);
        WriteRowAddress(raw, rowAddress);
    }
    else {

        WRITE_COMMAND(raw, COMMAND_READ_1);
        WriteColumnAddress(raw, colAddress);
        WriteRowAddress(raw, rowAddress);
        WRITE_COMMAND(raw, COMMAND_READ_2);
    }

    // Wait for the nand to be ready
    WaitReady(raw);

    // Read data area if needed
    if (data) {
        WRITE_COMMAND(raw, COMMAND_READ_1);
        ReadData(raw, (unsigned char *) data, pageDataSize);

        if (spare) {
            ReadData(raw, (unsigned char *) spare, pageSpareSize);
        }
    }
    else {
        // Read spare area only
        WRITE_COMMAND(raw, COMMAND_READ_1);
        ReadData(raw, (unsigned char *) spare, pageSpareSize);
    }

    // Disable CE
    DISABLE_CE(raw);

    return 0;
}

/**
 * \brief Writes the data and/or the spare areas of a page of a NandFlash into the  provided buffers.
 *
 * \param raw  Pointer to a RawNandFlash instance.
 * \param block  Number of the physical block to write resides.
 * \param page Number of the page to write inside the given block.
 * \param data  Buffer where the data area will be stored.
 * \param spare  Buffer where the spare area will be stored.
 * \return 0 if successful; otherwise returns NandCommon_ERROR_BADBLOCK.
 * \note If one of the buffer pointer is 0, the corresponding area is not written.
 */
unsigned char RawNandFlash_WritePage(
    const struct RawNandFlash *raw,
    unsigned short block,
    unsigned short page,
    void *data,
    void *spare)
{
    unsigned char numTries = NUMWRITETRIES;

    TRACE_DEBUG("RawNandFlash_WritePage(B#%d:P#%d)\r\n", block, page);

    while (numTries > 0) {

        if (!WritePage(raw, block, page, data, spare)) {

            return 0;
        }
        numTries--;
    }

    TRACE_ERROR("RawNandFlash_WritePage: Failed to write page after %d tries\n\r", NUMWRITETRIES);
    return NandCommon_ERROR_BADBLOCK;
}

/**
 * \brief Copy the data in a page of the NandFlash device to an other page on that same chip.
 *
 * \param raw  Pointer to a RawNandFlash instance.
 * \param sourceBlock  Source block number.
 * \param sourcePage  Source page number inside the source block.
 * \param destBlock  Destination block number.
 * \param destPage  Destination page number inside the destination block.
 * \return 0 if successful; otherwise returns an NandCommon_ERROR_BADBLOCK.
 * \note Both pages must have be even or odd; it is not possible to copy
 * and even page to an odd page and vice-versa.
 */
unsigned char RawNandFlash_CopyPage(
    struct RawNandFlash *raw,
    unsigned short sourceBlock,
    unsigned short sourcePage,
    unsigned short destBlock,
    unsigned short destPage)
{
    unsigned char numTries = NUMCOPYTRIES;

    TRACE_DEBUG("RawNandFlash_CopyPage(B#%d:P#%d -> B#%d:P#%d)\n\r",
              sourceBlock, sourcePage, destBlock, destPage);

    while (numTries) {

        if (!CopyPage(raw, sourceBlock, sourcePage, destBlock, destPage)) {

            return 0;
        }
        numTries--;
    }

    TRACE_ERROR("RawNandFlash_CopyPage: Failed to copy page after %d tries\n\r", NUMCOPYTRIES);
    return NandCommon_ERROR_BADBLOCK;
}

/**
 * \brief Copies the data of one whole block of a NandFlash device to another block.
 *
 * \param raw  Pointer to a RawNandFlash instance.
 * \param sourceBlock  Source block number.
 * \param destBlock  Destination block number.
 * \return 0 if successful; otherwise returns an NandCommon_ERROR_BADBLOCK.
 */
unsigned char RawNandFlash_CopyBlock(
    struct RawNandFlash *raw,
    unsigned short sourceBlock,
    unsigned short destBlock)
{
    unsigned short numPages = NandFlashModel_GetBlockSizeInPages(MODEL(raw));
    unsigned int i;

    ASSERT(sourceBlock != destBlock,
           "RawNandFlash_CopyBlock: Source block must be different from dest block\n\r");
    TRACE_DEBUG("RawNandFlash_CopyBlock(B#%d->B#%d)\n\r",
              sourceBlock, destBlock);

    // Copy all pages
    for (i=0; i < numPages; i++) {

        if (RawNandFlash_CopyPage(raw, sourceBlock, i, destBlock, i)) {

            TRACE_ERROR(
                      "RawNandFlash_CopyBlock: Failed to copy page %u\n\r",
                      i);
            return NandCommon_ERROR_BADBLOCK;
        }
    }

    return 0;
}

unsigned char *RawNandFlash_GetDataBuffer(
    struct RawNandFlash *raw)
{
    ASSERT(raw->dataLock == 0, "Data buffer is busy\n\r")  
    raw->dataLock = 1;
    return raw->tmpData;
}

void RawNandFlash_ReleaseDataBuffer(
    struct RawNandFlash *raw)
{
    ASSERT(raw->dataLock == 1, "Data buffer is not busy\n\r")  
    raw->dataLock = 0;
}

unsigned char *RawNandFlash_GetSpareBuffer(
    struct RawNandFlash *raw)
{
    ASSERT(raw->spareLock == 0, "Spare buffer is busy\n\r")  
    raw->spareLock = 1;
    return raw->tmpSpare;
}

void RawNandFlash_ReleaseSpareBuffer(
    struct RawNandFlash *raw)
{
    ASSERT(raw->spareLock == 1, "Spare buffer is not busy\n\r")  
    raw->spareLock = 0;
}


#endif
