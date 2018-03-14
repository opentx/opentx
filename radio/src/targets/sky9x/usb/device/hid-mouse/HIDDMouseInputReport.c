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
    Title: HIDDMouseInputReport implementation

    About: Purpose
        Implementation of the HIDDMouseInputReport class.
*/

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "HIDDMouseInputReport.h"
#include "HIDDMouseDriverDescriptors.h"
#include <utility/assert.h>

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes a mouse input report instance.
/// \param report Pointer to a HIDDMouseInputReport instance.
//------------------------------------------------------------------------------
void HIDDMouseInputReport_Initialize(HIDDMouseInputReport *report)
{
    report->bmButtons = 0;
    report->bX = 0;
    report->bY = 0;
}

/*
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void HIDDMouseInputReport_UpdateButtons(HIDDMouseInputReport *report)
{
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void HIDDMouseInputReport_UpdateAxis(HIDDMouseInputReport *report)
{
}
*/
