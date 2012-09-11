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

#include "MappedNandFlash.h"
#include <utility/trace.h>
#include <utility/assert.h>
#include <utility/math.h>

#include <string.h>

//------------------------------------------------------------------------------
//         Internal definitions
//------------------------------------------------------------------------------

/// Casts
#define MANAGED(mapped) ((struct ManagedNandFlash *) mapped)
#define ECC(mapped)     ((struct EccNandFlash *) mapped)
#define RAW(mapped)     ((struct RawNandFlash *) mapped)
#define MODEL(mapped)   ((struct NandFlashModel *) mapped)

/// Logical block mapping pattern
#define PATTERN(i)      ((i << 1) & 0x73)

//------------------------------------------------------------------------------
//         Internal functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Scans a mapped nandflash to find an existing logical block mapping. If a
/// block contains the mapping, its index is stored in the provided variable (if
/// pointer is not 0).
/// Returns 0 if mapping has been found; otherwise returns
/// NandCommon_ERROR_NOMAPPING if no mapping exists, or another
/// NandCommon_ERROR_xxx code.
/// \param mapped  Pointer to a MappedNandFlash instance.
/// \param logicalMappingBlock  Pointer to a variable for storing the block
///                             number.
//------------------------------------------------------------------------------
static unsigned char FindLogicalMappingBlock(
    const struct MappedNandFlash *mapped,
    signed short *logicalMappingBlock)
{
    unsigned short block;
    unsigned char found;
    unsigned short numBlocks = ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped));
    unsigned short pageDataSize = NandFlashModel_GetPageDataSize(MODEL(mapped));
    unsigned char *pDataBuffer =  RawNandFlash_GetDataBuffer(RAW(mapped));
    unsigned char error;
    //unsigned char data[NandCommon_MAXPAGEDATASIZE];
    unsigned int i;
    
    TRACE_INFO("FindLogicalMappingBlock()~%d\n\r", numBlocks);

    // Search each LIVE block
    found = 0;
    block = 0;
    while (!found && (block < numBlocks)) {

        // Check that block is LIVE
        if (MANAGED(mapped)->blockStatuses[block].status == NandBlockStatus_LIVE) {

            // Read block
            TRACE_INFO("Checking LIVE block #%d\n\r", block);
            error = ManagedNandFlash_ReadPage(MANAGED(mapped), block, 0, pDataBuffer, 0);
            if (!error) {

                // Compare data with logical mapping pattern
                i = 0;
                found = 1;
                while ((i < pageDataSize) && found) {

                    if (pDataBuffer[i] != PATTERN(i)) {
    
                        found = 0;
                    }
                    i++;
                }
    
                // If this is the mapping, stop looking
                if (found) {
    
                    TRACE_WARNING_WP("-I- Logical mapping in block #%d\n\r",
                                     block);
                    if (logicalMappingBlock) {
    
                        *logicalMappingBlock = block;
                    }
                    RawNandFlash_ReleaseDataBuffer(RAW(mapped));
                    return 0;
                }
            }
            else if (error != NandCommon_ERROR_WRONGSTATUS) {
    
                TRACE_ERROR(
                          "FindLogicalMappingBlock: Failed to scan block #%d\n\r",
                          block);
                RawNandFlash_ReleaseDataBuffer(RAW(mapped));
                return error;
            }
        }

        block++;
    }

    RawNandFlash_ReleaseDataBuffer(RAW(mapped));
    TRACE_WARNING("No logical mapping found in device\n\r");
    return NandCommon_ERROR_NOMAPPING;
}

