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
 \unit HIDDKeyboardOutputReport

 !!!Purpose

 Definition of a class for manipulating HID keyboard output reports.

 !!!Usage

 -# Initialize a newly-created output report instance with
    HIDDKeyboardOutputReport_Initialize.
 -# Retrieve the status of the three LEDs using
    HIDDKeyboardOutputReport_GetNumLockStatus,
    HIDDKeyboardOutputReport_GetCapsLockStatus and
    HIDDKeyboardOutputReport_GetScrollLockStatus.
*/

#ifndef HIDKEYBOARDOUTPUTREPORT_H
#define HIDKEYBOARDOUTPUTREPORT_H

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
/// HID output report structure used by the host to control the state of
/// the keyboard LEDs.
/// 
/// Only the first three bits are relevant, the other 5 are used as
/// padding bits.
//------------------------------------------------------------------------------
typedef struct {

    unsigned char numLockStatus:1,      /// State of the num. lock LED.
                  capsLockStatus:1,     /// State of the caps lock LED.
                  scrollLockStatus:1,   /// State of the scroll lock LED.
                  padding:5;            /// Padding bits.

} __attribute__ ((packed)) HIDDKeyboardOutputReport; // GCC

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

extern void HIDDKeyboardOutputReport_Initialize(
    HIDDKeyboardOutputReport *report);

extern unsigned char HIDDKeyboardOutputReport_GetNumLockStatus(
    const HIDDKeyboardOutputReport *report);

extern unsigned char HIDDKeyboardOutputReport_GetCapsLockStatus(
    const HIDDKeyboardOutputReport *report);

extern unsigned char HIDDKeyboardOutputReport_GetScrollLockStatus(
    const HIDDKeyboardOutputReport *report);

#endif //#ifndef HIDKEYBOARDOUTPUTREPORT_H

