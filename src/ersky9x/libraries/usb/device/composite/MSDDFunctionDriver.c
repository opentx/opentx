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
//      Includes
//-----------------------------------------------------------------------------

// GENERAL
#include <utility/trace.h>
#include <utility/assert.h>
// USB
#include <usb/common/core/USBGenericRequest.h>
#include <usb/common/core/USBFeatureRequest.h>
#include <usb/device/core/USBD.h>
#include <usb/device/core/USBDDriver.h>
// MSD
#include <usb/device/massstorage/SBCMethods.h>
#include <usb/device/massstorage/MSDDStateMachine.h>
#include "MSDDFunctionDriverDescriptors.h"

//-----------------------------------------------------------------------------
//         Internal types
//-----------------------------------------------------------------------------

/// Driver structure for an HID device implementing keyboard functionalities.
typedef struct {

    /// Pointer to USB device driver instance
    USBDDriver * pUsbdDriver;
    /// Pointer to MSD driver instance
    MSDDriver * pMsdDriver;
    /// Interface Number of MS Function
    unsigned char interfaceNum;
    /// Interrupt IN endpoint address
    unsigned char bulkInEndpoint;
    /// Interrupt OUT endpoint address
    unsigned char bulkOutEndpoint;

} MSDFunctionDriver;

//-----------------------------------------------------------------------------
//         Internal variables
//-----------------------------------------------------------------------------

/// Mass storage general driver instance.
static MSDDriver msdDriver;

/// Mass storage function driver instance.
static MSDFunctionDriver msdFunDriver;

//-----------------------------------------------------------------------------
//      Internal functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Resets the state of the MSD driver
//-----------------------------------------------------------------------------
static void MSDD_Reset()
{
    TRACE_INFO_WP("MSDReset ");

    msdDriver.state = MSDD_STATE_READ_CBW;
    msdDriver.waitResetRecovery = 0;
    msdDriver.commandState.state = 0;
}

//-----------------------------------------------------------------------------
//      Exported functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Initializes a MSD driver
/// \param luns    Pointer to a list of LUNs
/// \param numLuns Number of LUN in list
/// \param interfaceNum Interface number to access the MSD function
/// \param bulkInEndpoint Bulk IN endpoint address
/// \param bulkOutEndpoint Bulk OUT endpoint address
//-----------------------------------------------------------------------------
void MSDDFunctionDriver_Initialize(USBDDriver * pUsbdDriver,
                                   MSDLun *luns, unsigned char numLuns,
                                   unsigned char interfaceNum,
                                   unsigned char bulkInEndpoint,
                                   unsigned char bulkOutEndpoint)
{
    TRACE_INFO("MSD init\n\r");
    
    msdFunDriver.pUsbdDriver = pUsbdDriver;
    msdFunDriver.pMsdDriver = &msdDriver;
    msdFunDriver.interfaceNum = interfaceNum;
    msdFunDriver.bulkInEndpoint = bulkInEndpoint;
    msdFunDriver.bulkOutEndpoint = bulkOutEndpoint;

    // Command state initialization
    msdDriver.commandState.state = 0;
    msdDriver.commandState.postprocess = 0;
    msdDriver.commandState.length = 0;
    msdDriver.commandState.transfer.semaphore = 0;

    // LUNs
    msdDriver.luns = luns;
    msdDriver.maxLun = (unsigned char) (numLuns - 1);

    // Reset BOT driver
    MSDD_Reset();
}

