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
/// Definitions for using USB %audio control interfaces.
///
/// !!!Usage
/// 
/// -# When declaring a standard USB interface descriptor for an %audio control
///    interface, use the "USB Audio control interface codes" constants.
//------------------------------------------------------------------------------

#ifndef AUDCONTROLINTERFACEDESCRIPTOR_H
#define AUDCONTROLINTERFACEDESCRIPTOR_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB Audio control interface codes"
///
/// This page lists the class, subclass and protocol codes that a USB Audio
/// Control interface descriptor should display.
/// 
/// !Codes
/// - AUDControlInterfaceDescriptor_CLASS
/// - AUDControlInterfaceDescriptor_SUBCLASS
/// - AUDControlInterfaceDescriptor_PROTOCOL

/// Class code for an audio control interface.
#define AUDControlInterfaceDescriptor_CLASS             0x01

/// Subclass code for an audio control interface.
#define AUDControlInterfaceDescriptor_SUBCLASS          0x01

/// Protocol code for an audio control interface.
#define AUDControlInterfaceDescriptor_PROTOCOL          0x00
//------------------------------------------------------------------------------

#endif //#ifndef AUDCONTROLINTERFACEDESCRIPTOR_H

