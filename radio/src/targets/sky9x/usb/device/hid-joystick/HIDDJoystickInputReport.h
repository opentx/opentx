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
 
 Class for manipulating HID Joystick input reports.

 !!!Usage

 -# Initialize a newly created input report with
    HIDDJoystickInputReport_Initialize
*/

#ifndef HIDDJOYSTICKINPUTREPORT_H
#define HIDDJOYSTICKINPUTREPORT_H

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
/// HID input report structure used by the Joystick driver to notify the
/// host of pressed keys.
//------------------------------------------------------------------------------
typedef struct {

    unsigned char buttons1; // bit 0 - button 1, bit 1 - button 2, ..., mapped to channels 9-16, on if channel > 0
    unsigned char buttons2; // mapped to channels 17-24, on if channel > 0
    unsigned char buttons3; // mapped to channels 25-32, on if channel > 0
    unsigned char X;        // analog value, mapped to channel 1
    unsigned char Y;        // analog value, mapped to channel 2
    unsigned char Z;        // analog value, mapped to channel 3
    unsigned char Rx;       // analog value, mapped to channel 4
    unsigned char Ry;       // analog value, mapped to channel 5
    unsigned char Rz;       // analog value, mapped to channel 6
    unsigned char S1;       // analog value, mapped to channel 7
    unsigned char S2;       // analog value, mapped to channel 8

} __attribute__ ((packed)) HIDDJoystickInputReport; // GCC

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

extern void HIDDJoystickInputReport_Initialize(HIDDJoystickInputReport *report);

#endif //#ifndef HIDDJOYSTICKINPUTREPORT_H

