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
//#define DMA_TRANSFER

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "RawNandFlash.h"
#include "NandCommon.h"
#include "NandFlashModelList.h"
#include <hsmc4/hsmc4.h>
#ifdef DMA_TRANSFER
#include <dma/dma.h>
#include <dmad/dmad.h>
#endif
#include <utility/trace.h>
#include <utility/assert.h>

#include <string.h>
#if defined(CHIP_NAND_CTRL)

//------------------------------------------------------------------------------
//         Internal definitions
//------------------------------------------------------------------------------

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


//------------------------------------------------------------------------------
//         Internal macros
//------------------------------------------------------------------------------
#define READ_DATA8(raw) \
    (*((volatile unsigned char *) raw->dataAddress))
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


//------------------------------------------------------------------------------
//         Internal functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Translates the given column and row address into first and other (1-4) address
/// cycles. The resulting values are stored in the provided variables if they are not null.
/// \param columnAddress  Column address to translate.
/// \param rowAddress  Row address to translate.
/// \param pAddressCycle0  First address cycle.
/// \param pAddressCycle1234 four address cycles.
//------------------------------------------------------------------------------
void NFC_TranslateAddress(
    const struct RawNandFlash *raw,
    unsigned short columnAddress,
    unsigned int rowAddress,
    unsigned int *pAddressCycle0,
    unsigned int *pAddressCycle1234,
    unsigned char useFiveAddress)
{
    unsigned short pageDataSize = NandFlashModel_GetPageDataSize(MODEL(raw));
    unsigned int numPages = NandFlashModel_GetDeviceSizeInPages(MODEL(raw));
    unsigned char numAddressCycles = 0;
    unsigned int addressCycle0 = 0;
    unsigned int addressCycle1234 = 0;
    
    // Check the data bus width of the NandFlash 
    if (NandFlashModel_GetDataBusWidth(MODEL(raw)) == 16) {
        // Div 2 is because we address in word and not in byte 
        columnAddress >>= 1;
    }
    // When 5 address cycle are used.
    if (useFiveAddress) {
        
        while (pageDataSize > 0) {
            if (numAddressCycles == 0) {
                addressCycle0 = (columnAddress & 0xFF);
            }
            else {
                addressCycle1234 |= (columnAddress & 0xFF) << ((numAddressCycles - 1) * 8);
            }
            pageDataSize >>= 8;
            columnAddress >>= 8;
            numAddressCycles ++;
        }
        while (numPages > 0) {
            if (numAddressCycles == 0) {
                addressCycle0 = (rowAddress & 0xFF);
            }
            else {
                addressCycle1234 |= (rowAddress & 0xFF) << ((numAddressCycles - 1) * 8);
            }
            numPages >>= 8;
            rowAddress >>= 8;
            numAddressCycles ++;
        }
    }
    // When less than 5 address cycle are used.    
    else {
        while (numPages > 0) {
            addressCycle1234 |= (rowAddress & 0xFF) << ((numAddressCycles) * 8);
            numPages >>= 8;
            rowAddress >>= 8;
            numAddressCycles ++;
        }   
    }
    // Store values
    if (pAddressCycle0) {
        *pAddressCycle0 = addressCycle0;
    }
    if (pAddressCycle1234) {
        *pAddressCycle1234 = addressCycle1234;
    }
    return;
}


//------------------------------------------------------------------------------
/// Return 1 if program or erase operation is completed.
/// and the program or erase operation is completed successfully, otherwise return 0.
/// \param raw  Pointer to a RawNandFlash instance.
//------------------------------------------------------------------------------
static unsigned char IsOperationComplete(const struct RawNandFlash *raw)
{
    unsigned char status;

    HSMC4_SendCommand(AT91C_HSMC4_HOSTCMD |               // Command.
                   0 |                                    // Host read data.
                   0 |                                    // Host is disabled.
                   AT91C_HSMC4_CSID_1 |                   // CSID.
                   AT91C_HSMC4_ACYCLE_HSMC4_ACYCLE_NONE | // No address cycle.
                   (COMMAND_STATUS << 2),                 // CMD1 (COMMAND_STATUS).
                   0,                                     // Dummy address cylce 1,2,3,4.
                   0                                      // Dummy address cylce 0.
    );
    status = READ_DATA8(raw);
    if (((status & STATUS_READY) != STATUS_READY) || ((status & STATUS_ERROR) != 0)) {
        return 0;
    }
    return 1;
}

