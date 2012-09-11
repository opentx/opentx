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
/// !Purpose
/// 
/// CCID driver
/// 
/// !Usage
/// 
/// Explanation on the usage of the code made available through the header file.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//       Headers
//------------------------------------------------------------------------------

#include <board.h>
#include <utility/trace.h>
#include <usb/device/core/USBD.h>
#include <usb/device/core/USBDDriver.h>
#include <usb/common/core/USBGenericRequest.h>
#include <usb/common/core/USBStringDescriptor.h>
#include <usb/device/ccid/cciddriver.h>
#include <usb/device/ccid/cciddriverdescriptors.h>
#include <iso7816/iso7816_4.h>
#include <string.h>

//------------------------------------------------------------------------------
//         Local definition
//------------------------------------------------------------------------------

/// Constants: IDs: Device product ID.
#define CCIDDriverDescriptors_PRODUCTID       0x6129
/// Constants: IDs: Device vendor ID.
#define CCIDDriverDescriptors_VENDORID        0x03EB
/// Constants: IDs: Device release number.
#define CCIDDriverDescriptors_RELEASE         0x0100

/// Returns the minimum between two values.
#define MIN(a, b)       ((a < b) ? a : b)

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

/// CCIDDriverConfiguration Descriptors
/// List of descriptors that make up the configuration descriptors of a
/// device using the CCID driver.
typedef struct {

    /// Configuration descriptor
    USBConfigurationDescriptor configuration;
    /// Interface descriptor
    USBInterfaceDescriptor     interface;
    /// CCID descriptor
    CCIDDescriptor             ccid;
    /// Bulk OUT endpoint descriptor
    USBEndpointDescriptor      bulkOut;
    /// Bulk IN endpoint descriptor
    USBEndpointDescriptor      bulkIn;
    /// Interrupt OUT endpoint descriptor
    USBEndpointDescriptor      interruptIn;

} __attribute__ ((packed)) CCIDDriverConfigurationDescriptors;

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

/// Driver structure for an CCID device
typedef struct {

    /// Standard USB device driver instance
    USBDDriver             usbdDriver;
    /// CCID message
    S_ccid_bulk_in_header  sCcidMessage;
    /// CCID command
    S_ccid_bulk_out_header sCcidCommand;
    /// Interrupt message answer
    unsigned char          BufferINT[4];
    /// Buffer data of message
    unsigned char          ProtocolDataStructure[10];
    /// Protocol used
    unsigned char          bProtocol;
    /// SlotStatus
    /// Bit 0 = Slot 0 current state
    /// Bit 1 = Slot 0 changed status
    /// Bit 2 = Slot 1 current state
    /// Bit 3 = Slot 1 changed status
    /// Bit 4 = Slot 2 current state
    /// Bit 5 = Slot 2 changed status
    unsigned char          SlotStatus;

} CCIDDriver;

//------------------------------------------------------------------------------
//         Local variables
//------------------------------------------------------------------------------

/// Static instance of the CCID device driver.
static CCIDDriver ccidDriver;

/// Standard USB device descriptor.
static const USBDeviceDescriptor deviceDescriptor = {

    sizeof(USBDeviceDescriptor),
    USBGenericDescriptor_DEVICE,
    USBDeviceDescriptor_USB2_00,
    0,
    0,
    0,
    CHIP_USB_ENDPOINTS_MAXPACKETSIZE(0),
    CCIDDriverDescriptors_VENDORID,
    CCIDDriverDescriptors_PRODUCTID,
    CCIDDriverDescriptors_RELEASE,
    1, // Index of manufacturer description
    2, // Index of product description
    3, // Index of serial number description
    1  // One possible configuration
};


/// List of configuration descriptors.
static const CCIDDriverConfigurationDescriptors configurationDescriptorsFS = {

    // Standard USB configuration descriptor
    {
        sizeof(USBConfigurationDescriptor),
        USBGenericDescriptor_CONFIGURATION,
        sizeof(CCIDDriverConfigurationDescriptors),
        1, // One interface in this configuration
        1, // This is configuration #1
        0, // No associated string descriptor
        BOARD_USB_BMATTRIBUTES,
        USBConfigurationDescriptor_POWER(100)
    },
    // CCID interface descriptor
    // Table 4.3-1 Interface Descriptor
    // Interface descriptor
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        0,                       // Interface 0
        0,                       // No alternate settings
        3,                       // uses bulk-IN, bulk-OUT and interrupt–IN
        SMART_CARD_DEVICE_CLASS,
        0,                       // Subclass code
        0,                       // bulk transfers optional interrupt-IN
        0                        // No associated string descriptor
    },
    {
        sizeof(CCIDDescriptor), // bLength: Size of this descriptor in bytes
        CCID_DECRIPTOR_TYPE,    // bDescriptorType:Functional descriptor type
        CCID1_10,               // bcdCCID: CCID version
        0,               // bMaxSlotIndex: Value 0 indicates that one slot is supported
        VOLTS_5_0,       // bVoltageSupport
        PROTOCOL_TO,     // dwProtocols
        3580,            // dwDefaultClock
        3580,            // dwMaxClock
        0,               // bNumClockSupported
        9600,            // dwDataRate : 9600 bauds
        9600,            // dwMaxDataRate : 9600 bauds
        0,               // bNumDataRatesSupported
        0xfe,            // dwMaxIFSD
        0,               // dwSynchProtocols
        0,               // dwMechanical
        //0x00010042,      // dwFeatures: Short APDU level exchanges
        CCID_FEATURES_AUTO_PCONF | CCID_FEATURES_AUTO_PNEGO | CCID_FEATURES_EXC_TPDU,
        0x0000010F,      // dwMaxCCIDMessageLength: For extended APDU level the value shall be between 261 + 10
        0xFF,            // bClassGetResponse: Echoes the class of the APDU
        0xFF,            // bClassEnvelope: Echoes the class of the APDU
        0,               // wLcdLayout: no LCD
        0,               // bPINSupport: No PIN
        1                // bMaxCCIDBusySlot
    },
    // Bulk-OUT endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS( USBEndpointDescriptor_OUT, CCID_EPT_DATA_OUT ),
        USBEndpointDescriptor_BULK,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(CCID_EPT_DATA_OUT),
            USBEndpointDescriptor_MAXBULKSIZE_FS),
        0x00                               // Does not apply to Bulk endpoints
    },
    // Bulk-IN endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS( USBEndpointDescriptor_IN, CCID_EPT_DATA_IN ),
        USBEndpointDescriptor_BULK,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(CCID_EPT_DATA_IN),
            USBEndpointDescriptor_MAXBULKSIZE_FS),
        0x00                               // Does not apply to Bulk endpoints
    },
    // Notification endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS( USBEndpointDescriptor_IN, CCID_EPT_NOTIFICATION ),
        USBEndpointDescriptor_INTERRUPT,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(CCID_EPT_NOTIFICATION),
            USBEndpointDescriptor_MAXINTERRUPTSIZE_FS),
        0x10                              
    }
};

