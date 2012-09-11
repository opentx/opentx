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

#include "COMPOSITEDDriver.h"
#include "COMPOSITEDDriverDescriptors.h"
#include <board.h>

//- USB Generic
#include <usb/common/core/USBGenericDescriptor.h>
#include <usb/common/core/USBConfigurationDescriptor.h>
#include <usb/common/core/USBInterfaceAssociationDescriptor.h>
#include <usb/common/core/USBEndpointDescriptor.h>
#include <usb/common/core/USBStringDescriptor.h>
#include <usb/common/core/USBGenericRequest.h>

//- CDC
#if defined(usb_CDCAUDIO) || defined(usb_CDCHID) || defined(usb_CDCCDC) || defined(usb_CDCMSD)
 #include <usb/common/cdc/CDCGenericDescriptor.h>
 #include <usb/common/cdc/CDCDeviceDescriptor.h>
 #include <usb/common/cdc/CDCCommunicationInterfaceDescriptor.h>
 #include <usb/common/cdc/CDCDataInterfaceDescriptor.h>
 #include <usb/common/cdc/CDCHeaderDescriptor.h>
 #include <usb/common/cdc/CDCCallManagementDescriptor.h>
 #include <usb/common/cdc/CDCAbstractControlManagementDescriptor.h>
 #include <usb/common/cdc/CDCUnionDescriptor.h>
 #include "CDCDFunctionDriverDescriptors.h"
#endif // (CDC defined)

//- HID
#if defined(usb_CDCHID) || defined(usb_HIDAUDIO) || defined(usb_HIDMSD)
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
#endif // (HID defined)

//- AUDIO
#if defined(usb_CDCAUDIO) || defined(usb_HIDAUDIO)
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
 #include "AUDDFunctionDriverDescriptors.h"
#endif // (AUDIO defined)

//- MSD
#if defined(usb_CDCMSD) || defined(usb_HIDMSD)
 #include <usb/common/massstorage/MSDeviceDescriptor.h>
 #include <usb/common/massstorage/MSInterfaceDescriptor.h>
 #include "MSDDFunctionDriverDescriptors.h"
#endif // (MSD defined)

//-----------------------------------------------------------------------------
//         Definitions
//-----------------------------------------------------------------------------

/// Device product ID.
#if defined(usb_CDCHID)
#define COMPOSITEDDriverDescriptors_PRODUCTID       0x6130
#elif defined(usb_CDCAUDIO)
#define COMPOSITEDDriverDescriptors_PRODUCTID       0x6131
#elif defined(usb_CDCMSD)
#define COMPOSITEDDriverDescriptors_PRODUCTID       0x6132
#elif defined(usb_CDCCDC)
#define COMPOSITEDDriverDescriptors_PRODUCTID       0x6133
#elif defined(usb_HIDAUDIO)
#define COMPOSITEDDriverDescriptors_PRODUCTID       0x6134
#elif defined(usb_HIDMSD)
#define COMPOSITEDDriverDescriptors_PRODUCTID       0x6135
#else
#error COMPOSITE Device Classes not defined!
#endif

/// Device vendor ID (Atmel).
#define COMPOSITEDDriverDescriptors_VENDORID        0x03EB

/// Device release number.
#define COMPOSITEDDriverDescriptors_RELEASE         0x0003

//-----------------------------------------------------------------------------
//         Macros
//-----------------------------------------------------------------------------

/// Returns the minimum between two values.
#define MIN(a, b)       ((a < b) ? a : b)

//-----------------------------------------------------------------------------
//         Internal structures
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Audio control header descriptor with one slave interface.
//-----------------------------------------------------------------------------
#ifdef __ICCARM__          // IAR
#pragma pack(1)            // IAR
#define __attribute__(...) // IAR
#endif                     // IAR

//- AUDIO
#if defined(usb_CDCAUDIO) || defined(usb_HIDAUDIO)
typedef struct {

    /// Header descriptor.
    AUDHeaderDescriptor header;
    /// Id of the first grouped interface.
    unsigned char bInterface0;

} __attribute__ ((packed)) AUDHeaderDescriptor1; // GCC

