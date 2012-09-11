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
/// Class for manipulating input terminal descriptors.
///
/// !!!Usage
///
/// - For a USB %device:
///     -# Declare an AUDInputTerminalDescriptor instance as part of the
///        configuration of the %device.
//------------------------------------------------------------------------------

#ifndef AUDINPUTTERMINALDESCRIPTOR_H
#define AUDINPUTTERMINALDESCRIPTOR_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB Audio Input terminal types"
///
/// This page lists the available types for an Input terminal.
/// 
/// !Types
/// 
/// - AUDInputTerminalDescriptor_USBSTREAMING
/// - AUDInputTerminalDescriptor_MICROPHONE
/// - AUDInputTerminalDescriptor_SPEAKERPHONE
/// - AUDInputTerminalDescriptor_LINEIN

/// A terminal receiving its data from a USB isochronous endpoint.
#define AUDInputTerminalDescriptor_USBSTREAMING         0x0101
/// A terminal sampling data from a microphone.
#define AUDInputTerminalDescriptor_MICROPHONE           0x0201
/// A terminal sampling data from a Handset In.
#define AUDInputTerminalDescriptor_HANDSETIN            0x0401
/// A terminal sampling data from a speakerphone
#define AUDInputTerminalDescriptor_SPEAKERPHONE         0x0403
/// A terminal sampling data from a Phone Line In.
#define AUDInputTerminalDescriptor_LINEIN               0x0501
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB Audio Channel spatial locations"
///
/// This page lists the possible spatial locations for audio channels.
/// 
/// !Locations
/// 
/// - AUDInputTerminalDescriptor_LEFTFRONT
/// - AUDInputTerminalDescriptor_RIGHTFRONT
/// - AUDInputTerminalDescriptor_CENTERFRONT

/// Front left channel.
#define AUDInputTerminalDescriptor_LEFTFRONT            (1 << 0)

/// Front right channel.
#define AUDInputTerminalDescriptor_RIGHTFRONT           (1 << 1)

/// Front center channel.
#define AUDInputTerminalDescriptor_CENTERFRONT          (1 << 2)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
/// Describes an input of a USB audio device.
//------------------------------------------------------------------------------
typedef struct {

    /// Size of descriptor in bytes.
    unsigned char bLength;
    /// Descriptor type (AUDGenericDescriptor_INTERFACE).
    unsigned char bDescriptorType;
    /// Descriptor subtype (AUDGenericDescriptor_INPUTTERMINAL).
    unsigned char bDescriptorSubType;
    /// ID of the terminal in the audio function.
    unsigned char bTerminalID;
    /// Terminal type.
    /// \sa "USB Audio Input terminal types"
    unsigned short wTerminalType;
    /// ID of the output terminal to which this input terminal is associated.
    unsigned char bAssocTerminal;
    /// Number of logical output channels in this terminal.
    unsigned char bNrChannels;
    /// Spatial configuration of the logical channels.
    unsigned short wChannelConfig;
    /// Index of a string descriptor for the first logical channel.
    unsigned char iChannelNames;
    /// Index of a string descriptor for this terminal.
    unsigned char iTerminal;

} __attribute__ ((packed)) AUDInputTerminalDescriptor; // GCC

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

#endif //#ifndef AUDINPUTTERMINALDESCRIPTOR_H

