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
/// Definition of a generic USB descriptor class.
///
/// !!!Usage
///
/// -# Declare or access USB descriptors by USBGenericDescriptor instance.
/// -# To get usful information (field values) from the USB descriptors, use
///    - USBGenericDescriptor_GetLength
///    - USBGenericDescriptor_GetType
/// -# To scan the descriptors, use
///    - USBGenericDescriptor_GetNextDescriptor
//------------------------------------------------------------------------------

#ifndef USBGENERICDESCRIPTOR_H
#define USBGENERICDESCRIPTOR_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB Descriptor types"
///
/// This page lists the codes of the usb descriptor types
///
/// !Types
/// - USBGenericDescriptor_DEVICE
/// - USBGenericDescriptor_CONFIGURATION
/// - USBGenericDescriptor_STRING
/// - USBGenericDescriptor_INTERFACE
/// - USBGenericDescriptor_ENDPOINT
/// - USBGenericDescriptor_DEVICEQUALIFIER
/// - USBGenericDescriptor_OTHERSPEEDCONFIGURATION
/// - USBGenericDescriptor_INTERFACEPOWER
/// - USBGenericDescriptor_OTG
/// - USBGenericDescriptor_DEBUG
/// - USBGenericDescriptor_INTERFACEASSOCIATION

/// Device descriptor type.
#define USBGenericDescriptor_DEVICE                     1
/// Configuration descriptor type.
#define USBGenericDescriptor_CONFIGURATION              2
/// String descriptor type.
#define USBGenericDescriptor_STRING                     3
/// Interface descriptor type.
#define USBGenericDescriptor_INTERFACE                  4
/// Endpoint descriptor type.
#define USBGenericDescriptor_ENDPOINT                   5
/// Device qualifier descriptor type.
#define USBGenericDescriptor_DEVICEQUALIFIER            6
/// Other speed configuration descriptor type.
#define USBGenericDescriptor_OTHERSPEEDCONFIGURATION    7
/// Interface power descriptor type.
#define USBGenericDescriptor_INTERFACEPOWER             8
/// On-The-Go descriptor type.
#define USBGenericDescriptor_OTG                        9
/// Debug descriptor type.
#define USBGenericDescriptor_DEBUG                      10
/// Interface association descriptor type.
#define USBGenericDescriptor_INTERFACEASSOCIATION       11
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

/// Holds the few fields shared by all USB descriptors.
typedef struct {

    /// Length of the descriptor in bytes.
    unsigned char bLength;
    /// Descriptor type.
    unsigned char bDescriptorType;

} __attribute__ ((packed)) USBGenericDescriptor; // GCC

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

extern unsigned int USBGenericDescriptor_GetLength(
    const USBGenericDescriptor *descriptor);

extern unsigned char USBGenericDescriptor_GetType(
    const USBGenericDescriptor *descriptor);

extern USBGenericDescriptor *USBGenericDescriptor_GetNextDescriptor(
    const USBGenericDescriptor *descriptor);

#endif //#ifndef USBGENERICDESCRIPTOR_H

