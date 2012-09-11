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

/*
    Title: CDCDSerialDriver implementation

    About: Purpose
        Implementation of the CDCDSerialDriver class methods.
*/

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "CDCDSerialDriver.h"
#include "CDCDSerialDriverDescriptors.h"
#include <utility/trace.h>
#include <utility/assert.h>
#include <usb/device/core/USBDDriver.h>
#include <usb/common/cdc/CDCLineCoding.h>
#include <usb/common/cdc/CDCGenericRequest.h>
#include <usb/common/cdc/CDCSetControlLineStateRequest.h>

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// USB driver for a CDC class implementing a virtual COM serial connection.
//------------------------------------------------------------------------------
typedef struct {

    /// Standard USBDDriver instance.
    USBDDriver usbdDriver;
    /// Current line coding (baudrate, parity, stop bits).
    CDCLineCoding lineCoding;
    /// Indicates if the RS232 carrier is active.
    unsigned char isCarrierActivated;
    /// Current serial port states
    unsigned short serialState;

} CDCDSerialDriver;

//------------------------------------------------------------------------------
//         Internal variables
//------------------------------------------------------------------------------

/// Static instance of the CDC serial driver.
static CDCDSerialDriver cdcdSerialDriver;

//------------------------------------------------------------------------------
//         Internal functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Callback function which should be invoked after the data of a
/// SetLineCoding request has been retrieved. Sends a zero-length packet
/// to the host for acknowledging the request.
//------------------------------------------------------------------------------
static void CDCDSerialDriver_SetLineCodingCallback()
{
    USBD_Write(0, 0, 0, 0, 0);
}

//------------------------------------------------------------------------------
/// Receives new line coding information from the USB host.
//------------------------------------------------------------------------------
static void CDCDSerialDriver_SetLineCoding()
{
    TRACE_INFO_WP("sLineCoding ");

    USBD_Read(0,
              (void *) &(cdcdSerialDriver.lineCoding),
              sizeof(CDCLineCoding),
              (TransferCallback) CDCDSerialDriver_SetLineCodingCallback,
              0);
}

//------------------------------------------------------------------------------
/// Sends the current line coding information to the host through Control
/// endpoint 0.
//------------------------------------------------------------------------------
static void CDCDSerialDriver_GetLineCoding()
{
    TRACE_INFO_WP("gLineCoding ");

    USBD_Write(0,
               (void *) &(cdcdSerialDriver.lineCoding),
               sizeof(CDCLineCoding),
               0,
               0);
}

//------------------------------------------------------------------------------
/// Changes the state of the serial driver according to the information
/// sent by the host via a SetControlLineState request, and acknowledges
/// the request with a zero-length packet.
//------------------------------------------------------------------------------
static void CDCDSerialDriver_SetControlLineState(unsigned char activateCarrier,
                                                 unsigned char isDTEPresent)
{
    TRACE_INFO_WP(
              "sControlLineState(%d, %d) ",
              activateCarrier,
              isDTEPresent);

    cdcdSerialDriver.isCarrierActivated = activateCarrier;
    USBD_Write(0, 0, 0, 0, 0);
}

//------------------------------------------------------------------------------
//         Optional RequestReceived() callback re-implementation
//------------------------------------------------------------------------------
#if !defined(NOAUTOCALLBACK)

//------------------------------------------------------------------------------
/// Re-implemented callback, invoked when a new USB Request is received.
//------------------------------------------------------------------------------
void USBDCallbacks_RequestReceived(const USBGenericRequest *request)
{
    CDCDSerialDriver_RequestHandler(request);
}

#endif

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes the USB Device CDC serial driver & USBD Driver.
//------------------------------------------------------------------------------
void CDCDSerialDriver_Initialize()
{
    TRACE_INFO("CDCDSerialDriver_Initialize\n\r");
    
    // Initialize Abstract Control Model attributes
    CDCLineCoding_Initialize(&(cdcdSerialDriver.lineCoding),
                             115200,
                             CDCLineCoding_ONESTOPBIT,
                             CDCLineCoding_NOPARITY,
                             8);
    cdcdSerialDriver.isCarrierActivated = 0;
    cdcdSerialDriver.serialState = 0;

    // Initialize the standard driver
    USBDDriver_Initialize(&(cdcdSerialDriver.usbdDriver),
                          &cdcdSerialDriverDescriptors,
                          0); // Multiple settings for interfaces not supported

    // Initialize the USB driver
    USBD_Init();
}

