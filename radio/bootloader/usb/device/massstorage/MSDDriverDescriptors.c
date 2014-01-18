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
//         Headers
//------------------------------------------------------------------------------

#include "MSDDriverDescriptors.h"
#include <board.h>
#include <usb/common/core/USBGenericDescriptor.h>
#include <usb/common/core/USBDeviceDescriptor.h>
#include <usb/common/core/USBConfigurationDescriptor.h>
#include <usb/common/core/USBInterfaceDescriptor.h>
#include <usb/common/core/USBEndpointDescriptor.h>
#include <usb/common/core/USBStringDescriptor.h>
#include <usb/common/massstorage/MSDeviceDescriptor.h>
#include <usb/common/massstorage/MSInterfaceDescriptor.h>

//------------------------------------------------------------------------------
//         Internal definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "MSD Device Descriptor IDs"
/// ...
/// !IDs
/// - MSDDriverDescriptors_VENDORID
/// - MSDDriverDescriptors_PRODUCTID
/// - MSDDriverDescriptors_RELEASE

/// Vendor ID for the Mass Storage device driver.
#define MSDDriverDescriptors_VENDORID       0x03EB
/// Product ID for the Mass Storage device driver.
#ifdef HID
#define HIDMSDDDriverDescriptors_PRODUCTID       0x6135
#else
#define MSDDriverDescriptors_PRODUCTID      0x6129
#endif
/// Device release number for the Mass Storage device driver.
#define MSDDriverDescriptors_RELEASE        0x0100

#ifdef HID
/** Address of the HID interrupt IN endpoint. */
#define HIDD_Descriptors_INTERRUPTIN                1
/** Address of the HID interrupt OUT endpoint. */
#define HIDD_Descriptors_INTERRUPTOUT               2
/** Address of the Mass Storage bulk-out endpoint. */
#define MSDD_Descriptors_BULKOUT                    4
/** Address of the Mass Storage bulk-in endpoint. */
#define MSDD_Descriptors_BULKIN                     5
#endif

//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Macros
//------------------------------------------------------------------------------

/// Returns the minimum between two values.
#define MIN(a, b)       ((a < b) ? a : b)

//------------------------------------------------------------------------------
//         Internal types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// List of configuration descriptors used by a Mass Storage device driver.
//------------------------------------------------------------------------------
#ifndef HID
typedef struct {

    /// Standard configuration descriptor.
    USBConfigurationDescriptor configuration;
    /// Mass storage interface descriptor.
    USBInterfaceDescriptor interface;
    /// Bulk-out endpoint descriptor.
    USBEndpointDescriptor bulkOut;
    /// Bulk-in endpoint descriptor.
    USBEndpointDescriptor bulkIn;

} __attribute__ ((packed)) MSDConfigurationDescriptors;
#endif

//------------------------------------------------------------------------------
//         Local variables
//------------------------------------------------------------------------------

#ifdef HID
/** Standard USB device descriptor for the composite device driver */
static const USBDeviceDescriptor deviceDescriptor =
{

    sizeof(USBDeviceDescriptor),
    USBGenericDescriptor_DEVICE,
    USBDeviceDescriptor_USB2_00,
    0x00,
    0x00,
    0x00,
    CHIP_USB_ENDPOINTS_MAXPACKETSIZE(0),
    HIDMSDDDriverDescriptors_VENDORID,
    HIDMSDDDriverDescriptors_PRODUCTID,
    HIDMSDDDriverDescriptors_RELEASE,
//    0, /* No string descriptor for manufacturer */
//    1, /* Index of product string descriptor is #1 */
//    0, /* No string descriptor for serial number */
    1, // Manufacturer string descriptor index.
    2, // Product string descriptor index.
    3, // Serial number string descriptor index.
    1 /* Device has 1 possible configuration */
};
#else
/// Mass storage driver device descriptor.
static const USBDeviceDescriptor deviceDescriptor = {

    sizeof(USBDeviceDescriptor),
    USBGenericDescriptor_DEVICE,          
    USBDeviceDescriptor_USB2_00,                        
    MSDeviceDescriptor_CLASS,                           
    MSDeviceDescriptor_SUBCLASS,                           
    MSDeviceDescriptor_PROTOCOL,                           
    CHIP_USB_ENDPOINTS_MAXPACKETSIZE(0),    
    MSDDriverDescriptors_VENDORID,               
    MSDDriverDescriptors_PRODUCTID,                 
    MSDDriverDescriptors_RELEASE,                         
    1, // Manufacturer string descriptor index.
    2, // Product string descriptor index.
    3, // Serial number string descriptor index.
    1  // Device has one possible configuration.
};
#endif

