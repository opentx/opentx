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
//      Headers
//------------------------------------------------------------------------------

#include <board.h>

//- Composite
#include "CDCHIDDDriver.h"
#include "CDCHIDDDriverDescriptors.h"

//- USB Generic
#include <usb/common/core/USBGenericDescriptor.h>
#include <usb/common/core/USBConfigurationDescriptor.h>
#include <usb/common/core/USBInterfaceAssociationDescriptor.h>
#include <usb/common/core/USBEndpointDescriptor.h>
#include <usb/common/core/USBStringDescriptor.h>
#include <usb/common/core/USBGenericRequest.h>

//- CDC
#include <usb/common/cdc/CDCGenericDescriptor.h>
#include <usb/common/cdc/CDCDeviceDescriptor.h>
#include <usb/common/cdc/CDCCommunicationInterfaceDescriptor.h>
#include <usb/common/cdc/CDCDataInterfaceDescriptor.h>
#include <usb/common/cdc/CDCHeaderDescriptor.h>
#include <usb/common/cdc/CDCCallManagementDescriptor.h>
#include <usb/common/cdc/CDCAbstractControlManagementDescriptor.h>
#include <usb/common/cdc/CDCUnionDescriptor.h>
#include "CDCDFunctionDriverDescriptors.h"

//- HID
#include <usb/device/hid-keyboard/HIDDKeyboardInputReport.h>
#include <usb/device/hid-keyboard/HIDDKeyboardOutputReport.h>
#include <usb/common/hid/HIDGenericDescriptor.h>
#include <usb/common/hid/HIDDeviceDescriptor.h>
#include <usb/common/hid/HIDInterfaceDescriptor.h>
#include <usb/common/hid/HIDDescriptor.h>
#include <usb/common/hid/HIDReport.h>
#include <usb/common/hid/HIDGenericDesktop.h>
#include <usb/common/hid/HIDLeds.h>
#include <usb/common/hid/HIDKeypad.h>
#include "HIDDFunctionDriverDescriptors.h"

//-----------------------------------------------------------------------------
//         Definitions
//-----------------------------------------------------------------------------

/// Device product ID.
#define CDCHIDDDriverDescriptors_PRODUCTID       0x6130

/// Device vendor ID (Atmel).
#define CDCHIDDDriverDescriptors_VENDORID        0x03EB

/// Device release number.
#define CDCHIDDDriverDescriptors_RELEASE         0x0003

//-----------------------------------------------------------------------------
//         Macros
//-----------------------------------------------------------------------------

/// Returns the minimum between two values.
#define MIN(a, b)       ((a < b) ? a : b)

//-----------------------------------------------------------------------------
//         Internal structures
//-----------------------------------------------------------------------------

#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

//-----------------------------------------------------------------------------
/// Configuration descriptor list for a device implementing a composite driver.
//-----------------------------------------------------------------------------
typedef struct {

    /// Standard configuration descriptor.
    USBConfigurationDescriptor configuration;

    /// --- CDC 0
    /// IAD 0
    USBInterfaceAssociationDescriptor cdcIAD0;
    /// Communication interface descriptor
    USBInterfaceDescriptor cdcCommunication0;
    /// CDC header functional descriptor.
    CDCHeaderDescriptor cdcHeader0;
    /// CDC call management functional descriptor.
    CDCCallManagementDescriptor cdcCallManagement0;
    /// CDC abstract control management functional descriptor.
    CDCAbstractControlManagementDescriptor cdcAbstractControlManagement0;
    /// CDC union functional descriptor (with one slave interface).
    CDCUnionDescriptor cdcUnion0;
    /// Notification endpoint descriptor.
    USBEndpointDescriptor cdcNotification0;
    /// Data interface descriptor.
    USBInterfaceDescriptor cdcData0;
    /// Data OUT endpoint descriptor.
    USBEndpointDescriptor cdcDataOut0;
    /// Data IN endpoint descriptor.
    USBEndpointDescriptor cdcDataIn0;

    /// --- HID
    USBInterfaceDescriptor hidInterface;
    HIDDescriptor hid;
    USBEndpointDescriptor hidInterruptIn;
    USBEndpointDescriptor hidInterruptOut;

} __attribute__ ((packed)) CdcHidDriverConfigurationDescriptors;

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
//         Exported variables
//------------------------------------------------------------------------------