//------------------------------------------------------------------------------
/// Sends data to the NandFlash chip from the provided buffer.
/// \param raw  Pointer to a RawNandFlash instance.
/// \param buffer  Buffer where the data area will be stored.
/// \param sramOffset  NFC internal sram start offset.
/// \param size   Number of data bytes to send.
//------------------------------------------------------------------------------
static void CopyDataToNfcInternalSram(
    const struct RawNandFlash *raw,
    unsigned char *data,
    unsigned short sramOffset,
    unsigned short size)
{
#ifdef DMA_TRANSFER
    unsigned int startSourceAddr;
    unsigned int startDestAddr;
    
    // Initialize DMA controller.
    DMAD_Initialize(BOARD_NAND_DMA_CHANNEL, 0);
    startSourceAddr = (unsigned int)data;
    startDestAddr = (unsigned int)(NFC_SRAM_BASE_ADDRESS + sramOffset);
    // Configure transfer size and width per transfer.
    DMAD_Configure_TransferController(BOARD_NAND_DMA_CHANNEL, size / 4, 2, 2, startSourceAddr, startDestAddr);
    // Configure single buffer transfer.
    DMAD_Configure_Buffer(BOARD_NAND_DMA_CHANNEL, DMA_TRANSFER_SINGLE, DMA_TRANSFER_SINGLE, 0, 0);
    // Start channel 0 transfer.
    DMAD_BufferTransfer(BOARD_NAND_DMA_CHANNEL, size / 4, 0 , 1);
    
#else        
    unsigned char * pBuffer;
    unsigned int i;
    pBuffer = (unsigned char *)(NFC_SRAM_BASE_ADDRESS + sramOffset);
    for (i = 0; i < size; i++) {
        *pBuffer++ = *data++;
    }
#endif    
}
//------------------------------------------------------------------------------
/// Sends data to the NandFlash chip from the provided buffer.
/// \param raw  Pointer to a RawNandFlash instance.
/// \param data  Buffer where the data area will be stored.
/// \param sramOffset  NFC internal sram start offset.
/// \param size   Number of data bytes to receive.
//------------------------------------------------------------------------------
static void CopyDataFromNfcInternalSram(
    const struct RawNandFlash *raw,
    unsigned char *data,
    unsigned short sramOffset,
    unsigned short size)
{
    
#ifdef DMA_TRANSFER
    unsigned int startSourceAddr;
    unsigned int startDestAddr;

    // Initialize DMA controller.
    DMAD_Initialize(BOARD_NAND_DMA_CHANNEL, 0);
    
    startSourceAddr = (unsigned int)(NFC_SRAM_BASE_ADDRESS + sramOffset);
    startDestAddr = (unsigned int)data;
    
    // Configure transfer size and width per transfer.
    DMAD_Configure_TransferController(BOARD_NAND_DMA_CHANNEL, size / 4, 2, 2, startSourceAddr, startDestAddr);
    // Configure single buffer transfer.
    DMAD_Configure_Buffer(BOARD_NAND_DMA_CHANNEL, DMA_TRANSFER_SINGLE, DMA_TRANSFER_SINGLE, 0, 0);
    // Start channel 0 transfer.
    DMAD_BufferTransfer(BOARD_NAND_DMA_CHANNEL, size / 4, 0 , 1);
    
#else
    unsigned char * pBuffer;
    unsigned int i;
    pBuffer = (unsigned char *)(NFC_SRAM_BASE_ADDRESS + sramOffset);
    for (i = 0; i < size; i++) {
        *data++ = *pBuffer++;
    }
#endif    
}

