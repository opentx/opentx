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

 Definition for the HID LEDs usage page.

 !!!Usage

 Uses the constants defined in this header file when declaring a Report
 descriptor which references the LEDs usage page.
*/

#ifndef HIDLEDS_H
#define HIDLEDS_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "HID LEDs Page ID"
/// This page lists the page ID of the HID LEDs usage page.
///
/// !ID
/// - HIDLeds_PAGEID

/// ID of the HID LEDs usage page.
#define HIDLeds_PAGEID                  0x08
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "HID LEDs Usages"
/// This page lists the Usages of the HID LEDs.
///
/// !Usages
/// - HIDLeds_NUMLOCK
/// - HIDLeds_CAPSLOCK
/// - HIDLeds_SCROLLLOCK

/// Num lock LED usage.
#define HIDLeds_NUMLOCK                 0x01
/// Caps lock LED usage.
#define HIDLeds_CAPSLOCK                0x02
/// Scroll lock LED usage.
#define HIDLeds_SCROLLLOCK              0x03
//------------------------------------------------------------------------------

#endif //#ifndef HIDLEDS_H

