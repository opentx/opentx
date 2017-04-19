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
    Title: USBGenericRequest implementation

    About: Purpose
        Implementation of the USBGenericRequest class.
*/

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "../../../usb/common/core/USBGenericRequest.h"

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
/// Returns the type of the given request.
/// \param request Pointer to a USBGenericRequest instance.
/// \return "USB Request Types"
//------------------------------------------------------------------------------
extern unsigned char USBGenericRequest_GetType(const USBGenericRequest *request)
{
    return ((request->bmRequestType >> 5) & 0x3);
}

//------------------------------------------------------------------------------
/// Returns the request code of the given request.
/// \param request Pointer to a USBGenericRequest instance.
/// \return Request code.
/// \sa "USB Request Codes"
//------------------------------------------------------------------------------
unsigned char USBGenericRequest_GetRequest(const USBGenericRequest *request)
{
    return request->bRequest;
}

//------------------------------------------------------------------------------
/// Returns the wValue field of the given request.
/// \param request - Pointer to a USBGenericRequest instance.
/// \return Request value.
//------------------------------------------------------------------------------
unsigned short USBGenericRequest_GetValue(const USBGenericRequest *request)
{
    return request->wValue;
}

//------------------------------------------------------------------------------
/// Returns the wIndex field of the given request.
/// \param request Pointer to a USBGenericRequest instance.
/// \return Request index;
//------------------------------------------------------------------------------
unsigned short USBGenericRequest_GetIndex(const USBGenericRequest *request)
{
    return request->wIndex;
}

//------------------------------------------------------------------------------
/// Returns the expected length of the data phase following a request.
/// \param request Pointer to a USBGenericRequest instance.
/// \return Length of data phase.
//------------------------------------------------------------------------------
unsigned short USBGenericRequest_GetLength(const USBGenericRequest *request)
{
    return request->wLength;
}

//------------------------------------------------------------------------------
/// Returns the endpoint number targetted by a given request.
/// \param request Pointer to a USBGenericRequest instance.
/// \return Endpoint number.
//------------------------------------------------------------------------------
unsigned char USBGenericRequest_GetEndpointNumber(
    const USBGenericRequest *request)
{
    return USBGenericRequest_GetIndex(request) & 0xF;
}

//------------------------------------------------------------------------------
/// Returns the intended recipient of a given request.
/// \param request Pointer to a USBGenericRequest instance.
/// \return Request recipient.
/// \sa "USB Request Recipients"
//------------------------------------------------------------------------------
unsigned char USBGenericRequest_GetRecipient(const USBGenericRequest *request)
{
    // Recipient is in bits [0..4] of the bmRequestType field
    return request->bmRequestType & 0xF;
}

//------------------------------------------------------------------------------
/// Returns the direction of the data transfer following the given request.
/// \param request Pointer to a USBGenericRequest instance.
/// \return Transfer direction.
/// \sa "USB Request Directions"
//------------------------------------------------------------------------------
unsigned char USBGenericRequest_GetDirection(const USBGenericRequest *request)
{
    // Transfer direction is located in bit D7 of the bmRequestType field
    if ((request->bmRequestType & 0x80) != 0) {

        return USBGenericRequest_IN;
    }
    else {

        return USBGenericRequest_OUT;
    }
}

