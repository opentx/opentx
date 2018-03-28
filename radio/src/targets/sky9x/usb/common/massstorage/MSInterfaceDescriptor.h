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
/// !Purpose
/// 
/// Definition of several constants used when manipulating Mass Storage interface
/// descriptors.
///
/// !Usage
/// 
/// - For a USB device:
///     -# When declaring an interface descriptor for a Mass Storage device, use
///        the class, subclass and protocol codes defined here (see
///        "MS interface codes").
//------------------------------------------------------------------------------

#ifndef MSINTERFACEDESCRIPTOR_H
#define MSINTERFACEDESCRIPTOR_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "MS interface codes"
/// This page lists the available class, subclass & protocol codes for a Mass
/// Storage interface.
/// 
/// !Codes
/// 
/// - MSInterfaceDescriptor_CLASS
/// - MSInterfaceDescriptor_SCSI
/// - MSInterfaceDescriptor_BULKONLY

/// Class code for a Mass Storage interface.
#define MSInterfaceDescriptor_CLASS         0x08

/// Subclass code for a Mass Storage interface using the SCSI protocol.
#define MSInterfaceDescriptor_SCSI          0x06

/// Protocol code for a Mass Storage interface using Bulk-Only Transport.
#define MSInterfaceDescriptor_BULKONLY      0x50
//------------------------------------------------------------------------------


#endif //#ifndef MSINTERFACEDESCRIPTOR_H

