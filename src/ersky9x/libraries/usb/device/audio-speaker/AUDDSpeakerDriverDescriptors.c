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

#include "AUDDSpeakerDriverDescriptors.h"
#include "AUDDSpeakerDriver.h"
#include <board.h>
#include <usb/common/core/USBGenericDescriptor.h>
#include <usb/common/core/USBDeviceDescriptor.h>
#include <usb/common/core/USBConfigurationDescriptor.h>
#include <usb/common/core/USBInterfaceDescriptor.h>
#include <usb/common/core/USBEndpointDescriptor.h>
#include <usb/common/core/USBStringDescriptor.h>
#include <usb/common/audio/AUDGenericDescriptor.h>
#include <usb/common/audio/AUDDeviceDescriptor.h>
#include <usb/common/audio/AUDControlInterfaceDescriptor.h>
#include <usb/common/audio/AUDStreamingInterfaceDescriptor.h>
#include <usb/common/audio/AUDEndpointDescriptor.h>
#include <usb/common/audio/AUDDataEndpointDescriptor.h>
#include <usb/common/audio/AUDFormatTypeOneDescriptor.h>
#include <usb/common/audio/AUDHeaderDescriptor.h>
#include <usb/common/audio/AUDFeatureUnitDescriptor.h>
#include <usb/common/audio/AUDInputTerminalDescriptor.h>
#include <usb/common/audio/AUDOutputTerminalDescriptor.h>

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "Audio Speaker Device Codes"
///
/// This page lists the %device IDs and release number of the USB Audio Speaker
/// %device.
///
/// !Codes
/// - AUDDSpeakerDriverDescriptors_VENDORID
/// - AUDDSpeakerDriverDescriptors_PRODUCTID
/// - AUDDSpeakerDriverDescriptors_RELEASE

/// Device vendor ID.
#define AUDDSpeakerDriverDescriptors_VENDORID            0x03EB
/// Device product ID.
#define AUDDSpeakerDriverDescriptors_PRODUCTID           0x6128
/// Device release number in BCD format.
#define AUDDSpeakerDriverDescriptors_RELEASE             0x0100
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Internal types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Audio control header descriptor with one slave interface.
//------------------------------------------------------------------------------
#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
/// Header descriptor with 1 interface.
//------------------------------------------------------------------------------
typedef struct {

    /// Header descriptor.
    AUDHeaderDescriptor header;
    /// Id of the first grouped interface - Speaker.
    unsigned char bInterface0;

} __attribute__ ((packed)) AUDHeaderDescriptor1; // GCC

//------------------------------------------------------------------------------
/// Feature unit descriptor with 3 channel controls (master, right, left).
//------------------------------------------------------------------------------
typedef struct {

    /// Feature unit descriptor.
    AUDFeatureUnitDescriptor feature;
    /// Available controls for each channel.
    unsigned char bmaControls[3];
    /// Index of a string descriptor for the feature unit.
    unsigned char iFeature;

} __attribute__ ((packed)) AUDFeatureUnitDescriptor3; // GCC

//------------------------------------------------------------------------------
/// List of descriptors for detailling the audio control interface of a
/// device using a USB audio speaker driver.
//------------------------------------------------------------------------------
typedef struct {

    /// Header descriptor (with one slave interface).
    AUDHeaderDescriptor1 header;
    /// Input terminal descriptor.
    AUDInputTerminalDescriptor input;
    /// Output terminal descriptor.
    AUDOutputTerminalDescriptor output;
    /// Feature unit descriptor.
    AUDFeatureUnitDescriptor3 feature;

} __attribute__ ((packed)) AUDDSpeakerDriverAudioControlDescriptors; // GCC

//------------------------------------------------------------------------------
/// Format type I descriptor with one discrete sampling frequency.
//------------------------------------------------------------------------------
typedef struct {

    /// Format type I descriptor.
    AUDFormatTypeOneDescriptor formatType;
    /// Sampling frequency in Hz.
    unsigned char tSamFreq[3];

} __attribute__ ((packed)) AUDFormatTypeOneDescriptor1; // GCC

