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
///   Definitions and methods for USB composite device implement.
/// 
/// !Usage
/// 
/// -# Initialize USB function specified driver ( for MSD currently )
///  - MSDDFunctionDriver_Initialize
///
/// -# Initialize USB composite driver and USB driver
///  - COMPOSITEDDriver_Initialize
///
/// -# Handle and dispach USB requests
///  - COMPOSITEDDriver_RequestHandler
///
/// -# Try starting a remote wake-up sequence
///  - COMPOSITEDDriver_RemoteWakeUp
//-----------------------------------------------------------------------------

#ifndef COMPOSITEDDRIVER_H
#define COMPOSITEDDRIVER_H


//-----------------------------------------------------------------------------
//         Headers
//-----------------------------------------------------------------------------

#include <usb/common/core/USBGenericRequest.h>
#include <usb/device/core/USBD.h>

#if defined(usb_CDCAUDIO) || defined(usb_CDCHID) || defined(usb_CDCCDC) || defined(usb_CDCMSD)
 #include "CDCDFunctionDriver.h"
 #include "CDCDFunctionDriverDescriptors.h"
#endif

#if defined(usb_CDCAUDIO) || defined(usb_HIDAUDIO)
 #include "AUDDFunctionDriver.h"
#endif

#if defined(usb_CDCHID) || defined(usb_HIDAUDIO) || defined(usb_HIDMSD)
 #include "HIDDFunctionDriver.h"
 #include "HIDDFunctionDriverDescriptors.h"
#endif

#if defined(usb_CDCMSD) || defined(usb_HIDMSD)
 #include "MSDDFunctionDriver.h"
#endif

//-----------------------------------------------------------------------------
//         Exported functions
//-----------------------------------------------------------------------------

// -COMPOSITE
extern void COMPOSITEDDriver_Initialize();

extern void COMPOSITEDDriver_RequestHandler(const USBGenericRequest *request);

extern void COMPOSITEDDriver_RemoteWakeUp(void);

#endif //#ifndef COMPOSITEDDRIVER_H