//------------------------------------------------------------------------------
/// Loads the logical mapping contained in the given physical block.
/// Returns 0 if successful; otherwise, returns a NandCommon_ERROR code.
/// \param mapped  Pointer to a MappedNandFlash instance.
/// \param physicalBlock  Physical block number.
//------------------------------------------------------------------------------
static unsigned char LoadLogicalMapping(
    struct MappedNandFlash *mapped,
    unsigned short physicalBlock)
{
    unsigned char error;
    unsigned short pageDataSize =
                    NandFlashModel_GetPageDataSize(MODEL(mapped));
    unsigned short numBlocks =
                    ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped));
    unsigned char *pDataBuffer =  RawNandFlash_GetDataBuffer(RAW(mapped));
    unsigned int remainingSize;
    unsigned char *currentBuffer;
    unsigned short currentPage;
    unsigned int readSize;
    unsigned int i;
    unsigned char status;
    signed short logicalBlock;
    //signed short firstBlock, lastBlock;

    TRACE_INFO("LoadLogicalMapping(B#%d)\n\r", physicalBlock);

    // Load mapping from pages #1 - #XXX of block
    currentBuffer = (unsigned char *) mapped->logicalMapping;
    remainingSize = sizeof(mapped->logicalMapping);
    currentPage = 1;
    while (remainingSize > 0) {

        // Read page
        readSize = min(remainingSize, pageDataSize);
        error = ManagedNandFlash_ReadPage(MANAGED(mapped),
                                          physicalBlock,
                                          currentPage,
                                          pDataBuffer,
                                          0);
        if (error) {

            RawNandFlash_ReleaseDataBuffer(RAW(mapped));
            TRACE_ERROR(
                      "LoadLogicalMapping: Failed to load mapping\n\r");
            return error;
        }

        // Copy page info
        memcpy(currentBuffer, pDataBuffer, readSize);

        currentBuffer += readSize;
        remainingSize -= readSize;
        currentPage++;
    }

    // Store mapping block index
    mapped->logicalMappingBlock = physicalBlock;

    // Power-loss recovery
    for (i=0; i < numBlocks; i++) {

        // Check that this is not the logical mapping block
        if (i != physicalBlock) {

            status = mapped->managed.blockStatuses[i].status;
            logicalBlock = MappedNandFlash_PhysicalToLogical(mapped, i);

            // Block is LIVE
            if (status == NandBlockStatus_LIVE) {

                // Block is not mapped -> release it
                if (logicalBlock == -1) {

                    TRACE_WARNING_WP("-I- Release unmapped LIVE #%d\n\r",
                                     i);
                    ManagedNandFlash_ReleaseBlock(MANAGED(mapped), i);
                }
            }
            // Block is DIRTY
            else if (status == NandBlockStatus_DIRTY) {

                // Block is mapped -> fake it as live
                if (logicalBlock != -1) {

                    TRACE_WARNING_WP("-I- Mark mapped DIRTY #%d -> LIVE\n\r",
                                     i);
                    mapped->managed.blockStatuses[i].status =
                                                    NandBlockStatus_LIVE;
                }
            }
            // Block is FREE or BAD
            else {

                // Block is mapped -> remove it from mapping
                if (logicalBlock != -1) {

                    TRACE_WARNING_WP("-I- Unmap FREE or BAD #%d\n\r", i);
                    mapped->logicalMapping[logicalBlock] = -1;
                }
            }
        }
    }

    RawNandFlash_ReleaseDataBuffer(RAW(mapped));
    TRACE_WARNING_WP("-I- Mapping loaded from block #%d\n\r", physicalBlock);

    return 0;
}

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes a MappedNandFlash instance. Scans the device to look for and
/// existing logical block mapping; otherwise starts from scratch (no block
/// mapped).
/// Returns 0 if successful; otherwise returns a NandCommon_ERROR_xxx code.
/// \param mapped  Pointer to a MappedNandFlash instance.
/// \param model  Pointer to the underlying nand chip model. Can be 0.
/// \param commandAddress  Address at which commands are sent.
/// \param addressAddress  Address at which addresses are sent.
/// \param dataAddress  Address at which data is sent.
/// \param pinChipEnable  Pin controlling the CE signal of the NandFlash.
/// \param pinReadyBusy  Pin used to monitor the ready/busy signal of the Nand.
/// \param baseBlock Basic physical block address of mapped area.
/// \param sizeInBlocks Number of blocks that is mapped.
//------------------------------------------------------------------------------
unsigned char MappedNandFlash_Initialize(
    struct MappedNandFlash *mapped,
    const struct NandFlashModel *model,
    unsigned int commandAddress,
    unsigned int addressAddress,
    unsigned int dataAddress,
    const Pin pinChipEnable,
    const Pin pinReadyBusy,
    unsigned short baseBlock,
    unsigned short sizeInBlocks)
{
    unsigned char error;
    unsigned short numBlocks;
    unsigned short block;
    signed short logicalMappingBlock = 0;

    TRACE_INFO("MappedNandFlash_Initialize()\n\r");

    // Initialize ManagedNandFlash
    error = ManagedNandFlash_Initialize(MANAGED(mapped),
                                        model,
                                        commandAddress,
                                        addressAddress,
                                        dataAddress,
                                        pinChipEnable,
                                        pinReadyBusy,
                                        baseBlock,
                                        sizeInBlocks);
    if (error) {

        return error;
    }

    // Scan to find logical mapping
    mapped->mappingModified = 0;
    error = FindLogicalMappingBlock(mapped, &logicalMappingBlock);
    if (!error) {

        // Extract mapping from block
        mapped->logicalMappingBlock = logicalMappingBlock;
        return LoadLogicalMapping(mapped, logicalMappingBlock);
    }
    else if (error == NandCommon_ERROR_NOMAPPING) {

        // Start with no block mapped
        mapped->logicalMappingBlock = -1;
        numBlocks = ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped));
        for (block=0; block < numBlocks; block++) {

            mapped->logicalMapping[block] = -1;
        }
    }
    else {
        
        TRACE_ERROR("MappedNandFlash_Initialize: Initialize device\n\r");
        return error;
    }

    return 0;
}

