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

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "NandFlashModel.h"
#include "NandCommon.h"
#if defined(CHIP_NAND_CTRL)
#include <hsmc4/hsmc4.h>
#endif
#include <utility/trace.h>

#include <string.h>

//------------------------------------------------------------------------------
//         Internal definitions
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// Get the power of input, given a certain result, i.e. input^(power) = result.
/// returns the value of "power" if succesfully find the power.
/// \param result  a certain output we want to calculate.
/// \param input  the input of the power.
//------------------------------------------------------------------------------
#if defined(OP_BOOTSTRAP_on)
unsigned int CALPOW(unsigned int result, unsigned int input)
{
	unsigned int i=0;

	while(i<32)
	{
		if(result == (input << i))
			return i;
		i++;
	}

	return 0;
}
#endif

//------------------------------------------------------------------------------
/// Get the interger part of input, given a certain result, i.e.  return = result / input.
/// returns the value of interger part of the result/input.
/// \param result  a certain output we want to calculate.
/// \param input  the input of the division.
//------------------------------------------------------------------------------
#if defined(OP_BOOTSTRAP_on)
unsigned int CALINT(unsigned int result, unsigned int input)
{
	unsigned int i=0;
	unsigned int tmpInput=0;
        
    while(1)
    {
    	tmpInput +=input;
  		i++;
	    if(tmpInput == result)
	      return i;
	    else if (tmpInput > result)
	      return (i-1);
    }

}
#endif

/*----------------------------------------------------------------------------
 *        Exported functions
 *----------------------------------------------------------------------------*/
 
/**
 * \brief Looks for a NandFlashModel corresponding to the given ID inside a list of
 * model. If found, the model variable is filled with the correct values.
 *
 * \param modelList  List of NandFlashModel instances.
 * \param size  Number of models in list.
 * \param chipId  Identifier returned by the Nand(id1|(id2<<8)|(id3<<16)|(id4<<24)).
 * \param model  NandFlashModel instance to update with the model parameters.
 * \return 0 if  a matching model has been found; otherwise returns NandCommon_ERROR_UNKNOWNMODEL.
 */
unsigned char NandFlashModel_Find(
    const struct NandFlashModel *modelList,
    unsigned int size,
    unsigned int chipId,
    struct NandFlashModel *model)
{
    unsigned char found = 0, id2, id4;
    unsigned int i;
    #if defined(CHIP_NAND_CTRL)
    unsigned char pageSize = 0;
    #endif
    id2 = (unsigned char)(chipId>>8);
    id4 = (unsigned char)(chipId>>24);

    TRACE_INFO("Nandflash ID is 0x%08X\n\r", chipId);

    for(i=0; i<size; i++) {
        if(modelList[i].deviceId == id2) {
            found = 1;

            if(model) {

                memcpy(model, &modelList[i], sizeof(struct NandFlashModel));

                if(model->blockSizeInKBytes == 0 || model->pageSizeInBytes == 0) {
                    TRACE_DEBUG("Fetch from ID4(0x%.2x):\r\n", id4);
                    /// Fetch from the extended ID4
                    /// ID4 D5  D4 BlockSize || D1  D0  PageSize
                    ///     0   0   64K      || 0   0   1K
                    ///     0   1   128K     || 0   1   2K
                    ///     1   0   256K     || 1   0   4K
                    ///     1   1   512K     || 1   1   8k
                    #if !defined(OP_BOOTSTRAP_on)
                    switch(id4 & 0x03) {
                        case 0x00: model->pageSizeInBytes = 1024; break;
                        case 0x01: model->pageSizeInBytes = 2048; break;
                        case 0x02: model->pageSizeInBytes = 4096; break;
                        case 0x03: model->pageSizeInBytes = 8192; break;
                    }
                    switch(id4 & 0x30) {
                        case 0x00: model->blockSizeInKBytes = 64;  break;
                        case 0x10: model->blockSizeInKBytes = 128; break;
                        case 0x20: model->blockSizeInKBytes = 256; break;
                        case 0x30: model->blockSizeInKBytes = 512; break;
                    }
					#else
					model->pageSizeInBytes = 1024 << (id4 & 0x03);
                    model->blockSizeInKBytes = (64) << ((id4 & 0x30) >>4);
					#endif
                }
                #if defined(CHIP_NAND_CTRL)
                    switch(model->pageSizeInBytes) {
                        case 1024: pageSize = AT91C_HSMC4_PAGESIZE_1056_Bytes; break;
                        case 2048: pageSize = AT91C_HSMC4_PAGESIZE_2112_Bytes; break;
                        case 4096: pageSize = AT91C_HSMC4_PAGESIZE_4224_Bytes; break;
                        default: TRACE_ERROR("Unsupportted page size for NAND Flash Controller\n\r");
                    }
                    HSMC4_SetMode(pageSize | AT91C_HSMC4_DTOMUL_1048576 | AT91C_HSMC4_EDGECTRL |AT91C_HSMC4_DTOCYC | AT91C_HSMC4_RSPARE);
                #endif
            }
            TRACE_DEBUG("NAND Model found:\r\n");
            TRACE_DEBUG(" * deviceId = 0x%02X\r\n", model->deviceId);
            TRACE_DEBUG(" * deviceSizeInMegaBytes = %d\r\n", model->deviceSizeInMegaBytes);
            TRACE_DEBUG(" * blockSizeInkBytes = %d\r\n", model->blockSizeInKBytes);
            TRACE_DEBUG(" * pageSizeInBytes = %d\r\n", model->pageSizeInBytes);
            TRACE_DEBUG(" * options = 0x%02X\r\n", model->options);
            break;
        }
    }

