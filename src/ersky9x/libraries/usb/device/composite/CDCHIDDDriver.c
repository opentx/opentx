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
//      Headers
//-----------------------------------------------------------------------------

// GENERAL
#include <utility/trace.h>
#include <utility/assert.h>
#include <utility/led.h>

// USB
#include <usb/device/core/USBD.h>
#include <usb/device/core/USBDDriver.h>

//- HID
#include <usb/device/hid-keyboard/HIDDKeyboardDriver.h>
#include <usb/device/hid-keyboard/HIDDKeyboardDriverDescriptors.h>
#include <usb/device/hid-keyboard/HIDDKeyboardCallbacks.h>
#include <usb/device/hid-keyboard/HIDDKeyboardInputReport.h>
#include <usb/device/hid-keyboard/HIDDKeyboardOutputReport.h>

#include <usb/common/hid/HIDGenericDescriptor.h>
#include <usb/common/hid/HIDDescriptor.h>
#include <usb/common/hid/HIDGenericRequest.h>
#include <usb/common/hid/HIDReportRequest.h>
#include <usb/common/hid/HIDIdleRequest.h>
#include <usb/common/hid/HIDKeypad.h>

//- MSD
#if defined(usb_CDCMSD) || defined(usb_HIDMSD)
#endif

//- CDCHID
#include "CDCHIDDDriver.h"
#include "CDCHIDDDriverDescriptors.h"

//-----------------------------------------------------------------------------
//         Defines
//-----------------------------------------------------------------------------

/// Interface setting spaces (4 byte aligned)
#define NUM_INTERFACES  ((CDCHIDDDriverDescriptors_NUMINTERFACE+3)&0xFC)

//-----------------------------------------------------------------------------
//         Types
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//         Internal variables
//-----------------------------------------------------------------------------

/// USBDDriver instance
static USBDDriver usbdDriver;

/// CDCDSeriaoPort instance
static CDCDSerialPort cdcdPort;

/// Array for storing the current setting of each interface
static unsigned char cdchiddDriverInterfaces[NUM_INTERFACES];

//-----------------------------------------------------------------------------
//         Internal functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//         Optional RequestReceived() callback re-implementation
//-----------------------------------------------------------------------------
#if !defined(NOAUTOCALLBACK)

void USBDCallbacks_RequestReceived(const USBGenericRequest *request)
{
    CDCHIDDDriver_RequestHandler(request);
}

#endif

//-----------------------------------------------------------------------------
//         ConfigurationChanged() callback re-implementation
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Invoked whenever the configuration value of a device is changed by the host
/// \param cfgnum Configuration number.
//-----------------------------------------------------------------------------
void USBDDriverCallbacks_ConfigurationChanged(unsigned char cfgnum)
{
    // HID
    HIDDFunctionCallbacks_ConfigurationChanged(cfgnum);
}

//-----------------------------------------------------------------------------
//         Exported functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Initializes the USB device composite device driver.
//-----------------------------------------------------------------------------
void CDCHIDDDriver_Initialize()
{
    // CDC
    CDCDFunctionDriver_ConfigurePort(&cdcdPort,
                                     CDCD_Descriptors_INTERFACENUM0,
                                     CDCD_Descriptors_NOTIFICATION0,
                                     CDCD_Descriptors_DATAIN0,
                                     CDCD_Descriptors_DATAOUT0);
    CDCDFunctionDriver_Initialize(&usbdDriver,
                                  &cdcdPort,
                                  1);

    // HID
    HIDDFunctionDriver_Initialize(&usbdDriver,
                                  HIDD_Descriptors_INTERFACENUM,
                                  HIDD_Descriptors_INTERRUPTIN,
                                  HIDD_Descriptors_INTERRUPTOUT);

    // Initialize the standard USB driver
    USBDDriver_Initialize(&usbdDriver,
                          &cdchiddDriverDescriptors,
                          cdchiddDriverInterfaces);

    // Initialize the USB driver
    USBD_Init();
}

//-----------------------------------------------------------------------------
/// Handles composite-specific USB requests sent by the host, and forwards
/// standard ones to the USB device driver.
/// \param request Pointer to a USBGenericRequest instance.
//-----------------------------------------------------------------------------
void CDCHIDDDriver_RequestHandler(const USBGenericRequest *request)
{
    // Check if this is a class request
    if (USBGenericRequest_GetType(request) == USBGenericRequest_CLASS) {

        unsigned char rc = 0;

        // CDC class request
        if (rc == 0) {

            rc = CDCDFunctionDriver_RequestHandler(request);
        }

        // HID class request
        if (rc == 0) {

            rc = HIDDFunctionDriver_RequestHandler(request);
        }

        if (!rc) {

            TRACE_WARNING(
              "CDCHIDDDriver_RequestHandler: Unsupported request (%d)\n\r",
              USBGenericRequest_GetRequest(request));
            USBD_Stall(0);
        }
        
    }
    // Check if this is a standard request
    else if (USBGenericRequest_GetType(request) == USBGenericRequest_STANDARD) {

        unsigned char rc = 0;

        rc = HIDDFunctionDriver_RequestHandler(request);

        // Forward request to the standard handler
        if (rc == 0) {

            USBDDriver_RequestHandler(&(usbdDriver), request);
        }
    }
    // Unsupported request type
    else {

        TRACE_WARNING(
          "CDCHIDDDriver_RequestHandler: Unsupported request type (%d)\n\r",
          USBGenericRequest_GetType(request));
        USBD_Stall(0);
    }
}

//-----------------------------------------------------------------------------
/// Starts a remote wake-up sequence if the host has explicitely enabled it
/// by sending the appropriate SET_FEATURE request.
//-----------------------------------------------------------------------------
void CDCHIDDDriver_RemoteWakeUp(void)
{
    // Remote wake-up has been enabled
    if (USBDDriver_IsRemoteWakeUpEnabled(&usbdDriver)) {

        USBD_RemoteWakeUp();
    }
    // Remote wake-up NOT enabled
    else {

        TRACE_WARNING("CDCHIDDDriver_RemoteWakeUp: not enabled\n\r");
    }
}


