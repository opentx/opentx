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

#include "HIDDJoystickDriver.h"
#include "HIDDJoystickDriverDescriptors.h"
#include <usb/common/core/USBGetDescriptorRequest.h>
#include <usb/common/hid/HIDGenericDescriptor.h>
#include <usb/common/hid/HIDDescriptor.h>
#include <usb/common/hid/HIDGenericRequest.h>
#include <usb/common/hid/HIDReportRequest.h>
#include <usb/common/hid/HIDIdleRequest.h>
#include <usb/device/core/USBD.h>
#include <usb/device/core/USBDDriver.h>
#include <string.h>

#define TRACE_INFO(...)       { }
#define TRACE_WARNING(...)    { }

//------------------------------------------------------------------------------
/// Driver structure for an HID device implementing joystick functionalities.
//------------------------------------------------------------------------------
typedef struct {

    /// Standard USB device driver instance.
    USBDDriver usbdDriver;
    /// Idle rate (in milliseconds) of the input report.
    unsigned char inputReportIdleRate;
    /// 
    unsigned char inputProtocol;
    /// Input report instance.
    HIDDJoystickInputReport inputReport;

} HIDDJoystickDriver;

//------------------------------------------------------------------------------
//         Internal variables
//------------------------------------------------------------------------------

/// Static instance of the HID joystick device driver.
static HIDDJoystickDriver hiddJoystickDriver;

//------------------------------------------------------------------------------
//         Internal functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Returns the descriptor requested by the host.
/// \param type Descriptor type.
/// \param length Maximum number of bytes to send.
/// \return 1 if the request has been handled by this function, otherwise 0.
//------------------------------------------------------------------------------
static unsigned char HIDDJoystickDriver_GetDescriptor(unsigned char type,
                                                      unsigned char length)
{
    extern int hiddReportDescriptorSize;

    const USBConfigurationDescriptor *pConfiguration;
    HIDDescriptor *hidDescriptor;

    switch (type) {

        case HIDGenericDescriptor_REPORT:
            TRACE_INFO("Report ");

            // Adjust length and send report descriptor
            if (length > hiddReportDescriptorSize) {

                length = hiddReportDescriptorSize;
            }
            USBD_Write(0, &hiddReportDescriptor, length, 0, 0);
            break;

        case HIDGenericDescriptor_HID:
            TRACE_INFO("HID ");

            // Configuration descriptor is different depending on configuration
            if (USBD_IsHighSpeed()) {

                pConfiguration =
                    hiddJoystickDriver.usbdDriver.pDescriptors->pHsConfiguration;
            }
            else {

                pConfiguration =
                    hiddJoystickDriver.usbdDriver.pDescriptors->pFsConfiguration;
            }

            // Parse the device configuration to get the HID descriptor
            USBConfigurationDescriptor_Parse(pConfiguration, 0, 0,
                                     (USBGenericDescriptor **) &hidDescriptor);

            // Adjust length and send HID descriptor
            if (length > sizeof(HIDDescriptor)) {

                length = sizeof(HIDDescriptor);
            }
            USBD_Write(0, hidDescriptor, length, 0, 0);
            break;

        default:
            return 0;
    }

    return 1;
}

//------------------------------------------------------------------------------
/// Sends the current Idle rate of the input report to the host.
//------------------------------------------------------------------------------
static void HIDDJoystickDriver_GetIdle()
{
    TRACE_INFO("gIdle ");

    USBD_Write(0, &(hiddJoystickDriver.inputReportIdleRate), 1, 0, 0);
}

//------------------------------------------------------------------------------
/// Retrieves the new idle rate of the input report from the USB host.
/// \param idleRate New input report idle rate.
//------------------------------------------------------------------------------
static void HIDDJoystickDriver_SetIdle(unsigned char idleRate)
{
    TRACE_INFO("sIdle(%d) ", idleRate);

    hiddJoystickDriver.inputReportIdleRate = idleRate;
    USBD_Write(0, 0, 0, 0, 0);
}

//------------------------------------------------------------------------------
/// Sends the requested report to the host.
/// \param type Report type.
/// \param length Maximum number of bytes to send.
//------------------------------------------------------------------------------
static void HIDDJoystickDriver_GetReport(unsigned char type,
                                         unsigned short length)
{
    TRACE_INFO("gReport ");

    // Check report type
    switch (type) {

        case HIDReportRequest_INPUT:
            TRACE_INFO("In ");

            // Adjust size and send report
            if (length > sizeof(HIDDJoystickInputReport)) {

                length = sizeof(HIDDJoystickInputReport);
            }
            USBD_Write(0, // Endpoint #0
                       &(hiddJoystickDriver.inputReport),
                       length,
                       0, // No callback
                       0);
            break;

        default:
            USBD_Stall(0);
    }
}

