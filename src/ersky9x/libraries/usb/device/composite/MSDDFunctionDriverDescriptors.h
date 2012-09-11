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

#ifndef MSDDFUNCTIONDRIVERDESCRIPTORS_H
#define MSDDFUNCTIONDRIVERDESCRIPTORS_H

//-----------------------------------------------------------------------------
//         Headers
//-----------------------------------------------------------------------------

#include <usb/device/core/USBDDriverDescriptors.h>

//-----------------------------------------------------------------------------
//         Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// \page "MS interface & endpoint descriptor settings"
/// This page lists the definition used by the Mass Storage driver.
/// 
/// !Settings
/// 
/// - MSDD_Descriptors_INTERFACENUM
/// - MSDD_Descriptors_BULKOUT
/// - MSDD_Descriptors_BULKIN

/// Number of the Mass Storage interface.
#if defined(usb_CDCMSD)
#define MSDD_Descriptors_INTERFACENUM   2
#elif defined(usb_HIDMSD)
#define MSDD_Descriptors_INTERFACENUM   1
#endif

/// Address of the Mass Storage bulk-out endpoint.
#define MSDD_Descriptors_BULKOUT        4

/// Address of the Mass Storage bulk-in endpoint.
#define MSDD_Descriptors_BULKIN         5
//-----------------------------------------------------------------------------


#endif // #define MSDDFUNCTIONDRIVERDESCRIPTORS_H

