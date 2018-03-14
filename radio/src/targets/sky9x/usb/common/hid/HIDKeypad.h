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

 Definitions of constants and methods for the HID keypad usage page.

 !!!Usage

 -# Use the constants declared in this file when instanciating a
    Report descriptor instance.
 -# When implementing the functionality of an HID keyboard, use the
    key codes defined here to indicate keys that are being pressed and
    released.
*/

#ifndef HIDKEYPAD_H
#define HIDKEYPAD_H

//------------------------------------------------------------------------------
//         Constants
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "HID Keypad Page ID"
/// This page lists HID Keypad page ID.
///
/// !ID
/// - HIDKeypad_PAGEID

/// Identifier for the HID keypad usage page
#define HIDKeypad_PAGEID                    0x07
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "HID Alphabetic Keys"
/// ...
///
/// !Keys
/// - HIDKeypad_A
/// - HIDKeypad_B
/// - HIDKeypad_C
/// - HIDKeypad_D
/// - HIDKeypad_E
/// - HIDKeypad_F
/// - HIDKeypad_G
/// - HIDKeypad_H
/// - HIDKeypad_I
/// - HIDKeypad_J
/// - HIDKeypad_K
/// - HIDKeypad_L
/// - HIDKeypad_M
/// - HIDKeypad_N
/// - HIDKeypad_O
/// - HIDKeypad_P
/// - HIDKeypad_Q
/// - HIDKeypad_R
/// - HIDKeypad_S
/// - HIDKeypad_T
/// - HIDKeypad_U
/// - HIDKeypad_V
/// - HIDKeypad_W
/// - HIDKeypad_X
/// - HIDKeypad_Y
/// - HIDKeypad_Z

/// Key code for 'a' and 'A'.
#define HIDKeypad_A                     4
/// Key code for 'b' and 'B'.
#define HIDKeypad_B                     5
/// Key code for 'c' and 'C'.
#define HIDKeypad_C                     6
/// Key code for 'd' and 'D'.
#define HIDKeypad_D                     7
/// Key code for 'e' and 'E'.
#define HIDKeypad_E                     8
/// Key code for 'f' and 'F'.
#define HIDKeypad_F                     9
/// Key code for 'g' and 'G'.
#define HIDKeypad_G                     10
/// Key code for 'h' and 'H'.
#define HIDKeypad_H                     11
/// Key code for 'i' and 'I'.
#define HIDKeypad_I                     12
/// Key code for 'j' and 'J'.
#define HIDKeypad_J                     13
/// Key code for 'k' and 'K'.
#define HIDKeypad_K                     14
/// Key code for 'l' and 'L'.
#define HIDKeypad_L                     15
/// Key code for 'm' and 'M'.
#define HIDKeypad_M                     16
/// Key code for 'n' and 'N'.
#define HIDKeypad_N                     17
/// Key code for 'o' and 'O'.
#define HIDKeypad_O                     18
/// Key code for 'p' and 'P'.
#define HIDKeypad_P                     19
/// Key code for 'q' and 'Q'.
#define HIDKeypad_Q                     20
/// Key code for 'r' and 'R'.
#define HIDKeypad_R                     21
/// Key code for 's' and 'S'.
#define HIDKeypad_S                     22
/// Key code for 't' and 'T'.
#define HIDKeypad_T                     23
/// Key code for 'u' and 'U'.
#define HIDKeypad_U                     24
/// Key code for 'v' and 'V'.
#define HIDKeypad_V                     25
/// Key code for 'w' and 'W'.
#define HIDKeypad_W                     26
/// Key code for 'x' and 'X'.
#define HIDKeypad_X                     27
/// Key code for 'y' and 'Y'.
#define HIDKeypad_Y                     28
/// Key code for 'z' and 'Z'.
#define HIDKeypad_Z                     29
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "HID Numeric Keys"
/// ...
///
/// !Keys
/// - HIDKeypad_1
/// - HIDKeypad_2
/// - HIDKeypad_3
/// - HIDKeypad_4
/// - HIDKeypad_5
/// - HIDKeypad_6
/// - HIDKeypad_7
/// - HIDKeypad_8
/// - HIDKeypad_9
/// - HIDKeypad_0

