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

//-----------------------------------------------------------------------------
/// \unit
/// !Purpose
///
/// Definitions, structs, functions  required by a Mass Storage device driver
/// state machine..
/// 
/// !Usage
///
/// - For a USB device:
///     -# MSDD_Write, MSDD_Read, MSDD_Halt should be defined for
///        usage in the state machine procedure.
///
///     -# MSDD_StateMachine is invoked to run the MSD state machine.
///        
//-----------------------------------------------------------------------------

#ifndef MSDDSTATEMACHINE_H
#define MSDDSTATEMACHINE_H

//-----------------------------------------------------------------------------
//         Headers
//-----------------------------------------------------------------------------

#include "../../../usb/device/massstorage/MSD.h"
#include "../../../usb/device/massstorage/MSDLun.h"
#include "debug.h"

//-----------------------------------------------------------------------------
//      Definitions
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "MSD Driver Possible states"
/// ...
///
/// !States
/// - MSDD_STATE_READ_CBW
/// - MSDD_STATE_WAIT_CBW
/// - MSDD_STATE_PROCESS_CBW
/// - MSDD_STATE_WAIT_HALT
/// - MSDD_STATE_SEND_CSW
/// - MSDD_STATE_WAIT_CSW
/// - MSDD_STATE_WAIT_RESET

//! \brief  Driver is expecting a command block wrapper
#define MSDD_STATE_READ_CBW              (1 << 0)

//! \brief  Driver is waiting for the transfer to finish
#define MSDD_STATE_WAIT_CBW              (1 << 1)

//! \brief  Driver is processing the received command
#define MSDD_STATE_PROCESS_CBW           (1 << 2)

//! \brief  Driver is halted because pipe halt or wait reset
#define MSDD_STATE_WAIT_HALT             (1 << 3)

//! \brief  Driver is starting the transmission of a command status wrapper
#define MSDD_STATE_SEND_CSW              (1 << 4)

//! \brief  Driver is waiting for the CSW transmission to finish
#define MSDD_STATE_WAIT_CSW              (1 << 5)

//! \brief  Driver is waiting for the MassStorageReset
#define MSDD_STATE_WAIT_RESET            (1 << 6)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "MSD Driver Result Codes"
/// This page lists result codes for MSD functions.
///
/// !Codes
/// - MSDD_STATUS_SUCCESS
/// - MSDD_STATUS_ERROR
/// - MSDD_STATUS_INCOMPLETE
/// - MSDD_STATUS_PARAMETER
/// - MSDD_STATUS_RW

//! \brief  Method was successful
#define MSDD_STATUS_SUCCESS              0x00

//! \brief  There was an error when trying to perform a method
#define MSDD_STATUS_ERROR                0x01

//! \brief  No error was encountered but the application should call the
//!         method again to continue the operation
#define MSDD_STATUS_INCOMPLETE           0x02

//! \brief  A wrong parameter has been passed to the method
#define MSDD_STATUS_PARAMETER            0x03

//! \brief An error when reading/writing disk (protected or not present)
#define MSDD_STATUS_RW                   0x04
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "MSD Driver Action Cases"
/// This page lists actions to perform during the post-processing phase of a
/// command.
///
/// !Actions 
/// - MSDD_CASE_PHASE_ERROR
/// - MSDD_CASE_STALL_IN
/// - MSDD_CASE_STALL_OUT

//! \brief  Indicates that the CSW should report a phase error
#define MSDD_CASE_PHASE_ERROR            (1 << 0)

//! \brief  The driver should halt the Bulk IN pipe after the transfer
#define MSDD_CASE_STALL_IN               (1 << 1)

//! \brief  The driver should halt the Bulk OUT pipe after the transfer
#define MSDD_CASE_STALL_OUT              (1 << 2)

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "MSD Driver Xfr Directions"
/// This page lists possible direction values for a data transfer
///
/// !Directions
/// - MSDD_DEVICE_TO_HOST
/// - MSDD_HOST_TO_DEVICE
/// - MSDD_NO_TRANSFER

#define MSDD_DEVICE_TO_HOST              0
#define MSDD_HOST_TO_DEVICE              1
#define MSDD_NO_TRANSFER                 2
//------------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//         Types
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
//! \brief  Structure for holding the result of a USB transfer
//! \see    MSDDriver_Callback
//------------------------------------------------------------------------------
typedef struct {

    volatile unsigned int   transferred; /// Number of bytes transferred
    volatile unsigned int   remaining;   /// Number of bytes not transferred
    volatile unsigned short semaphore;   /// Semaphore to indicate transfer completion
    volatile unsigned short status;      /// Operation result code

} MSDTransfer;

//------------------------------------------------------------------------------
//! \brief  Status of an executing command
//! \see    MSDCbw
//! \see    MSDCsw
//! \see    MSDTransfer
//------------------------------------------------------------------------------
typedef struct {

    MSDTransfer     transfer;    /// Current transfer status (USB)
    MSDTransfer     disktransfer;/// Current transfer status (Disk)
    unsigned int    length;      /// Remaining length of command
    MSCbw           cbw;         /// Received CBW (31 bytes)
    unsigned char   state;       /// Current command state
    MSCsw           csw;         /// CSW to send  (13 bytes)
    unsigned char   postprocess; /// Actions to perform when command is complete

} MSDCommandState;

//------------------------------------------------------------------------------
/// \brief  MSD driver state variables
/// \see    MSDCommandState
/// \see    MSDLun
//------------------------------------------------------------------------------
typedef struct {

    /// LUN list for the %device.
    MSDLun *luns;
    /// State of the currently executing command
    MSDCommandState commandState;
    /// Maximum LUN index
    unsigned char maxLun;
    /// Current state of the driver
    unsigned char state;
    /// Indicates if the driver is waiting for a reset recovery
    unsigned char waitResetRecovery;

} MSDDriver;

//-----------------------------------------------------------------------------
//      Inline functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// This function is to be used as a callback for USB or LUN transfers.
/// \param  transfer    Pointer to the transfer structure to update
/// \param  status      Operation result code
/// \param  transferred Number of bytes transferred by the command
/// \param  remaining   Number of bytes not transferred
//-----------------------------------------------------------------------------
static inline void MSDDriver_Callback(MSDTransfer *transfer,
                                      unsigned char status,
                                      unsigned int transferred,
                                      unsigned int remaining)
{
    TRACE_DEBUG("Cbk ");
    transfer->semaphore++;
    transfer->status = status;
    transfer->transferred = transferred;
    transfer->remaining = remaining;
}

//-----------------------------------------------------------------------------
//      Driver functions
//-----------------------------------------------------------------------------
//- MSD General support function
extern char MSDD_Read(
    void* pData,
    unsigned int dLength,
    TransferCallback fCallback,
    void* pArgument);

extern char MSDD_Write(
    void* pData,
    unsigned int dLength,
    TransferCallback fCallback,
    void* pArgument);

extern void MSDD_Halt(unsigned int stallCase);

extern unsigned int MSDD_IsHalted(void);

//-----------------------------------------------------------------------------
//      Exported functions
//-----------------------------------------------------------------------------

extern void MSDD_StateMachine(MSDDriver * pMsdDriver);

#endif // #define MSDDSTATEMACHINE_H

