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

#include "../../../usb/device/massstorage/MSDDriverDescriptors.h"

#include "../../../usb/common/core/USBStringDescriptor.h"
#include "../../../usb/common/massstorage/MSDeviceDescriptor.h"
#include "../../../usb/common/massstorage/MSInterfaceDescriptor.h"
#include "board_lowlevel.h"
#include "../../../usb/common/core/USBConfigurationDescriptor.h"
#include "../../../usb/common/core/USBDeviceDescriptor.h"
#include "../../../usb/common/core/USBEndpointDescriptor.h"
#include "../../../usb/common/core/USBGenericDescriptor.h"
#include "../../../usb/common/core/USBInterfaceDescriptor.h"

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
#define MSDDriverDescriptors_PRODUCTID      0x6129
/// Device release number for the Mass Storage device driver.
#define MSDDriverDescriptors_RELEASE        0x0100
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
typedef struct {

    /// Standard configuration descriptor.
    USBConfigurationDescriptor configuration;
#if defined(CHIP_USB_OTGHS)
    // OTG descriptor
    USBOtgDescriptor otgDescriptor;
#endif
    /// Mass storage interface descriptor.
    USBInterfaceDescriptor interface;
    /// Bulk-out endpoint descriptor.
    USBEndpointDescriptor bulkOut;
    /// Bulk-in endpoint descriptor.
    USBEndpointDescriptor bulkIn;

} __attribute__ ((packed)) MSDConfigurationDescriptors;

//------------------------------------------------------------------------------
//         Local variables
//------------------------------------------------------------------------------

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

#if defined(CHIP_USB_UDPHS) || defined(CHIP_USB_OTGHS)
/// Device qualifier descriptor.
static const USBDeviceQualifierDescriptor qualifierDescriptor = {

    sizeof(USBDeviceQualifierDescriptor),
    USBGenericDescriptor_DEVICEQUALIFIER,
    USBDeviceDescriptor_USB2_00,
    MSDeviceDescriptor_CLASS,            
    MSDeviceDescriptor_SUBCLASS,                           
    MSDeviceDescriptor_PROTOCOL,                           
    CHIP_USB_ENDPOINTS_MAXPACKETSIZE(0),    
    1, // Device has one possible configuration.
    0x00
};
#endif

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
#if defined(CHIP_USB_OTGHS)
    // OTG descriptor
    {
        sizeof(USBOtgDescriptor),
        USBGenericDescriptor_OTG,
        USBOTGDescriptor_HNP_SRP
    },
#endif
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

#if defined(CHIP_USB_UDPHS) || defined(CHIP_USB_OTGHS)
/// Full-speed other speed configuration descriptor.
static const MSDConfigurationDescriptors otherSpeedDescriptorsFS = {

    // Standard configuration descriptor.
    {
        sizeof(USBConfigurationDescriptor),
        USBGenericDescriptor_OTHERSPEEDCONFIGURATION,          
        sizeof(MSDConfigurationDescriptors),
        1, // Configuration has one interface.
        1, // This is configuration #1.
        0, // No string descriptor for configuration.
        BOARD_USB_BMATTRIBUTES,
        USBConfigurationDescriptor_POWER(100)
    },
#if defined(CHIP_USB_OTGHS)
    // OTG descriptor
    {
        sizeof(USBOtgDescriptor),
        USBGenericDescriptor_OTG,
        USBOTGDescriptor_HNP_SRP
    },
#endif
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
            USBEndpointDescriptor_MAXBULKSIZE_HS),
        0 // No string descriptor for endpoint.
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
            USBEndpointDescriptor_MAXBULKSIZE_HS),
        0 // No string descriptor for endpoint.
    }
};

/// High-speed configuration descriptor.
static const MSDConfigurationDescriptors configurationDescriptorsHS = {

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
#if defined(CHIP_USB_OTGHS)
    // OTG descriptor
    {
        sizeof(USBOtgDescriptor),
        USBGenericDescriptor_OTG,
        USBOTGDescriptor_HNP_SRP
    },
#endif
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
            USBEndpointDescriptor_MAXBULKSIZE_HS),
        0 // No string descriptor for endpoint.
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
            USBEndpointDescriptor_MAXBULKSIZE_HS),
        0 // No string descriptor for endpoint.
    }
};

/// High-speed other speed configuration descriptor.
static const MSDConfigurationDescriptors otherSpeedDescriptorsHS = {

    // Standard configuration descriptor.
    {
        sizeof(USBConfigurationDescriptor),
        USBGenericDescriptor_OTHERSPEEDCONFIGURATION,          
        sizeof(MSDConfigurationDescriptors),
        1, // Configuration has one interface.
        1, // This is configuration #1.
        0, // No string descriptor for configuration.
        BOARD_USB_BMATTRIBUTES,
        USBConfigurationDescriptor_POWER(100)
    },
#if defined(CHIP_USB_OTGHS)
    // OTG descriptor
    {
        sizeof(USBOtgDescriptor),
        USBGenericDescriptor_OTG,
        USBOTGDescriptor_HNP_SRP
    },
#endif
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
        0 // No string descriptor for endpoint.
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
        0 // No string descriptor for endpoint.
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
#if defined(CHIP_USB_UDPHS) || defined(CHIP_USB_OTGHS)
    &qualifierDescriptor,
    (USBConfigurationDescriptor *) &otherSpeedDescriptorsFS,
    &deviceDescriptor,
    (USBConfigurationDescriptor *) &configurationDescriptorsHS,
    &qualifierDescriptor,
    (USBConfigurationDescriptor *) &otherSpeedDescriptorsHS,
#else
    0, // No full-speed device qualifier descriptor
    0, // No full-speed other speed configuration
    0, // No high-speed device descriptor
    0, // No high-speed configuration descriptor
    0, // No high-speed device qualifier descriptor
    0, // No high-speed other speed configuration descriptor
#endif
    stringDescriptors,
    4 // Four string descriptors in array
};

