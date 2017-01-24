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
///
/// !!!Purpose
/// 
/// Collection of methods for using the USB device controller on AT91
/// microcontrollers.
/// 
/// !!!Usage
/// 
/// Please refer to the corresponding application note.
/// - "AT91 USB device framework"
/// - "USBD API" . "USBD API Methods"
//------------------------------------------------------------------------------

#ifndef USBD_H
#define USBD_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

// #include <board.h>
#include "../../../usb/common/core/USBEndpointDescriptor.h"
#include "../../../usb/common/core/USBGenericRequest.h"

//------------------------------------------------------------------------------
//      Compile Options
//------------------------------------------------------------------------------

/// Compile option for HS or OTG, use DMA. Remove this define for not use DMA.
#if defined(CHIP_USB_OTGHS) || defined(CHIP_USB_UDPHS)
#define DMA
#endif

//------------------------------------------------------------------------------
//      Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB device API return values"
///
/// This page lists the return values of the USB %device driver API
///
/// !Return codes
/// - USBD_STATUS_SUCCESS
/// - USBD_STATUS_LOCKED
/// - USBD_STATUS_ABORTED
/// - USBD_STATUS_RESET
           
/// Indicates the operation was successful.
#define USBD_STATUS_SUCCESS             0
/// Endpoint/device is already busy.
#define USBD_STATUS_LOCKED              1
/// Operation has been aborted.
#define USBD_STATUS_ABORTED             2
/// Operation has been aborted because the device has been reset.
#define USBD_STATUS_RESET               3
/// Part ot operation successfully done.
#define USBD_STATUS_PARTIAL_DONE        4
/// Operation failed because parameter error
#define USBD_STATUS_INVALID_PARAMETER   5
/// Operation failed because in unexpected state
#define USBD_STATUS_WRONG_STATE         6
/// Operation failed because HW not supported
#define USBD_STATUS_HW_NOT_SUPPORTED    0xFE
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB device states"
///
/// This page lists the %device states of the USB %device driver.
///
/// !States
/// - USBD_STATE_SUSPENDED
/// - USBD_STATE_ATTACHED
/// - USBD_STATE_POWERED
/// - USBD_STATE_DEFAULT
/// - USBD_STATE_ADDRESS
/// - USBD_STATE_CONFIGURED

/// The device is currently suspended.
#define USBD_STATE_SUSPENDED            0
/// USB cable is plugged into the device.
#define USBD_STATE_ATTACHED             1
/// Host is providing +5V through the USB cable.
#define USBD_STATE_POWERED              2
/// Device has been reset.
#define USBD_STATE_DEFAULT              3
/// The device has been given an address on the bus.
#define USBD_STATE_ADDRESS              4
/// A valid configuration has been selected.
#define USBD_STATE_CONFIGURED           5
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB device LEDs"
///
/// This page lists the LEDs used in the USB %device driver.
///
/// !LEDs
/// - USBD_LEDPOWER
/// - USBD_LEDUSB
/// - USBD_LEDOTHER

/// LED for indicating that the device is powered.
#define USBD_LEDPOWER                   0
/// LED for indicating USB activity.
#define USBD_LEDUSB                     1
/// LED for custom usage.
#define USBD_LEDOTHER                   2
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Buffer struct used for multi-buffer-listed transfer.
/// The driver can process 255 bytes of buffers or buffer list window.
//------------------------------------------------------------------------------
typedef struct _USBDTransferBuffer {
    /// Pointer to frame buffer
    unsigned char * pBuffer;
    /// Size of the frame (up to 64K-1)
    unsigned short size;
    /// Bytes transferred
    unsigned short transferred;
    /// Bytes in FIFO
    unsigned short buffered;
    /// Bytes remaining
    unsigned short remaining;
} USBDTransferBuffer;

#ifdef __ICCARM__          // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
/// Struct used for USBD DMA Link List Transfer Descriptor, must be 16-bytes
/// aligned.
/// (For USB, DMA transfer is linked to EPs and FIFO address is EP defined)
//------------------------------------------------------------------------------
typedef struct _USBDDmaDescriptor {
    /// Pointer to Next Descriptor
    void* pNxtDesc;
    /// Pointer to data buffer address
    void* pDataAddr;
    /// DMA Control setting register value
    unsigned int   ctrlSettings:8,  /// Control settings
                   reserved:8,      /// Not used
                   bufferLength:16; /// Length of buffer
    /// Loaded to DMA register, OK to modify
    unsigned int used;
} __attribute__((aligned(16))) USBDDmaDescriptor;

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
/// Callback used by transfer functions (USBD_Read & USBD_Write) to notify
/// that a transaction is complete.
//------------------------------------------------------------------------------
typedef void (*TransferCallback)(void *pArg,
                                 unsigned char status,
                                 unsigned int transferred,
                                 unsigned int remaining);

//------------------------------------------------------------------------------
/// Callback used by MBL transfer functions (USBD_Read & USBD_Write) to notify
/// that a transaction is complete.
/// \param pArg     Pointer to callback arguments.
/// \param status   USBD status.
/// \param nbFreed  Number of buffers that is freed since last callback.
//------------------------------------------------------------------------------
typedef void (*MblTransferCallback)(void *pArg,
                                    unsigned char status,
                                    unsigned int nbFreed);

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

extern void USBD_IrqHandler(void);

extern void USBD_Init(void);

extern void USBD_ConfigureSpeed(unsigned char forceFS);

extern void USBD_Connect(void);

extern void USBD_Disconnect(void);

extern char USBD_Write(
    unsigned char bEndpoint,
    const void *pData,
    unsigned int size,
    TransferCallback callback,
    void *pArg);

extern char USBD_MblWrite(
    unsigned char bEndpoint,
    void * pMbl,
    unsigned short wListSize,
    unsigned char bCircList,
    unsigned short wStartNdx,
    MblTransferCallback fCallback,
    void * pArgument);

extern char USBD_MblReuse(
    unsigned char bEndpoint,
    unsigned char * pNewBuffer,
    unsigned short wNewSize);

extern char USBD_Read(
    unsigned char bEndpoint,
    void *pData,
    unsigned int dLength,
    TransferCallback fCallback,
    void *pArg);

extern unsigned char USBD_Stall(unsigned char bEndpoint);

extern void USBD_Halt(unsigned char bEndpoint);

extern void USBD_Unhalt(unsigned char bEndpoint);

extern void USBD_ConfigureEndpoint(const USBEndpointDescriptor *pDescriptor);

extern unsigned char USBD_IsHalted(unsigned char bEndpoint);

extern void USBD_RemoteWakeUp(void);

extern void USBD_SetAddress(unsigned char address);

extern void USBD_SetConfiguration(unsigned char cfgnum);

extern unsigned char USBD_GetState(void);

extern unsigned char USBD_IsHighSpeed(void);

extern void USBD_Test(unsigned char bIndex);

/* USBD Callbacks */

#define USBDCallbacks_Reset()

#define USBDCallbacks_Suspended()

#define USBDCallbacks_Resumed()

extern void USBDCallbacks_RequestReceived(const USBGenericRequest *request);

extern void USBDDriverCallbacks_ConfigurationChanged(unsigned char cfgnum);

extern void USBDDriverCallbacks_InterfaceSettingChanged(unsigned char interface,
                                                        unsigned char setting);

#endif //#ifndef USBD_H

