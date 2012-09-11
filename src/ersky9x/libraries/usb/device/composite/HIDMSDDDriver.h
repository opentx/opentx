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
/// \unit
///
/// !Purpose
///
///   Definitions and methods for USB HID + MSD HIDMSD device implement.
/// 
/// !Usage
/// 
/// -# Initialize USB function specified driver ( for MSD currently )
///  - MSDDFunctionDriver_Initialize
///
/// -# Initialize USB HIDMSD driver and USB driver
///  - HIDMSDDDriver_Initialize
///
/// -# Handle and dispach USB requests
///  - HIDMSDDDriver_RequestHandler
///
/// -# Try starting a remote wake-up sequence
///  - HIDMSDDDriver_RemoteWakeUp
//-----------------------------------------------------------------------------

#ifndef HIDMSDDDRIVER_H
#define HIDMSDDDRIVER_H


//-----------------------------------------------------------------------------
//         Headers
//-----------------------------------------------------------------------------

#include <usb/common/core/USBGenericRequest.h>
#include <usb/device/core/USBD.h>

#include "HIDDFunctionDriver.h"
#include "MSDDFunctionDriver.h"

//-----------------------------------------------------------------------------
//         Exported functions
//-----------------------------------------------------------------------------

// -HIDMSD Composite device
extern void HIDMSDDDriver_Initialize(MSDLun *pLuns,
                                     unsigned char numLuns);

extern void HIDMSDDDriver_RequestHandler(const USBGenericRequest *request);

extern void HIDMSDDDriver_RemoteWakeUp(void);

#endif //#ifndef HIDMSDDDRIVER_H

