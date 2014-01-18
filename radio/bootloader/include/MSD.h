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
/// Mass Storage class definitions.
///
/// See
/// - <a 
/// href="http://www.usb.org/developers/devclass_docs/usb_msc_overview_1.2.pdf">
/// USB Mass Storage Class Spec. Overview</a>
/// - <a href="http://www.usb.org/developers/devclass_docs/usbmassbulk_10.pdf">
/// USB Mass Storage Class Bulk-Only Transport</a>
/// 
/// !Usage
/// 
/// -# Uses "MSD Requests" to check incoming requests from USB Host.
/// -# Uses "MSD Subclass Codes" and "MSD Protocol Codes" to fill %device
///    interface descriptors for a MSD %device.
/// -# Handle the incoming Bulk data with "MSD CBW Definitions" and MSCbw
///    structure.
/// -# Prepare the outgoing Bulk data with "MSD CSW Definitions" and MSCsw
///    structure.
//------------------------------------------------------------------------------

#ifndef MSD_H
#define MSD_H

//------------------------------------------------------------------------------
//      Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "MSD Requests"
/// This page lists MSD-specific requests ( Actually for Bulk-only protocol ).
///
/// !Requests
/// - MSD_BULK_ONLY_RESET
/// - MSD_GET_MAX_LUN

/// Reset the mass storage %device and its associated interface.
#define MSD_BULK_ONLY_RESET                     0xFF
/// Return the maximum LUN number supported by the %device.
#define MSD_GET_MAX_LUN                         0xFE
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "MSD Subclass Codes"
/// This page lists the Subclass Codes for bInterfaceSubClass field.
/// (Table 2.1, USB Mass Storage Class Spec. Overview)
///
/// !SubClasses
/// - MSD_SUBCLASS_RBC
/// - MSD_SUBCLASS_SFF_MCC
/// - MSD_SUBCLASS_QIC
/// - MSD_SUBCLASS_UFI
/// - MSD_SUBCLASS_SFF
/// - MSD_SUBCLASS_SCSI

/// Reduced Block Commands (RBC) T10
#define MSD_SUBCLASS_RBC                        0x01
/// C/DVD devices
#define MSD_SUBCLASS_SFF_MCC                    0x02
/// Tape device
#define MSD_SUBCLASS_QIC                        0x03
/// Floppy disk drive (FDD) device
#define MSD_SUBCLASS_UFI                        0x04
/// Floppy disk drive (FDD) device
#define MSD_SUBCLASS_SFF                        0x05
/// SCSI transparent command set
#define MSD_SUBCLASS_SCSI                       0x06
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// \page "MSD Protocol Codes"
/// This page lists the Transport Protocol codes for MSD.
/// (Table 3.1, USB Mass Storage Class Spec. Overview)
///
/// !Protocols
/// - MSD_PROTOCOL_CBI_COMPLETION
/// - MSD_PROTOCOL_CBI
/// - MSD_PROTOCOL_BULK_ONLY

/// Control/Bulk/Interrupt (CBI) Transport (with command complete interrupt)
#define MSD_PROTOCOL_CBI_COMPLETION             0x00
/// Control/Bulk/Interrupt (CBI) Transport (no command complete interrupt)
#define MSD_PROTOCOL_CBI                        0x01
/// Bulk-Only Transport
#define MSD_PROTOCOL_BULK_ONLY                  0x50
//------------------------------------------------------------------------------

/// Test unit control:
#define CTRL_NOT_READY                          0x00
#define CTRL_GOOD                               0x01
#define CTRL_BUSY                               0x02

//------------------------------------------------------------------------------
/// \page "MSD CBW Definitions"
/// This page lists the Command Block Wrapper (CBW) definitions.
///
/// !Constants
/// - MSD_CBW_SIZE
/// - MSD_CBW_SIGNATURE
///
/// !Fields
/// - MSD_CBW_DEVICE_TO_HOST

/// Command Block Wrapper Size
#define MSD_CBW_SIZE                            31
/// 'USBC' 0x43425355
#define MSD_CBW_SIGNATURE                       0x43425355

/// CBW bmCBWFlags field
#define MSD_CBW_DEVICE_TO_HOST                  (1 << 7)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "MSD CSW Definitions"
/// This page lists the Command Status Wrapper (CSW) definitions.
///
/// !Constants
/// - MSD_CSW_SIZE
/// - MSD_CSW_SIGNATURE
///
/// !Command Block Status Values
/// (Table 5.3 , USB Mass Storage Class Bulk-Only Transport)
/// - MSD_CSW_COMMAND_PASSED
/// - MSD_CSW_COMMAND_FAILED
/// - MSD_CSW_PHASE_ERROR

/// Command Status Wrapper Size
#define MSD_CSW_SIZE                            13
/// 'USBS' 0x53425355
#define MSD_CSW_SIGNATURE                       0x53425355

/// Command Passed (good status)
#define MSD_CSW_COMMAND_PASSED                  0
/// Command Failed
#define MSD_CSW_COMMAND_FAILED                  1
/// Phase Error
#define MSD_CSW_PHASE_ERROR                     2
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//      Structures
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Command Block Wrapper (CBW), 
/// See Table 5.1, USB Mass Storage Class Bulk-Only Transport.
///
/// The CBW shall start on a packet boundary and shall end as a
/// short packet with exactly 31 (1Fh) bytes transferred.
//------------------------------------------------------------------------------
typedef struct {

    /// 'USBC' 0x43425355 (little endian)
    unsigned int  dCBWSignature;
    /// Must be the same as dCSWTag
    unsigned int  dCBWTag;
    /// Number of bytes transfer
    unsigned int  dCBWDataTransferLength;
    /// Indicates the directin of the transfer:
    /// 0x80=IN=device-to-host,
    /// 0x00=OUT=host-to-device
    unsigned char bmCBWFlags;
    /// bits 0->3: bCBWLUN
    unsigned char bCBWLUN   :4,
                  bReserved1:4;           /// reserved
    /// bits 0->4: bCBWCBLength
    unsigned char bCBWCBLength:5,
                  bReserved2  :3;         /// reserved
    /// Command block
    unsigned char pCommand[16];

} MSCbw;

//------------------------------------------------------------------------------
/// Command Status Wrapper (CSW),
/// See Table 5.2, USB Mass Storage Class Bulk-Only Transport.
//------------------------------------------------------------------------------
typedef struct
{
    /// 'USBS' 0x53425355 (little endian)
    unsigned int  dCSWSignature;
    /// Must be the same as dCBWTag
    unsigned int  dCSWTag;
    /// For Data-Out the device shall report in the dCSWDataResidue the
    /// difference between the amount of data expected as stated in the
    /// dCBWDataTransferLength, and the actual amount of data processed by
    /// the device. For Data-In the device shall report in the dCSWDataResidue
    /// the difference between the amount of data expected as stated in the
    /// dCBWDataTransferLength and the actual amount of relevant data sent by
    /// the device. The dCSWDataResidue shall not exceed the value sent in the
    /// dCBWDataTransferLength.
    unsigned int  dCSWDataResidue;
    /// Indicates the success or failure of the command.
    unsigned char bCSWStatus;

} MSCsw;

#endif //#ifndef MSD_H

