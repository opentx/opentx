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

#include "TranslatedNandFlash.h"
#include <utility/trace.h>
#include <utility/assert.h>
#include <utility/math.h>

#include <string.h>

//------------------------------------------------------------------------------
//         Internal definitions
//------------------------------------------------------------------------------

/// Casts
#define MAPPED(translated)          ((struct MappedNandFlash *) translated)
#define MANAGED(translated)         ((struct ManagedNandFlash *) translated)
#define ECC(translated)             ((struct EccNandFlash *) translated)
#define RAW(translated)             ((struct RawNandFlash *) translated)
#define MODEL(translated)           ((struct NandFlashModel *) translated)

/// Minimum number of blocks that should be kept unallocated
#define MINNUMUNALLOCATEDBLOCKS     32

/// Maximum allowed erase count difference
#define MAXERASEDIFFERENCE          5

//------------------------------------------------------------------------------
//         Internal functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Returns 1 if there are enough free blocks to perform a single block
/// allocation; otherwise returns 0.
/// \param translated  Pointer to a TranslatedNandFlash instance.
//------------------------------------------------------------------------------
static unsigned char BlockCanBeAllocated(
    const struct TranslatedNandFlash *translated)
{
    unsigned short count;

    // Count number of free and dirty blocks (unallocated blocks)
    count = ManagedNandFlash_CountBlocks(MANAGED(translated), NandBlockStatus_DIRTY)
            + ManagedNandFlash_CountBlocks(MANAGED(translated), NandBlockStatus_FREE);

    // Check that count is greater than minimum number of unallocated blocks
    if (count > MINNUMUNALLOCATEDBLOCKS) {

        return 1;
    }
    else {

        return 0;
    }
}

//------------------------------------------------------------------------------
/// Returns 1 if the given page inside the currently written block is clean (has
/// not been written yet); otherwise returns 0.
/// \param translated  Pointer to a TranslatedNandFlash instance.
/// \param page  Page number.
//------------------------------------------------------------------------------
static unsigned char PageIsClean(
    const struct TranslatedNandFlash *translated,
    unsigned short page)
{
    ASSERT(page < NandFlashModel_GetBlockSizeInPages(MODEL(translated)),
           "PageIsClean: Page out-of-bounds\n\r");

    return ((translated->currentBlockPageStatuses[page / 8] >> (page % 8)) & 1) == 0;
}

//------------------------------------------------------------------------------
/// Marks the given page as being dirty (i.e. written).
/// \param translated  Pointer to a TranslatedNandFlash instance.
/// \param page  Page number.
//------------------------------------------------------------------------------
static void MarkPageDirty(
    struct TranslatedNandFlash *translated,
    unsigned short page)
{
    ASSERT(page < NandFlashModel_GetBlockSizeInPages(MODEL(translated)),
           "PageIsClean: Page out-of-bounds\n\r");

    translated->currentBlockPageStatuses[page / 8] |= 1 << (page % 8);
}

//------------------------------------------------------------------------------
/// Marks all pages as being clean.
/// \param translated  Pointer to a TranslatedNandFlash instance.
//------------------------------------------------------------------------------
static void MarkAllPagesClean(struct TranslatedNandFlash *translated)
{
    memset(translated->currentBlockPageStatuses, 0,
           sizeof(translated->currentBlockPageStatuses));
}

