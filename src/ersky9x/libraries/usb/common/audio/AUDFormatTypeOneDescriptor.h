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
/// Definitions for using USB %audio format type I descriptors.
///
/// !!!Usage
///
/// -# Declare an instance of AUDFormatTypeOneDescriptor as part of the
///    configuration descriptors of an audio device.
//------------------------------------------------------------------------------

#ifndef AUDFORMATTYPEONEDESCRIPTOR_H
#define AUDFORMATTYPEONEDESCRIPTOR_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

/// Format type for a format type I descriptor.
#define AUDFormatTypeOneDescriptor_FORMATTYPEONE        0x01

/// AUDFormatTypeOneDescriptor_PCM - PCM format.
#define AUDFormatTypeOneDescriptor_PCM                  0x0001

/// Indicates the sampling frequency can have any value in the provided range.
#define AUDFormatTypeOneDescriptor_CONTINUOUS           0

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
/// Describes an audio data stream that is constructed on a sample-by-sample
/// basis. This type must be augmented with either the continuous sampling
/// frequency range (if bSamFreqType = <AUDFormatTypeOneDescriptor_CONTINUOUS>)
/// or with an array containing the possible discrete frequencies.
//------------------------------------------------------------------------------
typedef struct {

    /// Size of descriptor in bytes.
    unsigned char bLength;
    /// Descriptor type (AUDGenericDescriptor_INTERFACE).
    unsigned char bDescriptorType;
    /// Descriptor subtype (AUDStreamingInterfaceDescriptor_FORMATTYPE).
    unsigned char bDescriptorSubType;
    /// Format type (AUDFormatTypeOneDescriptor_FORMATTYPEONE).
    unsigned char bFormatType;
    /// Number of physical channels in the audio stream.
    unsigned char bNrChannels;
    /// Number of bytes occupied by one audio subframe.
    unsigned char bSubFrameSize;
    /// Number of bits effectively used in an audio subframe.
    unsigned char bBitResolution;
    /// Number of supported discrete sampling frequencies, or
    /// AUDFormatTypeOneDescriptor_CONTINUOUS.
    unsigned char bSamFreqType;

} __attribute__ ((packed)) AUDFormatTypeOneDescriptor; // GCC

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

#endif //#ifndef AUDFORMATTYPEONEDESCRIPTOR_H