#if defined (CHIP_USB_UDPHS) || defined(CHIP_USB_OTGHS)
static const CCIDDriverConfigurationDescriptors configurationDescriptorsHS = {

    // Standard USB configuration descriptor
    {
        sizeof(USBConfigurationDescriptor),
        USBGenericDescriptor_CONFIGURATION,
        sizeof(CCIDDriverConfigurationDescriptors),
        1, // One interface in this configuration
        1, // This is configuration #1
        0, // No associated string descriptor
        BOARD_USB_BMATTRIBUTES,
        USBConfigurationDescriptor_POWER(100)
    },
    // CCID interface descriptor
    // Table 4.3-1 Interface Descriptor
    // Interface descriptor
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        0,                       // Interface 0
        0,                       // No alternate settings
        3,                       // uses bulk-IN, bulk-OUT and interrupt–IN
        SMART_CARD_DEVICE_CLASS,
        0,                       // Subclass code
        0,                       // bulk transfers optional interrupt-IN
        0                        // No associated string descriptor
    },
    {
        sizeof(CCIDDescriptor), // bLength: Size of this descriptor in bytes
        CCID_DECRIPTOR_TYPE,    // bDescriptorType:Functional descriptor type
        CCID1_10,               // bcdCCID: CCID version
        0,               // bMaxSlotIndex: Value 0 indicates that one slot is supported
        VOLTS_5_0,       // bVoltageSupport
        PROTOCOL_TO,     // dwProtocols
        3580,            // dwDefaultClock
        3580,            // dwMaxClock
        0,               // bNumClockSupported
        9600,            // dwDataRate : 9600 bauds
        9600,            // dwMaxDataRate : 9600 bauds
        0,               // bNumDataRatesSupported
        0xfe,            // dwMaxIFSD
        0,               // dwSynchProtocols
        0,               // dwMechanical
        //0x00010042,      // dwFeatures: Short APDU level exchanges
        CCID_FEATURES_AUTO_PCONF | CCID_FEATURES_AUTO_PNEGO | CCID_FEATURES_EXC_TPDU,
        0x0000010F,      // dwMaxCCIDMessageLength: For extended APDU level the value shall be between 261 + 10
        0xFF,            // bClassGetResponse: Echoes the class of the APDU
        0xFF,            // bClassEnvelope: Echoes the class of the APDU
        0,               // wLcdLayout: no LCD
        0,               // bPINSupport: No PIN
        1                // bMaxCCIDBusySlot
    },
    // Bulk-OUT endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS( USBEndpointDescriptor_OUT, CCID_EPT_DATA_OUT ),
        USBEndpointDescriptor_BULK,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(CCID_EPT_DATA_OUT),
            USBEndpointDescriptor_MAXBULKSIZE_HS),
        0x00                               // Does not apply to Bulk endpoints
    },
    // Bulk-IN endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS( USBEndpointDescriptor_IN, CCID_EPT_DATA_IN ),
        USBEndpointDescriptor_BULK,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(CCID_EPT_DATA_IN),
            USBEndpointDescriptor_MAXBULKSIZE_HS),
        0x00                               // Does not apply to Bulk endpoints
    },
    // Notification endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS( USBEndpointDescriptor_IN, CCID_EPT_NOTIFICATION ),
        USBEndpointDescriptor_INTERRUPT,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(CCID_EPT_NOTIFICATION),
            USBEndpointDescriptor_MAXINTERRUPTSIZE_HS),
        0x10                              
    }
};

/// Qualifier descriptor
const USBDeviceQualifierDescriptor deviceQualifierDescriptor = {

    sizeof(USBDeviceQualifierDescriptor),  // Size of this descriptor in bytes
    USBGenericDescriptor_DEVICEQUALIFIER,  // Qualifier Descriptor Type
    USBDeviceDescriptor_USB2_00,           // USB specification 2.00
    0x00,                                  // Class is specified in interface
    0x00,                                  // Subclass is specified in interface
    0x00,                                  // Protocol is specified in interface
    CHIP_USB_ENDPOINTS_MAXPACKETSIZE(0),    
    0x01,                                  // One possible configuration
    0x00                                   // Reserved for future use, must be zero
};

/// OtherSpeed configuration descriptor in Full Speed mode
static const CCIDDriverConfigurationDescriptors sOtherSpeedConfigurationFS = {

    // Standard USB configuration descriptor
    {
        sizeof(USBConfigurationDescriptor),
        USBGenericDescriptor_OTHERSPEEDCONFIGURATION,
        sizeof(CCIDDriverConfigurationDescriptors),
        1, // One interface in this configuration
        1, // This is configuration #1
        0, // No associated string descriptor
        BOARD_USB_BMATTRIBUTES,
        USBConfigurationDescriptor_POWER(100)
    },
    // CCID interface descriptor
    // Table 4.3-1 Interface Descriptor
    // Interface descriptor
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        0,                       // Interface 0
        0,                       // No alternate settings
        3,                       // uses bulk-IN, bulk-OUT and interrupt–IN
        SMART_CARD_DEVICE_CLASS,
        0,                       // Subclass code
        0,                       // bulk transfers optional interrupt-IN
        0                        // No associated string descriptor
    },
    {
        sizeof(CCIDDescriptor), // bLength: Size of this descriptor in bytes
        CCID_DECRIPTOR_TYPE,    // bDescriptorType:Functional descriptor type
        CCID1_10,               // bcdCCID: CCID version
        0,               // bMaxSlotIndex: Value 0 indicates that one slot is supported
        VOLTS_5_0,       // bVoltageSupport
        PROTOCOL_TO,     // dwProtocols
        3580,            // dwDefaultClock
        3580,            // dwMaxClock
        0,               // bNumClockSupported
        9600,            // dwDataRate : 9600 bauds
        9600,            // dwMaxDataRate : 9600 bauds
        0,               // bNumDataRatesSupported
        0xfe,            // dwMaxIFSD
        0,               // dwSynchProtocols
        0,               // dwMechanical
        //0x00010042,      // dwFeatures: Short APDU level exchanges
        CCID_FEATURES_AUTO_PCONF | CCID_FEATURES_AUTO_PNEGO | CCID_FEATURES_EXC_TPDU,
        0x0000010F,      // dwMaxCCIDMessageLength: For extended APDU level the value shall be between 261 + 10
        0xFF,            // bClassGetResponse: Echoes the class of the APDU
        0xFF,            // bClassEnvelope: Echoes the class of the APDU
        0,               // wLcdLayout: no LCD
        0,               // bPINSupport: No PIN
        1                // bMaxCCIDBusySlot
    },
    // Bulk-OUT endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS( USBEndpointDescriptor_OUT, CCID_EPT_DATA_OUT ),
        USBEndpointDescriptor_BULK,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(CCID_EPT_DATA_OUT),
            USBEndpointDescriptor_MAXBULKSIZE_FS),
        0x00                               // Does not apply to Bulk endpoints
    },
    // Bulk-IN endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS( USBEndpointDescriptor_IN, CCID_EPT_DATA_IN ),
        USBEndpointDescriptor_BULK,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(CCID_EPT_DATA_IN),
            USBEndpointDescriptor_MAXBULKSIZE_FS),
        0x00                               // Does not apply to Bulk endpoints
    },
    // Notification endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS( USBEndpointDescriptor_IN, CCID_EPT_NOTIFICATION ),
        USBEndpointDescriptor_INTERRUPT,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(CCID_EPT_NOTIFICATION),
            USBEndpointDescriptor_MAXINTERRUPTSIZE_FS),
        0x10                              
    }
};