/// Standard USB device descriptor for the composite device driver
static const USBDeviceDescriptor deviceDescriptor = {

    sizeof(USBDeviceDescriptor),
    USBGenericDescriptor_DEVICE,
    USBDeviceDescriptor_USB2_00,
    0xEF,// MI
    0x02,//
    0x01,//
    CHIP_USB_ENDPOINTS_MAXPACKETSIZE(0),
    CDCHIDDDriverDescriptors_VENDORID,
    CDCHIDDDriverDescriptors_PRODUCTID,
    CDCHIDDDriverDescriptors_RELEASE,
    0, // No string descriptor for manufacturer
    1, // Index of product string descriptor is #1
    0, // No string descriptor for serial number
    1 // Device has 1 possible configuration
};

#if defined(CHIP_USB_UDPHS) || defined(CHIP_USB_OTGHS)

/// USB device qualifier descriptor.
static const USBDeviceQualifierDescriptor qualifierDescriptor = {

    sizeof(USBDeviceQualifierDescriptor),
    USBGenericDescriptor_DEVICEQUALIFIER,
    USBDeviceDescriptor_USB2_00,
    0xEF,// MI
    0x02,//
    0x01,//
    CHIP_USB_ENDPOINTS_MAXPACKETSIZE(0),
    1, // Device has one possible configuration
    0 // Reserved
};

