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
    Title: CDCLineCoding

    About: Purpose
        Implementation of the CDCLineCoding class.
*/

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "CDCLineCoding.h"
#include <utility/assert.h>

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes the bitrate, number of stop bits, parity checking and
/// number of data bits of a CDCLineCoding object.
/// \param lineCoding Pointer to a CDCLineCoding instance.
/// \param bitrate Bitrate of the virtual COM connection.
/// \param stopbits Number of stop bits ("CDC LineCoding StopBits").
/// \param parity Parity check type ("CDC LineCoding ParityChecking").
/// \param databits Number of data bits.
//------------------------------------------------------------------------------
void CDCLineCoding_Initialize(CDCLineCoding *lineCoding,
                              unsigned int bitrate,
                              unsigned char stopbits,
                              unsigned char parity,
                              unsigned char databits)
{
    ASSERT(stopbits <= CDCLineCoding_TWOSTOPBITS,
           "CDCLineCoding_Initialize: Invalid stopbits value (%d)\n\r",
           stopbits);
    ASSERT(parity <= CDCLineCoding_SPACEPARITY,
           "CDCLineCoding_Initialize: Invalid parity value (%d)\n\r",
           parity);
    ASSERT(((databits >= 5) && (databits <= 8)) || (databits == 16),
           "CDCLineCoding_Initialize: Invalid databits value (%d)\n\r",
           databits);

    lineCoding->dwDTERate = bitrate;
    lineCoding->bCharFormat = stopbits;
    lineCoding->bParityType = parity;
    lineCoding->bDataBits = databits;
}