//-----------------------------------------------------------------------------
/// Feature unit descriptor with 3 channel controls (master, right, left).
//-----------------------------------------------------------------------------
typedef struct {

    /// Feature unit descriptor.
    AUDFeatureUnitDescriptor feature;
    /// Available controls for each channel.
    unsigned char bmaControls[3];
    /// Index of a string descriptor for the feature unit.
    unsigned char iFeature;

} __attribute__ ((packed)) AUDFeatureUnitDescriptor3; // GCC

//-----------------------------------------------------------------------------
/// List of descriptors for detailling the audio control interface of a
/// device using a USB audio speaker driver.
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/// Format type I descriptor with one discrete sampling frequency.
//-----------------------------------------------------------------------------
typedef struct {

    /// Format type I descriptor.
    AUDFormatTypeOneDescriptor formatType;
    /// Sampling frequency in Hz.
    unsigned char tSamFreq[3];

} __attribute__ ((packed)) AUDFormatTypeOneDescriptor1; // GCC
#endif // (AUDIO defined)

//-----------------------------------------------------------------------------
/// Configuration descriptor list for a device implementing a composite driver.
//-----------------------------------------------------------------------------
typedef struct {

    /// Standard configuration descriptor.
    USBConfigurationDescriptor configuration;

  #if defined(usb_CDCAUDIO) || defined(usb_CDCHID) || defined(usb_CDCCDC) || defined(usb_CDCMSD)
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
  #endif // (CDC defined)

  #if defined(usb_CDCHID) || defined(usb_HIDAUDIO) || defined(usb_HIDMSD)
    /// --- HID
    USBInterfaceDescriptor hidInterface;
    HIDDescriptor hid;
    USBEndpointDescriptor hidInterruptIn;
    USBEndpointDescriptor hidInterruptOut;
  #endif // (HID defined)

  #if defined(usb_CDCAUDIO) || defined(usb_HIDAUDIO)
    /// --- AUDIO
    /// IAD 1
    USBInterfaceAssociationDescriptor audIAD;
    /// Audio control interface.
    USBInterfaceDescriptor audInterface;
    /// Descriptors for the audio control interface.
    AUDDSpeakerDriverAudioControlDescriptors audControl;
    /// -- AUDIO out
    /// Streaming out interface descriptor (with no endpoint, required).
    USBInterfaceDescriptor audStreamingOutNoIsochronous;
    /// Streaming out interface descriptor.
    USBInterfaceDescriptor audStreamingOut;
    /// Audio class descriptor for the streaming out interface.
    AUDStreamingInterfaceDescriptor audStreamingOutClass;
    /// Stream format descriptor.
    AUDFormatTypeOneDescriptor1 audStreamingOutFormatType;
    /// Streaming out endpoint descriptor.
    AUDEndpointDescriptor audStreamingOutEndpoint;
    /// Audio class descriptor for the streaming out endpoint.
    AUDDataEndpointDescriptor audStreamingOutDataEndpoint;
  #endif // (AUDIO defined)

  #if defined(usb_CDCCDC)
    /// --- CDC 1
    /// IAD 1
    USBInterfaceAssociationDescriptor cdcIAD1;
    /// Communication interface descriptor
    USBInterfaceDescriptor cdcCommunication1;
    /// CDC header functional descriptor.
    CDCHeaderDescriptor cdcHeader1;
    /// CDC call management functional descriptor.
    CDCCallManagementDescriptor cdcCallManagement1;
    /// CDC abstract control management functional descriptor.
    CDCAbstractControlManagementDescriptor cdcAbstractControlManagement1;
    /// CDC union functional descriptor (with one slave interface).
    CDCUnionDescriptor cdcUnion1;
    /// Notification endpoint descriptor.
    USBEndpointDescriptor cdcNotification1;
    /// Data interface descriptor.
    USBInterfaceDescriptor cdcData1;
    /// Data OUT endpoint descriptor.
    USBEndpointDescriptor cdcDataOut1;
    /// Data IN endpoint descriptor.
    USBEndpointDescriptor cdcDataIn1;
  #endif // (Another CDC defined)

  #if defined(usb_CDCMSD) || defined(usb_HIDMSD)
    /// --- MSD
    /// Mass storage interface descriptor.
    USBInterfaceDescriptor msdInterface;
    /// Bulk-out endpoint descriptor.
    USBEndpointDescriptor msdBulkOut;
    /// Bulk-in endpoint descriptor.
    USBEndpointDescriptor msdBulkIn;
  #endif // (MSD defined)

} __attribute__ ((packed)) CompositeDriverConfigurationDescriptors;

