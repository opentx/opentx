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
#include "../FatFs/diskio.h"

//------------------------------------------------------------------------------
//         Constants
//------------------------------------------------------------------------------

/// Number of SD Slots
#define NUM_SD_SLOTS            1
#define SD_BLOCK_SIZE           512

//------------------------------------------------------------------------------
/// Checks if the device is write protected.
/// \return 1 if protected.
//------------------------------------------------------------------------------
#if 0
static unsigned char CardIsProtected(unsigned char slot)
{
    if (slot == 0) {
      #ifdef BOARD_SD_PIN_WP
        PIO_Configure(&pinMciWriteProtect, 1);
        return (PIO_Get(&pinMciWriteProtect) != 0);
      #else
        return 0;
      #endif
    }

    if (slot == 1) {
      #ifdef BOARD_SD_MCI1_PIN_WP
        PIO_Configure(&pinMciWriteProtect1, 1);
        return (PIO_Get(&pinMciWriteProtect1) != 0);
      #else
        return 0;
      #endif
    }

    return 0;
}
#endif

//------------------------------------------------------------------------------
/// Configure the PIO for SD
//------------------------------------------------------------------------------
#if 0
static void ConfigurePIO(unsigned char mciID)
{
    #ifdef BOARD_SD_PINS
    const Pin pinSd0[] = {BOARD_SD_PINS};
    #endif

    #ifdef BOARD_SD_MCI1_PINS
    const Pin pinSd1[] = {BOARD_SD_MCI1_PINS};
    #endif

    if(mciID == 0) {
        #ifdef BOARD_SD_PINS
        PIO_Configure(pinSd0, PIO_LISTSIZE(pinSd0));
        #endif
    } else {
        #ifdef BOARD_SD_MCI1_PINS
        PIO_Configure(pinSd1, PIO_LISTSIZE(pinSd1));
        #endif
    }
}
#endif

//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
static unsigned char MEDSdcard_Read(Media         *media,
                                    unsigned int   address,
                                    void          *data,
                                    unsigned int   length,
                                    MediaCallback callback,
                                    void          *argument)
{
  TRACE_DEBUG("MEDSdcard_Read(address=%d length=%d)\n\r", address, length);

    // Check that the media is ready
    if (media->state != MED_STATE_READY) {

        TRACE_INFO("Media busy\n\r");
        return MED_STATUS_BUSY;
    }

    // Check that the data to read is not too big
    if ((length + address) > media->size) {

        TRACE_WARNING("MEDSdcard_Read: Data too big: %d, %d\n\r",
                      (int)length, (int)address);
        return MED_STATUS_ERROR;
    }

    // Enter Busy state
    media->state = MED_STATE_BUSY;

    disk_read (0, data, address, length);

    // Leave the Busy state
    media->state = MED_STATE_READY;

    // Invoke callback
    if (callback != 0) {

        callback(argument, MED_STATUS_SUCCESS, 0, 0);
    }

    return MED_STATUS_SUCCESS;
}

//------------------------------------------------------------------------------
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
//------------------------------------------------------------------------------
static unsigned char MEDSdcard_Write(Media         *media,
                                    unsigned int   address,
                                    void          *data,
                                    unsigned int   length,
                                    MediaCallback callback,
                                    void          *argument)
{

  TRACE_DEBUG("MEDSdcard_Write(address=%d length=%d)\n\r", address, length);

    // Check that the media if ready
    if (media->state != MED_STATE_READY) {

        TRACE_WARNING("MEDSdcard_Write: Media is busy\n\r");
        return MED_STATUS_BUSY;
    }

    // Check that the data to write is not too big
    if ((length + address) > media->size) {

        TRACE_WARNING("MEDSdcard_Write: Data too big\n\r");
        return MED_STATUS_ERROR;
    }

    // Put the media in Busy state
    media->state = MED_STATE_BUSY;

    disk_write(0, data, address, length);

    // Leave the Busy state
    media->state = MED_STATE_READY;

    // Invoke the callback if it exists
    if (callback != 0) {

        callback(argument, MED_STATUS_SUCCESS, 0, 0);
    }

    return MED_STATUS_SUCCESS;
}


//------------------------------------------------------------------------------
/// Initializes a Media instance and the associated physical interface
/// \param  media Pointer to the Media instance to initialize
/// \return 1 if success.
//------------------------------------------------------------------------------

unsigned char MEDSdcard_Initialize(Media *media, unsigned char mciID)
{
    TRACE_INFO("MEDSdcard init\n\r");

    // Initialize SDcard
    //--------------------------------------------------------------------------

    if ( !CardIsConnected(  ) )
    {
        return 0;
    }

    media->write = MEDSdcard_Write;
    media->read = MEDSdcard_Read;
    media->lock = 0;
    media->unlock = 0;
    media->handler = 0;
    media->flush = 0;

    media->blockSize = SD_BLOCK_SIZE;
    media->baseAddress = 0;

    media->size = SD_GET_BLOCKNR();

    media->mappedRD  = 0;
    media->mappedWR  = 0;
    media->protected = 0;
    media->removable = 1;

    media->state = MED_STATE_READY;

    media->transfer.data = 0;
    media->transfer.address = 0;
    media->transfer.length = 0;
    media->transfer.callback = 0;
    media->transfer.argument = 0;

    return 1;
}