#ifdef HID
/** USB FS configuration descriptors for the composite device driver */
static const HidMsdDriverConfigurationDescriptors configurationDescriptorsFS =
{

    /* Standard configuration descriptor */
    {
        sizeof(USBConfigurationDescriptor),
        USBGenericDescriptor_CONFIGURATION,
        sizeof(HidMsdDriverConfigurationDescriptors),
        HIDMSDDriverDescriptors_NUMINTERFACE,
        1, /* This is configuration #1 */
        0, /* No string descriptor for this configuration */
        BOARD_USB_BMATTRIBUTES,
        USBConfigurationDescriptor_POWER(100)
    },

    /* Interface descriptor */
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        HIDMSDDriverDescriptors_HID_INTERFACE,
        0, /* This is alternate setting #0 */
        2, /* Two endpoints used */
        HIDInterfaceDescriptor_CLASS,
        HIDInterfaceDescriptor_SUBCLASS_NONE,
        HIDInterfaceDescriptor_PROTOCOL_NONE,
        0  /* No associated string descriptor */
    },
    /* HID descriptor */
    {
        sizeof(HIDDescriptor1),
        HIDGenericDescriptor_HID,
        HIDDescriptor_HID1_11,
        0, /* Device is not localized, no country code */
        1, /* One HID-specific descriptor (apart from this one) */
        HIDGenericDescriptor_REPORT,
        HIDDKeyboard_REPORTDESCRIPTORSIZE
    },
    /* Interrupt IN endpoint descriptor */
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(
            USBEndpointDescriptor_IN,
            HIDD_Descriptors_INTERRUPTIN),
        USBEndpointDescriptor_INTERRUPT,
        sizeof(HIDDKeyboardInputReport),
        HIDDKeyboardDescriptors_INTERRUPTIN_POLLING_FS
    },
    /* Interrupt OUT endpoint descriptor */
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(
            USBEndpointDescriptor_OUT,
            HIDD_Descriptors_INTERRUPTOUT),
        USBEndpointDescriptor_INTERRUPT,
        sizeof(HIDDKeyboardOutputReport),
        HIDDKeyboardDescriptors_INTERRUPTIN_POLLING_FS
    },

    /* Mass Storage interface descriptor. */
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        HIDMSDDriverDescriptors_MSD_INTERFACE,
        0, /* This is alternate setting #0. */
        2, /* Interface uses two endpoints. */
        MSInterfaceDescriptor_CLASS,            
        MSInterfaceDescriptor_SCSI,                 
        MSInterfaceDescriptor_BULKONLY,            
        0 /* No string descriptor for interface. */
    },
    /* Bulk-OUT endpoint descriptor */
    {
        sizeof(USBEndpointDescriptor), 
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(
            USBEndpointDescriptor_OUT,
            MSDD_Descriptors_BULKOUT),	//MSDDriverDescriptors_BULKOUT
        USBEndpointDescriptor_BULK,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(MSDD_Descriptors_BULKOUT),//MSDDriverDescriptors_BULKOUT
            USBEndpointDescriptor_MAXBULKSIZE_FS),
        0 /* No string descriptor for endpoint. */
    },
    /* Bulk-IN endpoint descriptor */
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(
            USBEndpointDescriptor_IN,
            MSDD_Descriptors_BULKIN),	//MSDDriverDescriptors_BULKIN
        USBEndpointDescriptor_BULK,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(MSDD_Descriptors_BULKIN),//MSDDriverDescriptors_BULKIN
            USBEndpointDescriptor_MAXBULKSIZE_FS),
        0 /* No string descriptor for endpoint. */
    }

};