//------------------------------------------------------------------------------
/// Holds a list of descriptors returned as part of the configuration of
/// a USB audio speaker device.
//------------------------------------------------------------------------------
typedef struct {

    /// Standard configuration.
    USBConfigurationDescriptor configuration;
    /// Audio control interface.
    USBInterfaceDescriptor control;
    /// Descriptors for the audio control interface.
    AUDDSpeakerDriverAudioControlDescriptors controlDescriptors;
    //- AUDIO OUT
    /// Streaming out interface descriptor (with no endpoint, required).
    USBInterfaceDescriptor streamingOutNoIsochronous;
    /// Streaming out interface descriptor.
    USBInterfaceDescriptor streamingOut;
    /// Audio class descriptor for the streaming out interface.
    AUDStreamingInterfaceDescriptor streamingOutClass;
    /// Stream format descriptor.
    AUDFormatTypeOneDescriptor1 streamingOutFormatType;
    /// Streaming out endpoint descriptor.
    AUDEndpointDescriptor streamingOutEndpoint;
    /// Audio class descriptor for the streaming out endpoint.
    AUDDataEndpointDescriptor streamingOutDataEndpoint; 

} __attribute__ ((packed)) AUDDSpeakerDriverConfigurationDescriptors; // GCC

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
//         Exported variables
//------------------------------------------------------------------------------

/// Device descriptor for a USB audio speaker driver.
const USBDeviceDescriptor deviceDescriptor = {

    sizeof(USBDeviceDescriptor),
    USBGenericDescriptor_DEVICE,
    USBDeviceDescriptor_USB2_00,
    AUDDeviceDescriptor_CLASS,
    AUDDeviceDescriptor_SUBCLASS,
    AUDDeviceDescriptor_PROTOCOL,
    CHIP_USB_ENDPOINTS_MAXPACKETSIZE(0),
    AUDDSpeakerDriverDescriptors_VENDORID,
    AUDDSpeakerDriverDescriptors_PRODUCTID,
    AUDDSpeakerDriverDescriptors_RELEASE,
    1, // Manufacturer string descriptor index
    2, // Product string descriptor index
    3, // Index of serial number string descriptor
    1  // One possible configuration
};

#if defined(CHIP_USB_UDPHS) || defined(CHIP_USB_OTGHS)

/// USB device qualifier descriptor.
const USBDeviceQualifierDescriptor qualifierDescriptor = {

    sizeof(USBDeviceDescriptor),
    USBGenericDescriptor_DEVICE,
    USBDeviceDescriptor_USB2_00,
    AUDDeviceDescriptor_CLASS,
    AUDDeviceDescriptor_SUBCLASS,
    AUDDeviceDescriptor_PROTOCOL,
    CHIP_USB_ENDPOINTS_MAXPACKETSIZE(0),
    1, // Device has one possible configuration
    0 // Reserved
};

#endif