/// OtherSpeed configuration descriptor in High Speed mode
static const CCIDDriverConfigurationDescriptors sOtherSpeedConfigurationHS = {

    // Standard USB configuration descriptor
    {
        sizeof(USBConfigurationDescriptor),
        USBGenericDescriptor_OTHERSPEEDCONFIGURATION,
        sizeof(CCIDDriverConfigurationDescriptors),
        1, // One interface in this configuration
        1, // This is configuration #1
        0, // No associated string descriptor
        BOARD_USB_BMATTRIBUTES,
        USBConfigurationDescriptor_POWER(100)
    },
    // CCID interface descriptor
    // Table 4.3-1 Interface Descriptor
    // Interface descriptor
    {
        sizeof(USBInterfaceDescriptor),
        USBGenericDescriptor_INTERFACE,
        0,                       // Interface 0
        0,                       // No alternate settings
        3,                       // uses bulk-IN, bulk-OUT and interrupt–IN
        SMART_CARD_DEVICE_CLASS,
        0,                       // Subclass code
        0,                       // bulk transfers optional interrupt-IN
        0                        // No associated string descriptor
    },
    {
        sizeof(CCIDDescriptor), // bLength: Size of this descriptor in bytes
        CCID_DECRIPTOR_TYPE,    // bDescriptorType:Functional descriptor type
        CCID1_10,               // bcdCCID: CCID version
        0,               // bMaxSlotIndex: Value 0 indicates that one slot is supported
        VOLTS_5_0,       // bVoltageSupport
        PROTOCOL_TO,     // dwProtocols
        3580,            // dwDefaultClock
        3580,            // dwMaxClock
        0,               // bNumClockSupported
        9600,            // dwDataRate : 9600 bauds
        9600,            // dwMaxDataRate : 9600 bauds
        0,               // bNumDataRatesSupported
        0xfe,            // dwMaxIFSD
        0,               // dwSynchProtocols
        0,               // dwMechanical
        //0x00010042,      // dwFeatures: Short APDU level exchanges
        CCID_FEATURES_AUTO_PCONF | CCID_FEATURES_AUTO_PNEGO | CCID_FEATURES_EXC_TPDU,
        0x0000010F,      // dwMaxCCIDMessageLength: For extended APDU level the value shall be between 261 + 10
        0xFF,            // bClassGetResponse: Echoes the class of the APDU
        0xFF,            // bClassEnvelope: Echoes the class of the APDU
        0,               // wLcdLayout: no LCD
        0,               // bPINSupport: No PIN
        1                // bMaxCCIDBusySlot
    },
    // Bulk-OUT endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS( USBEndpointDescriptor_OUT, CCID_EPT_DATA_OUT ),
        USBEndpointDescriptor_BULK,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(CCID_EPT_DATA_OUT),
            USBEndpointDescriptor_MAXBULKSIZE_HS),
        0x00                               // Does not apply to Bulk endpoints
    },
    // Bulk-IN endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS( USBEndpointDescriptor_IN, CCID_EPT_DATA_IN ),
        USBEndpointDescriptor_BULK,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(CCID_EPT_DATA_IN),
            USBEndpointDescriptor_MAXBULKSIZE_HS),
        0x00                               // Does not apply to Bulk endpoints
    },
    // Notification endpoint descriptor
    {
        sizeof(USBEndpointDescriptor),
        USBGenericDescriptor_ENDPOINT,
        USBEndpointDescriptor_ADDRESS( USBEndpointDescriptor_IN, CCID_EPT_NOTIFICATION ),
        USBEndpointDescriptor_INTERRUPT,
        MIN(CHIP_USB_ENDPOINTS_MAXPACKETSIZE(CCID_EPT_NOTIFICATION),
            USBEndpointDescriptor_MAXINTERRUPTSIZE_HS),
        0x10                              
    }
};
#endif

/// Language ID string descriptor.
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
    USBStringDescriptor_UNICODE('T'),
    USBStringDescriptor_UNICODE('M'),
    USBStringDescriptor_UNICODE('E'),
    USBStringDescriptor_UNICODE('L')
};

/// Product name.
static const unsigned char productDescriptor[] = {

    USBStringDescriptor_LENGTH(23),
    USBGenericDescriptor_STRING,
    USBStringDescriptor_UNICODE('A'),
    USBStringDescriptor_UNICODE('T'),
    USBStringDescriptor_UNICODE('M'),
    USBStringDescriptor_UNICODE('E'),
    USBStringDescriptor_UNICODE('L'),
    USBStringDescriptor_UNICODE(' '),
    USBStringDescriptor_UNICODE('A'),
    USBStringDescriptor_UNICODE('T'),
    USBStringDescriptor_UNICODE('9'),
    USBStringDescriptor_UNICODE('1'),
    USBStringDescriptor_UNICODE(' '),
    USBStringDescriptor_UNICODE('C'),
    USBStringDescriptor_UNICODE('C'),
    USBStringDescriptor_UNICODE('I'),
    USBStringDescriptor_UNICODE('D'),
    USBStringDescriptor_UNICODE(' '),
    USBStringDescriptor_UNICODE('D'),
    USBStringDescriptor_UNICODE('R'),
    USBStringDescriptor_UNICODE('I'),
    USBStringDescriptor_UNICODE('V'),
    USBStringDescriptor_UNICODE('E'),
    USBStringDescriptor_UNICODE('R'),
    USBStringDescriptor_UNICODE(' ')
};

/// Product serial number.
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

/// Array of pointers to string descriptors.
static const unsigned char *stringDescriptors[] = {

    languageIdDescriptor,
    manufacturerDescriptor,
    productDescriptor,
    serialNumberDescriptor
};


