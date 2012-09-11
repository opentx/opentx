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
/// Class for manipulating USB %audio feature unit descriptor.
///
/// !!!Usage
///
/// -# Declare an AUDFeatureUnitDescriptor instance as part of the
///    configuration descriptors returned by a USB %audio %device.
//------------------------------------------------------------------------------

#ifndef AUDFEATUREUNITDESCRIPTOR_H
#define AUDFEATUREUNITDESCRIPTOR_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB Audio Channel controls"
///
/// This page lists the available controls for each channel of the audio
/// interface. Each channel can have any control combination; simply perform
/// a bitwise OR ('|') to combine several values.
/// 
/// !Controls
/// -# AUDFeatureUnitDescriptor_MUTE
/// -# AUDFeatureUnitDescriptor_VOLUME
/// -# AUDFeatureUnitDescriptor_BASS
/// -# AUDFeatureUnitDescriptor_MID
/// -# AUDFeatureUnitDescriptor_TREBLE

/// Channel mute control.
#define AUDFeatureUnitDescriptor_MUTE               (1 << 0)

/// Channel volume control.
#define AUDFeatureUnitDescriptor_VOLUME             (1 << 1)

/// Channel bass control.
#define AUDFeatureUnitDescriptor_BASS               (1 << 2)

/// Channel middle control.
#define AUDFeatureUnitDescriptor_MID                (1 << 3)

/// Channel treble control.
#define AUDFeatureUnitDescriptor_TREBLE             (1 << 4)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
/// Describes available controls for each channel of the unit or terminal
/// it is connected to. This type must be augmented with the relevant number
/// of bmaControls fields and the iFeature field.
//------------------------------------------------------------------------------
typedef struct {

    /// Size of descriptor in bytes.
    unsigned char bLength;
    /// Descriptor type (AUDGenericDescriptor_INTERFACE).
    unsigned char bDescriptorType;
    /// Descriptor subtype (AUDGenericDescriptor_FEATURE).
    unsigned char bDescriptorSubType;
    /// Identifier of this feature unit.
    unsigned char bUnitID;
    /// Identifier of the unit or terminal this feature unit is connected to.
    unsigned char bSourceID;
    /// Size in bytes of a channel controls field.
    unsigned char bControlSize;

} __attribute__ ((packed)) AUDFeatureUnitDescriptor; // GCC

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

#endif //#ifndef AUDFEATUREUNITDESCRIPTOR_H