/// Configuration descriptors for a USB audio speaker driver.
const AUDDSpeakerDriverConfigurationDescriptors fsConfigurationDescriptors = {

    // Configuration descriptor
    {
        sizeof(USBConfigurationDescriptor),
        USBGenericDescriptor_CONFIGURATION,
        sizeof(AUDDSpeakerDriverConfigurationDescriptors),
        2, // This configuration has 2 interfaces
        1, // This is configuration #1
        0, // No string descriptor
        BOARD_USB_BMATTRIBUTES,
        USBConfigurationDescriptor_POWER(100)
    },
    // Audio control interface standard descriptor
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        AUDDSpeakerDriverDescriptors_CONTROL,
        0, // This is alternate setting #0
        0, // This interface uses no endpoint
        AUDControlInterfaceDescriptor_CLASS,
        AUDControlInterfaceDescriptor_SUBCLASS,
        AUDControlInterfaceDescriptor_PROTOCOL,
        0 // No string descriptor
    },
    // Audio control interface descriptors
    {
        // Header descriptor
        {
            {
                sizeof(AUDHeaderDescriptor1),
                AUDGenericDescriptor_INTERFACE,
                AUDGenericDescriptor_HEADER,
                AUDHeaderDescriptor_AUD1_00,
                sizeof(AUDDSpeakerDriverAudioControlDescriptors),
                1 // One streaming interface
            },
            AUDDSpeakerDriverDescriptors_STREAMING
        },
        // Input terminal descriptor
        {
            sizeof(AUDInputTerminalDescriptor),
            AUDGenericDescriptor_INTERFACE,
            AUDGenericDescriptor_INPUTTERMINAL,
            AUDDSpeakerDriverDescriptors_INPUTTERMINAL,
            AUDInputTerminalDescriptor_USBSTREAMING,
            AUDDSpeakerDriverDescriptors_OUTPUTTERMINAL,
            AUDDSpeakerDriver_NUMCHANNELS,
            AUDInputTerminalDescriptor_LEFTFRONT
            | AUDInputTerminalDescriptor_RIGHTFRONT,
            0, // No string descriptor for channels
            0 // No string descriptor for input terminal
        },
        // Output terminal descriptor
        {
            sizeof(AUDOutputTerminalDescriptor),
            AUDGenericDescriptor_INTERFACE,
            AUDGenericDescriptor_OUTPUTTERMINAL,
            AUDDSpeakerDriverDescriptors_OUTPUTTERMINAL,
            AUDOutputTerminalDescriptor_SPEAKER,
            AUDDSpeakerDriverDescriptors_INPUTTERMINAL,
            AUDDSpeakerDriverDescriptors_FEATUREUNIT,
            0 // No string descriptor
        },
        // Feature unit descriptor
        {
            {
                sizeof(AUDFeatureUnitDescriptor3),
                AUDGenericDescriptor_INTERFACE,
                AUDGenericDescriptor_FEATUREUNIT,
                AUDDSpeakerDriverDescriptors_FEATUREUNIT,
                AUDDSpeakerDriverDescriptors_INPUTTERMINAL,
                1, // 1 byte per channel for controls
            },
            {
                AUDFeatureUnitDescriptor_MUTE, // Master channel controls
                0, // Right channel controls
                0  // Left channel controls
            },
            0 // No string descriptor
        }
    },
    // Audio streaming interface with 0 endpoints
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        AUDDSpeakerDriverDescriptors_STREAMING,
        0, // This is alternate setting #0
        0, // This interface uses no endpoints
        AUDStreamingInterfaceDescriptor_CLASS,
        AUDStreamingInterfaceDescriptor_SUBCLASS,
        AUDStreamingInterfaceDescriptor_PROTOCOL,
        0 // No string descriptor
    },
    // Audio streaming interface with data endpoint
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        AUDDSpeakerDriverDescriptors_STREAMING,
        1, // This is alternate setting #1
        1, // This interface uses 1 endpoint
        AUDStreamingInterfaceDescriptor_CLASS,
        AUDStreamingInterfaceDescriptor_SUBCLASS,
        AUDStreamingInterfaceDescriptor_PROTOCOL,
        0 // No string descriptor
    },
    // Audio streaming class-specific descriptor
    {
        sizeof(AUDStreamingInterfaceDescriptor),
        AUDGenericDescriptor_INTERFACE,
        AUDStreamingInterfaceDescriptor_GENERAL,
        AUDDSpeakerDriverDescriptors_INPUTTERMINAL,
        0, // No internal delay because of data path
        AUDFormatTypeOneDescriptor_PCM
    },
    // Format type I descriptor
    {
        {
            sizeof(AUDFormatTypeOneDescriptor1),
            AUDGenericDescriptor_INTERFACE,
            AUDStreamingInterfaceDescriptor_FORMATTYPE,
            AUDFormatTypeOneDescriptor_FORMATTYPEONE,
            AUDDSpeakerDriver_NUMCHANNELS,
            AUDDSpeakerDriver_BYTESPERSAMPLE,
            AUDDSpeakerDriver_BYTESPERSAMPLE*8,
            1 // One discrete frequency supported
        },
        {
            AUDDSpeakerDriver_SAMPLERATE & 0xFF,
            (AUDDSpeakerDriver_SAMPLERATE >> 8) & 0xFF,
            (AUDDSpeakerDriver_SAMPLERATE >> 16) & 0xFF
        }
    },
    // Audio streaming endpoint standard descriptor
    {
        sizeof(AUDEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(
            USBEndpointDescriptor_OUT,
            AUDDSpeakerDriverDescriptors_DATAOUT),
        USBEndpointDescriptor_ISOCHRONOUS,
        CHIP_USB_ENDPOINTS_MAXPACKETSIZE(AUDDSpeakerDriverDescriptors_DATAOUT),
        AUDDSpeakerDriverDescriptors_FS_INTERVAL, // Polling interval = 1 ms
        0, // This is not a synchronization endpoint
        0 // No associated synchronization endpoint
    },
    // Audio streaming endpoint class-specific descriptor
    {
        sizeof(AUDDataEndpointDescriptor),
        AUDGenericDescriptor_ENDPOINT,
        AUDDataEndpointDescriptor_SUBTYPE,
        0, // No attributes
        0, // Endpoint is not synchronized
        0 // Endpoint is not synchronized
    }
};