//------------------------------------------------------------------------------
/// Erases the specified block of the device. Returns 0 if the operation was
/// successful; otherwise returns an error code.
/// \param raw  Pointer to a RawNandFlash instance.
/// \param block  Number of the physical block to erase.
//------------------------------------------------------------------------------
static unsigned char EraseBlock(
    const struct RawNandFlash *raw,
    unsigned short block)
{
    unsigned char error = 0;
    unsigned int rowAddress;
    unsigned int addressCycle1234;
    
    TRACE_DEBUG("EraseBlock(%d)\r\n", block);

    // Calculate address used for erase
    rowAddress = block * NandFlashModel_GetBlockSizeInPages(MODEL(raw));
    
    NFC_TranslateAddress(raw, 0, rowAddress, 0, &addressCycle1234, 0);

    // Start erase
    HSMC4_SendCommand (AT91C_HSMC4_HOSTCMD |                // Command.
                    0 |                                     // Host read data.
                    0 |                                     // Host is disabled.
                    AT91C_HSMC4_CSID_1 |                    // CSID.
                    AT91C_HSMC4_ACYCLE_HSMC4_ACYCLE_THREE | // Number of address cycle.
                    AT91C_HSMC4_VCMD2 |                     // CMD2 enabled.
                    (COMMAND_ERASE_2 << 10) |               // CMD2 (ERASE CONFIRME).
                    (COMMAND_ERASE_1 << 2),                 // CMD1 (ERASE).
                    addressCycle1234,                       // Address cylce 1, 2, 3, 4.
                    0                                       // Dummy address cylce 0.
                    );
                    
    while( !HSMC4_isReadyBusy() );
    #if !defined (OP_BOOTSTRAP_on)
    if (!IsOperationComplete(raw)) {
        TRACE_ERROR(
                 "EraseBlock: Could not erase block %d.\n\r",
                 block);
        error = NandCommon_ERROR_CANNOTERASE;
    }
    #endif
    return error;
}

//------------------------------------------------------------------------------
/// Writes the data and/or the spare area of a page on a NandFlash chip. If one
/// of the buffer pointer is 0, the corresponding area is not written.
/// Returns 0 if the write operation is successful; otherwise returns 1.
/// \param raw  Pointer to a RawNandFlash instance.
/// \param block  Number of the block where the page to write resides.
/// \param page  Number of the page to write inside the given block.
/// \param data  Buffer containing the data area.
/// \param spare  Buffer containing the spare area.
//------------------------------------------------------------------------------
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
    unsigned int rowAddress;
    unsigned int addressCycle0;
    unsigned int addressCycle1234;

    TRACE_DEBUG("WritePage(B#%d:P#%d)\r\n", block, page);

    // Calculate physical address of the page
    rowAddress = block * NandFlashModel_GetBlockSizeInPages(MODEL(raw)) + page;
    /*
    if (spare){
        HSMC4_EnableSpareWrite();
    }
    else {
        HSMC4_DisableSpareWrite();
    }     
    */
    // Note: special case when ECC parity generation. 
    // ECC results are available as soon as the counter reaches the end of the main area.
    // But when reach PageSize for an example, it could not generate last ECC_PR, The 
    // workaround enable SPARE_WRITE, whatever real spare area write or not.
    HSMC4_EnableSpareWrite();
        
    // Write data area if needed
    if (data) {
        CopyDataToNfcInternalSram(raw, (unsigned char *) data, 0, pageDataSize);
        if (spare) {
            CopyDataToNfcInternalSram(raw, (unsigned char *) spare, pageDataSize, spareDataSize);
        }
        NFC_TranslateAddress(raw, 0, rowAddress, &addressCycle0, &addressCycle1234, 1);
    }
    if (spare && !data) {
        CopyDataToNfcInternalSram(raw, (unsigned char *) spare, 0, spareDataSize);
        NFC_TranslateAddress(raw, pageDataSize, rowAddress, &addressCycle0, &addressCycle1234, 1);
    }
    
    if (data || spare) {
        // Start write operation
        HSMC4_SendCommand (AT91C_HSMC4_HOSTCMD |                // Command.
                        AT91C_HSMC4_HOST_WR |                   // Host write data.
                        AT91C_HSMC4_HOST_EN |                   // Host is enabled.
                        AT91C_HSMC4_CSID_1 |                    // CSID.
                        AT91C_HSMC4_ACYCLE_HSMC4_ACYCLE_FIVE |  // Number of address cycle.
                        0 |                                     // CMD2 disabled.
                        0 |                                     // CMD2.
                        (COMMAND_WRITE_1 << 2),                 // CMD1.
                        addressCycle1234,                       // Address cylce 1, 2, 3, 4.
                        addressCycle0                           // Address cylce 0.
                    );
        while( !HSMC4_TransferComplete());
        
        HSMC4_SendCommand (AT91C_HSMC4_HOSTCMD |                // Command.
                        0 |                                     // No data Transfer.
                        0 |                                     // Host is disabled.
                        AT91C_HSMC4_CSID_1 |                    // CSID.
                        AT91C_HSMC4_ACYCLE_HSMC4_ACYCLE_NONE |  // No address cycle.
                        0 |                                     // CMD2 disabled.
                        0 |                                     // CMD2.
                        (COMMAND_WRITE_2 << 2),                 // CMD1.
                        0,                                      // Dummy address cylce 1, 2, 3, 4.
                        0                                       // Dummy address cylce 0.
                        );
        while( !HSMC4_isReadyBusy());
        if (!IsOperationComplete(raw)) {
            TRACE_ERROR("WritePage: Failed writing data area.\n\r");
            error = NandCommon_ERROR_CANNOTWRITE;
        }
    }
    HSMC4_DisableSpareWrite();
    return error;
}