/// Key code for '1' and '!'.
#define HIDKeypad_1                     30
/// Key code for '2' and '@'.
#define HIDKeypad_2                     31
/// Key code for '3' and '#'.
#define HIDKeypad_3                     32
/// Key code for '4' and '$'.
#define HIDKeypad_4                     33
/// Key code for '5' and '%'.
#define HIDKeypad_5                     34
/// Key code for '6' and '^'.
#define HIDKeypad_6                     35
/// Key code for '7' and '&'.
#define HIDKeypad_7                     36
/// Key code for '8' and '*'.
#define HIDKeypad_8                     37
/// Key code for '9' and '('.
#define HIDKeypad_9                     38
/// Key code for '0' and ')'.
#define HIDKeypad_0                     39
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "HID Special Keys"
/// ...
///
/// !Keys
/// - HIDKeypad_ENTER
/// - HIDKeypad_ESCAPE
/// - HIDKeypad_BACKSPACE
/// - HIDKeypad_TAB
/// - HIDKeypad_SPACEBAR
/// - HIDKeypad_PRINTSCREEN
/// - HIDKeypad_SCROLLLOCK
/// - HIDKeypad_NUMLOCK

/// Enter key code.
#define HIDKeypad_ENTER                 40
/// Escape key code.
#define HIDKeypad_ESCAPE                41
/// Backspace key code.
#define HIDKeypad_BACKSPACE             42
/// Tab key code.
#define HIDKeypad_TAB                   43
/// Spacebar key code.
#define HIDKeypad_SPACEBAR              44
/// Printscreen key code.
#define HIDKeypad_PRINTSCREEN           70
/// Scroll lock key code.
#define HIDKeypad_SCROLLLOCK            71
/// Num lock key code.
#define HIDKeypad_NUMLOCK               83
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "HID Modified Keys"
/// ...
///
/// !Keys
/// - HIDKeypad_LEFTCONTROL
/// - HIDKeypad_LEFTSHIFT
/// - HIDKeypad_LEFTALT
/// - HIDKeypad_LEFTGUI
/// - HIDKeypad_RIGHTCONTROL
/// - HIDKeypad_RIGHTSHIFT
/// - HIDKeypad_RIGHTALT
/// - HIDKeypad_RIGHTGUI

/// Key code for the left 'Control' key.
#define HIDKeypad_LEFTCONTROL           224
/// Key code for the left 'Shift' key.
#define HIDKeypad_LEFTSHIFT             225
/// Key code for the left 'Alt' key.
#define HIDKeypad_LEFTALT               226
/// Key code for the left 'GUI' (e.g. Windows) key.
#define HIDKeypad_LEFTGUI               227
/// Key code for the right 'Control' key.
#define HIDKeypad_RIGHTCONTROL          228
/// Key code for the right 'Shift' key.
#define HIDKeypad_RIGHTSHIFT            229
/// Key code for the right 'Alt' key.
#define HIDKeypad_RIGHTALT              230
/// Key code for the right 'GUI' key.
#define HIDKeypad_RIGHTGUI              231
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "HID Error Codes"
/// ...
///
/// !Codes
/// - HIDKeypad_ERRORROLLOVER
/// - HIDKeypad_POSTFAIL
/// - HIDKeypad_ERRORUNDEFINED

/// Indicates that too many keys have been pressed at the same time.
#define HIDKeypad_ERRORROLLOVER         1
/// ?
#define HIDKeypad_POSTFAIL              2
/// Indicates an undefined error.
#define HIDKeypad_ERRORUNDEFINED        3
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

extern unsigned char HIDKeypad_IsModifierKey(unsigned char key);

#endif //#ifndef HIDKEYPAD_H