/// USB configuration descriptors for the composite device driver
static const CdcHidDriverConfigurationDescriptors configurationDescriptorsHS =
{

    // Standard configuration descriptor
    {
        sizeof(USBConfigurationDescriptor),
        USBGenericDescriptor_CONFIGURATION,
        sizeof(CdcHidDriverConfigurationDescriptors),
        CDCHIDDDriverDescriptors_NUMINTERFACE,
        1, // This is configuration #1
        0, // No string descriptor for this configuration
        BOARD_USB_BMATTRIBUTES,
        USBConfigurationDescriptor_POWER(100)
    },

    // CDC 0
    // IAD for CDC/ACM port
    {
        sizeof(USBInterfaceAssociationDescriptor),
        USBGenericDescriptor_INTERFACEASSOCIATION,
        CDCD_Descriptors_INTERFACENUM0,
        2,
        CDCCommunicationInterfaceDescriptor_CLASS,
        CDCCommunicationInterfaceDescriptor_ABSTRACTCONTROLMODEL,
        CDCCommunicationInterfaceDescriptor_NOPROTOCOL,
        0  // No string descriptor for this interface
    },
    // Communication class interface standard descriptor
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        CDCD_Descriptors_INTERFACENUM0, // This is interface #0
        0, // This is alternate setting #0 for this interface
        1, // This interface uses 1 endpoint
        CDCCommunicationInterfaceDescriptor_CLASS,
        CDCCommunicationInterfaceDescriptor_ABSTRACTCONTROLMODEL,
        CDCCommunicationInterfaceDescriptor_NOPROTOCOL,
        0  // No string descriptor for this interface
    },
    // Class-specific header functional descriptor
    {
        sizeof(CDCHeaderDescriptor),
        CDCGenericDescriptor_INTERFACE,
        CDCGenericDescriptor_HEADER,
        CDCGenericDescriptor_CDC1_10
    },
    // Class-specific call management functional descriptor
    {
        sizeof(CDCCallManagementDescriptor),
        CDCGenericDescriptor_INTERFACE,
        CDCGenericDescriptor_CALLMANAGEMENT,
        CDCCallManagementDescriptor_SELFCALLMANAGEMENT,
        CDCD_Descriptors_INTERFACENUM0 + 1 // No associated data interface
    },
    // Class-specific abstract control management functional descriptor
    {
        sizeof(CDCAbstractControlManagementDescriptor),
        CDCGenericDescriptor_INTERFACE,
        CDCGenericDescriptor_ABSTRACTCONTROLMANAGEMENT,
        CDCAbstractControlManagementDescriptor_LINE
    },
    // Class-specific union functional descriptor with one slave interface
    {
        sizeof(CDCUnionDescriptor),
        CDCGenericDescriptor_INTERFACE,
        CDCGenericDescriptor_UNION,
        CDCD_Descriptors_INTERFACENUM0, // Number of master interface is #0
        CDCD_Descriptors_INTERFACENUM0 + 1 // First slave interface is #1
    },
    // Notification endpoint standard descriptor
    {
        sizeof(USBEndpointDescriptor), 
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(USBEndpointDescriptor_IN,
                                      CDCD_Descriptors_NOTIFICATION0),
        USBEndpointDescriptor_INTERRUPT,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(CDCD_Descriptors_NOTIFICATION0),
            CDCD_Descriptors_INTERRUPT_MAXPACKETSIZE),
        CDCD_Descriptors_INTERRUPTOUT_POLLING_HS
    },
    // Data class interface standard descriptor
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        CDCD_Descriptors_INTERFACENUM0 + 1, // This is interface #1
        0, // This is alternate setting #0 for this interface
        2, // This interface uses 2 endpoints
        CDCDataInterfaceDescriptor_CLASS,
        CDCDataInterfaceDescriptor_SUBCLASS,
        CDCDataInterfaceDescriptor_NOPROTOCOL,
        0  // No string descriptor for this interface
    },
    // Bulk-OUT endpoint standard descriptor
    {
        sizeof(USBEndpointDescriptor), 
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(USBEndpointDescriptor_OUT,
                                      CDCD_Descriptors_DATAOUT0),
        USBEndpointDescriptor_BULK,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(CDCD_Descriptors_DATAOUT0),
            CDCD_Descriptors_BULK_MAXPACKETSIZE),
        0 // Must be 0 for full-speed bulk endpoints
    },
    // Bulk-IN endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(USBEndpointDescriptor_IN,
                                      CDCD_Descriptors_DATAIN0),
        USBEndpointDescriptor_BULK,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(CDCD_Descriptors_DATAIN0),
            CDCD_Descriptors_BULK_MAXPACKETSIZE),
        0 // Must be 0 for full-speed bulk endpoints
    },

    // HID
    // Interface descriptor
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        HIDD_Descriptors_INTERFACENUM,
        0, // This is alternate setting #0
        2, // Two endpoints used
        HIDInterfaceDescriptor_CLASS,
        HIDInterfaceDescriptor_SUBCLASS_NONE,
        HIDInterfaceDescriptor_PROTOCOL_NONE,
        0  // No associated string descriptor
    },
    // HID descriptor
    {
        sizeof(HIDDescriptor),
        HIDGenericDescriptor_HID,
        HIDDescriptor_HID1_11,
        0, // Device is not localized, no country code
        1, // One HID-specific descriptor (apart from this one)
        HIDGenericDescriptor_REPORT,
        HIDD_Descriptors_REPORTSIZE
    },
    // Interrupt IN endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(
            USBEndpointDescriptor_IN,
            HIDD_Descriptors_INTERRUPTIN),
        USBEndpointDescriptor_INTERRUPT,
        sizeof(HIDDKeyboardInputReport),
        HIDD_Descriptors_INTERRUPTIN_POLLING_HS
    },
    // Interrupt OUT endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(
            USBEndpointDescriptor_OUT,
            HIDD_Descriptors_INTERRUPTOUT),
        USBEndpointDescriptor_INTERRUPT,
        sizeof(HIDDKeyboardOutputReport),
        HIDD_Descriptors_INTERRUPTOUT_POLLING_HS
    }
};

#endif

