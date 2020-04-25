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

 Definition of methods for using a HID joystick device driver.

 !!!Usage

 -# Re-implement the USBDCallbacks_RequestReceived callback to forward
    requests to HIDDJoystickDriver_RequestHandler. This is done
    automatically unless the NOAUTOCALLBACK symbol is defined during
    compilation.
 -# Initialize the driver using HIDDJoystickDriver_Initialize. The
    USB driver is automatically initialized by this method.
 -# Call the HIDDJoystickDriver_ChangePoints method when one or more
    keys are pressed/released.
*/

#ifndef HIDDJOYSTICKDRIVER_H
#define HIDDJOYSTICKDRIVER_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include <usb/common/core/USBGenericRequest.h>
#include "HIDDJoystickInputReport.h"

//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------
/*
    Function: HIDDJoystickDriver_Initialize
        Initializes the HID joystick device driver.
*/
extern void HIDDJoystickDriver_Initialize();

/*
    Function: HIDDJoystickDriver_RequestHandler
        Handles HID-specific SETUP request sent by the host.

    Parameters:
        request - Pointer to a USBGenericRequest instance.
*/
extern void HIDDJoystickDriver_RequestHandler(const USBGenericRequest *request);

extern unsigned char HIDDJoystickDriver_ChangeJoystickState(const HIDDJoystickInputReport *report);

#endif //#ifndef HIDDJOYSTICKDRIVER_H

