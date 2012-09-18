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

#include <string.h>
#include "Media.h"
#include "debug.h"
#include "board.h"
#include "sdmmc.h"

// TODO BSS
/*#if defined MCI2_INTERFACE
#   include "dmad/dmad.h"
#endif
*/

#define assert(...)

/*------------------------------------------------------------------------------
 *         Local variables
 *------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
 *      Internal Functions
 *------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
//! \brief  Reads a specified amount of data from a SDCARD memory
//! \param  media    Pointer to a Media instance
//! \param  address  Address of the data to read
//! \param  data     Pointer to the buffer in which to store the retrieved
//!                   data
//! \param  length   Length of the buffer
//! \param  callback Optional pointer to a callback function to invoke when
//!                   the operation is finished
//! \param  argument Optional pointer to an argument for the callback
//! \return Operation result code
 *------------------------------------------------------------------------------*/
static uint8_t MEDSdmmc_Read( Media* media, unsigned int address, void* data, unsigned int length, MediaCallback callback, void* argument )
{
    // uint8_t error ;

    // Check that the media is ready
    if (media->state != MED_STATE_READY)
    {
        TRACE_INFO("Media busy\n\r");
        return MED_STATUS_BUSY;
    }

    // Check that the data to read is not too big
    if ((length + address) > media->size)
    {
        TRACE_WARNING("MEDSdmmc_Read: Data too big: %d, %d\n\r", length, address ) ;

        return MED_STATUS_ERROR;
    }

    /* Enter Busy state */
    media->state = MED_STATE_BUSY;

    /*error = */SD_ReadBlocks((SdCard*)media->interface, address, length, data);
    // assert( error == 0 ) ;

    /* Leave the Busy state */
    media->state = MED_STATE_READY;

    /* Invoke callback */
    if ( callback != 0 )
    {
        callback( argument, MED_STATUS_SUCCESS, 0, 0 ) ;
    }

    return MED_STATUS_SUCCESS ;
}

/**
//! \brief  Writes data on a SDRAM media
//! \param  media    Pointer to a Media instance
//! \param  address  Address at which to write
//! \param  data     Pointer to the data to write
//! \param  length   Size of the data buffer
//! \param  callback Optional pointer to a callback function to invoke when
//!                   the write operation terminates
//! \param  argument Optional argument for the callback function
//! \return Operation result code
//! \see    Media
//! \see    MediaCallback
 *------------------------------------------------------------------------------*/
static uint8_t MEDSdmmc_Write( Media* media, unsigned int address, void* data, unsigned int length, MediaCallback callback, void* argument )
{
    // uint8_t error ;

    // Check that the media if ready
    if (media->state != MED_STATE_READY)
    {
        TRACE_WARNING("MEDSdmmc_Write: Media is busy\n\r");
        return MED_STATUS_BUSY;
    }

    // Check that the card is not protected
    if (media->protected)
    {
        return MED_STATUS_PROTECTED;
    }

    // Check that the data to write is not too big
    if ((length + address) > media->size)
    {
        TRACE_WARNING("MEDSdmmc_Write: Data too big\n\r");
        return MED_STATUS_ERROR;
    }

    // Put the media in Busy state
    media->state = MED_STATE_BUSY;

    /*error = */SD_WriteBlocks((SdCard*)media->interface, address, length, data);
    // assert( error == 0 ) ;

    // Leave the Busy state
    media->state = MED_STATE_READY;

    // Invoke the callback if it exists
    if ( callback != 0 )
    {
        callback( argument, MED_STATUS_SUCCESS, 0, 0 ) ;
    }

    return MED_STATUS_SUCCESS;
}

#if 0
// TODO BSS what do they do ???

/**
//! \brief Callback invoked when SD/MMC transfer done
 *------------------------------------------------------------------------------*/
static void SdMmcCallback(uint8_t status, void *pCommand)
{
    SdCmd       * pCmd = (SdCmd*)pCommand;
    Media       * pMed = pCmd->pArg;
    MEDTransfer * pXfr = &pMed->transfer;
    volatile uint8_t medStatus = MED_STATUS_SUCCESS;

    TRACE_INFO_WP("SDCb ");

    // Error
    if (status == SD_ERROR_BUSY)
    {
        //TRACE_WARNING("SD BUSY\n\r");
        medStatus = MED_STATUS_BUSY;
    }
    else
    {
        if (status != MED_STATUS_SUCCESS)
        {
            //TRACE_WARNING("SD st%d\n\r", status);
            medStatus = MED_STATUS_ERROR;
        }
    }

    pMed->state = MED_STATE_READY;
    if (pXfr->callback)
    {
        pXfr->callback(pXfr->argument,
                       medStatus,
                       pXfr->length * pMed->blockSize,
                       0);
    }
}