//------------------------------------------------------------------------------
/// Copies the data in a page of the NandFlash device to an other page on that
/// same chip. Both pages must have be even or odd; it is not possible to copy
/// and even page to an odd page and vice-versa.
/// Returns 0 if the operation is successful; otherwise returns a
/// NandCommon_ERROR code.
/// \param raw  Pointer to a RawNandFlash instance.
/// \param sourceBlock  Source block number.
/// \param sourcePage  Source page number inside the source block.
/// \param destBlock  Destination block number.
/// \param destPage  Destination page number inside the destination block.
//------------------------------------------------------------------------------
static unsigned char CopyPage(
    const struct RawNandFlash *raw,
    unsigned short sourceBlock,
    unsigned short sourcePage,
    unsigned short destBlock,
    unsigned short destPage)
{
    unsigned short numPages = NandFlashModel_GetBlockSizeInPages(MODEL(raw));
    unsigned int sourceRow = sourceBlock * numPages + sourcePage;
    unsigned int destRow = destBlock * numPages + destPage;
    unsigned char error = 0;
    unsigned int addressCycle0;
    unsigned int addressCycle1234;
    
    ASSERT((sourcePage & 1) == (destPage & 1),
           "CopyPage: Source and destination page must have the same parity.\n\r");

    TRACE_DEBUG("CopyPage(B#%d:P#%d -> B#%d:P#%d)\n\r",
              sourceBlock, sourcePage, destBlock, destPage);

    // Use the copy-back facility if available
    if (NandFlashModel_SupportsCopyBack(MODEL(raw))) {

        // Start operation
        NFC_TranslateAddress(raw, 0, sourceRow, &addressCycle0, &addressCycle1234, 1);
        // Start copy-back read
        HSMC4_SendCommand (AT91C_HSMC4_HOSTCMD |               // Command.
                    0 |                                     // Host read data.
                    0 |                                     // Host is disabled.
                    AT91C_HSMC4_CSID_1 |                    // CSID.
                    AT91C_HSMC4_ACYCLE_HSMC4_ACYCLE_FIVE |  // Number of address cycle.
                    AT91C_HSMC4_VCMD2 |                     // CMD2 enabled.
                    (COMMAND_COPYBACK_READ_2 << 10)|        // CMD2.
                    (COMMAND_COPYBACK_READ_1 << 2),         // CMD1.
                    addressCycle1234,                       // Address cylce 1, 2, 3, 4.
                    addressCycle0                           // Address cylce 0.
                    );
        while( !HSMC4_isReadyBusy() );

        // Start copy-back write
        NFC_TranslateAddress(raw, 0, destRow, &addressCycle0, &addressCycle1234, 1);
        HSMC4_SendCommand (AT91C_HSMC4_HOSTCMD |            // Command.
                    0 |                                     // No data transfer.
                    0 |                                     // Host is disabled. 
                    AT91C_HSMC4_CSID_1 |                    // CSID.
                    AT91C_HSMC4_ACYCLE_HSMC4_ACYCLE_FIVE |  // Number of address cycle.
                    AT91C_HSMC4_VCMD2 |                     // CMD2 enabled.
                    (COMMAND_COPYBACK_PROGRAM_2 << 10)|     // CMD2.
                    (COMMAND_COPYBACK_PROGRAM_1 << 2),      // CMD1.
                    addressCycle1234,                       // Address cylce 1, 2, 3, 4.
                    addressCycle0                           // Address cylce 0.
                    );
        while( !HSMC4_isReadyBusy() );

        // Check status
        if (!IsOperationComplete(raw)) {
            TRACE_ERROR("CopyPage: Failed to copy page.\n\r");
            error = NandCommon_ERROR_CANNOTCOPY;
        }
    }
    else {

        // Software copy
        unsigned char data[NandCommon_MAXPAGEDATASIZE];
        unsigned char spare[NandCommon_MAXPAGESPARESIZE];
        if (RawNandFlash_ReadPage(raw, sourceBlock, sourcePage, data, spare)) {

            TRACE_ERROR("CopyPage: Failed to read page to copy\n\r");
            error = NandCommon_ERROR_CANNOTREAD;
        }
        else if (RawNandFlash_WritePage(raw, destBlock, destPage, data, spare)) {

            TRACE_ERROR("CopyPage: Failed to write dest. page\n\r");
            error = NandCommon_ERROR_CANNOTWRITE;
        }
    }

    return error;
}

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes a RawNandFlash instance based on the given model and physical
/// interface. If no model is provided, then the function tries to autodetect
/// it.
/// Returns 0 if initialization is successful; otherwise returns
/// NandCommon_ERROR_UNKNOWNMODEL.
/// \param raw  Pointer to a RawNandFlash instance.
/// \param model  Pointer to the underlying nand chip model. Can be 0.
/// \param commandAddress  Address at which commands are sent.
/// \param addressAddress  Address at which addresses are sent.
/// \param dataAddress  Address at which data is sent.
/// \param pinChipEnable  Pin controlling the CE signal of the NandFlash.
/// \param pinReadyBusy  Pin used to monitor the ready/busy signal of the Nand.
//------------------------------------------------------------------------------
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

    return 0;
}