/// List of standard descriptors for the serial driver.
const USBDDriverDescriptors ccidDriverDescriptors = {

    &deviceDescriptor, // FS
    (USBConfigurationDescriptor *) &configurationDescriptorsFS,
#if defined (CHIP_USB_UDPHS) || defined(CHIP_USB_OTGHS)
    (USBDeviceQualifierDescriptor *) &deviceQualifierDescriptor, // FS
    (USBConfigurationDescriptor *) &sOtherSpeedConfigurationFS,
    &deviceDescriptor, // HS
    (USBConfigurationDescriptor *) &configurationDescriptorsHS,
    (USBDeviceQualifierDescriptor *) &deviceQualifierDescriptor, // HS
    (USBConfigurationDescriptor *) &sOtherSpeedConfigurationHS,
#else
    0, // No qualifier descriptor FS
    0, // No other-speed configuration FS
    0, // No device descriptor HS
    0, // No configuration HS
    0, // No qualifier descriptor HS
    0, // No other-speed configuration HS
#endif
    stringDescriptors,
    4 // Four string descriptors in array
};

//------------------------------------------------------------------------------
//      Internal functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Response Pipe, Bulk-IN Messages
/// Return the Slot Status to the host
/// Answer to:
///   PC_to_RDR_IccPowerOff
///   PC_to_RDR_GetSlotStatus
///   PC_to_RDR_IccClock
///   PC_to_RDR_T0APDU
///   PC_to_RDR_Mechanical
///   PC_to_RDR_Abort and Class specific ABORT request
//------------------------------------------------------------------------------
static void RDRtoPCSlotStatus( void )
{
    TRACE_DEBUG("RDRtoPCSlotStatus\n\r");

    // Header fields settings
    ccidDriver.sCcidMessage.bMessageType = RDR_TO_PC_SLOTSTATUS;
    ccidDriver.sCcidMessage.wLength   = 0;
    ccidDriver.sCcidMessage.bStatus   = ccidDriver.SlotStatus;
    ccidDriver.sCcidMessage.bError    = 0;
    // 00h Clock running
    // 01h Clock stopped in state L
    // 02h Clock stopped in state H
    // 03h Clock stopped in an unknown state
    // All other values are Reserved for Future Use.
    ccidDriver.sCcidMessage.bSpecific = 0;
}

//------------------------------------------------------------------------------
/// Response Pipe, Bulk-IN Messages
/// Answer to PC_to_RDR_IccPowerOn
//------------------------------------------------------------------------------
static void RDRtoPCDatablock_ATR( void )
{
    unsigned char i;
    unsigned char Atr[ATR_SIZE_MAX];
    unsigned char length;

    //TRACE_DEBUG("RDRtoPCDatablock\n\r");

    ISO7816_Datablock_ATR( Atr, &length );

    if( length > 5 ) {
        ccidDriver.ProtocolDataStructure[1] = Atr[5]&0x0F; // TD(1)
        ccidDriver.bProtocol = Atr[5]&0x0F;           // TD(1)
    }

    // S_ccid_protocol_t0
    // bmFindexDindex
    ccidDriver.ProtocolDataStructure[0] = Atr[2];     // TA(1)

    // bmTCCKST0
    // For T=0 ,B0 – 0b, B7-2 – 000000b
    // B1 – Convention used (b1=0 for direct, b1=1 for inverse)

    // bGuardTimeT0
    // Extra Guardtime between two characters. Add 0 to 254 etu to the normal 
    // guardtime of 12etu. FFh is the same as 00h.
    ccidDriver.ProtocolDataStructure[2] = Atr[4];     // TC(1)
    // AT91C_BASE_US0->US_TTGR = 0;  // TC1

    // bWaitingIntegerT0
    // WI for T=0 used to define WWT
    ccidDriver.ProtocolDataStructure[3] = Atr[7];     // TC(2)

    // bClockStop
    // ICC Clock Stop Support
    // 00 = Stopping the Clock is not allowed
    // 01 = Stop with Clock signal Low
    // 02 = Stop with Clock signal High
    // 03 = Stop with Clock either High or Low
    ccidDriver.ProtocolDataStructure[4] = 0x00;       // 0 to 3

    // Header fields settings
    ccidDriver.sCcidMessage.bMessageType = RDR_TO_PC_DATABLOCK;
    ccidDriver.sCcidMessage.wLength      = length;  // Size of ATR
    ccidDriver.sCcidMessage.bSizeToSend += length;  // Size of ATR
    // bChainParameter: 00 the response APDU begins and ends in this command
    ccidDriver.sCcidMessage.bSpecific    = 0;

    for( i=0; i<length; i++ ) {

        ccidDriver.sCcidMessage.abData[i]  = Atr[i];
    }

    // Set the slot to an active status
    ccidDriver.sCcidMessage.bStatus = 0;
    ccidDriver.sCcidMessage.bError = 0;
}

//------------------------------------------------------------------------------
/// Response Pipe, Bulk-IN Messages
/// In other cases, the response message has the following format: 
/// The response data will contain the optional data returned by the ICC, 
/// followed by the 2 byte-size status words SW1-SW2.
///
/// Answer to:
///   PC_to_RDR_XfrBlock
///   PC_to_RDR_Secure
//------------------------------------------------------------------------------
static void RDRtoPCDatablock( void )
{
    //TRACE_DEBUG("RDRtoPCDatablock\n\r");

    // Header fields settings
    ccidDriver.sCcidMessage.bMessageType = RDR_TO_PC_DATABLOCK;
    ccidDriver.sCcidMessage.bSizeToSend += ccidDriver.sCcidMessage.wLength;
    // bChainParameter: 00 the response APDU begins and ends in this command
    ccidDriver.sCcidMessage.bSpecific = 0;

    // Set the slot to an active status
    ccidDriver.sCcidMessage.bStatus = 0;
    ccidDriver.sCcidMessage.bError = 0;
}

//------------------------------------------------------------------------------
/// Response Pipe, Bulk-IN Messages
/// Answer to:
///   PC_to_RDR_GetParameters
///   PC_to_RDR_ResetParameters
///   PC_to_RDR_SetParameters
//------------------------------------------------------------------------------
static void RDRtoPCParameters( void )
{
    unsigned int i;

    TRACE_DEBUG("RDRtoPCParameters\n\r");

    // Header fields settings
    ccidDriver.sCcidMessage.bMessageType = RDR_TO_PC_PARAMETERS;

    //ccidDriver.sCcidMessage.bStatus = 0;
    ccidDriver.sCcidMessage.bError  = 0;

    if( ccidDriver.ProtocolDataStructure[1] == PROTOCOL_TO ) {

        // T=0
        ccidDriver.sCcidMessage.wLength   = sizeof(S_ccid_protocol_t0);
        ccidDriver.sCcidMessage.bSpecific = PROTOCOL_TO;
    }
    else {

        // T=1
        ccidDriver.sCcidMessage.wLength   = sizeof(S_ccid_protocol_t1);
        ccidDriver.sCcidMessage.bSpecific = PROTOCOL_T1;
    }

    ccidDriver.sCcidMessage.bSizeToSend += ccidDriver.sCcidMessage.wLength;

    for( i=0; i<ccidDriver.sCcidMessage.wLength; i++ ) {
        ccidDriver.sCcidMessage.abData[i] = ccidDriver.ProtocolDataStructure[i];
    }

}