/**
//! \brief  Reads a specified amount of data from a SDCARD memory
//! \param  media    Pointer to a Media instance
//! \param  address  Address of the data to read
//! \param  data     Pointer to the buffer in which to store the retrieved
//!                   data
//! \param  length   Length of the buffer
//! \param  callback Optional pointer to a callback function to invoke when
//!                   the operation is finished
//! \param  argument Optional pointer to an argument for the callback
//! \return Operation result code
 *------------------------------------------------------------------------------*/
static uint8_t MEDSdusb_Read( Media* media, uint32_t address, void* data, uint32_t length, MediaCallback callback, void* argument )
{
    MEDTransfer * pXfr;
    uint8_t error;

    TRACE_INFO_WP("SDuRd(%d,%d) ", address, length);

    // Check that the media is ready
    if ( media->state != MED_STATE_READY )
    {
        TRACE_INFO("MEDSdusb_Read: Busy\n\r");
        return MED_STATUS_BUSY;
    }

    // Check that the data to read is not too big
    if ((length + address) > media->size)
    {
        TRACE_WARNING("MEDSdusb_Read: Data too big: %d, %d\n\r", length, address);
        return MED_STATUS_ERROR;
    }
    // Enter Busy state
    media->state = MED_STATE_BUSY;

    // Start media transfer
    pXfr = &media->transfer;
    pXfr->data     = data;
    pXfr->address  = address;
    pXfr->length   = length;
    pXfr->callback = callback;
    pXfr->argument = argument;

    error = SD_Read( (SdCard*)media->interface, address, data, length, SdMmcCallback, media ) ;

    return (error ? MED_STATUS_ERROR : MED_STATUS_SUCCESS) ;
}

/**
//! \brief  Writes data on a SDRAM media
//! \param  media    Pointer to a Media instance
//! \param  address  Address at which to write
//! \param  data     Pointer to the data to write
//! \param  length   Size of the data buffer
//! \param  callback Optional pointer to a callback function to invoke when
//!                   the write operation terminates
//! \param  argument Optional argument for the callback function
//! \return Operation result code
//! \see    Media
//! \see    MediaCallback
 *------------------------------------------------------------------------------*/
static uint8_t MEDSdusb_Write( Media* media, uint32_t address, void* data, uint32_t length, MediaCallback callback, void* argument )
{
    MEDTransfer * pXfr;
    uint8_t error;
    TRACE_INFO_WP("SDuWr(%d,%d) ", address, length);

    // Check that the media if ready
    if (media->state != MED_STATE_READY)
    {
        TRACE_INFO("MEDSdusb_Write: Busy\n\r");
        return MED_STATUS_BUSY;
    }

    // Check that the card is not protected
    if (media->protected)
    {
        return MED_STATUS_PROTECTED;
    }

    // Check that the data to write is not too big
    if ((length + address) > media->size)
    {
        TRACE_WARNING("MEDSdmmc_Write: Data too big\n\r");
        return MED_STATUS_ERROR;
    }

    // Put the media in Busy state
    media->state = MED_STATE_BUSY;

    // Start media transfer
    pXfr = &media->transfer;
    pXfr->data = data;
    pXfr->address = address;
    pXfr->length = length;
    pXfr->callback = callback;
    pXfr->argument = argument;

    error = SD_Write( (SdCard*)media->interface, address, data, length, SdMmcCallback, media ) ;

    return (error ? MED_STATUS_ERROR : MED_STATUS_SUCCESS);
}
#endif

/*------------------------------------------------------------------------------
 *      Exported Functions
 *------------------------------------------------------------------------------*/

/**
 *  Initializes a Media instance and the associated physical interface
 *  \param  media Pointer to the Media instance to initialize
 *  \param  sdDrv Pointer to the SdCard instance that initialized.
 *  \param  protected SD card protected status.
 *  \return 1 if success.
 *------------------------------------------------------------------------------*/
