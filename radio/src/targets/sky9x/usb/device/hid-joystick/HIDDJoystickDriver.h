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

 Definition of methods for using a HID mouse device driver.

 !!!Usage

 -# Re-implement the USBDCallbacks_RequestReceived callback to forward
    requests to HIDDMouseDriver_RequestHandler. This is done
    automatically unless the NOAUTOCALLBACK symbol is defined during
    compilation.
 -# Initialize the driver using HIDDMouseDriver_Initialize. The
    USB driver is automatically initialized by this method.
 -# Call the HIDDMouseDriver_ChangePoints method when one or more
    keys are pressed/released.
*/

#ifndef HIDDKEYBOARDDRIVER_H
#define HIDDKEYBOARDDRIVER_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include <usb/common/core/USBGenericRequest.h>

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "HID Mouse Button bitmaps"
/// ...
/// !Bits
/// - HIDDMouse_LEFT_BUTTON
/// - HIDDMouse_RIGHT_BUTTON
/// - HIDDMouse_MIDDLE_BUTTON

/// Left mouse button
#define HIDDMouse_LEFT_BUTTON   (1 << 0)

/// Right mouse button
#define HIDDMouse_RIGHT_BUTTON  (1 << 1)

/// Middle mouse button
#define HIDDMouse_MIDDLE_BUTTON (1 << 2)

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------
/*
    Function: HIDDMouseDriver_Initialize
        Initializes the HID keyboard device driver.
*/
extern void HIDDMouseDriver_Initialize();

/*
    Function: HIDDMouseDriver_RequestHandler
        Handles HID-specific SETUP request sent by the host.

    Parameters:
        request - Pointer to a USBGenericRequest instance.
*/
extern void HIDDMouseDriver_RequestHandler(const USBGenericRequest *request);

extern unsigned char HIDDMouseDriver_ChangePoints(unsigned char bmButtons,
                                                  signed char deltaX,
                                                  signed char deltaY);

extern void HIDDMouseDriver_RemoteWakeUp(void);

#endif //#ifndef HIDDKEYBOARDDRIVER_H

