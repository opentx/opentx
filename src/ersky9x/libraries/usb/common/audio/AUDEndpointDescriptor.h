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
/// Re-definition of the standard USB endpoint descriptor with two additional
/// fields. This is required by the USB audio 1.00 specification.
///
/// !!!Usage
///
/// -# Declare an AUDEndpointDescriptor instance as part of the configuration
///    descriptors of an USB %audio %device.
//------------------------------------------------------------------------------

#ifndef AUDENDPOINTDESCRIPTOR_H
#define AUDENDPOINTDESCRIPTOR_H

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
/// Modified endpoint descriptor with two additional fields, with are
/// USB audio specific.
//------------------------------------------------------------------------------
typedef struct {

   /// Size of the descriptor in bytes.
   unsigned char bLength;
   /// Descriptor type (USBGenericDescriptor_ENDPOINT).
   unsigned char bDescriptorType;
   /// Address and direction of the endpoint.
   unsigned char bEndpointAddress;
   /// Endpoint type and additional characteristics (for isochronous endpoints).
   unsigned char bmAttributes;
   /// Maximum packet size (in bytes) of the endpoint.
   unsigned short wMaxPacketSize;
   /// Polling rate of the endpoint.
   unsigned char bInterval;
   /// Refresh rate for a feedback endpoint.
   unsigned char bRefresh;
   /// Address of the associated feedback endpoint if any.
   unsigned char bSyncAddress;
                                   
} __attribute__ ((packed)) AUDEndpointDescriptor; // GCC

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

#endif //#ifndef AUDENDPOINTDESCRIPTOR_H

