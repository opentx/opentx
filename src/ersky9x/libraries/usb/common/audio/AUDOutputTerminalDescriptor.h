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
/// Definition of a class for using USB %audio output terminal descriptors.
///
/// !!!Usage
///
/// - For a USB %device:
///     -# Declare a AUDOutputTerminalDescriptor instance as part of the
///        configuration descriptors of a USB %audio %device.
//------------------------------------------------------------------------------

#ifndef AUDOUTPUTTERMINALDESCRIPTOR_H
#define AUDOUTPUTTERMINALDESCRIPTOR_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB Audio Output terminal types"
///
/// This page lists the available types for an output terminal.
/// 
/// !Types
/// - AUDOutputTerminalDescriptor_USBTREAMING
/// - AUDOutputTerminalDescriptor_SPEAKER
/// - AUDOutputTerminalDescriptor_HANDSETOUT
/// - AUDOutputTerminalDescriptor_LINEOUT

/// A terminal sending data through USB isochronous endpoint.
#define AUDOutputTerminalDescriptor_USBTREAMING         0x0101
/// A terminal sending data to a USB host through an Isochronous endpoint.
#define AUDOutputTerminalDescriptor_SPEAKER             0x0301
/// A terminal sending data to Handset Out.
#define AUDOutputTerminalDescriptor_HANDSETOUT          0x0401
/// A terminal sending data to Phone Line Out.
#define AUDOutputTerminalDescriptor_LINEOUT             0x0501
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
/// \typedef AUDOutputTerminalDescriptor
/// Describes an output of the USB audio function.
//------------------------------------------------------------------------------
typedef struct {

    /// Size of descriptor in bytes.
    unsigned char bLength;
    /// Descriptor type (AUDGenericDescriptor_INTERFACE).
    unsigned char bDescriptorType;
    /// Descriptor subtype (AUDGenericDescriptor_OUTPUTTERMINAL).
    unsigned char bDescriptorSubType;
    /// Identifier for this terminal.
    unsigned char bTerminalID;
    /// Terminal type.
    /// \sa "USB Audio Output terminal types"
    unsigned short wTerminalType;
    /// Identifier of the associated input terminal.
    unsigned char bAssocTerminal;
    /// Identifier of the unit or terminal to which this terminal is connected.
    unsigned char bSourceID;
    /// Index of a string descriptor for this terminal.
    unsigned char iTerminal;

} __attribute__ ((packed)) AUDOutputTerminalDescriptor; // GCC

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

#endif //#ifndef AUDOUTPUTTERMINALDESCRIPTOR_H

