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
///  - DUALCDCDDriver_Initialize
///
/// -# Handle and dispach USB requests
///  - DUALCDCDDriver_RequestHandler
///
/// -# Try starting a remote wake-up sequence
///  - DUALCDCDDriver_RemoteWakeUp
//-----------------------------------------------------------------------------

#ifndef DUALCDCDDRIVER_H
#define DUALCDCDDRIVER_H


//-----------------------------------------------------------------------------
//         Headers
//-----------------------------------------------------------------------------

#include <usb/common/core/USBGenericRequest.h>
#include <usb/device/core/USBD.h>

#include "CDCDFunctionDriver.h"

//-----------------------------------------------------------------------------
//         Exported functions
//-----------------------------------------------------------------------------

// -DUALCDC
extern void DUALCDCDDriver_Initialize();

extern void DUALCDCDDriver_RequestHandler(const USBGenericRequest *request);

extern void DUALCDCDDriver_RemoteWakeUp(void);

#endif //#ifndef DUALCDCDDRIVER_H

