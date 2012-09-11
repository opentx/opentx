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
//         Headers
//-----------------------------------------------------------------------------

// GENERAL
#include <utility/trace.h>
#include <utility/assert.h>
// USB
#include <usb/device/core/USBD.h>
#include <usb/common/core/USBGetDescriptorRequest.h>
#include <usb/device/core/USBDDriver.h>
// HID
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

#include "HIDDFunctionDriver.h"
#include "HIDDFunctionDriverDescriptors.h"

//-----------------------------------------------------------------------------
//         Internal types
//-----------------------------------------------------------------------------

/// Driver structure for an HID device implementing keyboard functionalities.
typedef struct {

    /// Pointer to USB device driver instance
    USBDDriver * pUsbdDriver;
    /// Interface Number to access this function
    unsigned char interfaceNum;
    /// Interrupt IN endpoint address
    unsigned char interruptInEndpoint;
    /// Interrupt OUT endpoint address
    unsigned char interruptOutEndpoint;
    /// Idle rate (in milliseconds) of the input report
    unsigned char inputReportIdleRate;
    /// Input report instance.
    HIDDKeyboardInputReport inputReport;
    /// Output report instance.
    HIDDKeyboardOutputReport outputReport;

} HIDDKeyboardDriver;

//-----------------------------------------------------------------------------
//         Internal variables
//-----------------------------------------------------------------------------

/// Static instance of the HID keyboard device driver.
static HIDDKeyboardDriver hiddKeyboardDriver;