/// USB configuration descriptors for the composite device driver
static const CdcHidDriverConfigurationDescriptors configurationDescriptorsFS =
{

    // Standard configuration descriptor
    {
        sizeof(USBConfigurationDescriptor),
        USBGenericDescriptor_CONFIGURATION,
        sizeof(CdcHidDriverConfigurationDescriptors),
        CDCHIDDDriverDescriptors_NUMINTERFACE,
        1, // This is configuration #1
        0, // No string descriptor for this configuration
        BOARD_USB_BMATTRIBUTES,
        USBConfigurationDescriptor_POWER(100)
    },

    // CDC 0
    // IAD for CDC/ACM port
    {
        sizeof(USBInterfaceAssociationDescriptor),
        USBGenericDescriptor_INTERFACEASSOCIATION,
        CDCD_Descriptors_INTERFACENUM0,
        2,
        CDCCommunicationInterfaceDescriptor_CLASS,
        CDCCommunicationInterfaceDescriptor_ABSTRACTCONTROLMODEL,
        CDCCommunicationInterfaceDescriptor_NOPROTOCOL,
        0  // No string descriptor for this interface
    },
    // Communication class interface standard descriptor
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        CDCD_Descriptors_INTERFACENUM0, // This is interface #0
        0, // This is alternate setting #0 for this interface
        1, // This interface uses 1 endpoint
        CDCCommunicationInterfaceDescriptor_CLASS,
        CDCCommunicationInterfaceDescriptor_ABSTRACTCONTROLMODEL,
        CDCCommunicationInterfaceDescriptor_NOPROTOCOL,
        0  // No string descriptor for this interface
    },
    // Class-specific header functional descriptor
    {
        sizeof(CDCHeaderDescriptor),
        CDCGenericDescriptor_INTERFACE,
        CDCGenericDescriptor_HEADER,
        CDCGenericDescriptor_CDC1_10
    },
    // Class-specific call management functional descriptor
    {
        sizeof(CDCCallManagementDescriptor),
        CDCGenericDescriptor_INTERFACE,
        CDCGenericDescriptor_CALLMANAGEMENT,
        CDCCallManagementDescriptor_SELFCALLMANAGEMENT,
        CDCD_Descriptors_INTERFACENUM0 + 1 // No associated data interface
    },
    // Class-specific abstract control management functional descriptor
    {
        sizeof(CDCAbstractControlManagementDescriptor),
        CDCGenericDescriptor_INTERFACE,
        CDCGenericDescriptor_ABSTRACTCONTROLMANAGEMENT,
        CDCAbstractControlManagementDescriptor_LINE
    },
    // Class-specific union functional descriptor with one slave interface
    {
        sizeof(CDCUnionDescriptor),
        CDCGenericDescriptor_INTERFACE,
        CDCGenericDescriptor_UNION,
        CDCD_Descriptors_INTERFACENUM0, // Number of master interface is #0
        CDCD_Descriptors_INTERFACENUM0 + 1 // First slave interface is #1
    },
    // Notification endpoint standard descriptor
    {
        sizeof(USBEndpointDescriptor), 
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(USBEndpointDescriptor_IN,
                                      CDCD_Descriptors_NOTIFICATION0),
        USBEndpointDescriptor_INTERRUPT,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(CDCD_Descriptors_NOTIFICATION0),
            USBEndpointDescriptor_MAXINTERRUPTSIZE_FS),
        CDCD_Descriptors_INTERRUPTOUT_POLLING_FS
    },
    // Data class interface standard descriptor
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        CDCD_Descriptors_INTERFACENUM0 + 1, // This is interface #1
        0, // This is alternate setting #0 for this interface
        2, // This interface uses 2 endpoints
        CDCDataInterfaceDescriptor_CLASS,
        CDCDataInterfaceDescriptor_SUBCLASS,
        CDCDataInterfaceDescriptor_NOPROTOCOL,
        0  // No string descriptor for this interface
    },
    // Bulk-OUT endpoint standard descriptor
    {
        sizeof(USBEndpointDescriptor), 
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(USBEndpointDescriptor_OUT,
                                      CDCD_Descriptors_DATAOUT0),
        USBEndpointDescriptor_BULK,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(CDCD_Descriptors_DATAOUT0),
            USBEndpointDescriptor_MAXBULKSIZE_FS),
        0 // Must be 0 for full-speed bulk endpoints
    },
    // Bulk-IN endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(USBEndpointDescriptor_IN,
                                      CDCD_Descriptors_DATAIN0),
        USBEndpointDescriptor_BULK,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(CDCD_Descriptors_DATAIN0),
            USBEndpointDescriptor_MAXBULKSIZE_FS),
        0 // Must be 0 for full-speed bulk endpoints
    },

    // HID
    // Interface descriptor
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        HIDD_Descriptors_INTERFACENUM,
        0, // This is alternate setting #0
        2, // Two endpoints used
        HIDInterfaceDescriptor_CLASS,
        HIDInterfaceDescriptor_SUBCLASS_NONE,
        HIDInterfaceDescriptor_PROTOCOL_NONE,
        0  // No associated string descriptor
    },
    // HID descriptor
    {
        sizeof(HIDDescriptor),
        HIDGenericDescriptor_HID,
        HIDDescriptor_HID1_11,
        0, // Device is not localized, no country code
        1, // One HID-specific descriptor (apart from this one)
        HIDGenericDescriptor_REPORT,
        HIDD_Descriptors_REPORTSIZE
    },
    // Interrupt IN endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(
            USBEndpointDescriptor_IN,
            HIDD_Descriptors_INTERRUPTIN),
        USBEndpointDescriptor_INTERRUPT,
        sizeof(HIDDKeyboardInputReport),
        HIDD_Descriptors_INTERRUPTIN_POLLING_FS
    },
    // Interrupt OUT endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(
            USBEndpointDescriptor_OUT,
            HIDD_Descriptors_INTERRUPTOUT),
        USBEndpointDescriptor_INTERRUPT,
        sizeof(HIDDKeyboardOutputReport),
        HIDD_Descriptors_INTERRUPTOUT_POLLING_FS
    }
};

