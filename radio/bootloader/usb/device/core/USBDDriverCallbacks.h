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

    Definition of several callbacks which are triggered by the USB software
    driver after receiving specific requests.

 !!!Usage

    -# Re-implement the USBDDriverCallbacks_ConfigurationChanged
        callback to know when the hosts changes the active configuration of
        the device.
    -# Re-implement the USBDDriverCallbacks_InterfaceSettingChanged
        callback to get notified whenever the active setting of an interface
        is changed by the host.
*/

#ifndef USBDDRIVERCALLBACKS_H
#define USBDDRIVERCALLBACKS_H

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

extern void USBDDriverCallbacks_ConfigurationChanged(unsigned char cfgnum);

extern void USBDDriverCallbacks_InterfaceSettingChanged(unsigned char interface,
                                                        unsigned char setting);

#endif //#ifndef USBDDRIVERCALLBACKS_H