    // Check if chip has been detected
    if (found) {

        return 0;
    }
    else {

        return NandCommon_ERROR_UNKNOWNMODEL;
    }
}

/**
 * \brief Translates address/size access of a NandFlashModel to block, page and offset values.
 *
 * \param modelList  List of NandFlashModel instances.
 * \param address  Access address.
 * \param size  Access size in bytes.
 * \param block  Stores the first accessed block number.
 * \param page  Stores the first accessed page number inside the first block.
 * \param offset  Stores the byte offset inside the first accessed page.
 * \return 0 if the access is correct; otherwise returns NandCommon_ERROR_OUTOFBOUNDS.
 * \note The values are stored in the provided variables if their pointer is not 0.
 */
unsigned char NandFlashModel_TranslateAccess(
    const struct NandFlashModel *model,
    unsigned int address,
    unsigned int size,
    unsigned short *block,
    unsigned short *page,
    unsigned short *offset)
{
     // Check that access is not too big
    #if !defined(OP_BOOTSTRAP_on)
    if ((address + size) > NandFlashModel_GetDeviceSizeInBytes(model)) {

        TRACE_DEBUG("NandFlashModel_TranslateAccess: out-of-bounds access.\n\r");
        return NandCommon_ERROR_OUTOFBOUNDS;
    }
	#endif

    // Get Nand info
    unsigned int blockSize = NandFlashModel_GetBlockSizeInBytes(model);
    unsigned int pageSize = NandFlashModel_GetPageDataSize(model);

    // Translate address
    #if !defined(OP_BOOTSTRAP_on)
    unsigned short tmpBlock = address / blockSize;
    address -= tmpBlock * blockSize;
    unsigned short tmpPage = address / pageSize;
    address -= tmpPage * pageSize;
    unsigned short tmpOffset = address;
	#else
	unsigned short tmpBlock = CALINT(address, blockSize);
	address -= tmpBlock * blockSize;
	unsigned short tmpPage = CALINT(address, pageSize);
	address -= tmpPage * pageSize;
	unsigned short tmpOffset= address;
	#endif

    // Save results
    if (block) {

        *block = tmpBlock;
    }
    if (page) {

        *page = tmpPage;
    }
    if (offset) {

        *offset = tmpOffset;
    }

    return 0;
}


/**
 * \brief Returns the spare area placement scheme used by a particular nandflash model.
 *
 * \param model  Pointer to a NandFlashModel instance.
 */
const struct NandSpareScheme * NandFlashModel_GetScheme(
    const struct NandFlashModel *model)
{
    return model->scheme;
}

/**
 * \brief Returns the device ID of a particular NandFlash model.
 *
 * \param model  Pointer to a NandFlashModel instance.
 */
unsigned char NandFlashModel_GetDeviceId(
   const struct NandFlashModel *model)
{
    return model->deviceId;
}

/**
 * \brief Returns the number of blocks in the entire device.
 *
 * \param model  Pointer to a NandFlashModel instance.
 */
