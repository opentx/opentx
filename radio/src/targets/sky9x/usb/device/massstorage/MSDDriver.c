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
//      Includes
//------------------------------------------------------------------------------

#include "../../../usb/device/massstorage/MSDDriver.h"

#include "../../../usb/common/core/USBFeatureRequest.h"
#include "../../../usb/device/core/USBDDriver.h"
#include "../../../usb/device/massstorage/MSDDriverDescriptors.h"
#include "../../../usb/device/massstorage/SBCMethods.h"
#include "../../../Media.h"
#include "../../../usb/common/core/USBGenericRequest.h"
#include "../../../usb/device/core/USBD.h"
#include "debug.h"

//-----------------------------------------------------------------------------
//         Internal variables
//-----------------------------------------------------------------------------

/// Mass storage device driver instance.
static MSDDriver msdDriver;

/// Standard device driver instance.
static USBDDriver usbdDriver;

//-----------------------------------------------------------------------------
//      Internal functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Resets the state of the MSD driver
//-----------------------------------------------------------------------------
static void MSDDriver_Reset(void)
{
    TRACE_INFO_WP("MSDReset ");

    msdDriver.state = MSDD_STATE_READ_CBW;
    msdDriver.waitResetRecovery = 0;
    msdDriver.commandState.state = 0;
}

//-----------------------------------------------------------------------------
//         Callback re-implementation
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/// Invoked when a new SETUP request is received from the host. Forwards the
/// request to the Mass Storage device driver handler function.
/// \param request  Pointer to a USBGenericRequest instance.
//-----------------------------------------------------------------------------
void USBDCallbacks_RequestReceived(const USBGenericRequest *request)
{
    MSDDriver_RequestHandler(request);
}

//-----------------------------------------------------------------------------
/// Invoked when the configuration of the device changes. Resets the mass
/// storage driver.
/// \param cfgnum New configuration number.
//-----------------------------------------------------------------------------
void USBDDriverCallbacks_ConfigurationChanged(unsigned char cfgnum)
{
    if (cfgnum > 0) {

        MSDDriver_Reset();
    }
}

//-----------------------------------------------------------------------------
//      Driver functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Reads from host through MSD defined bulk OUT pipe. Act as USBD_Read but by
/// a fixed OUT endpoint.
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
    return USBD_Read(MSDDriverDescriptors_BULKOUT,
                     data,
                     size,
                     callback,
                     argument);
}

//-----------------------------------------------------------------------------
/// Writes to host through MSD defined bulk IN pipe. Act as USBD_Write but by
/// a fixed IN endpoint.
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
    return USBD_Write(MSDDriverDescriptors_BULKIN,
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

        USBD_Halt(MSDDriverDescriptors_BULKOUT);
    }

    if (stallCASE & MSDD_CASE_STALL_IN) {

        USBD_Halt(MSDDriverDescriptors_BULKIN);
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
    if (USBD_IsHalted(MSDDriverDescriptors_BULKOUT)) {

        stallCASE |= MSDD_CASE_STALL_OUT;
    }
    if (USBD_IsHalted(MSDDriverDescriptors_BULKIN)) {

        stallCASE |= MSDD_CASE_STALL_IN;
    }
    return stallCASE;
}

//-----------------------------------------------------------------------------
//      Exported functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Initializes the MSD driver and the associated USB driver.
/// \param  luns   Pointer to a list of LUNs
/// \param  numLuns Number of LUN in list
/// \see MSDLun
//-----------------------------------------------------------------------------
void MSDDriver_Initialize(MSDLun *defLuns, unsigned char numLuns)
{

  TRACE_INFO("MSD init\n\r");

    // Command state initialization
    msdDriver.commandState.state = 0;
    msdDriver.commandState.postprocess = 0;
    msdDriver.commandState.length = 0;
    msdDriver.commandState.transfer.semaphore = 0;

    // LUNs
    msdDriver.luns = defLuns;
    msdDriver.maxLun = (unsigned char) (numLuns - 1);

    // Reset BOT driver
    MSDDriver_Reset();

    // Init the USB driver
    USBDDriver_Initialize(&usbdDriver, &msdDriverDescriptors, 0);
    USBD_Init();
}

//-----------------------------------------------------------------------------
/// Handler for incoming SETUP requests on default Control endpoint 0.
///
/// Standard requests are forwarded to the USBDDriver_RequestHandler
/// method.
/// \param  request Pointer to a USBGenericRequest instance
//-----------------------------------------------------------------------------
void MSDDriver_RequestHandler(const USBGenericRequest *request)
{
    TRACE_INFO_WP("NewReq ");

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
                USBDDriver_RequestHandler(&usbdDriver, request);
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
            USBDDriver_RequestHandler(&usbdDriver, request);
        }
        break;

    //-------------------
    case MSD_GET_MAX_LUN:
    //-------------------
        TRACE_INFO_WP("gMaxLun ");

        // Check request parameters
        if ((request->wValue == 0)
            && (request->wIndex == 0)
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
            && (request->wIndex == 0)
            && (request->wLength == 0)) {

            // Reset the MSD driver
            MSDDriver_Reset();
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
        USBDDriver_RequestHandler(&usbdDriver, request);

        break;
    }
}

//-----------------------------------------------------------------------------
/// State machine for the MSD driver
//-----------------------------------------------------------------------------
void MSDDriver_StateMachine(void)
{
    if (USBD_GetState() < USBD_STATE_CONFIGURED){}
    else MSDD_StateMachine(&msdDriver);

}

//-----------------------------------------------------------------------------
/// Starts a remote wake-up sequence if the host has explicitely enabled it
/// by sending the appropriate SET_FEATURE request.
//-----------------------------------------------------------------------------
void MSDDriver_RemoteWakeUp(void)
{
    // Remote wake-up has been enabled
    if (USBDDriver_IsRemoteWakeUpEnabled(&usbdDriver)) {

        USBD_RemoteWakeUp();
    }
    // Remote wake-up NOT enabled
    else {

        TRACE_WARNING(
            "MSD..RemoteWakeUp: Host has not enabled remote wake-up\n\r");
    }
}