#ifdef __ICCARM__          // IAR
#pragma pack()             // IAR
#endif                     // IAR

//------------------------------------------------------------------------------
//         Exported variables
//------------------------------------------------------------------------------

/// Standard USB device descriptor for the composite device driver
const USBDeviceDescriptor deviceDescriptor = {

    sizeof(USBDeviceDescriptor),
    USBGenericDescriptor_DEVICE,
    USBDeviceDescriptor_USB2_00,
  #if defined(usb_HIDMSD)
    0x00,
    0x00,
    0x00,
  #else
    0xEF,// MI
    0x02,//
    0x01,//
  #endif
    CHIP_USB_ENDPOINTS_MAXPACKETSIZE(0),
    COMPOSITEDDriverDescriptors_VENDORID,
    COMPOSITEDDriverDescriptors_PRODUCTID,
    COMPOSITEDDriverDescriptors_RELEASE,
    0, // No string descriptor for manufacturer
    1, // Index of product string descriptor is #1
    0, // No string descriptor for serial number
    1 // Device has 1 possible configuration
};

#if defined(CHIP_USB_UDPHS) || defined(CHIP_USB_OTGHS)

/// USB device qualifier descriptor.
const USBDeviceQualifierDescriptor qualifierDescriptor = {

    sizeof(USBDeviceQualifierDescriptor),
    USBGenericDescriptor_DEVICEQUALIFIER,
    USBDeviceDescriptor_USB2_00,
  #if defined(usb_HIDMSD)
    0x00,
    0x00,
    0x00,
  #else
    0xEF,// MI
    0x02,//
    0x01,//
  #endif
    CHIP_USB_ENDPOINTS_MAXPACKETSIZE(0),
    1, // Device has one possible configuration
    0 // Reserved
};

#endif

