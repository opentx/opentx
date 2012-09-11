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
    Title: HIDGenericRequest

    About: Purpose
        Definition of constants for using HID-specific requests.

    About: Usage
        When constructing or receiving an HID SETUP request, use the request
        codes provided by this header file.
*/

#ifndef HIDGENERICREQUEST_H
#define HIDGENERICREQUEST_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "HID Request Codes"
/// ...
///
/// !Codes
/// - HIDGenericRequest_GETREPORT
/// - HIDGenericRequest_GETIDLE
/// - HIDGenericRequest_GETPROTOCOL
/// - HIDGenericRequest_SETREPORT
/// - HIDGenericRequest_SETIDLE
/// - HIDGenericRequest_SETPROTOCOL

/// GetReport request code.
#define HIDGenericRequest_GETREPORT             0x01
/// GetIdle request code.
#define HIDGenericRequest_GETIDLE               0x02
/// GetProtocol request code.
#define HIDGenericRequest_GETPROTOCOL           0x03
/// SetReport request code.
#define HIDGenericRequest_SETREPORT             0x09
/// SetIdle request code.
#define HIDGenericRequest_SETIDLE               0x0A
/// SetProtocol request code.
#define HIDGenericRequest_SETPROTOCOL           0x0B
//------------------------------------------------------------------------------

#endif //#ifndef HIDGENERICREQUEST_H