//------------------------------------------------------------------------------
/// Reads the data and/or spare area of a page in a mapped logical block.
/// Returns 0 if successful; otherwise, returns NandCommon_ERROR_BLOCKNOTMAPPED
/// if the block is not mapped, or a NandCommon_ERROR_xxx code.
/// \param mapped  Pointer to a MappedNandFlash instance.
/// \param block  Number of logical block to read page from.
/// \param page  Number of page to read inside given block.
/// \param data  Data area buffer, can be 0.
/// \param spare  Spare area buffer, can be 0.
//------------------------------------------------------------------------------
unsigned char MappedNandFlash_ReadPage(
    const struct MappedNandFlash *mapped,
    unsigned short block,
    unsigned short page,
    void *data,
    void *spare)
{
    signed short physicalBlock;

    TRACE_INFO("MappedNandFlash_ReadPage(LB#%d:P#%d)\n\r", block, page);

    // Check if block is mapped
    physicalBlock = mapped->logicalMapping[block];
    if (physicalBlock == -1) {

        TRACE_INFO( "MappedNandFlash_ReadPage: Block %d not mapped\n\r", block);
        return NandCommon_ERROR_BLOCKNOTMAPPED;
    }

    // Read page from corresponding physical block
    return ManagedNandFlash_ReadPage(MANAGED(mapped),
                                     physicalBlock,
                                     page,
                                     data,
                                     spare);
}

//------------------------------------------------------------------------------
/// Writes the data and/or spare area of a page in the given mapped logical
/// block.
/// Returns 0 if successful; otherwise, returns NandCommon_ERROR_BLOCKNOTMAPPED
/// (indicating the block is not logically mapped) or another NandCommon_ERROR
/// code.
/// \param mapped  Pointer to a MappedNandFlash instance.
/// \param block  Number of logical block to write on.
/// \param page  Number of page to write inside given block.
/// \param data  Data area buffer, can be 0.
/// \param spare  Spare area buffer, can be 0.
//------------------------------------------------------------------------------
unsigned char MappedNandFlash_WritePage(
    const struct MappedNandFlash *mapped,
    unsigned short block,
    unsigned short page,
    void *data,
    void *spare)
{
    signed short physicalBlock;

    TRACE_INFO("MappedNandFlash_WritePage(LB#%d:P#%d)\n\r", block, page);

    // Check if block is mapped
    physicalBlock = mapped->logicalMapping[block];
    if (physicalBlock == -1) {

        TRACE_ERROR("MappedNandFlash_WritePage: Block must be mapped\n\r");
        return NandCommon_ERROR_BLOCKNOTMAPPED;
    }

    // Write page on physical block
    return ManagedNandFlash_WritePage(MANAGED(mapped),
                                      physicalBlock,
                                      page,
                                      data,
                                      spare);
}

