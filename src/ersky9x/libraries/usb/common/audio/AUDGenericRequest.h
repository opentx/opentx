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

//------------------------------------------------------------------------------
/// \unit
///
/// !!!Purpose
///
/// Constants for using USB %audio SETUP requests.
///
/// !!!Usage
///
/// - For a USB %device:
///     -# After receiving an Audio request from the host, use
///        AUDGenericRequest_GetEntity to determine the target Unit or Terminal.
///     -# After receiving an Audio request from the host, use
///        AUDGenericRequest_GetInterface to know which interface is referenced.
//------------------------------------------------------------------------------

#ifndef AUDGENERICREQUEST_H
#define AUDGENERICREQUEST_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include <usb/common/core/USBGenericRequest.h>

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB Audio request codes"
///
/// This page lists the codes of the existing Audio requests.
/// 
/// !Requests
/// - AUDGenericRequest_SETCUR
/// - AUDGenericRequest_GETCUR

/// SET_CUR request code.
#define AUDGenericRequest_SETCUR                0x01

/// GET_CUR request code.
#define AUDGenericRequest_GETCUR                0x81

/// GET_MIN request code.
#define AUDGenericRequest_GETMIN                0x82

/// GET_MAX request code.
#define AUDGenericRequest_GETMAX                0x83

/// GET_RES request code.
#define AUDGenericRequest_GETRES                0x84
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

extern unsigned char AUDGenericRequest_GetEntity(
    const USBGenericRequest *request);
extern unsigned char AUDGenericRequest_GetInterface(
    const USBGenericRequest *request);

#endif //#ifndef AUDGENERICREQUEST_H

