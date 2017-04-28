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
/// !Purpose
/// 
/// Definitions of the descriptors required by a Mass Storage device driver.
/// 
/// !Usage
/// 
/// - For a USB %device:
///     -# When initializing a USBDDriver instance, use msdDriverDescriptors as
///        the list of standard descriptors.
//------------------------------------------------------------------------------

#ifndef MSDDRIVERDESCRIPTORS_H
#define MSDDRIVERDESCRIPTORS_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "../../../usb/device/core/USBDDriverDescriptors.h"

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "MSD endpoint addresses"
/// This page lists the addresses used by the Mass Storage driver %endpoints.
/// 
/// !Addresses
/// 
/// - MSDDriverDescriptors_BULKOUT
/// - MSDDriverDescriptors_BULKIN

/// Address of the Mass Storage bulk-out endpoint.
#define MSDDriverDescriptors_BULKOUT        1

/// Address of the Mass Storage bulk-in endpoint.
#if defined(at91sam9m10)
 #define MSDDriverDescriptors_BULKIN        6
#else
 #define MSDDriverDescriptors_BULKIN        2
#endif
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Exported variables
//------------------------------------------------------------------------------

extern const USBDDriverDescriptors msdDriverDescriptors;

#endif //#ifndef MSDDRIVERDESCRIPTORS_H