//-----------------------------------------------------------------------------
//         Internal functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Returns the descriptor requested by the host.
/// \param type Descriptor type.
/// \param length Maximum number of bytes to send.
/// \return 1 if the request has been handled by this function, otherwise 0.
//-----------------------------------------------------------------------------
static unsigned char HIDD_GetDescriptor(unsigned char type,
                                        unsigned char length)
{
    const USBConfigurationDescriptor *pConfiguration;
    HIDDescriptor *hidDescriptor;

    switch (type) {

        case HIDGenericDescriptor_REPORT:
            TRACE_INFO_WP("Report ");

            // Adjust length and send report descriptor
            if (length > HIDD_Descriptors_REPORTSIZE) {

                length = HIDD_Descriptors_REPORTSIZE;
            }
            USBD_Write(0, &hiddReportDescriptor, length, 0, 0);
            break;

        case HIDGenericDescriptor_HID:
            TRACE_INFO_WP("HID ");

            // Configuration descriptor is different depending on speed
            if (USBD_IsHighSpeed()) {

                pConfiguration = hiddKeyboardDriver
                    .pUsbdDriver->pDescriptors->pHsConfiguration;
            }
            else {

                pConfiguration = hiddKeyboardDriver
                    .pUsbdDriver->pDescriptors->pFsConfiguration;
            }

            // Parse the device configuration to get the HID descriptor
            USBConfigurationDescriptor_Parse(pConfiguration,
                                     0,
                                     0,
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

//-----------------------------------------------------------------------------
/// Sends the current Idle rate of the input report to the host.
//-----------------------------------------------------------------------------
static void HIDD_GetIdle()
{
    TRACE_INFO_WP("gIdle ");

    USBD_Write(0, &(hiddKeyboardDriver.inputReportIdleRate), 1, 0, 0);
}

//-----------------------------------------------------------------------------
/// Retrieves the new idle rate of the input report from the USB host.
/// \param idleRate New input report idle rate.
//-----------------------------------------------------------------------------
static void HIDD_SetIdle(unsigned char idleRate)
{
    TRACE_INFO_WP("sIdle(%d) ", idleRate);

    hiddKeyboardDriver.inputReportIdleRate = idleRate;
    USBD_Write(0, 0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
/// Sends the requested report to the host.
/// \param type Report type.
/// \param length Maximum number of bytes to send.
//-----------------------------------------------------------------------------
static void HIDD_GetReport(unsigned char type,
                           unsigned short length)
{
    TRACE_INFO_WP("gReport ");

    // Check report type
    switch (type) {

        case HIDReportRequest_INPUT:
            TRACE_INFO_WP("In ");

            // Adjust size and send report
            if (length > sizeof(HIDDKeyboardInputReport)) {

                length = sizeof(HIDDKeyboardInputReport);
            }
            USBD_Write(0, // Endpoint #0
                       &(hiddKeyboardDriver.inputReport),
                       length,
                       0, // No callback
                       0);
            break;

        case HIDReportRequest_OUTPUT:
            TRACE_INFO_WP("Out ");

            // Adjust size and send report
            if (length > sizeof(HIDDKeyboardOutputReport)) {

                length = sizeof(HIDDKeyboardOutputReport);
            }
            USBD_Write(0, // Endpoint #0
                       &(hiddKeyboardDriver.outputReport),
                       length,
                       0, // No callback
                       0);
            break;

        default:
            USBD_Stall(0);
    }
}

//-----------------------------------------------------------------------------
/// Callback invoked when an output report has been received from the host.
/// Forward the new status of the LEDs to the user program via the
//-----------------------------------------------------------------------------
static void HIDD_ReportReceived()
{
    TRACE_INFO_WP("oReport ");

    // Trigger callback
    HIDDKeyboardCallbacks_LedsChanged(
        HIDDKeyboardOutputReport_GetNumLockStatus(
                                &(hiddKeyboardDriver.outputReport)),
        HIDDKeyboardOutputReport_GetCapsLockStatus(
                                &(hiddKeyboardDriver.outputReport)),
        HIDDKeyboardOutputReport_GetScrollLockStatus(
                                &(hiddKeyboardDriver.outputReport)));

    // Restart transfer
    USBD_Read(hiddKeyboardDriver.interruptOutEndpoint,
              &(hiddKeyboardDriver.outputReport),
              sizeof(HIDDKeyboardOutputReport),
              (TransferCallback) HIDD_ReportReceived,
              0); // No argument for callback function
}

//-----------------------------------------------------------------------------
/// Retrieves the new value of a report from the host and saves it.
/// \param type Report type.
/// \param length Report length.
//-----------------------------------------------------------------------------
static void HIDD_SetReport(unsigned char type,
                           unsigned short length)
{
    TRACE_INFO_WP("sReport ");

    // Check report type
    switch (type) {
    
        case HIDReportRequest_INPUT:
            // SET_REPORT requests on input reports are ignored
            USBD_Stall(0);
            break;

        case HIDReportRequest_OUTPUT:
            // Check report length
            if (length != sizeof(HIDDKeyboardOutputReport)) {

                USBD_Stall(0);
            }
            else {
            
                USBD_Read(0, // Endpoint #0
                          &(hiddKeyboardDriver.outputReport),
                          length,
                          (TransferCallback) HIDD_ReportReceived,
                          0); // No argument to the callback function
            }
            break;

        default:
            USBD_Stall(0);
    }
}

//-----------------------------------------------------------------------------
//         Exported functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Initializes an USB HID keyboard function driver.
/// \param pUsbdDriver Pointer to the USB driver instance.
/// \param interfaceNum Interface number to access the MSD function
/// \param interruptInEndpoint Interrupt IN endpoint address
/// \param interruptOutEndpoint Interrupt OUT endpoint address
//-----------------------------------------------------------------------------
void HIDDFunctionDriver_Initialize(USBDDriver * pUsbdDriver,
                                   unsigned char interfaceNum,
                                   unsigned char interruptInEndpoint,
                                   unsigned char interruptOutEndpoint)
{
    hiddKeyboardDriver.inputReportIdleRate = 0;
    HIDDKeyboardInputReport_Initialize(&(hiddKeyboardDriver.inputReport));
    HIDDKeyboardOutputReport_Initialize(&(hiddKeyboardDriver.outputReport));

    hiddKeyboardDriver.pUsbdDriver = pUsbdDriver;
    hiddKeyboardDriver.interfaceNum = interfaceNum;
    hiddKeyboardDriver.interruptInEndpoint = interruptInEndpoint;
    hiddKeyboardDriver.interruptOutEndpoint = interruptOutEndpoint;
}

//-----------------------------------------------------------------------------
/// Handles HID-specific SETUP request sent by the host.
/// \param request Pointer to a USBGenericRequest instance.
/// \return 0 if the request is Unsupported, 1 if the request handled.
//-----------------------------------------------------------------------------
unsigned char HIDDFunctionDriver_RequestHandler(
    const USBGenericRequest *request)
{
    TRACE_INFO_WP("NewReq ");

    // Check if this is a standard request
    if (USBGenericRequest_GetType(request) == USBGenericRequest_STANDARD) {

        // This is a standard request
        switch (USBGenericRequest_GetRequest(request)) {
        
            case USBGenericRequest_GETDESCRIPTOR:
                // Check if this is a HID descriptor, otherwise forward it to
                // the standard driver
                if (!HIDD_GetDescriptor(
                        USBGetDescriptorRequest_GetDescriptorType(request),
                        USBGenericRequest_GetLength(request))) {

                    USBDDriver_RequestHandler(hiddKeyboardDriver.pUsbdDriver,
                                              request);
                }
                break;

            default:
                return 0;
        }
    }
    // Check if this is a class request
    else if (USBGenericRequest_GetType(request) == USBGenericRequest_CLASS) {

        // This is a class-specific request
        switch (USBGenericRequest_GetRequest(request)) {

            case HIDGenericRequest_GETIDLE:
                HIDD_GetIdle();
                break;

            case HIDGenericRequest_SETIDLE:
                HIDD_SetIdle(HIDIdleRequest_GetIdleRate(request));
                break;

            case HIDGenericRequest_GETREPORT:
                HIDD_GetReport(
                    HIDReportRequest_GetReportType(request),
                    USBGenericRequest_GetLength(request));
                break;

            case HIDGenericRequest_SETREPORT:
                HIDD_SetReport(
                    HIDReportRequest_GetReportType(request),
                    USBGenericRequest_GetLength(request));
                break;

            default:
                return 0;
        }
    }
    return 1;
}

//-----------------------------------------------------------------------------
/// Invoked whenever the configuration of the device is changed by the host.
/// \param cfgnum Newly configuration number.
//-----------------------------------------------------------------------------
void HIDDFunctionCallbacks_ConfigurationChanged(unsigned char cfgnum)
{
    if (cfgnum > 0) {
    
        // Start receiving output reports
        USBD_Read(hiddKeyboardDriver.interruptOutEndpoint,
                  &(hiddKeyboardDriver.outputReport),
                  sizeof(HIDDKeyboardOutputReport),
                  (TransferCallback) HIDD_ReportReceived,
                  0); // No argument for callback function
    }
}

//-----------------------------------------------------------------------------
/// Reports a change in which keys are currently pressed or release to the
/// host.
/// \param pressedKeys Pointer to an array of key codes indicating keys that
///         have been pressed since the last call to
///         <HIDDKeyboardDriver_ChangeKeys>.
/// \param pressedKeysSize Number of key codes in the pressedKeys array.
/// \param releasedKeys Pointer to an array of key codes indicates keys that
///         have been released since the last call to
///         <HIDDKeyboardDriver_ChangeKeys>.
/// \param releasedKeysSize Number of key codes in the releasedKeys array.
/// \return <USBD_STATUS_SUCCESS> if the report has been sent to the host;
///         otherwise an error code.
//-----------------------------------------------------------------------------
unsigned char HIDDKeyboardDriver_ChangeKeys(unsigned char *pressedKeys,
                                            unsigned char pressedKeysSize,
                                            unsigned char *releasedKeys,
                                            unsigned char releasedKeysSize)
{
    // Press keys
    while (pressedKeysSize > 0) {

        // Check if this is a standard or modifier key
        if (HIDKeypad_IsModifierKey(*pressedKeys)) {

            // Set the corresponding bit in the input report
            HIDDKeyboardInputReport_PressModifierKey(
                &(hiddKeyboardDriver.inputReport),
                *pressedKeys);
        }
        else {

            HIDDKeyboardInputReport_PressStandardKey(
                &(hiddKeyboardDriver.inputReport),
                *pressedKeys);
        }

        pressedKeysSize--;
        pressedKeys++;
    }

    // Release keys
    while (releasedKeysSize > 0) {

        // Check if this is a standard or modifier key
        if (HIDKeypad_IsModifierKey(*releasedKeys)) {

            // Set the corresponding bit in the input report
            HIDDKeyboardInputReport_ReleaseModifierKey(
                &(hiddKeyboardDriver.inputReport),
                *releasedKeys);
        }
        else {

            HIDDKeyboardInputReport_ReleaseStandardKey(
                &(hiddKeyboardDriver.inputReport),
                *releasedKeys);
        }

        releasedKeysSize--;
        releasedKeys++;
    }

    // Send input report through the interrupt IN endpoint
    return USBD_Write(hiddKeyboardDriver.interruptInEndpoint,
                      &(hiddKeyboardDriver.inputReport),
                      sizeof(HIDDKeyboardInputReport),
                      0,
                      0);
}

//-----------------------------------------------------------------------------
/// Starts a remote wake-up sequence if the host has explicitely enabled it
/// by sending the appropriate SET_FEATURE request.
//-----------------------------------------------------------------------------
void HIDDKeyboardDriver_RemoteWakeUp(void)
{
    // Remote wake-up has been enabled
    if (USBDDriver_IsRemoteWakeUpEnabled(hiddKeyboardDriver.pUsbdDriver)) {

        USBD_RemoteWakeUp();
    }
}