//------------------------------------------------------------------------------
/// Response Pipe, Bulk-IN Messages
/// Answer to:
///   PC_to_RDR_Escape
//------------------------------------------------------------------------------
static void RDRtoPCEscape( unsigned char length, unsigned char *data_send_from_CCID )
{
    unsigned int i;

    TRACE_DEBUG("RDRtoPCEscape\n\r");

    // Header fields settings
    ccidDriver.sCcidMessage.bMessageType = RDR_TO_PC_ESCAPE;

    ccidDriver.sCcidMessage.wLength   = length;

    ccidDriver.sCcidMessage.bStatus = 0;
    ccidDriver.sCcidMessage.bError  = 0;

    ccidDriver.sCcidMessage.bSpecific = 0;  // bRFU

    for( i=0; i<length; i++ ) {
        ccidDriver.sCcidMessage.abData[i] = data_send_from_CCID[i];
    }
}

//------------------------------------------------------------------------------
/// Response Pipe, Bulk-IN Messages
/// Answer to: 
///   PC_to_RDR_SetDataRateAndClockFrequency
//------------------------------------------------------------------------------
static void RDRtoPCDataRateAndClockFrequency( unsigned int dwClockFrequency, 
                                       unsigned int dwDataRate )
{
    TRACE_DEBUG("RDRtoPCDataRateAndClockFrequency\n\r");

    // Header fields settings
    ccidDriver.sCcidMessage.bMessageType = RDR_TO_PC_DATARATEANDCLOCKFREQUENCY;

    ccidDriver.sCcidMessage.wLength   = 8;

    ccidDriver.sCcidMessage.bStatus = 0;
    ccidDriver.sCcidMessage.bError  = 0;

    ccidDriver.sCcidMessage.bSpecific = 0;  // bRFU

    ccidDriver.sCcidMessage.abData[0] = dwClockFrequency;
    
    ccidDriver.sCcidMessage.abData[4] = dwDataRate;
}

//------------------------------------------------------------------------------
/// Command Pipe, Bulk-OUT Messages
/// Power On Command - Cold Reset & Warm Reset 
/// Return the ATR to the host
//------------------------------------------------------------------------------
static void PCtoRDRIccPowerOn( void )
{
    TRACE_DEBUG("PCtoRDRIccPowerOn\n\r");

    if( CCID_FEATURES_AUTO_VOLT == (configurationDescriptorsFS.ccid.dwFeatures & CCID_FEATURES_AUTO_VOLT) ) {

        // bPowerSelect = ccidDriver.sCcidCommand.bSpecific_0;
        ccidDriver.sCcidCommand.bSpecific_0 = VOLTS_AUTO;
    }

    ISO7816_cold_reset();

    // for emulation only //JCB 
    if ( ccidDriver.sCcidCommand.bSpecific_0 != VOLTS_5_0 ) {

        TRACE_ERROR("POWER_NOT_SUPPORTED\n\r");
    }

    else {

        RDRtoPCDatablock_ATR();

    }
}

//------------------------------------------------------------------------------
/// Command Pipe, Bulk-OUT Messages
/// Power Off Command - Set the ICC in an inactive state
/// Return the slot status to the host
//------------------------------------------------------------------------------
static void PCtoRDRIccPowerOff( void )
{
    unsigned char bStatus;

    TRACE_DEBUG("PCtoRDRIccPowerOff\n\r");

    ISO7816_IccPowerOff();

    //JCB stub
    bStatus = ICC_BS_PRESENT_NOTACTIVATED;

    // Set the slot to an inactive status
    ccidDriver.sCcidMessage.bStatus = 0;
    ccidDriver.sCcidMessage.bError = 0;

    // if error, see Table 6.1-2 errors

    // Return the slot status to the host
    RDRtoPCSlotStatus();
}

//------------------------------------------------------------------------------
/// Command Pipe, Bulk-OUT Messages
/// Get slot status
//------------------------------------------------------------------------------
static void PCtoRDRGetSlotStatus( void )
{
    TRACE_DEBUG("PCtoRDRGetSlotStatus\n\r");

    ccidDriver.sCcidMessage.bStatus = 0;
    ccidDriver.sCcidMessage.bError = 0;

    // Return the slot status to the host
    RDRtoPCSlotStatus();
}

//------------------------------------------------------------------------------
/// Command Pipe, Bulk-OUT Messages
/// If the command header is valid, an APDU command is received and can be read
/// by the application
//------------------------------------------------------------------------------
static void PCtoRDRXfrBlock( void )
{
    unsigned char indexMessage = 0;
    unsigned char i;

    //TRACE_DEBUG("PCtoRDRXfrBlock\n\r");

    i = 0;

    // Check the block length
    if ( ccidDriver.sCcidCommand.wLength > (configurationDescriptorsFS.ccid.dwMaxCCIDMessageLength-10) ) {

        ccidDriver.sCcidMessage.bStatus = 1;
        ccidDriver.sCcidMessage.bError  = 0;
    }
    // check bBWI
    else if ( 0 != ccidDriver.sCcidCommand.bSpecific_0 ) {

         TRACE_ERROR("Bad bBWI\n\r");
    }
    else {

        // APDU or TPDU
        switch(configurationDescriptorsFS.ccid.dwFeatures 
              & (CCID_FEATURES_EXC_TPDU|CCID_FEATURES_EXC_SAPDU|CCID_FEATURES_EXC_APDU)) {

            case CCID_FEATURES_EXC_TPDU:
                if (ccidDriver.ProtocolDataStructure[1] == PROTOCOL_TO) {

                    // Send commande APDU
                    indexMessage = ISO7816_XfrBlockTPDU_T0( ccidDriver.sCcidCommand.APDU , 
                                            ccidDriver.sCcidMessage.abData, 
                                            ccidDriver.sCcidCommand.wLength );
                }
                else {
                    if (ccidDriver.ProtocolDataStructure[1] == PROTOCOL_T1) {
                        TRACE_INFO("Not supported T=1\n\r");
                    }
                    else {
                        TRACE_INFO("Not supported\n\r");
                    }
                }
                break;

            case CCID_FEATURES_EXC_APDU:
                TRACE_INFO("Not supported\n\r");
                break;

            default:
                break;
        }

    }

    ccidDriver.sCcidMessage.wLength = indexMessage;
    TRACE_DEBUG("USB: 0x%X, 0x%X, 0x%X, 0x%X, 0x%X\n\r", ccidDriver.sCcidMessage.abData[0], 
                                                                    ccidDriver.sCcidMessage.abData[1], 
                                                                    ccidDriver.sCcidMessage.abData[2], 
                                                                    ccidDriver.sCcidMessage.abData[3],
                                                                    ccidDriver.sCcidMessage.abData[4] );
     RDRtoPCDatablock();

}

