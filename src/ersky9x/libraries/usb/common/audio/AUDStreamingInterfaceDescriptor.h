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
/// Definition of a class for manipulating USB %audio streaming interface
/// descriptors.
///
/// !!!Usage
///
/// - For a USB %device:
///     -# Declare an AUDStreamingInterfaceDescriptor instance as part of the
///        configuration descriptors returned by a USB %audio %device.
//------------------------------------------------------------------------------

#ifndef AUDSTREAMINGINTERFACEDESCRIPTOR_H
#define AUDSTREAMINGINTERFACEDESCRIPTOR_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB Audio streaming interface codes"
///
/// This page lists the class, subclass and protocol codes that an Audio
/// Streaming interface should display in its descriptor.
/// 
/// !Codes
/// - AUDStreamingInterfaceDescriptor_CLASS
/// - AUDStreamingInterfaceDescriptor_SUBCLASS
/// - AUDStreamingInterfaceDescriptor_PROTOCOL

/// Class code for an USB audio streaming interface.
#define AUDStreamingInterfaceDescriptor_CLASS               0x01

/// Subclass code for an audio streaming interface.
#define AUDStreamingInterfaceDescriptor_SUBCLASS            0x02

/// Protocol code for an audio streaming interface.
#define AUDStreamingInterfaceDescriptor_PROTOCOL            0x00
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB Audio streaming descriptor subtypes"
///
/// This page lists the possible subtypes for audio-specific descriptor appended
/// to an Audio Streaming interface.
/// 
/// !Subtypes
/// - AUDStreamingInterfaceDescriptor_GENERAL
/// - AUDStreamingInterfaceDescriptor_FORMATTYPE
/// - AUDStreamingInterfaceDescriptor_FORMATSPECIFIC

/// General descriptor subtype.
#define AUDStreamingInterfaceDescriptor_GENERAL             0x01

/// Format type descriptor subtype.
#define AUDStreamingInterfaceDescriptor_FORMATTYPE          0x02

/// Format specific descriptor subtype.
#define AUDStreamingInterfaceDescriptor_FORMATSPECIFIC      0x03
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
/// Provides additional information about an audio streaming interface to
/// the USB host.
//------------------------------------------------------------------------------
typedef struct {

    /// Size of descriptor in bytes.
    unsigned char bLength;
    /// Descriptor type (AUDGenericDescriptor_INTERFACE).
    unsigned char bDescriptorType;
    /// Descriptor subtype (AUDStreamingInterfaceDescriptor_GENERAL).
    unsigned char bDescriptorSubType;
    /// Terminal ID to which the interface is connected.
    unsigned char bTerminalLink;
    /// Delay introduced by the data path, in number of frames.
    unsigned char bDelay;
    /// Audio data format used by this interface.
    unsigned short wFormatTag;

} __attribute__ ((packed)) AUDStreamingInterfaceDescriptor; // GCC

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

#endif //#ifndef AUDSTREAMINGINTERFACEDESCRIPTOR_H

