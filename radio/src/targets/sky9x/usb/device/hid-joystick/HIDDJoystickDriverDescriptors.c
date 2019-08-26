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

/*
    Title: HIDDJoystickDriverDescriptors

    About: Purpose
        Declaration of the descriptors used by the HID device joystick driver.
*/

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "HIDDJoystickDriverDescriptors.h"
#include "HIDDJoystickInputReport.h"
#include <usb/common/core/USBDeviceDescriptor.h>
#include <usb/common/core/USBConfigurationDescriptor.h>
#include <usb/common/core/USBInterfaceDescriptor.h>
#include <usb/common/core/USBEndpointDescriptor.h>
#include <usb/common/core/USBStringDescriptor.h>
#include <usb/common/hid/HIDGenericDescriptor.h>
#include <usb/common/hid/HIDDeviceDescriptor.h>
#include <usb/common/hid/HIDInterfaceDescriptor.h>
#include <usb/common/hid/HIDDescriptor.h>
#include <usb/common/hid/HIDReport.h>
#include <usb/common/hid/HIDGenericDesktop.h>
#include <usb/common/hid/HIDButton.h>
#include <usb/device/core/USBDDriverDescriptors.h>

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "HID Joystick Device Descriptor IDs"
/// ...
///
/// !IDs
/// - HIDDJoystickDriverDescriptors_PRODUCTID
/// - HIDDJoystickDriverDescriptors_VENDORID
/// - HIDDJoystickDriverDescriptors_RELEASE

/// Device product ID.
#define HIDDJoystickDriverDescriptors_PRODUCTID       0x6200
/// Device vendor ID.
#define HIDDJoystickDriverDescriptors_VENDORID        0x03EB
/// Device release number.
#define HIDDJoystickDriverDescriptors_RELEASE         0x0100
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Internal types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// List of descriptors that make up the configuration descriptors of a
/// %device using the HID Joystick driver.
//------------------------------------------------------------------------------
typedef struct {

    /// Configuration descriptor.
    USBConfigurationDescriptor configuration;
    /// Interface descriptor.
    USBInterfaceDescriptor interface;
    /// HID descriptor.
    HIDDescriptor hid;
    /// Interrupt IN endpoint descriptor.
    USBEndpointDescriptor interruptIn;

} __attribute__ ((packed)) HIDDJoystickDriverConfigurationDescriptors;

//------------------------------------------------------------------------------
//         Internal variables
//------------------------------------------------------------------------------

#define BOARD_USB_ENDPOINTS_MAXPACKETSIZE(i) (((i == 0)||(i == 3)||(i == 4)) ? 64 :\
                                             (((i == 1) || (i == 2)) ? 512 : 1024))

/// Device descriptor.
static const USBDeviceDescriptor deviceDescriptor = {

    sizeof(USBDeviceDescriptor),
    USBGenericDescriptor_DEVICE,
    USBDeviceDescriptor_USB2_00,
    HIDDeviceDescriptor_CLASS,
    HIDDeviceDescriptor_SUBCLASS,
    HIDDeviceDescriptor_PROTOCOL,
    BOARD_USB_ENDPOINTS_MAXPACKETSIZE(0),
    HIDDJoystickDriverDescriptors_VENDORID,
    HIDDJoystickDriverDescriptors_PRODUCTID,
    HIDDJoystickDriverDescriptors_RELEASE,
    1, // Index of manufacturer description
    2, // Index of product description
    3, // Index of serial number description
    1  // One possible configuration
};

/// Report descriptor used by the driver.
const unsigned char hiddReportDescriptor[] = {

    HIDReport_GLOBAL_USAGEPAGE + 1, HIDGenericDesktop_PAGEID,           // USAGE_PAGE (Generic Desktop)
    HIDReport_LOCAL_USAGE + 1, HIDGenericDesktop_GAMEPAD,               // USAGE (Game Pad)
    HIDReport_COLLECTION + 1, HIDReport_COLLECTION_APPLICATION,         // COLLECTION (Application)
        HIDReport_COLLECTION + 1, HIDReport_COLLECTION_PHYSICAL,        // COLLECTION (Physical)
            HIDReport_GLOBAL_USAGEPAGE + 1, HIDButton_PAGEID,           // USAGE_PAGE (Button)
            HIDReport_LOCAL_USAGEMINIMUM + 1, 1,                        // USAGE_MINIMUM (Button 1)
            HIDReport_LOCAL_USAGEMAXIMUM + 1, 24,                       // USAGE_MAXIMUM (Button 24)
            HIDReport_GLOBAL_LOGICALMINIMUM + 1, 0,                     // LOGICAL_MINIMUM (0)
            HIDReport_GLOBAL_LOGICALMAXIMUM + 1, 1,                     // LOGICAL_MAXIMUM (1)
            HIDReport_GLOBAL_REPORTCOUNT + 1, 24,                       // REPORT_COUNT (24)
            HIDReport_GLOBAL_REPORTSIZE + 1, 1,                         // REPORT_SIZE (1)
            HIDReport_INPUT + 1, HIDReport_VARIABLE,                    // INPUT (Data,Var,Abs)
            HIDReport_GLOBAL_USAGEPAGE + 1, HIDGenericDesktop_PAGEID,   // USAGE_PAGE (Generic Desktop)
            HIDReport_LOCAL_USAGE + 1, HIDGenericDesktop_X_AXIS,        // USAGE (X)
            HIDReport_LOCAL_USAGE + 1, HIDGenericDesktop_Y_AXIS,        // USAGE (Y)
            HIDReport_LOCAL_USAGE + 1, HIDGenericDesktop_Z_AXIS,        // USAGE (Z)
            HIDReport_LOCAL_USAGE + 1, HIDGenericDesktop_X_ROTATION,    // USAGE (Rx)
            HIDReport_LOCAL_USAGE + 1, HIDGenericDesktop_Y_ROTATION,    // USAGE (Ry)
            HIDReport_LOCAL_USAGE + 1, HIDGenericDesktop_Z_ROTATION,    // USAGE (Rz)
            HIDReport_LOCAL_USAGE + 1, HIDGenericDesktop_SLIDER,        // USAGE (Slider)
            HIDReport_LOCAL_USAGE + 1, HIDGenericDesktop_SLIDER,        // USAGE (Slider)
            HIDReport_GLOBAL_LOGICALMINIMUM + 1, (unsigned char) -127,  // LOGICAL_MINIMUM (-127)
            HIDReport_GLOBAL_LOGICALMAXIMUM + 1, 127,                   // LOGICAL_MAXIMUM (127)
            HIDReport_GLOBAL_REPORTSIZE + 1, 8,                         // REPORT_SIZE (8)
            HIDReport_GLOBAL_REPORTCOUNT + 1, 8,                        // REPORT_COUNT (8)
            HIDReport_INPUT + 1, HIDReport_VARIABLE,                    // INPUT (Data,Var,Abs)
        HIDReport_ENDCOLLECTION,                                        // END_COLLECTION
    HIDReport_ENDCOLLECTION                                             // END_COLLECTION
};
int hiddReportDescriptorSize = sizeof(hiddReportDescriptor);

