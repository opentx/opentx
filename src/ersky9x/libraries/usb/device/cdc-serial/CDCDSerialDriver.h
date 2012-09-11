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

/**
 \unit

 !!!Purpose

 Definition of a class for implementing a USB device CDC serial driver.

 !!!Usage

 -# Re-implement the USBDCallbacks_RequestReceived method to pass
    received requests to CDCDSerialDriver_RequestHandler. *This is
    automatically done unless the NOAUTOCALLBACK symbol is defined*.
 -# Initialize the CDC serial and USB drivers using
    CDCDSerialDriver_Initialize.
 -# Logically connect the device to the host using USBD_Connect.
 -# Send serial data to the USB host using CDCDSerialDriver_Write.
 -# Receive serial data from the USB host using CDCDSerialDriver_Read.
*/

#ifndef CDCDSERIALDRIVER_H
#define CDCDSERIALDRIVER_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include <usb/common/core/USBGenericRequest.h>
#include <usb/device/core/USBD.h>

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "CDC Serial Port States"
/// This page lists the bit map for CDC Serial Port States.
///
/// !BitMaps
/// - CDCDSerialDriver_STATE_RXDRIVER
/// - CDCDSerialDriver_STATE_TXCARRIER
/// - CDCDSerialDriver_STATE_BREAK
/// - CDCDSerialDriver_STATE_RINGSIGNAL
/// - CDCDSerialDriver_STATE_FRAMING
/// - CDCDSerialDriver_STATE_PARITY
/// - CDCDSerialDriver_STATE_OVERRUN

/// Indicates the receiver carrier signal is present.
#define CDCDSerialDriver_STATE_RXDRIVER         (1 << 0)
/// Indicates the transmission carrier signal is present.
#define CDCDSerialDriver_STATE_TXCARRIER        (1 << 1)
/// Indicates a break has been detected.
#define CDCDSerialDriver_STATE_BREAK            (1 << 2)
/// Indicates a ring signal has been detected.
#define CDCDSerialDriver_STATE_RINGSIGNAL       (1 << 3)
/// Indicates a framing error has occured.
#define CDCDSerialDriver_STATE_FRAMING          (1 << 4)
/// Indicates a parity error has occured.
#define CDCDSerialDriver_STATE_PARITY           (1 << 5)
/// Indicates a data overrun error has occured.
#define CDCDSerialDriver_STATE_OVERRUN          (1 << 6)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//      Exported functions
//------------------------------------------------------------------------------

extern void CDCDSerialDriver_Initialize();

extern void CDCDSerialDriver_RequestHandler(const USBGenericRequest *request);

extern unsigned char CDCDSerialDriver_Write(
    void *data,
    unsigned int size,
    TransferCallback callback,
    void *argument);

extern unsigned char CDCDSerialDriver_Read(
    void *data,
    unsigned int size,
    TransferCallback callback,
    void *argument);

extern unsigned short CDCDSerialDriver_GetSerialState();

extern void CDCDSerialDriver_SetSerialState(unsigned short serialState);

#endif //#ifndef CDCSERIALDRIVER_H