unsigned short NandFlashModel_GetDeviceSizeInBlocks(
   const struct NandFlashModel *model)
{
	#if !defined(OP_BOOTSTRAP_on)
    return ((1024) / model->blockSizeInKBytes) * model->deviceSizeInMegaBytes;
	#else
	unsigned int pow;
	pow = CALPOW((1024 * model->deviceSizeInMegaBytes), model->blockSizeInKBytes);
	return (0x1 << pow);
	#endif
}

/**
 * \brief Returns the number of pages in the entire device.
 *
 * \param model  Pointer to a NandFlashModel instance.
 */
unsigned int NandFlashModel_GetDeviceSizeInPages(
   const struct NandFlashModel *model)
{
    return (unsigned int) NandFlashModel_GetDeviceSizeInBlocks(model) //* 8 // HACK
           * NandFlashModel_GetBlockSizeInPages(model);
}

/**
 * \brief Returns the size of the whole device in bytes (this does not include the
 * size of the spare zones).
 *
 * \param model  Pointer to a NandFlashModel instance.
 */
unsigned long long NandFlashModel_GetDeviceSizeInBytes(
   const struct NandFlashModel *model)
{
    return ((unsigned long long) model->deviceSizeInMegaBytes) << 20;
}

/**
 * \brief Returns the size of the whole device in Mega bytes (this does not include the
 * size of the spare zones).
 *
 * \param model  Pointer to a NandFlashModel instance.
 */
unsigned int NandFlashModel_GetDeviceSizeInMBytes(
   const struct NandFlashModel *model)
{
    return ((unsigned int) model->deviceSizeInMegaBytes);
}

/**
 * \brief Returns the number of pages in one single block of a device.
 *
 * \param model  Pointer to a NandFlashModel instance.
 */
unsigned short NandFlashModel_GetBlockSizeInPages(
   const struct NandFlashModel *model)
{
    #if !defined(OP_BOOTSTRAP_on)
    return model->blockSizeInKBytes * 1024 / model->pageSizeInBytes;
	#else
	unsigned int pow;
	pow = CALPOW((model->blockSizeInKBytes * 1024), model->pageSizeInBytes);
	return (0x1 << pow);
	#endif
}

/**
 * \brief Returns the size in bytes of one single block of a device. This does not
 * take into account the spare zones size.
 *
 * \param model  Pointer to a NandFlashModel instance.
 */
unsigned int NandFlashModel_GetBlockSizeInBytes(
   const struct NandFlashModel *model)
{
    return (model->blockSizeInKBytes *1024);
}

/**
 * \brief  Returns the size of the data area of a page in bytes.
 * take into account the spare zones size.
 *
 * \param model  Pointer to a NandFlashModel instance.
 */
unsigned short NandFlashModel_GetPageDataSize(
   const struct NandFlashModel *model)
{
    return model->pageSizeInBytes;
}

/**
 * \brief  Returns the size of the spare area of a page in bytes.
 *
 * \param model  Pointer to a NandFlashModel instance.
 */
unsigned char NandFlashModel_GetPageSpareSize(
   const struct NandFlashModel *model)
{
    return (model->pageSizeInBytes>>5); /// Spare size is 16/512 of data size
}

/**
 * \brief  Returns the number of bits used by the data bus of a NandFlash device.
 *
 * \param model  Pointer to a NandFlashModel instance.
 */
unsigned char NandFlashModel_GetDataBusWidth(
   const struct NandFlashModel *model)
{
    return (model->options&NandFlashModel_DATABUS16)? 16: 8;
}

/**
 * \brief  Check if the given NandFlash model uses the "small blocks/pages"
 *
 * \param model  Pointer to a NandFlashModel instance.
 * \return 1 if the model uses the "small blocks/pages"; otherwise return 0.
 */
unsigned char NandFlashModel_HasSmallBlocks(
   const struct NandFlashModel *model)
{
    return (model->pageSizeInBytes <= 512 )? 1: 0;
}

/**
 * \brief  Check if if the device supports the copy-back operation.
 *
 * \param model  Pointer to a NandFlashModel instance.
 * \return 1 if the model supports the copy-back operation; otherwise return 0.
 */
unsigned char NandFlashModel_SupportsCopyBack(
    const struct NandFlashModel *model)
{
    return ((model->options & NandFlashModel_COPYBACK) != 0);
}
