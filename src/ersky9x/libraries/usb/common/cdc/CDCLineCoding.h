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

 Line coding structure used for by the CDC GetLineCoding and SetLineCoding
 requests.

 !!!Usage

 -# Initialize a CDCLineCoding instance using CDCLineCoding_Initialize.
 -# Send a CDCLineCoding object to the host in response to a GetLineCoding
    request.
 -# Receive a CDCLineCoding object from the host after a SetLineCoding
    request.
*/

#ifndef CDCLINECODING_H
#define CDCLINECODING_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "CDC LineCoding StopBits"
/// This page lists Stop Bits for CDC Line Coding.
///
/// !Stop bits
/// - CDCLineCoding_ONESTOPBIT
/// - CDCLineCoding_ONE5STOPBIT
/// - CDCLineCoding_TWOSTOPBITS

/// The transmission protocol uses one stop bit.
#define CDCLineCoding_ONESTOPBIT            0
/// The transmission protocol uses 1.5 stop bit.
#define CDCLineCoding_ONE5STOPBIT           1
/// The transmissin protocol uses two stop bits.
#define CDCLineCoding_TWOSTOPBITS           2
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "CDC LineCoding ParityCheckings"
/// This page lists Parity checkings for CDC Line Coding.
///
/// !Parity checking
/// - CDCLineCoding_NOPARITY
/// - CDCLineCoding_ODDPARITY
/// - CDCLineCoding_EVENPARITY
/// - CDCLineCoding_MARKPARITY
/// - CDCLineCoding_SPACEPARITY

/// No parity checking.
#define CDCLineCoding_NOPARITY              0
/// Odd parity checking.
#define CDCLineCoding_ODDPARITY             1
/// Even parity checking.
#define CDCLineCoding_EVENPARITY            2
/// Mark parity checking.
#define CDCLineCoding_MARKPARITY            3
/// Space parity checking.
#define CDCLineCoding_SPACEPARITY           4
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
/// Format of the data returned when a GetLineCoding request is received.
//------------------------------------------------------------------------------
typedef struct {

    /// Data terminal rate in bits per second.
    unsigned int dwDTERate;
    /// Number of stop bits.
    /// \sa "CDC LineCoding StopBits".
    char bCharFormat;
    /// Type of parity checking used.
    /// \sa "CDC LineCoding ParityCheckings".
    char bParityType;
    /// Number of data bits (5, 6, 7, 8 or 16).
    char bDataBits;

} __attribute__ ((packed)) CDCLineCoding; // GCC

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

extern void CDCLineCoding_Initialize(CDCLineCoding *lineCoding,
                                     unsigned int bitrate,
                                     unsigned char stopbits,
                                     unsigned char parity,
                                     unsigned char databits);

#endif //#ifndef CDCLINECODING_H

