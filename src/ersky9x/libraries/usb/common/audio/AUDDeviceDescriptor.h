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
/// Fields values for an USB Audio %device descriptor.
///
/// !!!Usage
///
/// -# When declaring the %device descriptor of a USB %audio %device, use
///    "USB Audio device descriptor values" defined here.
//------------------------------------------------------------------------------

#ifndef AUDDEVICEDESCRIPTOR_H
#define AUDDEVICEDESCRIPTOR_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB Audio device descriptor values"
///
/// This page lists the class, subclass & protocol codes that a USB audio
/// device should display in its device descriptor.
/// 
/// !Codes
/// - AUDDeviceDescriptor_CLASS
/// - AUDDeviceDescriptor_SUBCLASS
/// - AUDDeviceDescriptor_PROTOCOL

/// Class code for a USB audio device.
#define AUDDeviceDescriptor_CLASS                       0x00

/// Subclass code for a USB audio device.
#define AUDDeviceDescriptor_SUBCLASS                    0x00

/// Protocol code for a USB audio device.
#define AUDDeviceDescriptor_PROTOCOL                    0x00
//------------------------------------------------------------------------------

#endif //#ifndef AUDDEVICEDESCRIPTOR_H