//-----------------------------------------------------------------------------
/// Invoked when a new SETUP request is received from the host. Forwards the
/// request to the Mass Storage device driver handler function.
/// \param request  Pointer to a USBGenericRequest instance.
/// \return 0 if the request is Unsupported, 1 if the request handled.
//-----------------------------------------------------------------------------
unsigned char MSDDFunctionDriver_RequestHandler(
    const USBGenericRequest *request)
{
    // Handle requests
    switch (USBGenericRequest_GetRequest(request)) {
    //---------------------
    case USBGenericRequest_CLEARFEATURE:
    //---------------------
        TRACE_INFO_WP("ClrFeat ");

        switch (USBFeatureRequest_GetFeatureSelector(request)) {

        //---------------------
        case USBFeatureRequest_ENDPOINTHALT:
        //---------------------
            TRACE_INFO_WP("Hlt ");

            // Do not clear the endpoint halt status if the device is waiting
            // for a reset recovery sequence
            if (!msdDriver.waitResetRecovery) {

                // Forward the request to the standard handler
                return 0;
            }
            else {

                TRACE_INFO_WP("No ");
            }

            USBD_Write(0, 0, 0, 0, 0);
            break;

        //------
        default:
        //------
            // Forward the request to the standard handler
            return 0;
        }
        break;

    //-------------------
    case MSD_GET_MAX_LUN:
    //-------------------
        TRACE_INFO_WP("gMaxLun ");

        // Check request parameters
        if ((request->wValue == 0)
            && (request->wIndex == msdFunDriver.interfaceNum)
            && (request->wLength == 1)) {

            USBD_Write(0, &(msdDriver.maxLun), 1, 0, 0);

        }
        else {

            TRACE_WARNING(
                "MSDDriver_RequestHandler: GetMaxLUN(%d,%d,%d)\n\r",
                request->wValue, request->wIndex, request->wLength);
            USBD_Stall(0);
        }
        break;

    //-----------------------
    case MSD_BULK_ONLY_RESET:
    //-----------------------
        TRACE_INFO_WP("Rst ");

        // Check parameters
        if ((request->wValue == 0)
            && (request->wIndex == msdFunDriver.interfaceNum)
            && (request->wLength == 0)) {

            // Reset the MSD driver
            MSDD_Reset();
            USBD_Write(0, 0, 0, 0, 0);
        }
        else {

            TRACE_WARNING(
                "MSDDriver_RequestHandler: Reset(%d,%d,%d)\n\r",
                request->wValue, request->wIndex, request->wLength);
            USBD_Stall(0);
        }
        break;

    //------
    default:
    //------
        // Forward request to standard handler
        return 0;
    }

    return 1;
}

//-----------------------------------------------------------------------------
/// Invoked whenever the configuration of the device is changed by the host.
/// \param cfgnum Newly configuration number.
//-----------------------------------------------------------------------------
void MSDDFunctionCallbacks_ConfigurationChanged(unsigned char cfgnum)
{
    if (cfgnum > 0) {

        MSDD_Reset();
    }
}

//-----------------------------------------------------------------------------
/// Reads from host through MSD defined pipe. Act as USBD_Read.
/// \param data Pointer to the data buffer that contains data read from host.
/// \param size The number of bytes should be read (buffer size).
/// \param callback Pointer to the function invoked on end of reading.
/// \param argument Pointer to additional argument data struct.
//-----------------------------------------------------------------------------
char MSDD_Read(void *data,
               unsigned int size,
               TransferCallback callback,
               void *argument)

{
    return USBD_Read(msdFunDriver.bulkOutEndpoint,
                     data,
                     size,
                     callback,
                     argument);
}

//-----------------------------------------------------------------------------
/// Writes to host through MSD defined pipe. Act as USBD_Write.
/// \param data Pointer to the data that writes to the host.
/// \param size The number of bytes should be write.
/// \param callback Pointer to the function invoked on end of writing.
/// \param argument Pointer to additional argument data struct.
//-----------------------------------------------------------------------------
char MSDD_Write(void *data,
                unsigned int size,
                TransferCallback callback,
                void *argument)
{
    return USBD_Write(msdFunDriver.bulkInEndpoint,
                      data,
                      size,
                      callback,
                      argument);
}

//-----------------------------------------------------------------------------
/// HALT Specified USB pipe.
/// \param stallCASE Case of the stall condition (Bulk In/Out/Both).
//-----------------------------------------------------------------------------
void MSDD_Halt(unsigned int stallCASE)
{
    if (stallCASE & MSDD_CASE_STALL_OUT) {

        USBD_Halt(msdFunDriver.bulkOutEndpoint);
    }

    if (stallCASE & MSDD_CASE_STALL_IN) {

        USBD_Halt(msdFunDriver.bulkInEndpoint);
    }
}

//-----------------------------------------------------------------------------
/// Return halted status
/// \return stallCASE bitmap, case of the stall condition
///         (bit: MSDD_CASE_STALL_OUT or MSDD_CASE_STALL_IN)
//-----------------------------------------------------------------------------
unsigned int MSDD_IsHalted(void)
{
    unsigned int stallCASE = 0;
    if (USBD_IsHalted(msdFunDriver.bulkOutEndpoint)) {

        stallCASE |= MSDD_CASE_STALL_OUT;
    }
    if (USBD_IsHalted(msdFunDriver.bulkInEndpoint)) {

        stallCASE |= MSDD_CASE_STALL_IN;
    }
    return stallCASE;
}

//-----------------------------------------------------------------------------
/// State machine for the MSD driver
//-----------------------------------------------------------------------------
void MSDDriver_StateMachine(void)
{
    MSDD_StateMachine(&msdDriver);
}