//------------------------------------------------------------------------------
/// Command Pipe, Bulk-OUT Messages
/// return parameters by the command: RDR_to_PC_Parameters
//------------------------------------------------------------------------------
static void PCtoRDRGetParameters( void )
{
    TRACE_DEBUG("PCtoRDRGetParameters\n\r");

    // We support only one slot

    // bmIccStatus
    if( ISO7816_StatusReset() ) {
        // 0: An ICC is present and active (power is on and stable, RST is inactive
        ccidDriver.sCcidMessage.bStatus = 0;
    }
    else {
        // 1: An ICC is present and inactive (not activated or shut down by hardware error)
        ccidDriver.sCcidMessage.bStatus = 1;
    }

    RDRtoPCParameters();
}

//------------------------------------------------------------------------------
/// Command Pipe, Bulk-OUT Messages
/// This command resets the slot parameters to their default values
//------------------------------------------------------------------------------
static void PCtoRDRResetParameters( void )
{
    TRACE_DEBUG("PCtoRDRResetParameters\n\r");

    ccidDriver.SlotStatus = ICC_NOT_PRESENT;
    ccidDriver.sCcidMessage.bStatus = ccidDriver.SlotStatus;

    RDRtoPCParameters();
}

//------------------------------------------------------------------------------
/// Command Pipe, Bulk-OUT Messages
/// This command is used to change the parameters for a given slot.
//------------------------------------------------------------------------------
static void PCtoRDRSetParameters( void )
{
    TRACE_DEBUG("PCtoRDRSetParameters\n\r");

    ccidDriver.SlotStatus = ccidDriver.sCcidCommand.bSlot;
    ccidDriver.sCcidMessage.bStatus = ccidDriver.SlotStatus;
    // Not all feature supported

    RDRtoPCParameters();
}

//------------------------------------------------------------------------------
/// Command Pipe, Bulk-OUT Messages
/// This command allows the CCID manufacturer to define and access extended 
/// features. 
/// Information sent via this command is processed by the CCID control logic.
//------------------------------------------------------------------------------
static void PCtoRDREscape( void )
{
    TRACE_DEBUG("PCtoRDREscape\n\r");

    // If needed by the user
    ISO7816_Escape();

    // stub, return all value send
    RDRtoPCEscape( ccidDriver.sCcidCommand.wLength, ccidDriver.sCcidCommand.APDU);    
}

//------------------------------------------------------------------------------
/// Command Pipe, Bulk-OUT Messages
/// This command stops or restarts the clock.
//------------------------------------------------------------------------------
static void PCtoRDRICCClock( void )
{
    TRACE_DEBUG("PCtoRDRICCClock\n\r");

    if( 0 == ccidDriver.sCcidCommand.bSpecific_0 ) {
        // restarts the clock
        ISO7816_RestartClock();
    }
    else {
        // stop clock in the state shown in the bClockStop field
        ISO7816_StopClock();
    }

    RDRtoPCSlotStatus( );    
}

//------------------------------------------------------------------------------
/// Command Pipe, Bulk-OUT Messages
/// This command changes the parameters used to perform the transportation of 
/// APDU messages by the T=0 protocol. 
//------------------------------------------------------------------------------
static void PCtoRDRtoAPDU( void )
{
    unsigned char bmChanges;
    unsigned char bClassGetResponse;
    unsigned char bClassEnvelope;

    TRACE_DEBUG("PCtoRDRtoAPDU\n\r");

    if( configurationDescriptorsFS.ccid.dwFeatures == (CCID_FEATURES_EXC_SAPDU|CCID_FEATURES_EXC_APDU) ) {

        bmChanges = ccidDriver.sCcidCommand.bSpecific_0;
        bClassGetResponse = ccidDriver.sCcidCommand.bSpecific_1;
        bClassEnvelope = ccidDriver.sCcidCommand.bSpecific_2;

        ISO7816_toAPDU();
    }

    RDRtoPCSlotStatus();    
}

//------------------------------------------------------------------------------
/// Command Pipe, Bulk-OUT Messages
/// This is a command message to allow entering the PIN for verification or
/// modification.
//------------------------------------------------------------------------------
static void PCtoRDRSecure( void )
{
    TRACE_DEBUG("PCtoRDRSecure\n\r");

    TRACE_DEBUG("For user\n\r");
}

//------------------------------------------------------------------------------
/// Command Pipe, Bulk-OUT Messages
/// This command is used to manage motorized type CCID functionality. 
/// The Lock Card function is used to hold the ICC. 
/// This prevents an ICC from being easily removed from the CCID. 
/// The Unlock Card function is used to remove the hold initiated by the Lock 
/// Card function
//------------------------------------------------------------------------------
static void PCtoRDRMechanical( void )
{
    TRACE_DEBUG("PCtoRDRMechanical\n\r");
    TRACE_DEBUG("Not implemented\n\r");

    RDRtoPCSlotStatus();
}

//------------------------------------------------------------------------------
/// Command Pipe, Bulk-OUT Messages
/// This command is used with the Control pipe Abort request to tell the CCID 
/// to stop any current transfer at the specified slot and return to a state 
/// where the slot is ready to accept a new command pipe Bulk-OUT message.
//------------------------------------------------------------------------------
static void PCtoRDRAbort( void )
{
    TRACE_DEBUG("PCtoRDRAbort\n\r");

    RDRtoPCSlotStatus();
}

//------------------------------------------------------------------------------
/// Command Pipe, Bulk-OUT Messages
/// This command is used to manually set the data rate and clock frequency of 
/// a specific slot.
//------------------------------------------------------------------------------
static void PCtoRDRSetDataRateAndClockFrequency( void )
{
    unsigned int dwClockFrequency;
    unsigned int dwDataRate;

    TRACE_DEBUG("PCtoRDRSetDatarateandClockFrequency\n\r");

    dwClockFrequency = ccidDriver.sCcidCommand.APDU[0]
                     + (ccidDriver.sCcidCommand.APDU[1]<<8)
                     + (ccidDriver.sCcidCommand.APDU[2]<<16)
                     + (ccidDriver.sCcidCommand.APDU[3]<<24);

    dwDataRate = ccidDriver.sCcidCommand.APDU[4]
               + (ccidDriver.sCcidCommand.APDU[5]<<8)
               + (ccidDriver.sCcidCommand.APDU[6]<<16)
               + (ccidDriver.sCcidCommand.APDU[7]<<24);

    ISO7816_SetDataRateandClockFrequency( dwClockFrequency, dwDataRate );

    RDRtoPCDataRateAndClockFrequency( dwClockFrequency, dwDataRate );

}