//------------------------------------------------------------------------------
/// Resets a NandFlash device.
/// \param raw  Pointer to a RawNandFlash instance.
//------------------------------------------------------------------------------
void RawNandFlash_Reset(const struct RawNandFlash *raw)
{
    TRACE_DEBUG("RawNandFlash_Reset()\n\r");
    HSMC4_SendCommand(AT91C_HSMC4_HOSTCMD |              // Command.
                   0 |                                   // Host read data.
                   0 |                                   // Host is disabled.
                  AT91C_HSMC4_CSID_1 |                   // CSID.
                  AT91C_HSMC4_ACYCLE_HSMC4_ACYCLE_NONE | // No address Cycle.
                  (COMMAND_RESET << 2),                  // CMD1 (COMMAND_RESET).
                  0,                                     // Dummy address cylce 1,2,3,4.
                  0                                      // Dummy address cylce 0.
    );
    while( !HSMC4_isReadyBusy() );
}

//------------------------------------------------------------------------------
/// Reads and returns the identifiers of a NandFlash chip.
/// \param raw  Pointer to a RawNandFlash instance.
/// \return id1|(id2<<8)|(id3<<16)|(id4<<24)
//------------------------------------------------------------------------------
unsigned int RawNandFlash_ReadId(const struct RawNandFlash *raw)
{
    unsigned int chipId;

    TRACE_DEBUG("RawNandFlash_ReadId()\n\r");
    HSMC4_SendCommand(AT91C_HSMC4_HOSTCMD |              // Command.
                   0 |                                   // Host read Data.
                   0 |                                   // Host is disabled.
                  AT91C_HSMC4_CSID_1 |                   // CSID.
                  AT91C_HSMC4_ACYCLE_HSMC4_ACYCLE_ONE |  // One address Cycle.
                  (COMMAND_READID << 2),                 // CMD1 (COMMAND_READID).
                  0,                                     // Dummy address cylce 1,2,3,4.
                  0                                      // Dummy address cylce 0.
    );
    chipId  = READ_DATA8(raw);
    chipId |= READ_DATA8(raw) << 8;
    chipId |= READ_DATA8(raw) << 16;
    chipId |= READ_DATA8(raw) << 24;
    return chipId;
}