//------------------------------------------------------------------------------
/// Maps a logical block number to an actual physical block. This allocates
/// the physical block (meaning it must be FREE), and releases the previous
/// block being replaced (if any).
/// Returns 0 if successful; otherwise returns a NandCommon_ERROR_xxx code.
/// \param mapped  Pointer to a MappedNandFlash instance.
/// \param logicalBlock  Logical block number to map.
/// \param physicalBlock  Physical block to map to the logical one.
//------------------------------------------------------------------------------
unsigned char MappedNandFlash_Map(
    struct MappedNandFlash *mapped,
    unsigned short logicalBlock,
    unsigned short physicalBlock)
{
    unsigned char error;
    signed short oldPhysicalBlock;

    TRACE_INFO("MappedNandFlash_Map(LB#%d -> PB#%d)\n\r",
               logicalBlock, physicalBlock);
    ASSERT(
       logicalBlock < ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped)),
       "MappedNandFlash_Map: logicalBlock out-of-range\n\r");
    ASSERT(
       physicalBlock < ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped)),
       "MappedNandFlash_Map: physicalBlock out-of-range\n\r");

    // Allocate physical block
    error = ManagedNandFlash_AllocateBlock(MANAGED(mapped), physicalBlock);
    if (error) {

        return error;
    }

    // Release currently mapped block (if any)
    oldPhysicalBlock = mapped->logicalMapping[logicalBlock];
    if (oldPhysicalBlock != -1) {

        error =
            ManagedNandFlash_ReleaseBlock(MANAGED(mapped), oldPhysicalBlock);
        if (error) {

            return error;
        }                                      
    }

    // Set mapping
    mapped->logicalMapping[logicalBlock] = physicalBlock;
    mapped->mappingModified = 1;

    return 0;
}

//------------------------------------------------------------------------------
/// Unmaps a logical block by releasing the corresponding physical block (if 
/// any).
/// Returns 0 if successful; otherwise returns a NandCommon_ERROR code.
/// \param mapped  Pointer to a MappedNandFlash instance.
/// \param logicalBlock  Number of logical block to unmap.
//------------------------------------------------------------------------------
unsigned char MappedNandFlash_Unmap(
    struct MappedNandFlash *mapped,
    unsigned short logicalBlock)
{
    signed short physicalBlock = mapped->logicalMapping[logicalBlock];
    unsigned char error;

    TRACE_INFO("MappedNandFlash_Unmap(LB#%d)\n\r", logicalBlock);
    ASSERT(
        logicalBlock < ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped)),
        "MappedNandFlash_Unmap: logicalBlock out-of-range\n\r");

    if (physicalBlock != -1) {

        error = ManagedNandFlash_ReleaseBlock(MANAGED(mapped), physicalBlock);
        if (error) {

            return error;
        }
    }
    mapped->logicalMapping[logicalBlock] = -1;
    mapped->mappingModified = 1;

    return 0;
}

//------------------------------------------------------------------------------
/// Returns the physical block mapped with the given logical block, or -1 if it
/// is not mapped.
/// \param mapped  Pointer to a MappedNandFlash instance.
/// \param logicalBlock  Logical block number.
//------------------------------------------------------------------------------
signed short MappedNandFlash_LogicalToPhysical(
    const struct MappedNandFlash *mapped,
    unsigned short logicalBlock)
{
    ASSERT(
        logicalBlock < ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped)),
        "MappedNandFlash_LogicalToPhysical: logicalBlock out-of-range\n\r");

    return mapped->logicalMapping[logicalBlock];
}

//------------------------------------------------------------------------------
/// Returns the logical block which is mapped to given physical block, or -1 if
/// the latter is not mapped.
/// \param mapped  Pointer to a MappedNandFlash instance.
/// \param physicalBlock  Physical block number.
//------------------------------------------------------------------------------
signed short MappedNandFlash_PhysicalToLogical(
    const struct MappedNandFlash *mapped,
    unsigned short physicalBlock)
{
    unsigned short numBlocks =
                    ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped));
    signed short logicalBlock;

    ASSERT(
       physicalBlock < ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped)),
       "MappedNandFlash_PhysicalToLogical: physicalBlock out-of-range\n\r");

    // Search the mapping for the desired physical block
    for (logicalBlock=0; logicalBlock < numBlocks; logicalBlock++) {

        if (mapped->logicalMapping[logicalBlock] == physicalBlock) {

            return logicalBlock;
        }
    }

    return -1;
}