/// USB configuration descriptors for the composite device driver
const CompositeDriverConfigurationDescriptors configurationDescriptors = {

    // Standard configuration descriptor
    {
        sizeof(USBConfigurationDescriptor),
        USBGenericDescriptor_CONFIGURATION,
        sizeof(CompositeDriverConfigurationDescriptors),
        COMPOSITEDDriverDescriptors_NUMINTERFACE,
        1, // This is configuration #1
        0, // No string descriptor for this configuration
        BOARD_USB_BMATTRIBUTES,
        USBConfigurationDescriptor_POWER(100)
    },

  #if defined(usb_CDCAUDIO) || defined(usb_CDCHID) || defined(usb_CDCCDC) || defined(usb_CDCMSD)
    // CDC
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
        10 // Endpoint is polled every 10ms
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
  #endif // (CDC defined)

  #if defined(usb_CDCHID) || defined(usb_HIDAUDIO) || defined(usb_HIDMSD)
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
        HIDD_Descriptors_INTERRUPTIN_POLLING
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
        HIDD_Descriptors_INTERRUPTIN_POLLING
    },
  #endif // (HID defined)

  #if defined(usb_CDCAUDIO) || defined(usb_HIDAUDIO)
    // AUDIO
    // IAD for AUDIO function
    {
        sizeof(USBInterfaceAssociationDescriptor),
        USBGenericDescriptor_INTERFACEASSOCIATION,
        AUDD_Descriptors_INTERFACE,
        2,
        AUDControlInterfaceDescriptor_CLASS,
        AUDControlInterfaceDescriptor_SUBCLASS,
        AUDControlInterfaceDescriptor_PROTOCOL,
        0  // No string descriptor for this interface
    },
    // Audio control interface standard descriptor
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        AUDD_Descriptors_CONTROL,
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
            AUDD_Descriptors_STREAMING
        },
        // Input terminal descriptor
        {
            sizeof(AUDInputTerminalDescriptor),
            AUDGenericDescriptor_INTERFACE,
            AUDGenericDescriptor_INPUTTERMINAL,
            AUDD_Descriptors_INPUTTERMINAL,
            AUDInputTerminalDescriptor_USBSTREAMING,
            AUDD_Descriptors_OUTPUTTERMINAL,
            AUDD_NUMCHANNELS,
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
            AUDD_Descriptors_OUTPUTTERMINAL,
            AUDOutputTerminalDescriptor_SPEAKER,
            AUDD_Descriptors_INPUTTERMINAL,
            AUDD_Descriptors_FEATUREUNIT,
            0 // No string descriptor
        },
        // Feature unit descriptor
        {
            {
                sizeof(AUDFeatureUnitDescriptor3),
                AUDGenericDescriptor_INTERFACE,
                AUDGenericDescriptor_FEATUREUNIT,
                AUDD_Descriptors_FEATUREUNIT,
                AUDD_Descriptors_INPUTTERMINAL,
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
        AUDD_Descriptors_STREAMING,
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
        AUDD_Descriptors_STREAMING,
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
        AUDD_Descriptors_INPUTTERMINAL,
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
            AUDD_NUMCHANNELS,
            AUDD_BYTESPERSAMPLE,
            AUDD_BYTESPERSAMPLE*8,
            1 // One discrete frequency supported
        },
        {
            AUDD_SAMPLERATE & 0xFF,
            (AUDD_SAMPLERATE >> 8) & 0xFF,
            (AUDD_SAMPLERATE >> 16) & 0xFF
        }
    },
    // Audio streaming endpoint standard descriptor
    {
        sizeof(AUDEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(
            USBEndpointDescriptor_OUT,
            AUDD_Descriptors_DATAOUT),
        USBEndpointDescriptor_ISOCHRONOUS,
        CHIP_USB_ENDPOINTS_MAXPACKETSIZE(AUDD_Descriptors_DATAOUT),
        1, // Polling interval = 2^(x-1) milliseconds (1 ms)
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
    },
  #endif // (AUDIO defined)

  #if defined(usb_CDCCDC)
    // CDC 1
    // IAD for CDC/ACM port 1
    {
        sizeof(USBInterfaceAssociationDescriptor),
        USBGenericDescriptor_INTERFACEASSOCIATION,
        CDCD_Descriptors_INTERFACENUM1,
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
        CDCD_Descriptors_INTERFACENUM1, // This is interface #2
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
        CDCD_Descriptors_INTERFACENUM1 + 1 // No associated data interface
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
        CDCD_Descriptors_INTERFACENUM1, // Number of master interface is #2
        CDCD_Descriptors_INTERFACENUM1+1 // First slave interface is #3
    },
    // Notification endpoint standard descriptor
    {
        sizeof(USBEndpointDescriptor), 
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(USBEndpointDescriptor_IN,
                                      CDCD_Descriptors_NOTIFICATION1),
        USBEndpointDescriptor_INTERRUPT,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(CDCD_Descriptors_NOTIFICATION1),
            USBEndpointDescriptor_MAXINTERRUPTSIZE_FS),
        10 // Endpoint is polled every 10ms
    },
    // Data class interface standard descriptor
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        CDCD_Descriptors_INTERFACENUM1 + 1, // This is interface #3
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
                                      CDCD_Descriptors_DATAOUT1),
        USBEndpointDescriptor_BULK,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(CDCD_Descriptors_DATAOUT1),
            USBEndpointDescriptor_MAXBULKSIZE_FS),
        0 // Must be 0 for full-speed bulk endpoints
    },
    // Bulk-IN endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(USBEndpointDescriptor_IN,
                                      CDCD_Descriptors_DATAIN1),
        USBEndpointDescriptor_BULK,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(CDCD_Descriptors_DATAIN1),
            USBEndpointDescriptor_MAXBULKSIZE_FS),
        0 // Must be 0 for full-speed bulk endpoints
    },
  #endif // (2 CDCs defined)

  #if defined(usb_CDCMSD) || defined(usb_HIDMSD)
    // Mass Storage interface descriptor.
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        MSDD_Descriptors_INTERFACENUM,
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
            MSDD_Descriptors_BULKOUT),
        USBEndpointDescriptor_BULK,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(MSDD_Descriptors_BULKOUT),
            USBEndpointDescriptor_MAXBULKSIZE_FS),
        0 // No string descriptor for endpoint.
    },
    // Bulk-IN endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS(
            USBEndpointDescriptor_IN,
            MSDD_Descriptors_BULKIN),
        USBEndpointDescriptor_BULK,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(MSDD_Descriptors_BULKIN),
            USBEndpointDescriptor_MAXBULKSIZE_FS),
        0 // No string descriptor for endpoint.
    },
  #endif // (MSD defined)

};

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
//         Exported variables
//------------------------------------------------------------------------------

