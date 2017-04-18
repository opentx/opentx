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


#include "../../../usb/device/massstorage/MSDLun.h"
#include "board_lowlevel.h"
#include "../../../usb/device/core/USBD.h"
#include "debug.h"

//------------------------------------------------------------------------------
//         Constants
//------------------------------------------------------------------------------

/// Default LUN block size in bytes
#define     DEFAULT_LUN_BLOCK_SIZE      BLOCK_SIZE

//------------------------------------------------------------------------------
//         Internal variables
//------------------------------------------------------------------------------

/// Inquiry data to return to the host for the Lun.
static SBCInquiryData inquiryData = {

    SBC_DIRECT_ACCESS_BLOCK_DEVICE, // Direct-access block device
    SBC_PERIPHERAL_DEVICE_CONNECTED,// Peripheral device is connected
    0x00,                           // Reserved bits
    0x01,                           // Media is removable
    0x02, // SBC_SPC_VERSION_4,     // SPC-4 supported
    0x2,                            // Response data format, must be 0x2
    0,                              // Hierarchical addressing not supported
    0,                              // ACA not supported
    0x0,                            // Obsolete bits
    sizeof(SBCInquiryData) - 5,     // Additional length
    0,                              // No embedded SCC
    0,                              // No access control coordinator
    SBC_TPGS_NONE,                  // No target port support group
    0,                              // Third-party copy not supported
    0x0,                            // Reserved bits
    0,                              // Protection information not supported
    0x0,                            // Obsolete bit
    0,                              // No embedded enclosure service component
    0x0,                            // ???
    0,                              // Device is not multi-port
    0x0,                            // Obsolete bits
    0x0,                            // Unused feature
    0x0,                            // Unused features
    0,                              // Task management model not supported
    0x0,                            // ???
    {'A','T','M','E','L',' ',' ',' '},
    {'M','a','s','s',' ',
     'S','t','o','r','a','g','e',' ',
     'M','S','D'},
    {'0','.','0','1'},
    {'M','a','s','s',' ',
     'S','t','o','r','a','g','e',' ',
     'E','x','a','m','p','l','e'},
    0x00,                           // Unused features
    0x00,                           // Reserved bits
    {SBC_VERSION_DESCRIPTOR_SBC_3}, // SBC-3 compliant device
    {0, 0, 0, 0, 0,
     0, 0, 0, 0, 0,
     0, 0, 0, 0, 0,
     0, 0, 0, 0, 0,
     0, 0} // Reserved
};

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//! \brief  Initializes a LUN instance. Must be invoked at least once even no
//!         Media is linked.
//! \param  lun          Pointer to the MSDLun instance to initialize
//! \param  media        Media on which the LUN is constructed, set to 0 to
//!                      disconnect the Media or initialize an ejected LUN.
//! \param  ioBuffer     Pointer to a buffer used for read/write operation and
//!                      which must be blockSize bytes long.
//! \param  ioBufferSize Size of the allocated IO buffer.
//! \param  baseAddress  Base address of the LUN in number of media blocks
//! \param  size         Total size of the LUN in number of media blocks
//! \param  blockSize    One block of the LUN in number of media blocks
//! \param  protected    The LUN area is forced to readonly even the media
//!                      is writable
//! \param  dataMonitor  Pointer to a Monitor Function to analyze the flow of
//!                      this LUN.
//------------------------------------------------------------------------------
void LUN_Init(MSDLun        *lun,
              Media         *media,
              unsigned char *ioBuffer,
              unsigned int   ioBufferSize,
              unsigned int   baseAddress,
              unsigned int   size,
              unsigned short blockSize,
              unsigned char  protected,
              void (*dataMonitor)(unsigned char flowDirection,
                                  unsigned int  dataLength,
                                  unsigned int  fifoNullCount,
                                  unsigned int  fifoFullCount))
{
    unsigned int logicalBlockAddress;
    TRACE_INFO("LUN init\n\r");

    // Initialize inquiry data
    lun->inquiryData = &inquiryData;

    // Initialize request sense data
    lun->requestSenseData.bResponseCode = SBC_SENSE_DATA_FIXED_CURRENT;
    lun->requestSenseData.isValid = 1;
    lun->requestSenseData.bObsolete1 = 0;
    lun->requestSenseData.bSenseKey = SBC_SENSE_KEY_NO_SENSE;
    lun->requestSenseData.bReserved1 = 0;
    lun->requestSenseData.isILI = 0;
    lun->requestSenseData.isEOM = 0;
    lun->requestSenseData.isFilemark = 0;
    lun->requestSenseData.pInformation[0] = 0;
    lun->requestSenseData.pInformation[1] = 0;
    lun->requestSenseData.pInformation[2] = 0;
    lun->requestSenseData.pInformation[3] = 0;
    lun->requestSenseData.bAdditionalSenseLength
        = sizeof(SBCRequestSenseData) - 8;
    lun->requestSenseData.bAdditionalSenseCode = 0;
    lun->requestSenseData.bAdditionalSenseCodeQualifier = 0;
    lun->requestSenseData.bFieldReplaceableUnitCode = 0;
    lun->requestSenseData.bSenseKeySpecific = 0;
    lun->requestSenseData.pSenseKeySpecific[0] = 0;
    lun->requestSenseData.pSenseKeySpecific[0] = 0;
    lun->requestSenseData.isSKSV = 0;

    STORE_DWORDB(0, lun->readCapacityData.pLogicalBlockAddress);
    STORE_DWORDB(0, lun->readCapacityData.pLogicalBlockLength);

    // Initialize LUN
    lun->media = media;
    if (media == 0) {
        lun->status = LUN_NOT_PRESENT;
        return;
    }

    lun->baseAddress = baseAddress;

    // Customized block size
    if (blockSize) {
        lun->blockSize = blockSize;
    }
    else {
        if (media->blockSize < DEFAULT_LUN_BLOCK_SIZE)
            lun->blockSize = DEFAULT_LUN_BLOCK_SIZE / media->blockSize;
        else
            lun->blockSize = 1;
    }

    if (size) {
        lun->size = size;
    }
    else {
        lun->size = media->size;
        //if (blockSize)
        //    lun->size = media->size / blockSize;
        //else {
        //    if (media->blockSize < DEFAULT_LUN_BLOCK_SIZE)
        //        lun->size = media->size / lun->blockSize;
        //    else
        //        lun->size = media->size;
        //}
    }
    
    TRACE_INFO("LUN: blkSize %d, size %d\n\r", (int)lun->blockSize, (int)lun->size);
    if (protected) lun->protected = 1;
    else           lun->protected = media->protected;

    lun->ioFifo.pBuffer = ioBuffer;
    lun->ioFifo.bufferSize = ioBufferSize;

    lun->dataMonitor = dataMonitor;

    // Initialize read capacity data
    logicalBlockAddress = lun->size / lun->blockSize - 1;
    STORE_DWORDB(logicalBlockAddress,
                 lun->readCapacityData.pLogicalBlockAddress);
    STORE_DWORDB(lun->blockSize * media->blockSize,
                 lun->readCapacityData.pLogicalBlockLength);

    // Indicate media change
    lun->status = LUN_CHANGED;
}