//------------------------------------------------------------------------------
/// Allocates the best-fitting physical block for the given logical block.
/// Returns 0 if successful; otherwise returns NandCommon_ERROR_NOBLOCKFOUND if
/// there are no more free blocks, or a NandCommon_ERROR code.
/// \param translated  Pointer to a TranslatedNandFlash instance.
/// \param block  Logical block number.
//------------------------------------------------------------------------------
static unsigned char AllocateBlock(
    struct TranslatedNandFlash *translated,
    unsigned short block)
{
    unsigned short freeBlock, liveBlock;
    unsigned char error;
    signed int eraseDifference;

    TRACE_DEBUG("Allocating a new block\n\r");

    // Find youngest free block and youngest live block
    if (ManagedNandFlash_FindYoungestBlock(MANAGED(translated),
                                           NandBlockStatus_FREE,
                                           &freeBlock)) {

        TRACE_ERROR("AllocateBlock: Could not find a free block\n\r");
        return NandCommon_ERROR_NOBLOCKFOUND;
    }

    // If this is the last free block, save the logical mapping in it and
    // clean dirty blocks
    TRACE_DEBUG("Number of FREE blocks: %d\n\r",
              ManagedNandFlash_CountBlocks(MANAGED(translated), NandBlockStatus_FREE));
    if (ManagedNandFlash_CountBlocks(MANAGED(translated),
                                     NandBlockStatus_FREE) == 1) {

        // Save mapping and clean dirty blocks
        TRACE_DEBUG("Last FREE block, cleaning up ...\n\r");

        error = MappedNandFlash_SaveLogicalMapping(MAPPED(translated), freeBlock);
        if (error) {

            TRACE_ERROR("AllocateBlock: Failed to save mapping\n\r");
            return error;
        }
        error = ManagedNandFlash_EraseDirtyBlocks(MANAGED(translated));
        if (error) {

            TRACE_ERROR("AllocatedBlock: Failed to erase dirty blocks\n\r");
            return error;
        }

        // Allocate new block
        return AllocateBlock(translated, block);
    }

    // Find youngest LIVE block to check the erase count difference
    if (!ManagedNandFlash_FindYoungestBlock(MANAGED(translated),
                                            NandBlockStatus_LIVE,
                                            &liveBlock)) {

        // Calculate erase count difference
        TRACE_DEBUG("Free block erase count = %d\n\r", MANAGED(translated)->blockStatuses[freeBlock].eraseCount);
        TRACE_DEBUG("Live block erase count = %d\n\r", MANAGED(translated)->blockStatuses[liveBlock].eraseCount);
        eraseDifference = absv(MANAGED(translated)->blockStatuses[freeBlock].eraseCount
                              - MANAGED(translated)->blockStatuses[liveBlock].eraseCount);

        // Check if it is too big
        if (eraseDifference > MAXERASEDIFFERENCE) {

            TRACE_WARNING("Erase difference too big, switching blocks\n\r");
            MappedNandFlash_Map(
                MAPPED(translated),
                MappedNandFlash_PhysicalToLogical(
                    MAPPED(translated),
                    liveBlock),
                freeBlock);
            ManagedNandFlash_CopyBlock(MANAGED(translated),
                                       liveBlock,
                                       freeBlock);

            // Allocate a new block
            return AllocateBlock(translated, block);
        }
    }

    // Map block
    TRACE_DEBUG("Allocating PB#%d for LB#%d\n\r", freeBlock, block);
    MappedNandFlash_Map(MAPPED(translated), block, freeBlock);

    return 0;
}

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes a TranslatedNandFlash instance.
/// Returns 0 if successful; otherwise returns a NandCommon_ERROR_xxx code.
/// \param translated  Pointer to a TranslatedNandFlash instance.
/// \param model  Pointer to the underlying nand chip model. Can be 0.
/// \param commandAddress  Address at which commands are sent.
/// \param addressAddress  Address at which addresses are sent.
/// \param dataAddress  Address at which data is sent.
/// \param pinChipEnable  Pin controlling the CE signal of the NandFlash.
/// \param pinReadyBusy  Pin used to monitor the ready/busy signal of the Nand.
/// \param baseBlock Basic physical block address of mapped area.
/// \param sizeInBlocks Number of blocks that is mapped.
//------------------------------------------------------------------------------
unsigned char TranslatedNandFlash_Initialize(
    struct TranslatedNandFlash *translated,
    const struct NandFlashModel *model,
    unsigned int commandAddress,
    unsigned int addressAddress,
    unsigned int dataAddress,
    const Pin pinChipEnable,
    const Pin pinReadyBusy,
    unsigned short baseBlock,
    unsigned short sizeInBlocks)
{
    translated->currentLogicalBlock = -1;
    translated->previousPhysicalBlock = -1;
    MarkAllPagesClean(translated);

    // Initialize MappedNandFlash
    return MappedNandFlash_Initialize(MAPPED(translated),
                                      model,
                                      commandAddress,
                                      addressAddress,
                                      dataAddress,
                                      pinChipEnable,
                                      pinReadyBusy,
                                      baseBlock,
                                      sizeInBlocks);
}

