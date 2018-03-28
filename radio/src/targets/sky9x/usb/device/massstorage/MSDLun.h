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
/// \unit
/// !Purpose
/// 
/// Logical Unit Number (LUN) used by the Mass Storage driver and the SCSI
/// protocol. Represents a logical hard-drive.
/// 
/// !Usage
/// -# Initialize Memory related pins (see pio & board.h).
/// -# Initialize a Media instance for the LUN (see memories).
/// -# Initlalize the LUN with LUN_Init, and link to the initialized Media.
/// -# To read data from the LUN linked media, uses LUN_Read.
/// -# To write data to the LUN linked media, uses LUN_Write.
/// -# To unlink the media, uses LUN_Eject.
//------------------------------------------------------------------------------

#ifndef MSDLUN_H
#define MSDLUN_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "../../../Media.h"
#include "../../../usb/device/core/USBD.h"
#include "../../../usb/device/massstorage/MSDIOFifo.h"
#include "../../../usb/device/massstorage/SBC.h"

//------------------------------------------------------------------------------
//      Definitions
//------------------------------------------------------------------------------

/// LUN RC: success
#define LUN_STATUS_SUCCESS          0x00
/// LUN RC: error
#define LUN_STATUS_ERROR            0x02

/// Media of LUN is removed
#define LUN_NOT_PRESENT             0x00
/// LUN is ejected by host
#define LUN_EJECTED                 0x01
/// Media of LUN is changed
#define LUN_CHANGED                 0x10
/// LUN Not Ready to Ready transition
#define LUN_TRANS_READY             LUN_CHANGED
/// Media of LUN is ready
#define LUN_READY                   0x11

//------------------------------------------------------------------------------
//      Structures
//------------------------------------------------------------------------------

/// LUN structure
typedef struct {

    /// Pointer to a SBCInquiryData instance.
    SBCInquiryData        *inquiryData;
    /// Fifo for USB transfer, must be assigned.
    MSDIOFifo             ioFifo;
    /// Pointer to Media instance for the LUN.
    Media                 *media;
    /// Pointer to a Monitor Function to analyze the flow of LUN.
    /// \param flowDirection 1 - device to host (READ10)
    ///                      0 - host to device (WRITE10)
    /// \param dataLength Length of data transferred in bytes.
    /// \param fifoNullCount Times that FIFO is NULL to wait
    /// \param fifoFullCount Times that FIFO is filled to wait
    void                 (*dataMonitor)(unsigned char flowDirection,
                                        unsigned int  dataLength,
                                        unsigned int  fifoNullCount,
                                        unsigned int  fifoFullCount);
    /// The start position of the media (blocks) allocated to the LUN.
    unsigned int          baseAddress;
    /// The size of the media (blocks) allocated to the LUN.
    unsigned int          size;
    /// Sector size of the media in number of media blocks
    unsigned short        blockSize;
    /// The LUN can be readonly even the media is writable
#ifdef __cplusplus
    unsigned char         protectd;
#else
    unsigned char         protected;
#endif

    /// The LUN status (Ejected/Changed/)
    unsigned char         status;

    /// Data for the RequestSense command.
    SBCRequestSenseData   requestSenseData;
    /// Data for the ReadCapacity command.
    SBCReadCapacity10Data readCapacityData;

} MSDLun;

//------------------------------------------------------------------------------
//      Exported functions
//------------------------------------------------------------------------------
extern void LUN_Init(MSDLun        *lun,
                     Media         *media,
                     unsigned char *ioBuffer,
                     unsigned int   ioBufferSize,
                     unsigned int   baseAddress,
                     unsigned int   size,
                     unsigned short blockSize,
#ifdef __cplusplus
                     unsigned char  protectd,
#else
                     unsigned char  protected,
#endif
                     void (*dataMonitor)(unsigned char flowDirection,
                                         unsigned int  dataLength,
                                         unsigned int  fifoNullCount,
                                         unsigned int  fifoFullCount));

extern unsigned char LUN_Eject(MSDLun *lun);

extern unsigned char LUN_Write(MSDLun *lun,
                               unsigned int blockAddress,
                               void         *data,
                               unsigned int length,
                               TransferCallback   callback,
                               void         *argument);

extern unsigned char LUN_Read(MSDLun        *lun,
                              unsigned int blockAddress,
                              void         *data,
                              unsigned int length,
                              TransferCallback   callback,
                              void         *argument);

#endif //#ifndef MSDLUN_H

