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

/*
    Title: HIDDKeyboardOutputReport implementation

    About: Purpose
        Implementation of the HIDDKeyboardOutputReport class.
*/

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "HIDDKeyboardOutputReport.h"

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes a keyboard output report.
/// \param report Pointer to a HIDDKeyboardOutputReport instance.
//------------------------------------------------------------------------------
void HIDDKeyboardOutputReport_Initialize(HIDDKeyboardOutputReport *report)
{
    report->numLockStatus = 0;
    report->capsLockStatus = 0;
    report->scrollLockStatus = 0;
    report->padding = 0;
}

//------------------------------------------------------------------------------
/// Indicates the current status of the num. lock LED according to the
/// given report.
/// \param report Pointer to a HIDDKeyboardOutputReport instance.
/// \return 1 if the num. lock LED is light on; otherwise 0.
//------------------------------------------------------------------------------
unsigned char HIDDKeyboardOutputReport_GetNumLockStatus(
    const HIDDKeyboardOutputReport *report)
{
    return report->numLockStatus;
}

//------------------------------------------------------------------------------
/// Indicates the current status of the caps lock LED according to the
/// given report.
/// \param report Pointer to a HIDDKeyboardOutputReport instance.
/// \return 1 if the caps lock LED is light on; otherwise 0.
//------------------------------------------------------------------------------
unsigned char HIDDKeyboardOutputReport_GetCapsLockStatus(
    const HIDDKeyboardOutputReport *report)
{
    return report->capsLockStatus;
}

//------------------------------------------------------------------------------
/// Indicates the current status of the scroll lock LED according to the
/// given report.
/// \param report Pointer to a HIDDKeyboardOutputReport instance.
/// \return 1 if the scroll lock LED is light on; otherwise 0.
//------------------------------------------------------------------------------
unsigned char HIDDKeyboardOutputReport_GetScrollLockStatus(
    const HIDDKeyboardOutputReport *report)
{
    return report->scrollLockStatus;
}

