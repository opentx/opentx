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

#ifndef CDCDFUNCTIONDRIVERDESCRIPTORS_H
#define CDCDFUNCTIONDRIVERDESCRIPTORS_H

//-----------------------------------------------------------------------------
//      Headers
//-----------------------------------------------------------------------------

#include <board.h>
#include <usb/device/core/USBDDriverDescriptors.h>

//-----------------------------------------------------------------------------
//         Definitions
//-----------------------------------------------------------------------------

#if defined(usb_CDCAUDIO)
#define CDCD_Descriptors_INTERFACENUM0              0
#define CDCD_Descriptors_NOTIFICATION0              3
#define CDCD_Descriptors_DATAIN0                    2
#define CDCD_Descriptors_DATAOUT0                   1
#endif

/// Default CDC interrupt endpoints max packat size (8).
#define CDCD_Descriptors_INTERRUPT_MAXPACKETSIZE    8
/// Default CDC bulk endpoints max packat size (128, for HS actually).
#define CDCD_Descriptors_BULK_MAXPACKETSIZE         128

/// Default CDC interrupt IN endpoint polling rate of Full Speed (16ms).
#define CDCD_Descriptors_INTERRUPTIN_POLLING_FS     16
/// Default CDC interrupt IN endpoint polling rate of High Speed (16ms).
#define CDCD_Descriptors_INTERRUPTIN_POLLING_HS     8
/// Default interrupt OUT endpoint polling rate of Full Speed (16ms).
#define CDCD_Descriptors_INTERRUPTOUT_POLLING_FS    16
/// Default interrupt OUT endpoint polling rate of High Speed (16ms).
#define CDCD_Descriptors_INTERRUPTOUT_POLLING_HS    8

#endif //  #define CDCFUNCTIONDRIVERDESCRIPTORS_H
