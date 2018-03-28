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
/// Declaration of constants for using Device Descriptors with a Mass Storage
/// driver.
/// 
/// !Usage
/// 
/// - For a USB device:
///     -# When declaring a USBDeviceDescriptor instance, use the Mass Storage
///        codes defined in this file (see "MS device codes").
//------------------------------------------------------------------------------

#ifndef MSDEVICEDESCRIPTOR_H
#define MSDEVICEDESCRIPTOR_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "MS device codes"
/// This page lists the class, subclass & protocol codes used by a device with
/// a Mass Storage driver.
/// 
/// !Codes
/// 
/// - MSDeviceDescriptor_CLASS
/// - MSDeviceDescriptor_SUBCLASS
/// - MSDeviceDescriptor_PROTOCOL

/// Class code for a Mass Storage device.
#define MSDeviceDescriptor_CLASS        0

/// Subclass code for a Mass Storage device.
#define MSDeviceDescriptor_SUBCLASS     0

/// Protocol code for a Mass Storage device.
#define MSDeviceDescriptor_PROTOCOL     0
//------------------------------------------------------------------------------

#endif //#ifndef MSDEVICEDESCRIPTOR_H