/// List of descriptors required by an USB audio speaker device driver.
const USBDDriverDescriptors compositedDriverDescriptors = {

    &deviceDescriptor,
    (const USBConfigurationDescriptor *) &configurationDescriptors,
#if defined (CHIP_USB_UDPHS) || defined(CHIP_USB_OTGHS)
    &qualifierDescriptor,
    (const USBConfigurationDescriptor *) &configurationDescriptors,
    &deviceDescriptor,
    (const USBConfigurationDescriptor *) &configurationDescriptors,
    &qualifierDescriptor,
    (const USBConfigurationDescriptor *) &configurationDescriptors,
#else
    0, 0, 0, 0, 0, 0,
#endif
    stringDescriptors,
    4 // Number of string descriptors
};

#if defined(usb_CDCHID) || defined(usb_HIDAUDIO) || defined(usb_HIDMSD)
/// Report descriptor used by the driver.
const unsigned char hiddReportDescriptor[] = {

    HIDReport_GLOBAL_USAGEPAGE + 1, HIDGenericDesktop_PAGEID,
    HIDReport_LOCAL_USAGE + 1, HIDGenericDesktop_KEYBOARD,
    HIDReport_COLLECTION + 1, HIDReport_COLLECTION_APPLICATION,

        // Input report: modifier keys
        HIDReport_GLOBAL_REPORTSIZE + 1, 1,
        HIDReport_GLOBAL_REPORTCOUNT + 1, 8,
        HIDReport_GLOBAL_USAGEPAGE + 1, HIDKeypad_PAGEID,
        HIDReport_LOCAL_USAGEMINIMUM + 1,
            HIDD_Descriptors_FIRSTMODIFIERKEY,
        HIDReport_LOCAL_USAGEMAXIMUM + 1,
            HIDD_Descriptors_LASTMODIFIERKEY,
        HIDReport_GLOBAL_LOGICALMINIMUM + 1, 0,
        HIDReport_GLOBAL_LOGICALMAXIMUM + 1, 1,
        HIDReport_INPUT + 1, HIDReport_VARIABLE,

        // Input report: standard keys
        HIDReport_GLOBAL_REPORTCOUNT + 1, 3,
        HIDReport_GLOBAL_REPORTSIZE + 1, 8,
        HIDReport_GLOBAL_LOGICALMINIMUM + 1,
            HIDD_Descriptors_FIRSTSTANDARDKEY,
        HIDReport_GLOBAL_LOGICALMAXIMUM + 1,
            HIDD_Descriptors_LASTSTANDARDKEY,
        HIDReport_GLOBAL_USAGEPAGE + 1, HIDKeypad_PAGEID,
        HIDReport_LOCAL_USAGEMINIMUM + 1,
            HIDD_Descriptors_FIRSTSTANDARDKEY,
        HIDReport_LOCAL_USAGEMAXIMUM + 1,
            HIDD_Descriptors_LASTSTANDARDKEY,
        HIDReport_INPUT + 1, 0 /* Data array */,

        // Output report: LEDs
        HIDReport_GLOBAL_REPORTCOUNT + 1, 3,
        HIDReport_GLOBAL_REPORTSIZE + 1, 1,
        HIDReport_GLOBAL_USAGEPAGE + 1, HIDLeds_PAGEID,
        HIDReport_GLOBAL_LOGICALMINIMUM + 1, 0,
        HIDReport_GLOBAL_LOGICALMAXIMUM + 1, 1,
        HIDReport_LOCAL_USAGEMINIMUM + 1, HIDLeds_NUMLOCK,
        HIDReport_LOCAL_USAGEMAXIMUM + 1, HIDLeds_SCROLLLOCK,
        HIDReport_OUTPUT + 1, HIDReport_VARIABLE,

        // Output report: padding
        HIDReport_GLOBAL_REPORTCOUNT + 1, 1,
        HIDReport_GLOBAL_REPORTSIZE + 1, 5,
        HIDReport_OUTPUT + 1, HIDReport_CONSTANT,

    HIDReport_ENDCOLLECTION
};
#endif // (HID defined)