//------------------------------------------------------------------------------
/// Report the CMD_NOT_SUPPORTED error to the host
//------------------------------------------------------------------------------
static void vCCIDCommandNotSupported( void )
{
    // Command not supported
    // vCCIDReportError(CMD_NOT_SUPPORTED);

    TRACE_DEBUG("CMD_NOT_SUPPORTED\n\r");

    // Header fields settings
    ccidDriver.sCcidMessage.bMessageType = RDR_TO_PC_SLOTSTATUS;
    ccidDriver.sCcidMessage.wLength      = 0;
    ccidDriver.sCcidMessage.bSpecific    = 0;

    ccidDriver.sCcidMessage.bStatus |= ICC_CS_FAILED;

    // Send the response to the host
    //vCCIDSendResponse();
}

//------------------------------------------------------------------------------
/// Sent CCID response on USB
//------------------------------------------------------------------------------
static void vCCIDSendResponse( void )
{
    unsigned char bStatus;

    do {
        bStatus = USBD_Write( CCID_EPT_DATA_IN, (void*)&ccidDriver.sCcidMessage, 
                              ccidDriver.sCcidMessage.bSizeToSend, 0, 0 );
    }
    while (bStatus != USBD_STATUS_SUCCESS);
}


//------------------------------------------------------------------------------
///  Description: CCID Command dispatcher
//------------------------------------------------------------------------------
static void CCIDCommandDispatcher( void )
{
    unsigned char MessageToSend = 0;

    //TRACE_DEBUG("Command: 0x%X 0x%x 0x%X 0x%X 0x%X 0x%X 0x%X\n\r\n\r",
    //               (unsigned int)ccidDriver.sCcidCommand.bMessageType,
    //               (unsigned int)ccidDriver.sCcidCommand.wLength,
    //               (unsigned int)ccidDriver.sCcidCommand.bSlot,
    //               (unsigned int)ccidDriver.sCcidCommand.bSeq,
    //               (unsigned int)ccidDriver.sCcidCommand.bSpecific_0,
    //               (unsigned int)ccidDriver.sCcidCommand.bSpecific_1,
    //               (unsigned int)ccidDriver.sCcidCommand.bSpecific_2);

    // Check the slot number
    if ( ccidDriver.sCcidCommand.bSlot > 0 ) {

        TRACE_ERROR("BAD_SLOT_NUMBER\n\r");
    }

    TRACE_DEBUG("typ=0x%X\n\r", ccidDriver.sCcidCommand.bMessageType);

    ccidDriver.sCcidMessage.bStatus = 0;

    ccidDriver.sCcidMessage.bSeq  = ccidDriver.sCcidCommand.bSeq;
    ccidDriver.sCcidMessage.bSlot = ccidDriver.sCcidCommand.bSlot;

    ccidDriver.sCcidMessage.bSizeToSend = sizeof(S_ccid_bulk_in_header)-(ABDATA_SIZE+1);


    // Command dispatcher
    switch ( ccidDriver.sCcidCommand.bMessageType ) {

        case PC_TO_RDR_ICCPOWERON:
            PCtoRDRIccPowerOn();
            MessageToSend = 1;
            break;

        case PC_TO_RDR_ICCPOWEROFF:
            PCtoRDRIccPowerOff();
            MessageToSend = 1;
            break;

        case PC_TO_RDR_GETSLOTSTATUS:
            PCtoRDRGetSlotStatus();
            MessageToSend = 1;
            break;

        case PC_TO_RDR_XFRBLOCK:
            PCtoRDRXfrBlock();
            MessageToSend = 1;
            break;

        case PC_TO_RDR_GETPARAMETERS:
            PCtoRDRGetParameters();
            MessageToSend = 1;
            break;

        case PC_TO_RDR_RESETPARAMETERS:
            PCtoRDRResetParameters();
            MessageToSend = 1;
            break;

        case PC_TO_RDR_SETPARAMETERS:
            PCtoRDRSetParameters();
            MessageToSend = 1;
            break;

        case PC_TO_RDR_ESCAPE:
            PCtoRDREscape();
            MessageToSend = 1;
            break;

        case PC_TO_RDR_ICCCLOCK:
            PCtoRDRICCClock();
            MessageToSend = 1;
            break;

        case PC_TO_RDR_T0APDU:
            // Only CCIDs reporting a short or extended APDU level in the dwFeatures 
            // field of the CCID class descriptor may take this command into account.
            if( (CCID_FEATURES_EXC_SAPDU == (CCID_FEATURES_EXC_SAPDU&configurationDescriptorsFS.ccid.dwFeatures))
            || (CCID_FEATURES_EXC_APDU  == (CCID_FEATURES_EXC_APDU &configurationDescriptorsFS.ccid.dwFeatures)) ) {

                // command supported
                PCtoRDRtoAPDU();
            }
            else {
                // command not supported
                TRACE_DEBUG("PC_TO_RDR_T0APDU\n\r");
                vCCIDCommandNotSupported();
            }
            MessageToSend = 1;
            break;

        case PC_TO_RDR_SECURE:
            PCtoRDRSecure();
            MessageToSend = 1;
            break;

        case PC_TO_RDR_MECHANICAL:
            PCtoRDRMechanical();
            MessageToSend = 1;
            break;

        case PC_TO_RDR_ABORT:
            PCtoRDRAbort();
            MessageToSend = 1;
            break;

        case PC_TO_RDR_SETDATARATEANDCLOCKFREQUENCY:
            PCtoRDRSetDataRateAndClockFrequency();
            MessageToSend = 1;
            break;

        default:
            TRACE_DEBUG("default: 0x%X\n\r", ccidDriver.sCcidCommand.bMessageType);
            vCCIDCommandNotSupported();
            MessageToSend = 1;
            break;

    }

    if( MessageToSend == 1 ) {
        vCCIDSendResponse();
    }
}