//------------------------------------------------------------------------------
/// Reads the data and/or the spare area of a page on a translated nandflash.
/// If the block is not currently mapped but could be (i.e. there are available
/// physical blocks), then the data/spare is filled with 0xFF.
/// Returns 0 if successful; otherwise returns NandCommon_ERROR_NOMOREBLOCKS
/// if no more block can be allocated, or a NandCommon_ERROR code.
/// \param translated  Pointer to a TranslatedNandFlash instance.
/// \param block  Logical block number.
/// \param page  Number of page to read inside logical block.
/// \param data  Data area buffer, can be 0.
/// \param spare  Spare area buffer, can be 0.
//------------------------------------------------------------------------------
unsigned char TranslatedNandFlash_ReadPage(
    const struct TranslatedNandFlash *translated,
    unsigned short block,
    unsigned short page,
    void *data,
    void *spare)
{
    unsigned char error;

    TRACE_INFO("TranslatedNandFlash_ReadPage(B#%d:P#%d)\n\r", block, page);

    // If the page to read is in the current block, there is a previous physical
    // block and the page is clean -> read the page in the old block since the
    // new one does not contain meaningful data
    if ((block == translated->currentLogicalBlock)
        && (translated->previousPhysicalBlock != -1)
        && (PageIsClean(translated, page))) {

        TRACE_DEBUG("Reading page from current block\n\r");
        return ManagedNandFlash_ReadPage(MANAGED(translated),
                                         translated->previousPhysicalBlock,
                                         page,
                                         data,
                                         spare);
    }
    else {

        // Try to read the page from the logical block
        error = MappedNandFlash_ReadPage(MAPPED(translated), block, page, data, spare);
    
        // Block was not mapped
        if (error == NandCommon_ERROR_BLOCKNOTMAPPED) {
    
            ASSERT(!spare, "Cannot read the spare information of an unmapped block\n\r");
    
            // Check if a block can be allocated
            if (BlockCanBeAllocated(translated)) {
    
                // Return 0xFF in buffers with no error
                TRACE_DEBUG("Block #%d is not mapped but can be allocated, filling buffer with 0xFF\n\r", block);
                if (data) {
    
                    memset(data, 0xFF, NandFlashModel_GetPageDataSize(MODEL(translated)));
                }
                if (spare) {
    
                    memset(spare, 0xFF, NandFlashModel_GetPageSpareSize(MODEL(translated)));
                }
            }
            else {

                TRACE_ERROR("Block #%d is not mapped and there are no more blocks available\n\r", block);
                return NandCommon_ERROR_NOMOREBLOCKS;
            }
        }
        // Error
        else if (error) {
    
            return error;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------
/// Writes the data and/or spare area of a page on a translated nandflash.
/// Allocates block has needed to keep the wear even between all blocks.
/// \param translated  Pointer to a TranslatedNandFlash instance.
/// \param block  Logical block number.
/// \param page  Number of page to write inside logical block.
/// \param data  Data area buffer, can be 0.
/// \param spare  Spare area buffer, can be 0.
//------------------------------------------------------------------------------
unsigned char TranslatedNandFlash_WritePage(
    struct TranslatedNandFlash *translated,
    unsigned short block,
    unsigned short page,
    void *data,
    void *spare)
{
    unsigned char allocate = 1;
    unsigned char error;

    TRACE_INFO("TranslatedNandFlash_WritePage(B#%d:P#%d)\n\r", block, page);

    // A new block must be allocated unless:
    // 1. the block is not mapped and there are no more blocks to allocate
    if (MappedNandFlash_LogicalToPhysical(MAPPED(translated), block) == -1) {

        // Block is not mapped, check if it can be
        if (!BlockCanBeAllocated(translated)) {

            TRACE_ERROR("TranslatedNandFlash_WritePage: Not enough free blocks\n\r");
            return NandCommon_ERROR_NOMOREBLOCKS;
        }
        TRACE_DEBUG("Allocate because block not mapped\n\r");
    }
    // or 2. the block to write is the current one and the page to write is
    // clean
    else if (translated->currentLogicalBlock == block) {

        if (PageIsClean(translated, page)) {

            TRACE_DEBUG("NO allocate because write in current block\n\r");
            allocate = 0;
        }
        else {

            TRACE_DEBUG("Allocate because page DIRTY in current block\n\r");
        }
    }
    else {

        TRACE_DEBUG("Allocate because block is mapped and different from current block\n\r");
    }

    // Allocate block if needed
    if (allocate) {

        // Flush current block write (if any) and then allocate block
        error = TranslatedNandFlash_Flush(translated);
        if (error) {

            return error;
        }
        translated->previousPhysicalBlock = MappedNandFlash_LogicalToPhysical(
                                                MAPPED(translated),
                                                block);
        TRACE_DEBUG("Previous physical block is now #%d\n\r",
                  translated->previousPhysicalBlock);
        error = AllocateBlock(translated, block);
        if (error) {

            return error;
        }

        // Block becomes the current block with all pages clean
        translated->currentLogicalBlock = block;
        MarkAllPagesClean(translated);
    }

    // Start writing page
    error = MappedNandFlash_WritePage(MAPPED(translated),
                                      block,
                                      page,
                                      data,
                                      spare);
    if (error) {

        return error;
    }

    // If write went through, mark page as written
    MarkPageDirty(translated, page);
    return 0;
}

//------------------------------------------------------------------------------
/// Terminates the current write operation by copying all the missing pages from
/// the previous physical block.
/// \param translated  Pointer to a TranslatedNandFlash instance.
//------------------------------------------------------------------------------
unsigned char TranslatedNandFlash_Flush(struct TranslatedNandFlash *translated)
{
    unsigned int i;
    unsigned char error;
    unsigned int currentPhysicalBlock;

    // Check if there is a current block and a previous block
    if ((translated->currentLogicalBlock == -1)
        || (translated->previousPhysicalBlock == -1)) {

        return 0;
    }

    TRACE_INFO("TranslatedNandFlash_Flush(PB#%d -> LB#%d)\n\r",
              translated->previousPhysicalBlock, translated->currentLogicalBlock);

    // Copy missing pages in the current block
    currentPhysicalBlock = MappedNandFlash_LogicalToPhysical(
                                MAPPED(translated),
                                translated->currentLogicalBlock);

    for (i=0; i < NandFlashModel_GetBlockSizeInPages(MODEL(translated)); i++) {

        if (PageIsClean(translated, i)) {

            TRACE_DEBUG("Copying back page #%d of block #%d\n\r", i,
                      translated->previousPhysicalBlock);

            // Copy page
            error = ManagedNandFlash_CopyPage(MANAGED(translated),
                                              translated->previousPhysicalBlock,
                                              i,
                                              currentPhysicalBlock,
                                              i);
            if (error) {

                TRACE_ERROR("FinishCurrentWrite: copy page #%d\n\r", i);
                return error;
            }
        }
    }

    translated->currentLogicalBlock = -1;
    translated->previousPhysicalBlock = -1;
    return 0;
}

//------------------------------------------------------------------------------
/// Erase all blocks in the tranalated area of nand flash.
/// \param managed  Pointer to a TranslatedNandFlash instance.
/// \param level    Erase level.
//------------------------------------------------------------------------------
unsigned char TranslatedNandFlash_EraseAll(
    struct TranslatedNandFlash *translated,
    unsigned char level)
{
    MappedNandFlash_EraseAll(MAPPED(translated), level);
    if (level > NandEraseDIRTY) {
        translated->currentLogicalBlock = -1;
        translated->previousPhysicalBlock = -1;
        MarkAllPagesClean(translated);
    }
    return 0;
}

//------------------------------------------------------------------------------
/// Allocates a free block to save the current logical mapping on it.
/// Returns 0 if successful; otherwise returns a NandCommon_ERROR code.
/// \param translated  Pointer to a TranslatedNandFlash instance.
//------------------------------------------------------------------------------
unsigned char TranslatedNandFlash_SaveLogicalMapping(
    struct TranslatedNandFlash *translated)
{
    unsigned char error;
    unsigned short freeBlock;

    TRACE_INFO("TranslatedNandFlash_SaveLogicalMapping()\n\r");

    // Save logical mapping in the youngest free block
    // Find the youngest block
    error = ManagedNandFlash_FindYoungestBlock(MANAGED(translated),
                                               NandBlockStatus_FREE,
                                               &freeBlock);
    if (error) {

        TRACE_ERROR("TranNF_SaveLogicalMapping: No free block\n\r");
        return error;
    }

    // Check if this is the last free block, in which case dirty blocks are wiped
    // prior to saving the mapping
    if (ManagedNandFlash_CountBlocks(MANAGED(translated),
                                     NandBlockStatus_FREE) == 1) {

        TranslatedNandFlash_Flush(translated);
        error = ManagedNandFlash_EraseDirtyBlocks(MANAGED(translated));
        if (error) {
        
            TRACE_ERROR("TranNF_Flush: Could not erase dirty blocks\n\r");
            return error;
        }
    }

    // Save the mapping
    error = MappedNandFlash_SaveLogicalMapping(MAPPED(translated), freeBlock);
    if (error) {

        TRACE_ERROR("TranNF_Flush: Failed to save mapping in #%d\n\r",
                    freeBlock);
        return error;
    }

    return 0;
}

//------------------------------------------------------------------------------
/// Returns the number of available blocks in a translated nandflash.
/// \param translated  Pointer to a TranslatedNandFlash instance.
//------------------------------------------------------------------------------
unsigned short TranslatedNandFlash_GetDeviceSizeInBlocks(
   const struct TranslatedNandFlash *translated)
{
    return ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(translated))
           - MINNUMUNALLOCATEDBLOCKS
           - ManagedNandFlash_CountBlocks(MANAGED(translated),
                                          NandBlockStatus_BAD)
           - 1; // Logical mapping block
}

//------------------------------------------------------------------------------
/// Returns the number of available pages in a translated nandflash.
/// \param translated  Pointer to a TranslatedNandFlash instance.
//------------------------------------------------------------------------------
unsigned int TranslatedNandFlash_GetDeviceSizeInPages(
   const struct TranslatedNandFlash *translated)
{
    return TranslatedNandFlash_GetDeviceSizeInBlocks(translated)
           * NandFlashModel_GetBlockSizeInPages(MODEL(translated));
}

//------------------------------------------------------------------------------
/// Returns the number of available data bytes in a translated nandflash.
/// \param translated  Pointer to a TranslatedNandFlash instance.
//------------------------------------------------------------------------------
unsigned long long TranslatedNandFlash_GetDeviceSizeInBytes(
   const struct TranslatedNandFlash *translated)
{
    return TranslatedNandFlash_GetDeviceSizeInPages(translated)
           * NandFlashModel_GetPageDataSize(MODEL(translated));
}