//------------------------------------------------------------------------------
//! \brief  Eject the media from a LUN
//! \param  lun          Pointer to the MSDLun instance to initialize
//! \return Operation result code
//------------------------------------------------------------------------------
unsigned char LUN_Eject(MSDLun *lun)
{
    if (lun->media) {

        // Avoid any LUN R/W in progress
        if (lun->media->state == MED_STATE_BUSY) {

            return USBD_STATUS_LOCKED;
        }

        // Remove the link of the media
        lun->media = 0;
    }
    // LUN is removed
    lun->status = LUN_NOT_PRESENT;

    return USBD_STATUS_SUCCESS;
}

//------------------------------------------------------------------------------
//! \brief  Writes data on the a LUN starting at the specified block address.
//! \param  lun          Pointer to a MSDLun instance
//! \param  blockAddress First block address to write
//! \param  data         Pointer to the data to write
//! \param  length       Number of blocks to write
//! \param  callback     Optional callback to invoke when the write finishes
//! \return Operation result code
//------------------------------------------------------------------------------
unsigned char LUN_Write(MSDLun        *lun,
                        unsigned int blockAddress,
                        void         *data,
                        unsigned int length,
                        TransferCallback   callback,
                        void         *argument)
{
    unsigned int  medBlk, medLen;
    unsigned char status;

    TRACE_INFO_WP("LUNWrite(%u) ", blockAddress);

    // Check that the data is not too big
    if ((length + blockAddress) * lun->blockSize > lun->size) {

        TRACE_WARNING("LUN_Write: Data too big\n\r");
        status = USBD_STATUS_ABORTED;
    }
    else if (lun->media == 0 || lun->status != LUN_READY) {

        TRACE_WARNING("LUN_Write: Media not ready\n\r");
        status = USBD_STATUS_ABORTED;
    }
    else if (lun->protected) {
        TRACE_WARNING("LUN_Write: LUN is readonly\n\r");
        status = USBD_STATUS_ABORTED;
    }
    else {

        // Compute write start address
        medBlk = lun->baseAddress + blockAddress * lun->blockSize;
        medLen = length * lun->blockSize;

        // Start write operation
        status = MED_Write(lun->media,
                           medBlk,
                           data,
                           medLen,
                           (MediaCallback) callback,
                           argument);

        // Check operation result code
        if (status == MED_STATUS_SUCCESS) {

            status = USBD_STATUS_SUCCESS;
        }
        else {

            TRACE_WARNING("LUN_Write: Cannot write media\n\r");
            status = USBD_STATUS_ABORTED;
        }
    }

    return status;
}