//------------------------------------------------------------------------------
/// Erases the specified block of the device, retrying several time if it fails.
/// Returns 0 if successful; otherwise returns NandCommon_ERROR_BADBLOCK.
/// \param raw  Pointer to a RawNandFlash instance.
/// \param block  Number of the physical block to erase.
//------------------------------------------------------------------------------
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

    TRACE_ERROR("RawNandFlash_EraseBlock: Failed to erase block after %d tries\n\r", NUMERASETRIES);
    return NandCommon_ERROR_BADBLOCK;
    #else
    return EraseBlock(raw, block);
    #endif
}

//------------------------------------------------------------------------------
/// Reads the data and/or the spare areas of a page of a NandFlash into the
/// provided buffers. If a buffer pointer is 0, the corresponding area is not
/// read.
/// Returns 0 if the operation has been successful; otherwise returns 1.
/// \param raw  Pointer to a RawNandFlash instance.
/// \param block  Number of the block where the page to read resides.
/// \param page  Number of the page to read inside the given block.
/// \param data  Buffer where the data area will be stored.
/// \param spare  Buffer where the spare area will be stored.
//------------------------------------------------------------------------------
unsigned char RawNandFlash_ReadPage(
    const struct RawNandFlash *raw,
    unsigned short block,
    unsigned short page,
    void *data,
    void *spare)
{
    volatile unsigned int cntTry = 0;
    
    unsigned char hasSmallBlocks = NandFlashModel_HasSmallBlocks(MODEL(raw));
    unsigned int pageDataSize = NandFlashModel_GetPageDataSize(MODEL(raw));
    unsigned int pageSpareSize = NandFlashModel_GetPageSpareSize(MODEL(raw));
    unsigned int colAddress;
    unsigned int rowAddress;
    unsigned int addressCycle0;
    unsigned int addressCycle1234;
    ASSERT(data || spare, "RawNandFlash_ReadPage: At least one area must be read\n\r");
    TRACE_DEBUG("RawNandFlash_ReadPage(B#%d:P#%d)\r\n", block, page);
    // Calculate actual address of the page
    rowAddress = block * NandFlashModel_GetBlockSizeInPages(MODEL(raw)) + page;
    // Start operation
    if (data) {
        colAddress = 0;
    }
    else {
        // to read spare area in sequential access
        colAddress = pageDataSize;
    }

    if (spare) {
        HSMC4_EnableSpareRead();
    }
    else {
        HSMC4_DisableSpareRead();
    }
    NFC_TranslateAddress(raw, colAddress, rowAddress, &addressCycle0, &addressCycle1234, 1);
    // Use either small blocks or large blocks data area read
    if (hasSmallBlocks) {
        HSMC4_SendCommand (AT91C_HSMC4_HOSTCMD |            // Command.
                    0 |                                     // Host read data.
                    AT91C_HSMC4_HOST_EN |                   // Host is enable.
                    AT91C_HSMC4_CSID_1 |                    // CSID.
                    AT91C_HSMC4_ACYCLE_HSMC4_ACYCLE_FIVE |  // Number of address cycle.
                    0 |                                     // CMD2 disabled.
                    0 |                                     // CMD2.
                    (COMMAND_READ_A << 2),                  // CMD1.
                    addressCycle1234,                       // Address cylce 1, 2, 3, 4.
                    addressCycle0                           // Address cylce 0.
                    );
    }
    else {
        HSMC4_SendCommand (AT91C_HSMC4_HOSTCMD |            // Command.
                    0 |                                     // Host read data.
                    AT91C_HSMC4_HOST_EN |                   // Host is enabled.
                    AT91C_HSMC4_CSID_1 |                    // CSID.
                    AT91C_HSMC4_ACYCLE_HSMC4_ACYCLE_FIVE |  // Number of address cycle.
                    AT91C_HSMC4_VCMD2 |                     // CMD2 enabled.
                    (COMMAND_READ_2 << 10)|                 // CMD2.
                    (COMMAND_READ_1 << 2),                  // CMD1.
                    addressCycle1234,                       // Address cylce 1, 2, 3, 4.
                    addressCycle0                           // Address cylce 0.
                    );
    }
    // Wait for the nand to be ready
    cntTry = 0;
    while( !HSMC4_isReadyBusy() && (cntTry++) < 1000000);
    cntTry = 0;
    while( !HSMC4_TransferComplete() && (cntTry++) < 1000000);
    // Read data area if needed
    if (data) {
        CopyDataFromNfcInternalSram(raw, (unsigned char *) data, 0, pageDataSize);
        if (spare) {
            CopyDataFromNfcInternalSram(raw, (unsigned char *) spare, pageDataSize, pageSpareSize);
        }
    }
     else {
         // Read spare area only
         CopyDataFromNfcInternalSram(raw, (unsigned char *) spare, 0, pageSpareSize);
    }
    return 0;
}

