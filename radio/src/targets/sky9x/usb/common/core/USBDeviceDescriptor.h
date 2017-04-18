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

    Class for manipulating USB device descriptors.

 !!!Usage

    - Declare a USBDeviceDescriptor instance as the device descriptor of a
      USB device.
*/

#ifndef USBDEVICEDESCRIPTOR_H
#define USBDEVICEDESCRIPTOR_H

#include "board_lowlevel.h"
//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "USB release numbers"
///
/// This page lists the codes of USB release numbers.
///
/// !Code
/// - USBDeviceDescriptor_USB2_00

/// The device supports USB 2.00.
#define USBDeviceDescriptor_USB2_00         0x0200
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
/// USB standard device descriptor structure.
//------------------------------------------------------------------------------
typedef struct {

   /// Size of this descriptor in bytes.
   unsigned char bLength;
   /// Descriptor type (USBGenericDescriptor_DEVICE).
   unsigned char bDescriptorType;
   /// USB specification release number in BCD format.
   unsigned short bcdUSB;
   /// Device class code.
   unsigned char bDeviceClass;
   /// Device subclass code.
   unsigned char bDeviceSubClass;
   /// Device protocol code.
   unsigned char bDeviceProtocol;
   /// Maximum packet size of endpoint 0 (in bytes).
   unsigned char bMaxPacketSize0;
   /// Vendor ID.
   unsigned short idVendor;
   /// Product ID.
   unsigned short idProduct;
   /// Device release number in BCD format.
   unsigned short bcdDevice;
   /// Index of the manufacturer string descriptor.
   unsigned char iManufacturer;
   /// Index of the product string descriptor.
   unsigned char iProduct;
   /// Index of the serial number string descriptor.
   unsigned char iSerialNumber;
   /// Number of possible configurations for the device.
   unsigned char bNumConfigurations;

} __attribute__ ((packed)) USBDeviceDescriptor; // GCC

#if defined(CHIP_USB_OTGHS)
typedef struct {

   /// Size of this descriptor in bytes.
   unsigned char bLength;
   /// Descriptor type (USBGenericDescriptor_OTG).
   unsigned char bDescriptorType;
   /// Attribute Fields D7�2: Reserved D1: HNP support D0: SRP support
   unsigned char bmAttributes;

} __attribute__ ((packed)) USBOtgDescriptor; // GCC
#endif

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

#endif //#ifndef USBDEVICEDESCRIPTOR_H

