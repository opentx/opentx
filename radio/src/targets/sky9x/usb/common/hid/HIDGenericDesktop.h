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

 Constants for using the HID generic desktop usage page.

 !!!Usage

 Use these constants when declaring a Report descriptor which references
 the generic desktop page.
*/

#ifndef HIDGENERICDESKTOP_H
#define HIDGENERICDESKTOP_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "HID GenericDesktop Page ID"
/// ...
///
/// !ID
/// - HIDGenericDesktop_PAGEID

/// ID for the HID generic desktop usage page.
#define HIDGenericDesktop_PAGEID            0x01
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "HID GenericDesktop Usages"
/// ...
///
/// !Usages
/// - HIDGenericDesktop_POINTER
/// - HIDGenericDesktop_MOUSE
/// - HIDGenericDesktop_JOYSTICK
/// - HIDGenericDesktop_GAMEPAD
/// - HIDGenericDesktop_KEYBOARD
/// - HIDGenericDesktop_KEYPAD
/// - HIDGenericDesktop_MULTIAXIS
/// - HIDGenericDesktop_X
/// - HIDGenericDesktop_Y

/// Pointer usage ID.
#define HIDGenericDesktop_POINTER           0x01
/// Mouse usage ID.
#define HIDGenericDesktop_MOUSE             0x02
/// Joystick usage ID.
#define HIDGenericDesktop_JOYSTICK          0x04
/// Gamepad usage ID.
#define HIDGenericDesktop_GAMEPAD           0x05
/// Keyboard usage ID.
#define HIDGenericDesktop_KEYBOARD          0x06
/// Keypad usage ID.
#define HIDGenericDesktop_KEYPAD            0x07
/// Multi-axis controller usage ID.
#define HIDGenericDesktop_MULTIAXIS         0x08

/// Axis Usage X direction ID.
#define HIDGenericDesktop_X                 0x30
/// Axis Usage Y direction ID.
#define HIDGenericDesktop_Y                 0x31
//------------------------------------------------------------------------------

#endif //#ifndef HIDGENERICDESKTOP_H

