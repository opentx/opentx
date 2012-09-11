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

#ifndef HIDDFUNCTIONDRIVER_H
#define HIDDFUNCTIONDRIVER_H

//-----------------------------------------------------------------------------
//         Headers
//-----------------------------------------------------------------------------

#include <usb/device/core/USBDDriver.h>
#include <usb/common/core/USBGenericRequest.h>

//-----------------------------------------------------------------------------
//         Exported functions
//-----------------------------------------------------------------------------

//- Function API for composite device
extern void HIDDFunctionDriver_Initialize(USBDDriver * pUsbdDriver,
                                          unsigned char interfaceNum,
                                          unsigned char interruptInEndpoint,
                                          unsigned char interruptOutEndpoint);

extern unsigned char HIDDFunctionDriver_RequestHandler(
    const USBGenericRequest *request);

extern void HIDDFunctionCallbacks_ConfigurationChanged(unsigned char cfgnum);

//- HID Keyboard API
extern unsigned char HIDDKeyboardDriver_ChangeKeys(
    unsigned char *pressedKeys,
    unsigned char pressedKeysSize,
    unsigned char *releasedKeys,
    unsigned char releasedKeysSize);

extern void HIDDKeyboardDriver_RemoteWakeUp(void);

#endif // #define HIDDFUNCTIONDRIVER_H
