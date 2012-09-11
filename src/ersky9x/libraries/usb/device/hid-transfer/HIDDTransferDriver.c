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

#include "HIDDTransferDriver.h"
#include "HIDDTransferDriverDesc.h"
#include <utility/trace.h>
#include <usb/common/core/USBGetDescriptorRequest.h>
#include <usb/common/hid/HIDGenericDescriptor.h>
#include <usb/common/hid/HIDDescriptor.h>
#include <usb/common/hid/HIDGenericRequest.h>
#include <usb/common/hid/HIDReportRequest.h>
#include <usb/device/core/USBD.h>
#include <usb/device/core/USBDDriver.h>

#include <string.h>

//------------------------------------------------------------------------------
//         Internal types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Driver structure for an HID device implementing keyboard functionalities.
//------------------------------------------------------------------------------
typedef struct {

    /// Standard USB device driver instance.
    USBDDriver usbdDriver;

    // OUT Report - block input
    unsigned short iReportLen;
    unsigned char  iReportBuf[HIDDTransferDriver_REPORTSIZE];
    
    // IN Report - block output
    unsigned short oReportLen;
    unsigned char  oReportBuf[HIDDTransferDriver_REPORTSIZE];

    // Interrupt OUT Data - input
    /// Input data length
    unsigned short iLen;
    /// Input (report) Buffer
    unsigned char iBuf[HIDDTransferDriver_REPORTSIZE];

    // Nothing more now...

} HIDDTransferDriver;

//------------------------------------------------------------------------------
//         Internal variables
//------------------------------------------------------------------------------

/// Static instance of the HID Transfer device driver.
static HIDDTransferDriver hiddTransferDriver;