//------------------------------------------------------------------------------
/// Saves the logical mapping on a FREE, unmapped physical block. Allocates the
/// new block, releases the previous one (if any) and save the mapping.
/// Returns 0 if successful; otherwise, returns NandCommon_ERROR_WRONGSTATUS
/// if the block is not LIVE, or a NandCommon_ERROR code.
/// \param mapped  Pointer to a MappedNandFlash instance.
/// \param physicalBlock  Physical block number.
//------------------------------------------------------------------------------
unsigned char MappedNandFlash_SaveLogicalMapping(
    struct MappedNandFlash *mapped,
    unsigned short physicalBlock)
{
    unsigned char error;
    //unsigned char data[NandCommon_MAXPAGEDATASIZE];
    unsigned short pageDataSize =
                    NandFlashModel_GetPageDataSize(MODEL(mapped));
    unsigned char *pDataBuffer;
    //unsigned short numBlocks =
    //                ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped));
    unsigned int i;
    unsigned int remainingSize;
    unsigned char *currentBuffer;
    unsigned short currentPage;
    unsigned int writeSize;
    signed short previousPhysicalBlock;

    TRACE_INFO("MappedNandFlash_SaveLogicalMapping(B#%d)\n\r", physicalBlock);

    // If mapping has not been modified, do nothing
    if (!mapped->mappingModified) {
        return 0;
    }
    pDataBuffer =  RawNandFlash_GetDataBuffer(RAW(mapped));
    
    // Allocate new block
    error = ManagedNandFlash_AllocateBlock(MANAGED(mapped), physicalBlock);
    if (error) {
        goto error;
    }

    // Save mapping
    previousPhysicalBlock = mapped->logicalMappingBlock;
    mapped->logicalMappingBlock = physicalBlock;

    // Save actual mapping in pages #1-#XXX
    currentBuffer = (unsigned char *) mapped->logicalMapping;
    remainingSize = sizeof(mapped->logicalMapping);
    currentPage = 1;
    while (remainingSize > 0) {

        writeSize = min(remainingSize, pageDataSize);
        memset(pDataBuffer, 0xFF, pageDataSize);
        memcpy(pDataBuffer, currentBuffer, writeSize);
        error = ManagedNandFlash_WritePage(MANAGED(mapped),
                                           physicalBlock,
                                           currentPage,
                                           pDataBuffer,
                                           0);
        if (error) {
            TRACE_ERROR(
             "MappedNandFlash_SaveLogicalMapping: Failed to write mapping\n\r");
            goto error;
        }

        currentBuffer += writeSize;
        remainingSize -= writeSize;
        currentPage++;
    }

    // Mark page #0 of block with a distinguishible pattern, so the mapping can
    // be retrieved at startup
    for (i=0; i < pageDataSize; i++) {

        pDataBuffer[i] = PATTERN(i);
    }
    error = ManagedNandFlash_WritePage(MANAGED(mapped),
                                       physicalBlock, 0,
                                       pDataBuffer, 0);
    if (error) {
        TRACE_ERROR(
            "MappedNandFlash_SaveLogicalMapping: Failed to write pattern\n\r");
        goto error;
    }

    // Mapping is not modified anymore
    mapped->mappingModified = 0;

    // Release previous block (if any)
    if (previousPhysicalBlock != -1) {

        TRACE_DEBUG("Previous physical block was #%d\n\r",
                    previousPhysicalBlock);
        error = ManagedNandFlash_ReleaseBlock(MANAGED(mapped),
                                              previousPhysicalBlock);
        if (error) {
            goto error;
        }
    }

    TRACE_INFO("Mapping saved on block #%d\n\r", physicalBlock);

error:    
    RawNandFlash_ReleaseDataBuffer(RAW(mapped));
    return 0;
}

//------------------------------------------------------------------------------
/// Erase all blocks in the mapped area of nand flash.
/// \param managed  Pointer to a MappedNandFlash instance.
/// \param level    Erase level.
//------------------------------------------------------------------------------
unsigned char MappedNandFlash_EraseAll(
    struct MappedNandFlash *mapped,
    unsigned char level)
{
    unsigned int block;
    ManagedNandFlash_EraseAll(MANAGED(mapped), level);
    // Reset to no block mapped
    if (level > NandEraseDIRTY) {
        mapped->logicalMappingBlock = -1;
        mapped->mappingModified = 0;
        for (block=0;
             block < ManagedNandFlash_GetDeviceSizeInBlocks(MANAGED(mapped));
             block++) {
            mapped->logicalMapping[block] = -1;
        }
    }
    return 0;
}
