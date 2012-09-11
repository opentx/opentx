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
/// Definition of the USB audio-specific data endpoint descriptor.
///
/// !!!Usage
/// 
/// -# Declare an AUDDataEndpointDescriptor instance as part of the
///    configuration descriptors of a USB %audio %device.
//------------------------------------------------------------------------------

#ifndef AUDDATAENDPOINTDESCRIPTOR_H
#define AUDDATAENDPOINTDESCRIPTOR_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

/// Descriptor subtype for an Audio data endpoint.
#define AUDDataEndpointDescriptor_SUBTYPE                   0x01

//------------------------------------------------------------------------------
/// \page "USB Audio Lock delay units"
///
/// This page lists the valid lock delay unit types.
/// 
/// !Units
/// - AUDDataEndpointDescriptor_MILLISECONDS
/// - AUDDataEndpointDescriptor_PCMSAMPLES

/// Lock delay is expressed in milliseconds.
#define AUDDataEndpointDescriptor_MILLISECONDS              1

/// Lock delay is expressed in decoded PCM samples.
#define AUDDataEndpointDescriptor_PCMSAMPLES                2
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
/// Gives additional information about an USB endpoint used to transmit audio
/// data to or from the host.
//------------------------------------------------------------------------------
typedef struct {

    /// Size of descriptor in bytes.
    unsigned char bLength;
    /// Descriptor type (AUDGenericDescriptor_ENDPOINT).
    unsigned char bDescriptorType;
    ///  Descriptor subtype  (AUDDataEndpointDescriptor_SUBTYPE).
    unsigned char bDescriptorSubType;
    /// Indicates available controls and requirement on packet sizes.
    unsigned char bmAttributes;
    /// Indicates the units of the wLockDelay fields.
    /// \sa "USB Audio Lock delay units"
    unsigned char bLockDelayUnits;
    /// Time it takes for the endpoint to lock its internal clock circuitry.
    unsigned short wLockDelay;

} __attribute__ ((packed)) AUDDataEndpointDescriptor; // GCC

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

#endif //#ifndef AUDDATAENDPOINTDESCRIPTOR_H