/// String descriptor with the supported languages.
static const unsigned char languageIdDescriptor[] = {

    USBStringDescriptor_LENGTH(1),
    USBGenericDescriptor_STRING,
    USBStringDescriptor_ENGLISH_US
};

/// Manufacturer name.
static const unsigned char manufacturerDescriptor[] = {

    USBStringDescriptor_LENGTH(5),
    USBGenericDescriptor_STRING,
    USBStringDescriptor_UNICODE('A'),
    USBStringDescriptor_UNICODE('t'),
    USBStringDescriptor_UNICODE('m'),
    USBStringDescriptor_UNICODE('e'),
    USBStringDescriptor_UNICODE('l')
};

/// Product name.
static const unsigned char productDescriptor[] = {

    USBStringDescriptor_LENGTH(14),
    USBGenericDescriptor_STRING,
    USBStringDescriptor_UNICODE('C'),
    USBStringDescriptor_UNICODE('o'),
    USBStringDescriptor_UNICODE('m'),
    USBStringDescriptor_UNICODE('p'),
    USBStringDescriptor_UNICODE('o'),
    USBStringDescriptor_UNICODE('s'),
    USBStringDescriptor_UNICODE('i'),
    USBStringDescriptor_UNICODE('t'),
    USBStringDescriptor_UNICODE('e'),
    USBStringDescriptor_UNICODE(' '),
    USBStringDescriptor_UNICODE('D'),
    USBStringDescriptor_UNICODE('e'),
    USBStringDescriptor_UNICODE('m'),
    USBStringDescriptor_UNICODE('o')
};

/// Product serial number.
static const unsigned char serialNumberDescriptor[] = {

    USBStringDescriptor_LENGTH(4),
    USBGenericDescriptor_STRING,
    USBStringDescriptor_UNICODE('0'),
    USBStringDescriptor_UNICODE('1'),
    USBStringDescriptor_UNICODE('2'),
    USBStringDescriptor_UNICODE('3')
};

/// Array of pointers to the four string descriptors.
static const unsigned char *stringDescriptors[] = {

    languageIdDescriptor,
    manufacturerDescriptor,
    productDescriptor,
    serialNumberDescriptor,
};

//------------------------------------------------------------------------------
//         Exported variables
//------------------------------------------------------------------------------

/// List of descriptors required by an USB audio speaker device driver.
const USBDDriverDescriptors cdchiddDriverDescriptors = {

    &deviceDescriptor,
    (const USBConfigurationDescriptor *) &configurationDescriptorsFS,
#if defined (CHIP_USB_UDPHS) || defined(CHIP_USB_OTGHS)
    &qualifierDescriptor,
    (const USBConfigurationDescriptor *) &configurationDescriptorsHS,
    &deviceDescriptor,
    (const USBConfigurationDescriptor *) &configurationDescriptorsHS,
    &qualifierDescriptor,
    (const USBConfigurationDescriptor *) &configurationDescriptorsFS,
#else
    0, 0, 0, 0, 0, 0,
#endif
    stringDescriptors,
    4 // Number of string descriptors
};