//------------------------------------------------------------------------------
/// Writes the data and/or the spare area of a page on a NandFlash chip. If one
/// of the buffer pointer is 0, the corresponding area is not written. Retries
/// several time if there is an error.
/// Returns 0 if the write operation is successful; otherwise returns
/// NandCommon_ERROR_BADBLOCK.
/// \param raw  Pointer to a RawNandFlash instance.
/// \param block  Number of the block where the page to write resides.
/// \param page  Number of the page to write inside the given block.
/// \param data  Buffer containing the data area.
/// \param spare  Buffer containing the spare area.
//------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------
/// Copy the data in a page of the NandFlash device to an other page on that
/// same chip. Both pages must have be even or odd; it is not possible to copy
/// and even page to an odd page and vice-versa. Several retries are attempted
/// if errors are encountered.
/// Returns 0 if the operation is successful; otherwise returns
/// NandCommon_ERROR_BADBLOCK indicating that the destination block is bad.
/// \param raw  Pointer to a RawNandFlash instance.
/// \param sourceBlock  Source block number.
/// \param sourcePage  Source page number inside the source block.
/// \param destBlock  Destination block number.
/// \param destPage  Destination page number inside the destination block.
//------------------------------------------------------------------------------
unsigned char RawNandFlash_CopyPage(
    const struct RawNandFlash *raw,
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

//------------------------------------------------------------------------------
/// Copies the data of one whole block of a NandFlash device to another block.
/// Returns 0 if successful; otherwise returns NandCommon_ERROR_BADBLOCK.
/// \param raw  Pointer to a RawNandFlash instance.
/// \param sourceBlock  Source block number.
/// \param destBlock  Destination block number.
//------------------------------------------------------------------------------
unsigned char RawNandFlash_CopyBlock(
    const struct RawNandFlash *raw,
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

#endif

