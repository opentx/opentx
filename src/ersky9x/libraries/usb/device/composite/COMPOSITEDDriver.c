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
#if defined(usb_CDCHID) || defined(usb_HIDAUDIO) || defined(usb_HIDMSD)
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
#endif // (HID defined)

//- MSD
#if defined(usb_CDCMSD) || defined(usb_HIDMSD)
#endif

//- COMPOSITE
#include "COMPOSITEDDriver.h"
#include "COMPOSITEDDriverDescriptors.h"

//-----------------------------------------------------------------------------
//         Defines
//-----------------------------------------------------------------------------

/// Interface setting spaces (4 byte aligned)
#define NUM_INTERFACES  ((COMPOSITEDDriverDescriptors_NUMINTERFACE+3)&0xFC)

//-----------------------------------------------------------------------------
//         Types
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//         Internal variables
//-----------------------------------------------------------------------------

/// USBDDriver instance
static USBDDriver usbdDriver;

// CDC
#if defined(usb_CDCAUDIO) || defined(usb_CDCHID) || defined(usb_CDCCDC) || defined(usb_CDCMSD)
/// CDCDSeriaoPort instance
static CDCDSerialPort cdcdPort;
#endif // CDC defined

/// Array for storing the current setting of each interface
static unsigned char compositedDriverInterfaces[NUM_INTERFACES];

//-----------------------------------------------------------------------------
//         Internal functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//         Optional RequestReceived() callback re-implementation
//-----------------------------------------------------------------------------
#if !defined(NOAUTOCALLBACK)

void USBDCallbacks_RequestReceived(const USBGenericRequest *request)
{
    COMPOSITEDDriver_RequestHandler(request);
}

#endif

//-----------------------------------------------------------------------------
/// Invoked whenever the active setting of an interface is changed by the
/// host. Changes the status of the third LED accordingly.
/// \param interface Interface number.
/// \param setting Newly active setting.
//-----------------------------------------------------------------------------
void USBDDriverCallbacks_InterfaceSettingChanged(unsigned char interface,
                                                 unsigned char setting)
{
    // AUDIO
  #if defined(usb_CDCAUDIO) || defined(usb_HIDAUDIO)
    AUDDFunctionCallbacks_InterfaceSettingChanged(interface, setting);
  #endif
}

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
  #if defined(usb_CDCHID) || defined(usb_HIDAUDIO) || defined(usb_HIDMSD)
    HIDDFunctionCallbacks_ConfigurationChanged(cfgnum);
  #endif
}

//-----------------------------------------------------------------------------
//         Exported functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Initializes the USB device composite device driver.
//-----------------------------------------------------------------------------
void COMPOSITEDDriver_Initialize()
{
    // CDC
  #if defined(usb_CDCAUDIO) || defined(usb_CDCHID) || defined(usb_CDCCDC) || defined(usb_CDCMSD)
    CDCDFunctionDriver_ConfigurePort(&cdcdPort,
                                     CDCD_Descriptors_INTERFACENUM0,
                                     CDCD_Descriptors_NOTIFICATION0,
                                     CDCD_Descriptors_DATAIN0,
                                     CDCD_Descriptors_DATAOUT0);
    CDCDFunctionDriver_Initialize(&usbdDriver,
                                  &cdcdPort,
                                  1);
  #endif

    // AUDIO
  #if defined(usb_CDCAUDIO) || defined(usb_HIDAUDIO)
    AUDDFunctionDriver_Initialize();
  #endif

    // HID
  #if defined(usb_CDCHID) || defined(usb_HIDAUDIO) || defined(usb_HIDMSD)
    HIDDFunctionDriver_Initialize(&usbdDriver,
                                  HIDD_Descriptors_INTERFACENUM,
                                  HIDD_Descriptors_INTERRUPTIN,
                                  HIDD_Descriptors_INTERRUPTOUT);
  #endif

    // MSD
  #if defined(usb_CDCMSD) || defined(usb_HIDMSD)
    // Function driver initialize is put to main() for additional LUN list
  #endif

    // Initialize the standard USB driver
    USBDDriver_Initialize(&usbdDriver,
                          &compositedDriverDescriptors,
                          compositedDriverInterfaces);

    // Initialize the USB driver
    USBD_Init();
}

//-----------------------------------------------------------------------------
/// Handles composite-specific USB requests sent by the host, and forwards
/// standard ones to the USB device driver.
/// \param request Pointer to a USBGenericRequest instance.
//-----------------------------------------------------------------------------
void COMPOSITEDDriver_RequestHandler(const USBGenericRequest *request)
{
    // Check if this is a class request
    if (USBGenericRequest_GetType(request) == USBGenericRequest_CLASS) {

        unsigned char rc = 0;

        // AUDIO class request
      #if defined(usb_CDCAUDIO) || defined(usb_HIDAUDIO)
        if (rc == 0) {

            rc = AUDDFunctionDriver_RequestHandler(request);
        }
      #endif

        // CDC class request
      #if defined(usb_CDCAUDIO) || defined(usb_CDCHID) || defined(usb_CDCMSD) || defined(usb_CDCCDC)
        if (rc == 0) {

            rc = CDCDFunctionDriver_RequestHandler(request);
        }
      #endif

        // MSD class request
      #if defined(usb_CDCMSD) || defined(usb_HIDMSD)
        if (rc == 0) {

            rc = MSDDFunctionDriver_RequestHandler(request);
        }
      #endif

        // HID class request
      #if defined(usb_CDCHID) || defined(usb_HIDAUDIO) || defined(usb_HIDMSD)
        if (rc == 0) {

            rc = HIDDFunctionDriver_RequestHandler(request);
        }
      #endif

        if (!rc) {

            TRACE_WARNING(
              "COMPOSITEDDriver_RequestHandler: Unsupported request (%d)\n\r",
              USBGenericRequest_GetRequest(request));
            USBD_Stall(0);
        }
        
    }
    // Check if this is a standard request
    else if (USBGenericRequest_GetType(request) == USBGenericRequest_STANDARD) {

        unsigned char rc = 0;

      #if defined(usb_CDCHID) || defined(usb_HIDAUDIO) || defined(usb_HIDMSD)
        rc = HIDDFunctionDriver_RequestHandler(request);
      #endif

      #if defined(usb_CDCMSD) || defined(usb_HIDMSD)
        if (rc == 0) {

            rc = MSDDFunctionDriver_RequestHandler(request);
        }
      #endif

        // Forward request to the standard handler
        if (rc == 0) {

            USBDDriver_RequestHandler(&(usbdDriver), request);
        }
    }
    // Unsupported request type
    else {

        TRACE_WARNING(
          "COMPOSITEDDriver_RequestHandler: Unsupported request type (%d)\n\r",
          USBGenericRequest_GetType(request));
        USBD_Stall(0);
    }
}

//-----------------------------------------------------------------------------
/// Starts a remote wake-up sequence if the host has explicitely enabled it
/// by sending the appropriate SET_FEATURE request.
//-----------------------------------------------------------------------------
void COMPOSITEDDriver_RemoteWakeUp(void)
{
    // Remote wake-up has been enabled
    if (USBDDriver_IsRemoteWakeUpEnabled(&usbdDriver)) {

        USBD_RemoteWakeUp();
    }
    // Remote wake-up NOT enabled
    else {

        TRACE_WARNING("COMPOSITEDDriver_RemoteWakeUp: not enabled\n\r");
    }
}


