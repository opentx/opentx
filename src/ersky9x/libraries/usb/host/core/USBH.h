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
/// \unit
///
/// !!!Purpose
/// 
/// Collection of methods for using the USB device controller on AT91
/// microcontrollers.
/// 
/// !!!Usage
/// 
/// Please refer to the corresponding application note.
/// - "AT91 USB device framework"
/// - "USBD API" . "USBD API Methods"
//------------------------------------------------------------------------------

#ifndef USBH_H
#define USBH_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include <memories/Media.h>
//#include <usb/common/core/USBEndpointDescriptor.h>
//#include <usb/common/core/USBGenericRequest.h>
#include <usb/host/ohci/ohci.h>
//------------------------------------------------------------------------------
//      Definitions
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

extern void USBH_Init(OHCI_HCCA* pHCCA);
extern void USBH_ResetPort(unsigned char portNumber);
extern unsigned char USBH_IsDeviceConnectedOnPort(unsigned char portNumber);
extern void USBH_program(unsigned int* pBHED, unsigned int* pBCED, OHCI_HCCA* pHCCA);
extern void USBH_enablingPort(void);


#endif //#ifndef USBD_H

