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

 Definition of several constants for declaring CDC descriptors.
*/

#ifndef CDCGENERICDESCRIPTOR_H
#define CDCGENERICDESCRIPTOR_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "CDC Specification Release Numbers"
/// This page list the CDC Spec. Release Numbers.
///
/// !Numbers
/// - CDCGenericDescriptor_CDC1_10

/// Identify CDC specification version 1.10.
#define CDCGenericDescriptor_CDC1_10                            0x0110
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "CDC Descriptro Types"
/// This page lists CDC descriptor types.
///
/// !Types
/// - CDCGenericDescriptor_INTERFACE
/// - CDCGenericDescriptor_ENDPOINT

///Indicates that a CDC descriptor applies to an interface.
#define CDCGenericDescriptor_INTERFACE                          0x24
/// Indicates that a CDC descriptor applies to an endpoint.
#define CDCGenericDescriptor_ENDPOINT                           0x25
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "CDC Descriptor Subtypes"
/// This page lists CDC descriptor sub types
///
/// !Types
/// - CDCGenericDescriptor_HEADER
/// - CDCGenericDescriptor_CALLMANAGEMENT
/// - CDCGenericDescriptor_ABSTRACTCONTROLMANAGEMENT
/// - CDCGenericDescriptor_UNION

/// Header functional descriptor subtype.
#define CDCGenericDescriptor_HEADER                             0x00
/// Call management functional descriptor subtype.
#define CDCGenericDescriptor_CALLMANAGEMENT                     0x01
/// Abstract control management descriptor subtype.
#define CDCGenericDescriptor_ABSTRACTCONTROLMANAGEMENT          0x02
/// Union descriptor subtype.
#define CDCGenericDescriptor_UNION                              0x06
//------------------------------------------------------------------------------

#endif //#ifndef CDCGENERICDESCRIPTOR_H