#if defined(CHIP_USB_UDPHS) || defined(CHIP_USB_OTGHS)
/// Configuration descriptors for a USB audio speaker driver.
const AUDDSpeakerDriverConfigurationDescriptors hsConfigurationDescriptors = {

    // Configuration descriptor
    {
        sizeof(USBConfigurationDescriptor),
        USBGenericDescriptor_CONFIGURATION,
        sizeof(AUDDSpeakerDriverConfigurationDescriptors),
        2, // This configuration has 2 interfaces
        1, // This is configuration #1
        0, // No string descriptor
        BOARD_USB_BMATTRIBUTES,
        USBConfigurationDescriptor_POWER(100)
    },
    // Audio control interface standard descriptor
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        AUDDSpeakerDriverDescriptors_CONTROL,
        0, // This is alternate setting #0
        0, // This interface uses no endpoint
        AUDControlInterfaceDescriptor_CLASS,
        AUDControlInterfaceDescriptor_SUBCLASS,
        AUDControlInterfaceDescriptor_PROTOCOL,
        0 // No string descriptor
    },
    // Audio control interface descriptors
    {
        // Header descriptor
        {
            {
                sizeof(AUDHeaderDescriptor1),
                AUDGenericDescriptor_INTERFACE,
                AUDGenericDescriptor_HEADER,
                AUDHeaderDescriptor_AUD1_00,
                sizeof(AUDDSpeakerDriverAudioControlDescriptors),
                1 // One streaming interface
            },
            AUDDSpeakerDriverDescriptors_STREAMING
        },
        // Input terminal descriptor
        {
            sizeof(AUDInputTerminalDescriptor),
            AUDGenericDescriptor_INTERFACE,
            AUDGenericDescriptor_INPUTTERMINAL,
            AUDDSpeakerDriverDescriptors_INPUTTERMINAL,
            AUDInputTerminalDescriptor_USBSTREAMING,
            AUDDSpeakerDriverDescriptors_OUTPUTTERMINAL,
            AUDDSpeakerDriver_NUMCHANNELS,
            AUDInputTerminalDescriptor_LEFTFRONT
            | AUDInputTerminalDescriptor_RIGHTFRONT,
            0, // No string descriptor for channels
            0 // No string descriptor for input terminal
        },
        // Output terminal descriptor
        {
            sizeof(AUDOutputTerminalDescriptor),
            AUDGenericDescriptor_INTERFACE,
            AUDGenericDescriptor_OUTPUTTERMINAL,
            AUDDSpeakerDriverDescriptors_OUTPUTTERMINAL,
            AUDOutputTerminalDescriptor_SPEAKER,
            AUDDSpeakerDriverDescriptors_INPUTTERMINAL,
            AUDDSpeakerDriverDescriptors_FEATUREUNIT,
            0 // No string descriptor
        },
        // Feature unit descriptor
        {
            {
                sizeof(AUDFeatureUnitDescriptor3),
                AUDGenericDescriptor_INTERFACE,
                AUDGenericDescriptor_FEATUREUNIT,
                AUDDSpeakerDriverDescriptors_FEATUREUNIT,
                AUDDSpeakerDriverDescriptors_INPUTTERMINAL,
                1, // 1 byte per channel for controls
            },
            {
                AUDFeatureUnitDescriptor_MUTE, // Master channel controls
                0, // Right channel controls
                0  // Left channel controls
            },
            0 // No string descriptor
        }
    },
    // Audio streaming interface with 0 endpoints
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        AUDDSpeakerDriverDescriptors_STREAMING,
        0, // This is alternate setting #0
        0, // This interface uses no endpoints
        AUDStreamingInterfaceDescriptor_CLASS,
        AUDStreamingInterfaceDescriptor_SUBCLASS,
        AUDStreamingInterfaceDescriptor_PROTOCOL,
        0 // No string descriptor
    },
    // Audio streaming interface with data endpoint
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        AUDDSpeakerDriverDescriptors_STREAMING,
        1, // This is alternate setting #1
        1, // This interface uses 1 endpoint
        AUDStreamingInterfaceDescriptor_CLASS,
        AUDStreamingInterfaceDescriptor_SUBCLASS,
        AUDStreamingInterfaceDescriptor_PROTOCOL,
        0 // No string descriptor
    },
    // Audio streaming class-specific descriptor
    {
        sizeof(AUDStreamingInterfaceDescriptor),
        AUDGenericDescriptor_INTERFACE,
        AUDStreamingInterfaceDescriptor_GENERAL,
        AUDDSpeakerDriverDescriptors_INPUTTERMINAL,
        0, // No internal delay because of data path
        AUDFormatTypeOneDescriptor_PCM
    },
    // Format type I descriptor
    {
        {
            sizeof(AUDFormatTypeOneDescriptor1),
            AUDGenericDescriptor_INTERFACE,
            AUDStreamingInterfaceDescriptor_FORMATTYPE,
            AUDFormatTypeOneDescriptor_FORMATTYPEONE,
            AUDDSpeakerDriver_NUMCHANNELS,
            AUDDSpeakerDriver_BYTESPERSAMPLE,
            AUDDSpeakerDriver_BYTESPERSAMPLE*8,
            1 // One discrete frequency supported
        },
        {
            AUDDSpeakerDriver_SAMPLERATE & 0xFF,
            (AUDDSpeakerDriver_SAMPLERATE >> 8) & 0xFF,
            (AUDDSpeakerDriver_SAMPLERATE >> 16) & 0xFF
        }
    },
    // Audio streaming endpoint standard descriptor
    {
        sizeof(AUDEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(
            USBEndpointDescriptor_OUT,
            AUDDSpeakerDriverDescriptors_DATAOUT),
        USBEndpointDescriptor_ISOCHRONOUS,
        CHIP_USB_ENDPOINTS_MAXPACKETSIZE(AUDDSpeakerDriverDescriptors_DATAOUT),
        AUDDSpeakerDriverDescriptors_HS_INTERVAL, // Polling interval = 1 ms
        0, // This is not a synchronization endpoint
        0 // No associated synchronization endpoint
    },
    // Audio streaming endpoint class-specific descriptor
    {
        sizeof(AUDDataEndpointDescriptor),
        AUDGenericDescriptor_ENDPOINT,
        AUDDataEndpointDescriptor_SUBTYPE,
        0, // No attributes
        0, // Endpoint is not synchronized
        0 // Endpoint is not synchronized
    }
};
#endif // defined(CHIP_USB_UDPHS) || defined(CHIP_USB_OTGHS)