//------------------------------------------------------------------------------
/// Retrieves the new value of a report from the host and saves it.
/// \param type Report type.
/// \param length Report length.
//------------------------------------------------------------------------------
static void HIDDJoystickDriver_SetReport(unsigned char type,
                                         unsigned short length)
{
    TRACE_INFO("sReport ");

    // Check report type
    switch (type) {
    
        case HIDReportRequest_INPUT:
            // SET_REPORT requests on input reports are ignored
            USBD_Stall(0);
            break;

        default:
            USBD_Stall(0);
    }
}

//------------------------------------------------------------------------------
//         Optional RequestReceived() callback re-implementation
//------------------------------------------------------------------------------
#if !defined(NOAUTOCALLBACK)

//------------------------------------------------------------------------------
/// Callback function when new request receivce from host
/// \param request Pointer to the USBGenericRequest instance
//------------------------------------------------------------------------------
//void USBDCallbacks_RequestReceived(const USBGenericRequest *request)
//{
//    HIDDJoystickDriver_RequestHandler(request);
//}

#endif

//------------------------------------------------------------------------------
//         ConfigurationChanged() callback re-implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Callback function when configureation changed
/// \param cfgnum New configuration number
//------------------------------------------------------------------------------
//void USBDDriverCallbacks_ConfigurationChanged(unsigned char cfgnum)
//{
//    (void)cfgnum;
//}

//------------------------------------------------------------------------------
//      Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes the HID Joystick %device driver.
//------------------------------------------------------------------------------
void HIDDJoystickDriver_Initialize()
{
    hiddJoystickDriver.inputReportIdleRate = 0;
    HIDDJoystickInputReport_Initialize(&(hiddJoystickDriver.inputReport));
    USBDDriver_Initialize(&(hiddJoystickDriver.usbdDriver),
                          &hiddJoystickDriverDescriptors,
                          0); // Multiple interface settings not supported
    USBD_Init();
}

//------------------------------------------------------------------------------
/// Handles HID-specific SETUP request sent by the host.
/// \param request Pointer to a USBGenericRequest instance
//------------------------------------------------------------------------------
void HIDDJoystickDriver_RequestHandler(const USBGenericRequest *request)
{
    TRACE_INFO("NewReq ");

    // Check if this is a standard request
    if (USBGenericRequest_GetType(request) == USBGenericRequest_STANDARD) {

        // This is a standard request
        switch (USBGenericRequest_GetRequest(request)) {
        
            case USBGenericRequest_GETDESCRIPTOR:
                // Check if this is a HID descriptor, otherwise forward it to
                // the standard driver
                if (!HIDDJoystickDriver_GetDescriptor(
                        USBGetDescriptorRequest_GetDescriptorType(request),
                        USBGenericRequest_GetLength(request))) {

                    USBDDriver_RequestHandler(&(hiddJoystickDriver.usbdDriver),
                                              request);
                }
                break;

            default:
                USBDDriver_RequestHandler(&(hiddJoystickDriver.usbdDriver),
                                              request);
        }
    }
    // Check if this is a class request
    else if (USBGenericRequest_GetType(request) == USBGenericRequest_CLASS) {

        // This is a class-specific request
        switch (USBGenericRequest_GetRequest(request)) {

            case HIDGenericRequest_GETIDLE:
                HIDDJoystickDriver_GetIdle();
                break;

            case HIDGenericRequest_SETIDLE:
                HIDDJoystickDriver_SetIdle(HIDIdleRequest_GetIdleRate(request));
                break;

            case HIDGenericRequest_GETREPORT:
                HIDDJoystickDriver_GetReport(
                    HIDReportRequest_GetReportType(request),
                    USBGenericRequest_GetLength(request));
                break;

            case HIDGenericRequest_SETREPORT:
                HIDDJoystickDriver_SetReport(
                    HIDReportRequest_GetReportType(request),
                    USBGenericRequest_GetLength(request));
                break;

            case HIDGenericRequest_GETPROTOCOL:
                USBD_Write(0, &hiddJoystickDriver.inputProtocol, 1, 0, 0);
                break;

            case HIDGenericRequest_SETPROTOCOL:
                hiddJoystickDriver.inputProtocol = request->wValue;
                USBD_Write(0, 0, 0, 0, 0);
                break;

            default:
                TRACE_WARNING(
                  "HIDDJoystickDriver_RequestHandler: Unknown request 0x%02X\n\r",
                  USBGenericRequest_GetRequest(request));
                USBD_Stall(0);
        }
    }
    else {

        // Vendor request ?
        USBD_Stall(0);
    }
}

//------------------------------------------------------------------------------
/// Update the Joystick state via input report
/// to host
/// \param report Pointer to a HIDDJoystickInputReport instance
//------------------------------------------------------------------------------
unsigned char HIDDJoystickDriver_ChangeJoystickState(const HIDDJoystickInputReport *report)
{
    memcpy(&(hiddJoystickDriver.inputReport), report, sizeof(HIDDJoystickInputReport));

    // Send input report through the interrupt IN endpoint
    return USBD_Write(HIDDJoystickDriverDescriptors_INTERRUPTIN,
                      &(hiddJoystickDriver.inputReport),
                      sizeof(HIDDJoystickInputReport),
                      0,
                      0);
}
