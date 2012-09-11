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

 Definitions of constants used when declaring a CDC data class interface
 descriptor.
*/

#ifndef CDCDATAINTERFACEDESCRIPTOR_H
#define CDCDATAINTERFACEDESCRIPTOR_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "CDC Data Interface Values"
/// This page lists the values for CDC Data Interface Descriptor.
///
/// !Values
/// - CDCDataInterfaceDescriptor_CLASS
/// - CDCDataInterfaceDescriptor_SUBCLASS
/// - CDCDataInterfaceDescriptor_NOPROTOCOL

/// Interface class code for a data class interface.
#define CDCDataInterfaceDescriptor_CLASS        0x0A
/// Interface subclass code for a data class interface.
#define CDCDataInterfaceDescriptor_SUBCLASS     0x00
/// Protocol code for a data class interface which does not implement any
/// particular protocol.
#define CDCDataInterfaceDescriptor_NOPROTOCOL   0x00
//------------------------------------------------------------------------------

#endif //#ifndef CDCDATAINTERFACEDESCRIPTOR_H

