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

    Definition of the USBGenericRequest class and its methods.

 !!!Usage

    -# Declare or access USB requests by USBGenericRequest instance.
    -# To get usful information (field values) from the USB requests, use
       - USBGenericRequest_GetType
       - USBGenericRequest_GetRequest
       - USBGenericRequest_GetValue
       - USBGenericRequest_GetIndex
       - USBGenericRequest_GetLength
       - USBGenericRequest_GetEndpointNumber
       - USBGenericRequest_GetRecipient
       - USBGenericRequest_GetDirection
*/

#ifndef USBGENERICREQUEST_H
#define USBGENERICREQUEST_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB Generic Request definitions"
///
/// This page lists the codes of USB generic request.
///
/// - USB Request codes
///    - USBGenericRequest_GETSTATUS
///    - USBGenericRequest_CLEARFEATURE
///    - USBGenericRequest_SETFEATURE
///    - USBGenericRequest_SETADDRESS
///    - USBGenericRequest_GETDESCRIPTOR
///    - USBGenericRequest_SETDESCRIPTOR
///    - USBGenericRequest_GETCONFIGURATION
///    - USBGenericRequest_SETCONFIGURATION
///    - USBGenericRequest_GETINTERFACE
///    - USBGenericRequest_SETINTERFACE
///    - USBGenericRequest_SYNCHFRAME
///
/// - USB Request Recipients
///    - USBGenericRequest_DEVICE
///    - USBGenericRequest_INTERFACE
///    - USBGenericRequest_ENDPOINT
///    - USBGenericRequest_OTHER
///
/// - USB Request Types
///    - USBGenericRequest_STANDARD
///    - USBGenericRequest_CLASS
///    - USBGenericRequest_VENDOR
///
/// - USB Request Directions
///    - USBGenericRequest_IN
///    - USBGenericRequest_OUT
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB Request codes"
///
/// This page lists the USB generic request codes.
///
/// !Codes
/// - USBGenericRequest_GETSTATUS
/// - USBGenericRequest_CLEARFEATURE
/// - USBGenericRequest_SETFEATURE
/// - USBGenericRequest_SETADDRESS
/// - USBGenericRequest_GETDESCRIPTOR
/// - USBGenericRequest_SETDESCRIPTOR
/// - USBGenericRequest_GETCONFIGURATION
/// - USBGenericRequest_SETCONFIGURATION
/// - USBGenericRequest_GETINTERFACE
/// - USBGenericRequest_SETINTERFACE
/// - USBGenericRequest_SYNCHFRAME

/// GET_STATUS request code.
#define USBGenericRequest_GETSTATUS             0
/// CLEAR_FEATURE request code.
#define USBGenericRequest_CLEARFEATURE          1
/// SET_FEATURE request code.
#define USBGenericRequest_SETFEATURE            3
/// SET_ADDRESS request code.
#define USBGenericRequest_SETADDRESS            5
/// GET_DESCRIPTOR request code.
#define USBGenericRequest_GETDESCRIPTOR         6
/// SET_DESCRIPTOR request code.
#define USBGenericRequest_SETDESCRIPTOR         7
/// GET_CONFIGURATION request code.
#define USBGenericRequest_GETCONFIGURATION      8
/// SET_CONFIGURATION request code.
#define USBGenericRequest_SETCONFIGURATION      9
/// GET_INTERFACE request code.
#define USBGenericRequest_GETINTERFACE          10
/// SET_INTERFACE request code.
#define USBGenericRequest_SETINTERFACE          11
/// SYNCH_FRAME request code.
#define USBGenericRequest_SYNCHFRAME            12
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB Request Recipients"
///
/// This page lists codes of USB request recipients.
///
/// !Recipients
/// - USBGenericRequest_DEVICE
/// - USBGenericRequest_INTERFACE
/// - USBGenericRequest_ENDPOINT
/// - USBGenericRequest_OTHER

/// Recipient is the whole device.
#define USBGenericRequest_DEVICE                0
/// Recipient is an interface.
#define USBGenericRequest_INTERFACE             1
/// Recipient is an endpoint.
#define USBGenericRequest_ENDPOINT              2
/// Recipient is another entity.
#define USBGenericRequest_OTHER                 3
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB Request Types"
///
/// This page lists codes of USB request types.
///
/// !Types
/// - USBGenericRequest_STANDARD
/// - USBGenericRequest_CLASS
/// - USBGenericRequest_VENDOR

/// Request is standard.
#define USBGenericRequest_STANDARD              0
/// Request is class-specific.
#define USBGenericRequest_CLASS                 1
/// Request is vendor-specific.
#define USBGenericRequest_VENDOR                2
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB Request Directions"
///
/// This page lists codes of USB request directions.
///
/// !Directions
/// - USBGenericRequest_IN
/// - USBGenericRequest_OUT

/// Transfer occurs from device to the host.
#define USBGenericRequest_OUT                   0
/// Transfer occurs from the host to the device.
#define USBGenericRequest_IN                    1
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Generic USB SETUP request sent over Control endpoints.
//------------------------------------------------------------------------------
typedef struct {

    /// Type of request
    /// \sa "USB Request Recipients"
    /// \sa "USB Request Types"
    /// \sa "USB Request Directions"
    unsigned char bmRequestType:8;
    /// Request code
    /// \sa "USB Request Codes"
    unsigned char bRequest:8;
    /// Request-specific value parameter.
    unsigned short wValue:16;
    /// Request-specific index parameter.
    unsigned short wIndex:16;
    /// Expected length (in bytes) of the data phase.
    unsigned short wLength:16;

} USBGenericRequest;

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

extern unsigned char USBGenericRequest_GetType(
    const USBGenericRequest *request);


extern unsigned char USBGenericRequest_GetRequest(
    const USBGenericRequest *request);

extern unsigned short USBGenericRequest_GetValue(
    const USBGenericRequest *request);

extern unsigned short USBGenericRequest_GetIndex(
    const USBGenericRequest *request);

extern unsigned short USBGenericRequest_GetLength(
    const USBGenericRequest *request);

extern unsigned char USBGenericRequest_GetEndpointNumber(
    const USBGenericRequest *request);

extern unsigned char USBGenericRequest_GetRecipient(
    const USBGenericRequest *request);

extern unsigned char USBGenericRequest_GetDirection(
    const USBGenericRequest *request);

#endif //#ifndef USBGENERICREQUEST_H