//------------------------------------------------------------------------------
/// SETUP request handler for a CCID device
/// \param pRequest Pointer to a USBGenericRequest instance
//------------------------------------------------------------------------------
static void CCID_RequestHandler(const USBGenericRequest *pRequest)
{
    TRACE_DEBUG("CCID_RHl\n\r");

    // Check if this is a class request
    if (USBGenericRequest_GetType(pRequest) == USBGenericRequest_CLASS) {

        // Check if the request is supported
        switch (USBGenericRequest_GetRequest(pRequest)) {

            case CCIDGenericRequest_ABORT:
                TRACE_DEBUG("CCIDGenericRequest_ABORT\n\r");
                break;

            case CCIDGenericRequest_GET_CLOCK_FREQUENCIES:
                TRACE_DEBUG("Not supported\n\r");
                // A CCID with bNumClockSupported equal to 00h does not have 
                // to support this request
                break;

            case CCIDGenericRequest_GET_DATA_RATES:
                TRACE_DEBUG("Not supported\n\r");
                // A CCID with bNumDataRatesSupported equal to 00h does not have 
                // to support this request.
                break;

            default:
                TRACE_WARNING( "CCIDDriver_RequestHandler: Unsupported request (%d)\n\r",
                                                    USBGenericRequest_GetRequest(pRequest));
                USBD_Stall(0);
        }
    }

    else if (USBGenericRequest_GetType(pRequest) == USBGenericRequest_STANDARD) {

        // Forward request to the standard handler
        USBDDriver_RequestHandler(&(ccidDriver.usbdDriver), pRequest);
    }
    else {

        // Unsupported request type
        TRACE_WARNING( "CCIDDriver_RequestHandler: Unsupported request type (%d)\n\r",
                                                    USBGenericRequest_GetType(pRequest));
        USBD_Stall(0);
    }
}


//------------------------------------------------------------------------------
//      Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Optional callback re-implementation
//------------------------------------------------------------------------------
#if !defined(NOAUTOCALLBACK)
// not static function
void USBDCallbacks_RequestReceived(const USBGenericRequest *request)
{
    CCID_RequestHandler(request);
}
#endif


//------------------------------------------------------------------------------
/// Handles SmartCart request
//------------------------------------------------------------------------------
void CCID_SmartCardRequest( void )
{
    unsigned char bStatus;

    do {

        bStatus = CCID_Read( (void*)&ccidDriver.sCcidCommand,
                             sizeof(S_ccid_bulk_out_header),
                             (TransferCallback)&CCIDCommandDispatcher,
                             (void*)0 );
    } 
    while (bStatus != USBD_STATUS_SUCCESS);

}

//------------------------------------------------------------------------------
/// Initializes the CCID device driver.
//------------------------------------------------------------------------------
void CCIDDriver_Initialize( void )
{
    TRACE_DEBUG("CCID_Init\n\r");
    USBDDriver_Initialize(&(ccidDriver.usbdDriver),
                          &ccidDriverDescriptors,
                          0); // Multiple interface settings not supported
    USBD_Init();
}

//------------------------------------------------------------------------------
/// Reads data from the Data OUT endpoint
/// \param pBuffer   Buffer to store the received data
/// \param dLength   data buffer length
/// \param fCallback Optional callback function
/// \param pArgument Optional parameter for the callback function
/// \return USBD_STATUS_LOCKED or USBD_STATUS_SUCCESS
//------------------------------------------------------------------------------
unsigned char CCID_Read(void *pBuffer,
                        unsigned int dLength,
                        TransferCallback fCallback,
                        void *pArgument)
{
    return USBD_Read(CCID_EPT_DATA_OUT, pBuffer, dLength, fCallback, pArgument);
}

//------------------------------------------------------------------------------
/// Sends data through the Data IN endpoint
/// \param pBuffer   Buffer holding the data to transmit
/// \param dLength   Length of data buffer
/// \param fCallback Optional callback function
/// \param pArgument Optional parameter for the callback function
/// \return USBD_STATUS_LOCKED or USBD_STATUS_SUCCESS
//------------------------------------------------------------------------------
unsigned char CCID_Write(void *pBuffer,
                         unsigned int dLength,
                         TransferCallback fCallback,
                         void *pArgument)
{
    return USBD_Write(CCID_EPT_DATA_IN, pBuffer, dLength, fCallback, pArgument);
}

//------------------------------------------------------------------------------
/// Sends data through the interrupt endpoint, ICC insertion event
/// RDR_to_PC_NotifySlotChange
/// \return USBD_STATUS_LOCKED or USBD_STATUS_SUCCESS
//------------------------------------------------------------------------------
unsigned char CCID_Insertion( void )
{
    TRACE_DEBUG("CCID_Insertion\n\r");

    // Build the Interrupt-IN message
    ccidDriver.BufferINT[0] = RDR_TO_PC_NOTIFYSLOTCHANGE;
    ccidDriver.BufferINT[1] = ICC_INSERTED_EVENT;
    ccidDriver.SlotStatus   = ICC_INSERTED_EVENT;

    // Notify the host that a ICC is inserted
    return USBD_Write( CCID_EPT_NOTIFICATION, ccidDriver.BufferINT, 2, 0, 0 );
}

//------------------------------------------------------------------------------
/// Sends data through the interrupt endpoint, ICC removal event
/// RDR_to_PC_NotifySlotChange
/// \return USBD_STATUS_LOCKED or USBD_STATUS_SUCCESS
//------------------------------------------------------------------------------
unsigned char CCID_Removal( void )
{
    TRACE_DEBUG("CCID_Removal\n\r");

    // Build the Interrupt-IN message
    ccidDriver.BufferINT[0] = RDR_TO_PC_NOTIFYSLOTCHANGE;
    ccidDriver.BufferINT[1] = ICC_NOT_PRESENT;
    ccidDriver.SlotStatus   = ICC_NOT_PRESENT;

    // Notify the host that a ICC is inserted
    return USBD_Write( CCID_EPT_NOTIFICATION, ccidDriver.BufferINT, 2, 0, 0 );
}

//------------------------------------------------------------------------------
/// Interrupt-IN Messages
/// This message is sent when any bit in the bHardwareErrorCode field is set. 
/// If this message is sent when there is no “outstanding” command, the bSeq 
/// field will be undefined.
/// \param bSlot ICC slot number
/// \param bSeq  Sequence number of the bulk OUT command when the hardware error
/// occured
/// \param bHardwareErrorCode Hardware error code
/// \return USBD_STATUS_LOCKED or USBD_STATUS_SUCCESS
//------------------------------------------------------------------------------
unsigned char RDRtoPCHardwareError( unsigned char bSlot, 
                                    unsigned char bSeq, 
                                    unsigned char bHardwareErrorCode )
{
    TRACE_DEBUG("RDRtoPCHardwareError\n\r");

    // Build the Interrupt-IN message
    ccidDriver.BufferINT[0] = RDR_TO_PC_HARDWAREERROR;
    ccidDriver.BufferINT[1] = bSlot;
    ccidDriver.BufferINT[2] = bSeq;
    ccidDriver.BufferINT[3] = bHardwareErrorCode;

    // Notify the host that a ICC is inserted
    return USBD_Write( CCID_EPT_NOTIFICATION, ccidDriver.BufferINT, 4, 0, 0 );
}


