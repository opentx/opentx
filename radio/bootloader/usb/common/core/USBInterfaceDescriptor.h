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

    Definition of a class for manipulating USB interface descriptors.

 !!!Usage

    - Declare USBInterfaceDescriptor instance as a part of the configuration
      descriptors of a USB device.

*/

#ifndef USBINTERFACEDESCRIPTOR_H
#define USBINTERFACEDESCRIPTOR_H

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
/// USB standard interface descriptor structure.
//------------------------------------------------------------------------------
typedef struct {

   /// Size of the descriptor in bytes.
   unsigned char bLength;
   /// Descriptor type (USBGenericDescriptor_INTERFACE).
   unsigned char bDescriptorType;
   /// Number of the interface in its configuration.
   unsigned char bInterfaceNumber;
   /// Value to select this alternate interface setting.
   unsigned char bAlternateSetting;
   /// Number of endpoints used by the inteface (excluding endpoint 0).
   unsigned char bNumEndpoints;
   /// Interface class code.
   unsigned char bInterfaceClass;
   /// Interface subclass code.
   unsigned char bInterfaceSubClass;
   /// Interface protocol code.
   unsigned char bInterfaceProtocol;
   /// Index of the interface string descriptor.
   unsigned char iInterface;
                                    
} __attribute__ ((packed)) USBInterfaceDescriptor; // GCC

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

#endif //#ifndef USBINTERFACEDESCRIPTOR_H