//------------------------------------------------------------------------------
//! \brief  Reads data from a LUN, starting at the specified block address.
//! \param  lun          Pointer to a MSDLun instance
//! \param  blockAddress First block address to read
//! \param  data         Pointer to a data buffer in which to store the data
//! \param  length       Number of blocks to read
//! \param  callback     Optional callback to invoke when the read finishes
//! \return Operation result code
//------------------------------------------------------------------------------
unsigned char LUN_Read(MSDLun        *lun,
                       unsigned int blockAddress,
                       void         *data,
                       unsigned int length,
                       TransferCallback   callback,
                       void         *argument)
{
    unsigned int medBlk, medLen;
    unsigned char status;

    // Check that the data is not too big
    if ((length + blockAddress) * lun->blockSize > lun->size) {

        TRACE_WARNING("LUN_Read: Area: (%d + %d)*%d > %d\n\r",
                      (int)length, (int)blockAddress, (int)lun->blockSize, (int)lun->size);
        status = USBD_STATUS_ABORTED;
    }
    else if (lun->media == 0 || lun->status != LUN_READY) {

        TRACE_WARNING("LUN_Read: Media not present\n\r");
        status = USBD_STATUS_ABORTED;
    }
    else {

        TRACE_INFO_WP("LUNRead(%u) ", blockAddress);

        // Compute read start address
        medBlk = lun->baseAddress + (blockAddress * lun->blockSize);
        medLen = length * lun->blockSize;

        // Start write operation
        status = MED_Read(lun->media,
                          medBlk,
                          data,
                          medLen,
                          (MediaCallback) callback,
                          argument);

        // Check result code
        if (status == MED_STATUS_SUCCESS) {

            status = USBD_STATUS_SUCCESS;
        }
        else {

            TRACE_WARNING("LUN_Read: Cannot read media\n\r");
            status = USBD_STATUS_ABORTED;
        }
    }

    return status;
}
