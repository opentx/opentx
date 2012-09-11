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
/// Constants for manipulating USB audio-specific descriptors.
///
/// !!!Usage
///
/// - For a USB %device:
///     -# When declaring an Audio-specific descriptor, use the descriptor types
///        and subtypes defined in this unit (see "USB Audio descriptor types"
///        and "USB Audio descriptor subtypes").
//------------------------------------------------------------------------------

#ifndef AUDGENERICDESCRIPTOR_H
#define AUDGENERICDESCRIPTOR_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB Audio descriptor types"
///
/// This page lists the available types for USB audio-specific descriptors.
/// 
/// !Types
/// - AUDGenericDescriptor_DEVICE
/// - AUDGenericDescriptor_CONFIGURATION
/// - AUDGenericDescriptor_STRING
/// - AUDGenericDescriptor_INTERFACE
/// - AUDGenericDescriptor_ENDPOINT

/// Descriptor gives detail about the whole device.
#define AUDGenericDescriptor_DEVICE                     0x21

/// Descriptor gives detail about a configuration.
#define AUDGenericDescriptor_CONFIGURATION              0x22

/// Descriptor gives detail about a string.
#define AUDGenericDescriptor_STRING                     0x23

/// Descriptor gives detail about an interface.
#define AUDGenericDescriptor_INTERFACE                  0x24

/// Descriptor gives detail about an endpoint.
#define AUDGenericDescriptor_ENDPOINT                   0x25
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB Audio descriptor subtypes"
///
/// This page lists the possible subtypes for USB audio-specific descriptors.
/// 
/// !Subtypes
/// - AUDGenericDescriptor_HEADER
/// - AUDGenericDescriptor_INPUTTERMINAL
/// - AUDGenericDescriptor_OUTPUTTERMINAL
/// - AUDGenericDescriptor_MIXERUNIT
/// - AUDGenericDescriptor_SELECTORUNIT
/// - AUDGenericDescriptor_FEATUREUNIT
/// - AUDGenericDescriptor_PROCESSINGUNIT
/// - AUDGenericDescriptor_EXTENSIONUNIT

/// Header descriptor subtype.
#define AUDGenericDescriptor_HEADER                     0x01

/// Input terminal descriptor subtype.
#define AUDGenericDescriptor_INPUTTERMINAL              0x02

/// Output terminal descriptor subtype.
#define AUDGenericDescriptor_OUTPUTTERMINAL             0x03

/// Mixer unit descriptor subtype.
#define AUDGenericDescriptor_MIXERUNIT                  0x04

/// Selector unit descriptor subtype.
#define AUDGenericDescriptor_SELECTORUNIT               0x05

/// Feature unit descriptor subtype.
#define AUDGenericDescriptor_FEATUREUNIT                0x06

/// Processing unit descriptor subtype.
#define AUDGenericDescriptor_PROCESSINGUNIT             0x07

///Extension unit descriptor subtype.
#define AUDGenericDescriptor_EXTENSIONUNIT              0x08
//------------------------------------------------------------------------------

#endif //#ifndef AUDGENERICDESCRIPTOR_H