/// String descriptor with the supported languages.
const unsigned char languageIdDescriptor[] = {

    USBStringDescriptor_LENGTH(1),
    USBGenericDescriptor_STRING,
    USBStringDescriptor_ENGLISH_US
};

/// Manufacturer name.
const unsigned char manufacturerDescriptor[] = {

    USBStringDescriptor_LENGTH(5),
    USBGenericDescriptor_STRING,
    USBStringDescriptor_UNICODE('A'),
    USBStringDescriptor_UNICODE('t'),
    USBStringDescriptor_UNICODE('m'),
    USBStringDescriptor_UNICODE('e'),
    USBStringDescriptor_UNICODE('l')
};

/// Product name.
const unsigned char productDescriptor[] = {

    USBStringDescriptor_LENGTH(15),
    USBGenericDescriptor_STRING,
    USBStringDescriptor_UNICODE('D'),
    USBStringDescriptor_UNICODE('e'),
    USBStringDescriptor_UNICODE('s'),
    USBStringDescriptor_UNICODE('k'),
    USBStringDescriptor_UNICODE('t'),
    USBStringDescriptor_UNICODE('o'),
    USBStringDescriptor_UNICODE('p'),
    USBStringDescriptor_UNICODE(' '),
    USBStringDescriptor_UNICODE('s'),
    USBStringDescriptor_UNICODE('p'),
    USBStringDescriptor_UNICODE('e'),
    USBStringDescriptor_UNICODE('a'),
    USBStringDescriptor_UNICODE('k'),
    USBStringDescriptor_UNICODE('e'),
    USBStringDescriptor_UNICODE('r')
};

/// Product serial number.
const unsigned char serialNumberDescriptor[] = {

    USBStringDescriptor_LENGTH(4),
    USBGenericDescriptor_STRING,
    USBStringDescriptor_UNICODE('0'),
    USBStringDescriptor_UNICODE('1'),
    USBStringDescriptor_UNICODE('2'),
    USBStringDescriptor_UNICODE('3')
};

/// Array of pointers to the four string descriptors.
const unsigned char *stringDescriptors[] = {

    languageIdDescriptor,
    manufacturerDescriptor,
    productDescriptor,
    serialNumberDescriptor,
};

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

/// List of descriptors required by an USB audio speaker device driver.
const USBDDriverDescriptors auddSpeakerDriverDescriptors = {

    &deviceDescriptor,
    (const USBConfigurationDescriptor *) &fsConfigurationDescriptors,
#if defined (CHIP_USB_UDPHS) || defined(CHIP_USB_OTGHS)
    &qualifierDescriptor,
    (const USBConfigurationDescriptor *) &hsConfigurationDescriptors,
    &deviceDescriptor,
    (const USBConfigurationDescriptor *) &hsConfigurationDescriptors,
    &qualifierDescriptor,
    (const USBConfigurationDescriptor *) &hsConfigurationDescriptors,
#else
    0, 0, 0, 0, 0, 0,
#endif
    stringDescriptors,
    4 // Number of string descriptors
};

