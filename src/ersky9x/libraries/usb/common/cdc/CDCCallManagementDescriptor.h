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

 Definition of a class for managing CDC call management descriptors.

 !!!Usage

 Should be included in a list of configuration descriptors for a USB
 device.
*/

#ifndef CDCCALLMANAGEMENTDESCRIPTOR_H
#define CDCCALLMANAGEMENTDESCRIPTOR_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "CDC CallManagement Capabilities"
/// This page lists CDC CallManagement Capabilities.
///
/// !Capabilities
/// - CDCCallManagementDescriptor_SELFCALLMANAGEMENT
/// - CDCCallManagementDescriptor_DATACALLMANAGEMENT

/// Device handles call management itself.
#define CDCCallManagementDescriptor_SELFCALLMANAGEMENT      (1 << 0)
/// Device can exchange call management information over a Data class interface.
#define CDCCallManagementDescriptor_DATACALLMANAGEMENT      (1 << 1)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
/// Describes the processing of calls for the communication class interface.
//------------------------------------------------------------------------------
typedef struct {

    /// Size of this descriptor in bytes.
    unsigned char bFunctionLength;
    /// Descriptor type (CDCDescriptors_INTERFACE).
    unsigned char bDescriptorType;
    /// Descriptor sub-type (CDCDescriptors_CALLMANAGEMENT).
    unsigned char bDescriptorSubtype;
    /// Configuration capabilities ("CDC CallManagement Capabilities").
    unsigned char bmCapabilities;
    /// Interface number of the data class interface used for call management
    /// (optional).
    unsigned char bDataInterface;

} __attribute__ ((packed)) CDCCallManagementDescriptor; // GCC

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

#endif //#ifndef CDCCALLMANAGEMENTDESCRIPTOR_H

