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

 Methods and definitions for manipulating a HID descriptor.
*/

#ifndef HIDDESCRIPTOR_H
#define HIDDESCRIPTOR_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "HID Release Numbers"
/// ...
///
/// !Numbers
/// - HIDDescriptor_HID1_11

/// Identifies version 1.11 of the HID specification.
#define HIDDescriptor_HID1_11           0x0111
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
/// Identifies the length of type of subordinate descriptors of a HID
/// device. This particular type only supports one subordinate descriptor.
//------------------------------------------------------------------------------
typedef struct {

    /// Size of descriptor in bytes.
    unsigned char bLength;
    /// Descriptor type (HIDGenericDescriptor_HID).
    unsigned char bDescriptorType;
    /// HID class specification release number in BCD format.
    unsigned short bcdHID;
    /// Country code of the device if it is localized.
    unsigned char bCountryCode; 
    /// Number of subordinate descriptors.
    unsigned char bNumDescriptors;
    /// Type of the first subordinate descriptor.
    unsigned char bDescriptorType0;
    /// Size in bytes of the first subordinate descriptor.
    unsigned short wDescriptorLength0;

} __attribute__ ((packed)) HIDDescriptor; // GCC

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

#endif //#ifndef HIDDESCRIPTOR_H