//------------------------------------------------------------------------------
/// Handles CDC-specific SETUP requests. Should be called from a
/// re-implementation of USBDCallbacks_RequestReceived() method.
/// \param Pointer to a USBGenericRequest instance.
//------------------------------------------------------------------------------
void CDCDSerialDriver_RequestHandler(const USBGenericRequest *request)
{
    TRACE_INFO_WP("NewReq ");

    // Handle the request
    switch (USBGenericRequest_GetRequest(request)) {

        case CDCGenericRequest_SETLINECODING:
            
            CDCDSerialDriver_SetLineCoding();
            break;

        case CDCGenericRequest_GETLINECODING:

            CDCDSerialDriver_GetLineCoding();
            break;

        case CDCGenericRequest_SETCONTROLLINESTATE:

            CDCDSerialDriver_SetControlLineState(
                CDCSetControlLineStateRequest_ActivateCarrier(request),
                CDCSetControlLineStateRequest_IsDtePresent(request));

            break;

        default:

            USBDDriver_RequestHandler(&(cdcdSerialDriver.usbdDriver), request);
            break;
    }
}

//------------------------------------------------------------------------------
/// Receives data from the host through the virtual COM port created by
/// the CDC device serial driver. This function behaves like USBD_Read.
/// \param data Pointer to the data buffer to put received data.
/// \param size Size of the data buffer in bytes.
/// \param callback Optional callback function to invoke when the transfer
///                 finishes.
/// \param argument Optional argument to the callback function.
/// \return USBD_STATUS_SUCCESS if the read operation has been started normally;
///         otherwise, the corresponding error code.
//------------------------------------------------------------------------------
unsigned char CDCDSerialDriver_Read(void *data,
                                    unsigned int size,
                                    TransferCallback callback,
                                    void *argument)
{
    return USBD_Read(CDCDSerialDriverDescriptors_DATAOUT,
                     data,
                     size,
                     callback,
                     argument);
}

//------------------------------------------------------------------------------
/// Sends a data buffer through the virtual COM port created by the CDC
/// device serial driver. This function behaves exactly like USBD_Write.
/// \param data Pointer to the data buffer to send.
/// \param size Size of the data buffer in bytes.
/// \param callback Optional callback function to invoke when the transfer
///                 finishes.
/// \param argument Optional argument to the callback function.
/// \return USBD_STATUS_SUCCESS if the read operation has been started normally;
///         otherwise, the corresponding error code.
//------------------------------------------------------------------------------
unsigned char CDCDSerialDriver_Write(void *data,
                                     unsigned int size,
                                     TransferCallback callback,
                                     void *argument)
{
    return USBD_Write(CDCDSerialDriverDescriptors_DATAIN,
                      data,
                      size,
                      callback,
                      argument);
}

//------------------------------------------------------------------------------
/// Returns the current status of the RS-232 line.
//------------------------------------------------------------------------------
unsigned short CDCDSerialDriver_GetSerialState()
{
    return cdcdSerialDriver.serialState;
}

//------------------------------------------------------------------------------
/// Sets the current serial state of the device to the given value.
/// \param serialState  New device state.
//------------------------------------------------------------------------------
void CDCDSerialDriver_SetSerialState(unsigned short serialState)
{
    ASSERT((serialState & 0xFF80) == 0,
           "CDCDSerialDriver_SetSerialState: Bits D7-D15 are reserved\n\r");

    // If new state is different from previous one, send a notification to the
    // host
    if (cdcdSerialDriver.serialState != serialState) {

        cdcdSerialDriver.serialState = serialState;
        USBD_Write(CDCDSerialDriverDescriptors_NOTIFICATION,
                   &(cdcdSerialDriver.serialState),
                   2,
                   0,
                   0);

        // Reset one-time flags
        cdcdSerialDriver.serialState &= ~(CDCDSerialDriver_STATE_OVERRUN
                                          | CDCDSerialDriver_STATE_PARITY
                                          | CDCDSerialDriver_STATE_FRAMING
                                          | CDCDSerialDriver_STATE_RINGSIGNAL
                                          | CDCDSerialDriver_STATE_BREAK);
    }
}