//------------------------------------------------------------------------------
//         Internal functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Returns the descriptor requested by the host.
/// \param type Descriptor type.
/// \param length Maximum number of bytes to send.
/// \return 1 if the request has been handled by this function, otherwise 0.
//------------------------------------------------------------------------------
static unsigned char HIDDTransferDriver_GetDescriptor(unsigned char type,
                                                      unsigned char length)
{
    const USBConfigurationDescriptor *pConfiguration;
    HIDDescriptor *hidDescriptor;

    switch (type) {

        case HIDGenericDescriptor_REPORT:
            TRACE_INFO("Report ");

            // Adjust length and send report descriptor
            if (length > HIDDTransferDriverDescriptors_REPORTSIZE) {

                length = HIDDTransferDriverDescriptors_REPORTSIZE;
            }
            USBD_Write(0, &hiddReportDescriptor, length, 0, 0);
            break;

        case HIDGenericDescriptor_HID:
            TRACE_INFO("HID ");

            // Configuration descriptor is different depending on configuration
            if (USBD_IsHighSpeed()) {

                pConfiguration =
                   hiddTransferDriver.usbdDriver.pDescriptors->pHsConfiguration;
            }
            else {

                pConfiguration =
                   hiddTransferDriver.usbdDriver.pDescriptors->pFsConfiguration;
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
/// Callback function when SetReport request data received from host
/// \param pArg Pointer to additional argument struct
/// \param status Result status
/// \param transferred Number of bytes transferred
/// \param remaining Number of bytes that are not transferred yet
//------------------------------------------------------------------------------
static void HIDDTransferDriver_ReportReceived(void *pArg,
                                              unsigned char status,
                                              unsigned int transferred,
                                              unsigned int remaining)
{
    hiddTransferDriver.iReportLen = transferred;
    USBD_Write(0, 0, 0, 0, 0);
}

//------------------------------------------------------------------------------
/// Callback function when GetReport request data sent to host
/// \param pArg Pointer to additional argument struct
/// \param status Result status
/// \param transferred Number of bytes transferred
/// \param remaining Number of bytes that are not transferred yet
//------------------------------------------------------------------------------
static void HIDDTransferDriver_ReportSent(void *pArg,
                                          unsigned char status,
                                          unsigned int transferred,
                                          unsigned int remaining)
{
    USBD_Read(0, 0, 0, 0, 0);
}

//------------------------------------------------------------------------------
/// Callback function when interrupt OUT data received from host
/// \param pArg Pointer to additional argument
/// \param status Result status
/// \param transferred Number of bytes transferred
/// \param remaining Number of bytes that are not transferred yet
//------------------------------------------------------------------------------
static void HIDDTransferDriver_DataReceived(void *pArg,
                                            unsigned char status,
                                            unsigned int transferred,
                                            unsigned int remaining)
{
    hiddTransferDriver.iLen = transferred;

    USBD_Read(HIDDTransferDriverDescriptors_INTERRUPTOUT,
              hiddTransferDriver.iBuf,
              HIDDTransferDriver_REPORTSIZE,
              (TransferCallback)HIDDTransferDriver_DataReceived,
              0);
}

//------------------------------------------------------------------------------
//         Optional RequestReceived() callback re-implementation
//------------------------------------------------------------------------------
#if !defined(NOAUTOCALLBACK)

//------------------------------------------------------------------------------
/// Callback function when new request receivce from host
/// \param request Pointer to the USBGenericRequest instance
//------------------------------------------------------------------------------
void USBDCallbacks_RequestReceived(const USBGenericRequest *request)
{
    HIDDTransferDriver_RequestHandler(request);
}

#endif

//------------------------------------------------------------------------------
//         ConfigurationChanged() callback re-implementation
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Callback function when configureation changed
/// \param cfgnum New configuration number
//------------------------------------------------------------------------------
void USBDDriverCallbacks_ConfigurationChanged(unsigned char cfgnum)
{
    if (cfgnum > 0) {

        hiddTransferDriver.iLen = 0;
        USBD_Read(HIDDTransferDriverDescriptors_INTERRUPTOUT,
                  hiddTransferDriver.iBuf,
                  HIDDTransferDriver_REPORTSIZE,
                  HIDDTransferDriver_DataReceived,
                  0);
    }
}

//------------------------------------------------------------------------------
//      Exported functions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
/// Initializes the HID Transfer %device driver.
//------------------------------------------------------------------------------
void HIDDTransferDriver_Initialize()
{
    hiddTransferDriver.iReportLen = 0;

    USBDDriver_Initialize(&(hiddTransferDriver.usbdDriver),
                          &hiddTransferDriverDescriptors,
                          0); // Multiple interface settings not supported
    USBD_Init();
}

//------------------------------------------------------------------------------
/// Handles HID-specific SETUP request sent by the host.
/// \param request Pointer to a USBGenericRequest instance
//------------------------------------------------------------------------------
void HIDDTransferDriver_RequestHandler(const USBGenericRequest *request)
{
    TRACE_INFO("NewReq ");

    // Check if this is a standard request
    if (USBGenericRequest_GetType(request) == USBGenericRequest_STANDARD) {

        // This is a standard request
        switch (USBGenericRequest_GetRequest(request)) {
        
            case USBGenericRequest_GETDESCRIPTOR:
                // Check if this is a HID descriptor, otherwise forward it to
                // the standard driver
                if (!HIDDTransferDriver_GetDescriptor(
                        USBGetDescriptorRequest_GetDescriptorType(request),
                        USBGenericRequest_GetLength(request))) {

                    USBDDriver_RequestHandler(&(hiddTransferDriver.usbdDriver),
                                              request);
                }
                break;

            default:
                USBDDriver_RequestHandler(&(hiddTransferDriver.usbdDriver),
                                              request);
        }
    }
    // Check if this is a class request
    else if (USBGenericRequest_GetType(request) == USBGenericRequest_CLASS) {

        unsigned short length = USBGenericRequest_GetLength(request);
        unsigned char type = HIDReportRequest_GetReportType(request);

        switch (USBGenericRequest_GetRequest(request)) {

            case HIDGenericRequest_SETREPORT:

                if (length <= HIDDTransferDriver_REPORTSIZE &&
                    type == HIDReportRequest_OUTPUT) {

                    USBD_Read(0,
                              hiddTransferDriver.iReportBuf,
                              length,
                              HIDDTransferDriver_ReportReceived,
                              0); // No argument to the callback function
                }
                else {

                    USBD_Stall(0);
                }
                break;

            case HIDGenericRequest_GETREPORT:

                if (length <= HIDDTransferDriver_REPORTSIZE &&
                    type == HIDReportRequest_INPUT) {
                    
                    USBD_Write(0,
                               hiddTransferDriver.oReportBuf,
                               length,
                               HIDDTransferDriver_ReportSent,
                               0);
                }
                else {

                    USBD_Stall(0);
                }
                break;

            default:
                TRACE_WARNING(
                      "HIDDTransferDriver_RequestHandler: request 0x%02X\n\r",
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
/// Try to read request buffer of SetReport.
/// Set pData to 0 to get current data length only.
/// \param pData Pointer to data buffer
/// \param dLength Data buffer length
/// \return Number of bytes read
//------------------------------------------------------------------------------
unsigned short HIDDTransferDriver_ReadReport(void *pData,
                                             unsigned int dLength)
{
    if (pData == 0) {

        return hiddTransferDriver.iReportLen;
    }

    if (dLength > HIDDTransferDriver_REPORTSIZE) {

        dLength = HIDDTransferDriver_REPORTSIZE;
    }
    if (dLength > hiddTransferDriver.iReportLen) {

        dLength = hiddTransferDriver.iReportLen;
    }
    hiddTransferDriver.iReportLen = 0;
    memcpy(pData, hiddTransferDriver.iReportBuf, dLength);

    return dLength;
}

//------------------------------------------------------------------------------
/// Try to read request buffer of interrupt OUT EP.
/// Set pData to 0 to get current data length only.
/// \param pData Pointer to data buffer
/// \param dLength Data buffer length
/// \return Number of bytes read
//------------------------------------------------------------------------------
unsigned short HIDDTransferDriver_Read(void *pData,
                                       unsigned int dLength)
{
    if (pData == 0) {

        return hiddTransferDriver.iLen;
    }

    if (dLength > HIDDTransferDriver_REPORTSIZE) {

        dLength = HIDDTransferDriver_REPORTSIZE;
    }
    if (dLength > hiddTransferDriver.iLen) {

        dLength = hiddTransferDriver.iLen;
    }
    hiddTransferDriver.iLen = 0;
    memcpy(pData, hiddTransferDriver.iBuf, dLength);

    return dLength;
}

//------------------------------------------------------------------------------
/// Set data in IN report buffer, which will be sent when GetReport request
/// issued.
/// \param pData Pointer to the data sent.
/// \param dLength The data length.
//------------------------------------------------------------------------------
void HIDDTransferDriver_SetReport(const void *pData,
                                  unsigned int dLength)
{
    if (pData == 0 || dLength == 0)
        return;
    if (dLength != HIDDTransferDriver_REPORTSIZE) {
        dLength = HIDDTransferDriver_REPORTSIZE;
    }
    if (hiddTransferDriver.oReportLen) {
        TRACE_INFO("Changing IN report!\n\r");
    }
    memcpy(hiddTransferDriver.oReportBuf, pData, dLength);
    hiddTransferDriver.oReportLen = dLength;
}

//------------------------------------------------------------------------------
/// Write data through USB interrupt IN EP.
/// \param pData Pointer to the data sent.
/// \param dLength The data length.
/// \param fCallback Callback function invoked when transferring done.
/// \param pArg Pointer to additional arguments.
//------------------------------------------------------------------------------
unsigned char HIDDTransferDriver_Write(const void *pData,
                                       unsigned int dLength,
                                       TransferCallback fCallback,
                                       void *pArg)
{
    if (dLength != HIDDTransferDriver_REPORTSIZE) {

        dLength = HIDDTransferDriver_REPORTSIZE;
    }
    return USBD_Write(HIDDTransferDriverDescriptors_INTERRUPTIN,
                      pData, dLength,
                      fCallback, pArg);
}

//------------------------------------------------------------------------------
/// Starts a remote wake-up sequence if the host has explicitely enabled it
/// by sending the appropriate SET_FEATURE request.
//------------------------------------------------------------------------------
void HIDDTransferDriver_RemoteWakeUp(void)
{
    // Remote wake-up has been enabled
    if (USBDDriver_IsRemoteWakeUpEnabled(&(hiddTransferDriver.usbdDriver))) {

        USBD_RemoteWakeUp();
    }
}

