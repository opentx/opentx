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

    Definition of a class for declaring USB descriptors required by the
    device driver.
*/

#ifndef USBDDRIVERDESCRIPTORS_H
#define USBDDRIVERDESCRIPTORS_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "../../../usb/common/core/USBConfigurationDescriptor.h"
#include "../../../usb/common/core/USBDeviceDescriptor.h"
#include "../../../usb/common/core/USBDeviceQualifierDescriptor.h"

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// List of all descriptors used by a USB device driver. Each descriptor can
/// be provided in two versions: full-speed and high-speed. Devices which are
/// not high-speed capable do not need to provided high-speed descriptors and
/// the full-speed qualifier & other speed descriptors.
//------------------------------------------------------------------------------
typedef struct {

    /// Pointer to the full-speed device descriptor.
    const USBDeviceDescriptor *pFsDevice;
    /// Pointer to the full-speed configuration descriptor.
    const USBConfigurationDescriptor *pFsConfiguration;
    /// Pointer to the full-speed qualifier descriptor.
    const USBDeviceQualifierDescriptor *pFsQualifier;
    /// Pointer to the full-speed other speed configuration descriptor.
    const USBConfigurationDescriptor *pFsOtherSpeed;
    /// Pointer to the high-speed device descriptor.
    const USBDeviceDescriptor *pHsDevice;
    /// Pointer to the high-speed configuration descriptor.
    const USBConfigurationDescriptor *pHsConfiguration;
    /// Pointer to the high-speed qualifier descriptor.
    const USBDeviceQualifierDescriptor *pHsQualifier;
    /// Pointer to the high-speed other speed configuration descriptor.
    const USBConfigurationDescriptor *pHsOtherSpeed;
    /// Pointer to the list of string descriptors.
    const unsigned char **pStrings;
    /// Number of string descriptors in list.
    unsigned char numStrings;

} USBDDriverDescriptors;

#endif //#ifndef USBDDRIVERDESCRIPTORS_H