void MEDSdcard_Initialize(Media *media, SdCard * sdDrv, uint8_t protected)
{
    TRACE_INFO("MEDSdcard_Initialize\n\r");

    // Initialize SDcard: Should have been done before media interface init
    //--------------------------------------------------------------------------

    // Initialize media fields
    //--------------------------------------------------------------------------
    media->interface = sdDrv;
    media->write = MEDSdmmc_Write;
    media->read = MEDSdmmc_Read;
    media->cancelIo = 0;
    media->lock = 0;
    media->unlock = 0;
    media->handler = 0;
    media->flush = 0;

    media->baseAddress = 0;

    if (SD_TOTAL_SIZE(sdDrv) == 0xFFFFFFFF)
    {
        media->blockSize = SD_BLOCK_SIZE;
        media->size = SD_TOTAL_BLOCK(sdDrv);
    }
    else
    {
        media->blockSize = SD_TOTAL_SIZE(sdDrv) / SD_TOTAL_BLOCK(sdDrv);
        if (media->blockSize != SD_BLOCK_SIZE)
        {
            media->blockSize = SD_BLOCK_SIZE;
            media->size = SD_TOTAL_SIZE(sdDrv) / SD_BLOCK_SIZE;
        }
        else
        {
            media->size = SD_TOTAL_BLOCK(sdDrv);
        }
    }

    media->mappedRD  = 0;
    media->mappedWR  = 0;
    media->protected = protected;
    media->removable = 1;

    media->state = MED_STATE_READY;

    media->transfer.data = 0;
    media->transfer.address = 0;
    media->transfer.length = 0;
    media->transfer.callback = 0;
    media->transfer.argument = 0;
}

/**
 *  Initializes a Media instance and the associated physical interface
 *  \param  media Pointer to the Media instance to initialize
 *  \param  sdDrv Pointer to the SdCard instance that initialized.
 *  \param  protected SD card protected status.
 *  \return 1 if success.
 *------------------------------------------------------------------------------*/
void MEDSdusb_Initialize(Media *media, SdCard * sdDrv, uint8_t protected)
{
    TRACE_INFO("MEDSdusb init\n\r");

    // Initialize SDcard: Should have been done before media interface init
    //--------------------------------------------------------------------------

    // Initialize media fields
    //--------------------------------------------------------------------------
    media->interface = sdDrv;
#if defined(AT91C_MCI_WRPROOF)
    media->write = MEDSdusb_Write;
#else
    media->write = MEDSdmmc_Write;
#endif
#if defined(AT91C_MCI_RDPROOF)
    media->read = MEDSdusb_Read;
#else
    media->read = MEDSdmmc_Read;
#endif
    media->cancelIo = 0;    // Cancel pending IO, add later.
    media->lock = 0;
    media->unlock = 0;
    media->handler = 0;
    media->flush = 0;

    media->baseAddress = 0;
    if ( SD_TOTAL_SIZE(sdDrv) == 0xFFFFFFFF)
    {
        media->blockSize = SD_BLOCK_SIZE;
        media->size = SD_TOTAL_BLOCK(sdDrv);
    }
    else
    {
        media->blockSize = SD_TOTAL_SIZE(sdDrv) / SD_TOTAL_BLOCK(sdDrv);
        if (media->blockSize != SD_BLOCK_SIZE)
        {
            media->blockSize = SD_BLOCK_SIZE;
            media->size = SD_TOTAL_SIZE(sdDrv) / SD_BLOCK_SIZE;
        }
        else
        {
            media->size = SD_TOTAL_BLOCK(sdDrv);
        }
    }

    media->mappedRD  = 0;
    media->mappedWR  = 0;
    media->protected = protected;
    media->removable = 1;

    media->state = MED_STATE_READY;

    media->transfer.data = 0;
    media->transfer.address = 0;
    media->transfer.length = 0;
    media->transfer.callback = 0;
    media->transfer.argument = 0;
}

/**
 *  erase all the Sdcard
 *  \param  media Pointer to the Media instance to initialize
 *------------------------------------------------------------------------------*/
void MEDSdcard_EraseAll(Media *media)
{
    uint8_t buffer[SD_BLOCK_SIZE];
    uint32_t block;
    uint16_t multiBlock = 1; // change buffer size for multiblocks
    // uint8_t error;

    TRACE_INFO("MEDSdcard Erase All ...\n\r");

    // Clear the block buffer
    memset(buffer, 0, media->blockSize * multiBlock);

    for ( block=0 ; block < (SD_TOTAL_BLOCK((SdCard*)media->interface)-multiBlock) ; block += multiBlock )
    {

        /*error = */SD_WriteBlocks((SdCard*)media->interface, block, multiBlock, buffer);
        // assert( error == 0 ) ; /* "\n\r-F- Failed to write block (%d) #%u\n\r", error, block */
    }
}

/**
 *  erase block
 *  \param  media Pointer to the Media instance to initialize
 *  \param  block to erase
 *------------------------------------------------------------------------------*/
void MEDSdcard_EraseBlock(Media *media, uint32_t block)
{
    uint8_t buffer[SD_BLOCK_SIZE];
    // uint8_t error;

    // Clear the block buffer
    memset(buffer, 0, media->blockSize);

    /*error = */SD_WriteBlocks((SdCard*)media->interface, block, 1, buffer);
    // assert( error == 0 ) ; /* "\n\r-F- Failed to write block (%d) #%u\n\r", error, block */
}