#else
/// Full-speed configuration descriptor.
static const MSDConfigurationDescriptors configurationDescriptorsFS = {

    // Standard configuration descriptor.
    {
        sizeof(USBConfigurationDescriptor),
        USBGenericDescriptor_CONFIGURATION,          
        sizeof(MSDConfigurationDescriptors),
        1, // Configuration has one interface.
        1, // This is configuration #1.
        0, // No string descriptor for configuration.
        BOARD_USB_BMATTRIBUTES,
        USBConfigurationDescriptor_POWER(100)
    },
    // Mass Storage interface descriptor.
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        0, // This is interface #0.
        0, // This is alternate setting #0.
        2, // Interface uses two endpoints.
        MSInterfaceDescriptor_CLASS,            
        MSInterfaceDescriptor_SCSI,                 
        MSInterfaceDescriptor_BULKONLY,            
        0 // No string descriptor for interface.
    },
    // Bulk-OUT endpoint descriptor
    {
        sizeof(USBEndpointDescriptor), 
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(
            USBEndpointDescriptor_OUT,
            MSDDriverDescriptors_BULKOUT),
        USBEndpointDescriptor_BULK,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(MSDDriverDescriptors_BULKOUT),
            USBEndpointDescriptor_MAXBULKSIZE_FS),
        0 // Must be 0 for full-speed Bulk endpoints.
    },
    // Bulk-IN endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(
            USBEndpointDescriptor_IN,
            MSDDriverDescriptors_BULKIN),
        USBEndpointDescriptor_BULK,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(MSDDriverDescriptors_BULKIN),
            USBEndpointDescriptor_MAXBULKSIZE_FS),
        0 // Must be 0 for full-speed Bulk endpoints.
    }
};
#endif

/// Language ID string descriptor.
static const unsigned char languageIdDescriptor[] = {

    USBStringDescriptor_LENGTH(1),
    USBGenericDescriptor_STRING,
    USBStringDescriptor_ENGLISH_US
};

/// Manufacturer string descriptor.
static const unsigned char manufacturerDescriptor[] = {

    USBStringDescriptor_LENGTH(7),
    USBGenericDescriptor_STRING,
    USBStringDescriptor_UNICODE('9'),
    USBStringDescriptor_UNICODE('X'),
    USBStringDescriptor_UNICODE(' '),
    USBStringDescriptor_UNICODE('T'),
    USBStringDescriptor_UNICODE('E'),
    USBStringDescriptor_UNICODE('A'),
    USBStringDescriptor_UNICODE('M'),
};

/// Product string descriptor.
static const unsigned char productDescriptor[] = {

    USBStringDescriptor_LENGTH(5),
    USBGenericDescriptor_STRING,
    USBStringDescriptor_UNICODE('S'),
    USBStringDescriptor_UNICODE('K'),
    USBStringDescriptor_UNICODE('Y'),
    USBStringDescriptor_UNICODE('9'),
    USBStringDescriptor_UNICODE('X'),
};

/// Serial number string descriptor. The serial number must be at least 12
/// characters long and made up of only letters & numbers to be compliant with
/// the MSD specification.
static const unsigned char serialNumberDescriptor[] = {

    USBStringDescriptor_LENGTH(12),
    USBGenericDescriptor_STRING,
    USBStringDescriptor_UNICODE('0'),
    USBStringDescriptor_UNICODE('1'),
    USBStringDescriptor_UNICODE('2'),
    USBStringDescriptor_UNICODE('3'),
    USBStringDescriptor_UNICODE('4'),
    USBStringDescriptor_UNICODE('5'),
    USBStringDescriptor_UNICODE('6'),
    USBStringDescriptor_UNICODE('7'),
    USBStringDescriptor_UNICODE('8'),
    USBStringDescriptor_UNICODE('9'),
    USBStringDescriptor_UNICODE('A'),
    USBStringDescriptor_UNICODE('B')
};

/// List of all string descriptors used.
static const unsigned char *stringDescriptors[] = {

    languageIdDescriptor,
    manufacturerDescriptor,
    productDescriptor,
    serialNumberDescriptor
};

//------------------------------------------------------------------------------
//         Global variables
//------------------------------------------------------------------------------

/// List of the standard descriptors used by the Mass Storage driver.
const USBDDriverDescriptors msdDriverDescriptors = {

    &deviceDescriptor,
    (USBConfigurationDescriptor *) &configurationDescriptorsFS,
    0, // No full-speed device qualifier descriptor
    0, // No full-speed other speed configuration
    0, // No high-speed device descriptor
    0, // No high-speed configuration descriptor
    0, // No high-speed device qualifier descriptor
    0, // No high-speed other speed configuration descriptor
    stringDescriptors,
    4 // Four string descriptors in array
};

