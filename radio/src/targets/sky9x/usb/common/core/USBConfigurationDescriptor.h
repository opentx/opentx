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
 
    Definitions and methods for USB configuration descriptor structures
    described by the USB specification.

 !!!Usage

    -# Declare USBConfigurationDescriptor instance as a part
       of the configuration descriptors of a USB device.
    -# To get useful information (field values) from the defined USB device
       configuration descriptor, use
       - USBConfigurationDescriptor_GetTotalLength
       - USBConfigurationDescriptor_GetNumInterfaces
       - USBConfigurationDescriptor_IsSelfPowered
    -# To pase the defined USB device configuration descriptor, use
       - USBConfigurationDescriptor_Parse
*/

#ifndef USBCONFIGURATIONDESCRIPTOR_H
#define USBCONFIGURATIONDESCRIPTOR_H

//-----------------------------------------------------------------------------
//         Headers
//-----------------------------------------------------------------------------

#include "../../../usb/common/core/USBEndpointDescriptor.h"
#include "../../../usb/common/core/USBGenericDescriptor.h"
#include "../../../usb/common/core/USBInterfaceDescriptor.h"

//-----------------------------------------------------------------------------
//         Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// \page "USB device Attributes"
///
/// This page lists the codes of the usb attributes.
///
/// !Attributes
/// - USBConfigurationDescriptor_BUSPOWERED_NORWAKEUP
/// - USBConfigurationDescriptor_SELFPOWERED_NORWAKEUP
/// - USBConfigurationDescriptor_BUSPOWERED_RWAKEUP
/// - USBConfigurationDescriptor_SELFPOWERED_RWAKEUP
/// - USBConfigurationDescriptor_POWER

/// Device is bus-powered and not support remote wake-up.
#define USBConfigurationDescriptor_BUSPOWERED_NORWAKEUP  0x80
/// Device is self-powered and not support remote wake-up.
#define USBConfigurationDescriptor_SELFPOWERED_NORWAKEUP 0xC0
/// Device is bus-powered  and supports remote wake-up.
#define USBConfigurationDescriptor_BUSPOWERED_RWAKEUP    0xA0
/// Device is self-powered and supports remote wake-up.
#define USBConfigurationDescriptor_SELFPOWERED_RWAKEUP   0xE0

/// Calculates the value of the power consumption field given the value in mA.
/// \param power The power consumption value in mA
/// \return The value that should be set to the field in descriptor
#define USBConfigurationDescriptor_POWER(power)     (power / 2)
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//         Types
//-----------------------------------------------------------------------------

#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

//-----------------------------------------------------------------------------
/// USB standard configuration descriptor structure.
//-----------------------------------------------------------------------------
typedef struct {

   /// Size of the descriptor in bytes.
   unsigned char bLength;
   /// Descriptor type (USBDESC_CONFIGURATION of "USB Descriptor types").
   unsigned char bDescriptorType;
   /// Length of all descriptors returned along with this configuration
   /// descriptor.
   unsigned short wTotalLength;
   /// Number of interfaces in this configuration.
   unsigned char bNumInterfaces;
   /// Value for selecting this configuration.
   unsigned char bConfigurationValue; 
   /// Index of the configuration string descriptor.
   unsigned char iConfiguration;
   /// Configuration characteristics.
   unsigned char bmAttributes;
   /// Maximum power consumption of the device when in this configuration.
   unsigned char bMaxPower;           
                                       
} __attribute__ ((packed)) USBConfigurationDescriptor; // GCC

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

//-----------------------------------------------------------------------------
//         Exported functions
//-----------------------------------------------------------------------------

extern unsigned int USBConfigurationDescriptor_GetTotalLength(
    const USBConfigurationDescriptor *configuration);

extern unsigned char USBConfigurationDescriptor_GetNumInterfaces(
    const USBConfigurationDescriptor *configuration);

extern unsigned char USBConfigurationDescriptor_IsSelfPowered(
    const USBConfigurationDescriptor *configuration);

extern void USBConfigurationDescriptor_Parse(
    const USBConfigurationDescriptor *configuration,
    USBInterfaceDescriptor **interfaces,
    USBEndpointDescriptor **endpoints,
    USBGenericDescriptor **others);

#endif //#ifndef USBCONFIGURATIONDESCRIPTOR_H

