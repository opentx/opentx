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
 
 Class for manipulating HID Mouse input reports.

 !!!Usage

 -# Initialize a newly created input report with
    HIDDMouseInputReport_Initialize
*/

#ifndef HIDDKEYBOARDINPUTREPORT_H
#define HIDDKEYBOARDINPUTREPORT_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "HID Mouse Keys"

/// Mouse Left Click Button
#define HIDDMouseInputReport_LEFT_BUTTON        (1 << 0)
/// Mouse Right Click Button
#define HIDDMouseInputReport_RIGHT_BUTTON       (1 << 1)
/// Mouse Middle Button
#define HIDDMouseInputReport_MIDDLE_BUTTON      (1 << 2)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
/// HID input report structure used by the Mouse driver to notify the
/// host of pressed keys.
//------------------------------------------------------------------------------
typedef struct {

    unsigned char bmButtons;        /// Bitmap state of three mouse buttons.
    signed char bX;                 /// Pointer displacement along the X axis.
    signed char bY;                 /// Pointer displacement along the Y axis.

} __attribute__ ((packed)) HIDDMouseInputReport; // GCC

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

extern void HIDDMouseInputReport_Initialize(HIDDMouseInputReport *report);

#endif //#ifndef HIDDKEYBOARDINPUTREPORT_H

