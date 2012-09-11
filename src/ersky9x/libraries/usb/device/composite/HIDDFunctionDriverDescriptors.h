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

#ifndef HIDDFUNCTIONDRIVERDESCRIPTORS_H
#define HIDDFUNCTIONDRIVERDESCRIPTORS_H

//-----------------------------------------------------------------------------
//         Headers
//-----------------------------------------------------------------------------

#include <usb/device/core/USBDDriverDescriptors.h>
#include <usb/common/hid/HIDKeypad.h>

//-----------------------------------------------------------------------------
//         Definitions
//-----------------------------------------------------------------------------

//- Interface & Endpoints
/// Interface Number.
/// Interrupt IN endpoint number.
/// Interrupt OUT endpoint number.
/// Interrupt IN endpoint polling rate (in milliseconds).
/// Interrupt OUT endpoint polling rate (in milliseconds).
#if defined(usb_HIDAUDIO)
#define HIDD_Descriptors_INTERFACENUM               0
#define HIDD_Descriptors_INTERRUPTIN                1
#define HIDD_Descriptors_INTERRUPTOUT               2
#define HIDD_Descriptors_INTERRUPTIN_POLLING        10
#endif

/// Default HID interrupt IN endpoint polling rate (16ms).
#define HIDD_Descriptors_INTERRUPTIN_POLLING_FS     16
#define HIDD_Descriptors_INTERRUPTIN_POLLING_HS     8
/// Default interrupt OUT endpoint polling rate (16ms).
#define HIDD_Descriptors_INTERRUPTOUT_POLLING_FS    16
#define HIDD_Descriptors_INTERRUPTOUT_POLLING_HS    8

//- Keypad keys
/// Key code of the first accepted modifier key.
#define HIDD_Descriptors_FIRSTMODIFIERKEY  HIDKeypad_LEFTCONTROL
/// Key code of the last accepted modifier key.
#define HIDD_Descriptors_LASTMODIFIERKEY   HIDKeypad_RIGHTGUI
/// Key code of the first accepted standard key.
#define HIDD_Descriptors_FIRSTSTANDARDKEY  0
/// Key code of the last accepted standard key.
#define HIDD_Descriptors_LASTSTANDARDKEY   HIDKeypad_NUMLOCK

//- Report descriptor
/// Size of the report descriptor in bytes.
#define HIDD_Descriptors_REPORTSIZE        61

//-----------------------------------------------------------------------------
//         Exported variables
//-----------------------------------------------------------------------------

/// Report descriptor used by the driver.
extern const unsigned char hiddReportDescriptor[];

#endif // #define HIDDFUNCTIONDRIVERDESCRIPTORS_H