/// Configuration descriptor.
static const HIDDJoystickDriverConfigurationDescriptors configurationDescriptors = {

    // Configuration descriptor
    {
        sizeof(USBConfigurationDescriptor),
        USBGenericDescriptor_CONFIGURATION,
        sizeof(HIDDJoystickDriverConfigurationDescriptors),
        1, // One interface in this configuration
        1, // This is configuration #1
        0, // No associated string descriptor
        BOARD_USB_BMATTRIBUTES,
        USBConfigurationDescriptor_POWER(100)
    },
    // Interface descriptor
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        0, // This is interface #0
        0, // This is alternate setting #0
        1, // One endpoints used
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
        sizeof(hiddReportDescriptor),
    },
    // Interrupt IN endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT, // 5
        USBEndpointDescriptor_ADDRESS(
            USBEndpointDescriptor_IN,
            HIDDJoystickDriverDescriptors_INTERRUPTIN),
        USBEndpointDescriptor_INTERRUPT,
        sizeof(HIDDJoystickInputReport),
        HIDDJoystickDriverDescriptors_INTERRUPTIN_POLLING
    }
};

/*
    Variables: String descriptors
        languageIdDescriptor - Language ID string descriptor.
        manufacturerDescriptor - Manufacturer name.
        productDescriptor - Product name.
        serialNumberDescriptor - Product serial number.
        stringDescriptors - Array of pointers to string descriptors.
*/
static const unsigned char languageIdDescriptor[] = {

    USBStringDescriptor_LENGTH(1),
    USBGenericDescriptor_STRING,
    USBStringDescriptor_ENGLISH_US
};

static const unsigned char manufacturerDescriptor[] = {

    USBStringDescriptor_LENGTH(5),
    USBGenericDescriptor_STRING,
    USBStringDescriptor_UNICODE('A'),
    USBStringDescriptor_UNICODE('T'),
    USBStringDescriptor_UNICODE('M'),
    USBStringDescriptor_UNICODE('E'),
    USBStringDescriptor_UNICODE('L')
};

static const unsigned char productDescriptor[] = {

    USBStringDescriptor_LENGTH(14),
    USBGenericDescriptor_STRING,
    USBStringDescriptor_UNICODE('S'),
    USBStringDescriptor_UNICODE('K'),
    USBStringDescriptor_UNICODE('Y'),
    USBStringDescriptor_UNICODE('9'),
    USBStringDescriptor_UNICODE('X'),
    USBStringDescriptor_UNICODE(' '),
    USBStringDescriptor_UNICODE('J'),
    USBStringDescriptor_UNICODE('o'),
    USBStringDescriptor_UNICODE('y'),
    USBStringDescriptor_UNICODE('s'),
    USBStringDescriptor_UNICODE('t'),
    USBStringDescriptor_UNICODE('i'),
    USBStringDescriptor_UNICODE('c'),
    USBStringDescriptor_UNICODE('k'),
};

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
    USBStringDescriptor_UNICODE('F')
};

static const unsigned char *stringDescriptors[] = {

    languageIdDescriptor,
    manufacturerDescriptor,
    productDescriptor,
    serialNumberDescriptor
};

//------------------------------------------------------------------------------
//         Exported variables
//------------------------------------------------------------------------------

/// List of descriptors used by the HID joystick driver.
USBDDriverDescriptors hiddJoystickDriverDescriptors = {

    &deviceDescriptor,
    (USBConfigurationDescriptor *) &configurationDescriptors,
    0, // No full-speed device qualifier descriptor
    0, // No full-speed other speed configuration
    0, // No high-speed device descriptor
    0, // No high-speed configuration descriptor
    0, // No high-speed device qualifier descriptor
    0, // No high-speed other speed configuration descriptor
    stringDescriptors,
    4 // Four string descriptors in list
};
