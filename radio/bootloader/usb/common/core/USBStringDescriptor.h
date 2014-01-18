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
/// Definition of a class for manipulating String descriptors.
//------------------------------------------------------------------------------

#ifndef USBSTRINGDESCRIPTOR_H
#define USBSTRINGDESCRIPTOR_H

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB String Descriptor definitions"
///
/// This page lists the codes and macros for USB string descriptor definition.
///
/// !Language IDs
/// - USBStringDescriptor_ENGLISH_US
///
/// !String Descriptor Length
/// - USBStringDescriptor_LENGTH
///
/// !ASCII to UNICODE convertion
/// - USBStringDescriptor_UNICODE

/// Language ID for US english.
#define USBStringDescriptor_ENGLISH_US          0x09, 0x04

/// Calculates the length of a string descriptor given the number of ascii
/// characters/language IDs in it.
/// \param length The ascii format string length.
/// \return The actual data length in bytes.
#define USBStringDescriptor_LENGTH(length)      ((length) * 2 + 2)
/// Converts an ascii character to its unicode representation.
/// \param ascii The ASCII character to convert
/// \return A 2-byte-array for the UNICODE based on given ASCII
#define USBStringDescriptor_UNICODE(ascii)      (ascii), 0
//------------------------------------------------------------------------------

#endif //#ifndef USBSTRINGDESCRIPTOR_H

