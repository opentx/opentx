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

#include "../../../usb/device/core/USBDDriver.h"

#include "debug.h"
#include <string.h>

#include "../../../usb/common/core/USBFeatureRequest.h"
#include "../../../usb/common/core/USBGetDescriptorRequest.h"
#include "../../../usb/common/core/USBInterfaceRequest.h"
#include "../../../usb/common/core/USBSetAddressRequest.h"
#include "../../../usb/common/core/USBSetConfigurationRequest.h"
#include "../../../usb/device/core/USBD.h"
#include "board_lowlevel.h"
#include "../../../usb/common/core/USBConfigurationDescriptor.h"
#include "../../../usb/common/core/USBDeviceDescriptor.h"
#include "../../../usb/common/core/USBDeviceQualifierDescriptor.h"
#include "../../../usb/common/core/USBEndpointDescriptor.h"
#include "../../../usb/common/core/USBGenericDescriptor.h"

//------------------------------------------------------------------------------
//      Local functions
//------------------------------------------------------------------------------
#if defined(CHIP_USB_OTGHS)
static unsigned char otg_features_supported = 0;
#endif

//------------------------------------------------------------------------------
/// Send a NULL packet
//------------------------------------------------------------------------------
static void TerminateCtrlInWithNull(void *pArg,
                                    unsigned char status,
                                    unsigned int transferred,
                                    unsigned int remaining)
{
    USBD_Write(0, // Endpoint #0
               0, // No data buffer
               0, // No data buffer
               (TransferCallback) 0,
               (void *)  0);
}

//------------------------------------------------------------------------------
/// Configures the device by setting it into the Configured state and
/// initializing all endpoints.
/// \param pDriver  Pointer to a USBDDriver instance.
/// \param cfgnum  Configuration number to set.
//------------------------------------------------------------------------------
static void SetConfiguration(USBDDriver *pDriver, unsigned char cfgnum)
{
    USBEndpointDescriptor *pEndpoints[CHIP_USB_NUMENDPOINTS+1];
    const USBConfigurationDescriptor *pConfiguration;

    // Use different descriptor depending on device speed
    if (USBD_IsHighSpeed()) {

        pConfiguration = pDriver->pDescriptors->pHsConfiguration;
    }
    else {

        pConfiguration = pDriver->pDescriptors->pFsConfiguration;
    }

    // Set & save the desired configuration
    USBD_SetConfiguration(cfgnum);
    pDriver->cfgnum = cfgnum;

    // If the configuration is not 0, configure endpoints
    if (cfgnum != 0) {
    
        // Parse configuration to get endpoint descriptors
        USBConfigurationDescriptor_Parse(pConfiguration, 0, pEndpoints, 0);
    
        // Configure endpoints
        int i = 0;
        while (pEndpoints[i] != 0) {
    
            USBD_ConfigureEndpoint(pEndpoints[i]);
            i++;
        }
    }
    // Should be done before send the ZLP
    USBDDriverCallbacks_ConfigurationChanged(cfgnum);

    // Acknowledge the request
    USBD_Write(0, // Endpoint #0
               0, // No data buffer
               0, // No data buffer
               (TransferCallback) 0,
               (void *)  0);
}

//------------------------------------------------------------------------------
/// Sends the current configuration number to the host.
/// \param pDriver  Pointer to a USBDDriver instance.
//------------------------------------------------------------------------------
static void GetConfiguration(const USBDDriver *pDriver)
{
    USBD_Write(0, &(pDriver->cfgnum), 1, 0, 0);
}

//------------------------------------------------------------------------------
/// Sends the current status of the device to the host.
/// \param pDriver  Pointer to a USBDDriver instance.
//------------------------------------------------------------------------------
static void GetDeviceStatus(const USBDDriver *pDriver)
{
    static unsigned short data;
    const USBConfigurationDescriptor *pConfiguration;

    data = 0;
    // Use different configuration depending on device speed
    if (USBD_IsHighSpeed()) {

        pConfiguration = pDriver->pDescriptors->pHsConfiguration;
    }
    else {

        pConfiguration = pDriver->pDescriptors->pFsConfiguration;
    }

    // Check current configuration for power mode (if device is configured)
    if (pDriver->cfgnum != 0) {

        if (USBConfigurationDescriptor_IsSelfPowered(pConfiguration)) {

            data |= 1;
        }
    }

    // Check if remote wake-up is enabled
    if (pDriver->isRemoteWakeUpEnabled) {

        data |= 2;
    }

    // Send the device status
    USBD_Write(0, &data, 2, 0, 0);
}

//------------------------------------------------------------------------------
/// Sends the current status of an endpoints to the USB host.
/// \param bEndpoint  Endpoint number.
//------------------------------------------------------------------------------
static void GetEndpointStatus(unsigned char bEndpoint)
{
    static unsigned short data;

    data = 0;
    // Check if the endpoint exists
    if (bEndpoint > CHIP_USB_NUMENDPOINTS) {

        USBD_Stall(0);
    }
    else {

        // Check if the endpoint if currently halted
        if (USBD_IsHalted(bEndpoint)) {

            data = 1;
        }
        
        // Send the endpoint status
        USBD_Write(0, &data, 2, 0, 0);
    }
}

//------------------------------------------------------------------------------
/// Sends the requested USB descriptor to the host if available, or STALLs  the
/// request.
/// \param pDriver  Pointer to a USBDDriver instance.
/// \param type  Type of the requested descriptor
/// \param index  Index of the requested descriptor.
/// \param length  Maximum number of bytes to return.
//------------------------------------------------------------------------------
static void GetDescriptor(
    const USBDDriver *pDriver,
    unsigned char type,
    unsigned char indexRDesc,
    unsigned int length)
{
    const USBDeviceDescriptor *pDevice;
    const USBConfigurationDescriptor *pConfiguration;
    const USBDeviceQualifierDescriptor *pQualifier;
    const USBConfigurationDescriptor *pOtherSpeed;
    const USBGenericDescriptor **pStrings =
        (const USBGenericDescriptor **) pDriver->pDescriptors->pStrings;
    const USBGenericDescriptor *pString;
    unsigned char numStrings = pDriver->pDescriptors->numStrings;
    unsigned char terminateWithNull = 0;

    // Use different set of descriptors depending on device speed
    if (USBD_IsHighSpeed()) {

        TRACE_DEBUG("HS ");
        pDevice = pDriver->pDescriptors->pHsDevice;
        pConfiguration = pDriver->pDescriptors->pHsConfiguration;
        pQualifier = pDriver->pDescriptors->pHsQualifier;
        pOtherSpeed = pDriver->pDescriptors->pHsOtherSpeed;
    }
    else {

        TRACE_DEBUG("FS ");
        pDevice = pDriver->pDescriptors->pFsDevice;
        pConfiguration = pDriver->pDescriptors->pFsConfiguration;
        pQualifier = pDriver->pDescriptors->pFsQualifier;
        pOtherSpeed = pDriver->pDescriptors->pFsOtherSpeed;
    }

    // Check the descriptor type
    switch (type) {
        
        case USBGenericDescriptor_DEVICE:
            TRACE_INFO_WP("Dev ");

            // Adjust length and send descriptor
            if (length > USBGenericDescriptor_GetLength((USBGenericDescriptor *) pDevice)) {

                length = USBGenericDescriptor_GetLength((USBGenericDescriptor *) pDevice);
            }
            USBD_Write(0, pDevice, length, 0, 0);
            break;

        case USBGenericDescriptor_CONFIGURATION:
            TRACE_INFO_WP("Cfg ");

            // Adjust length and send descriptor
            if (length > USBConfigurationDescriptor_GetTotalLength(pConfiguration)) {

                length = USBConfigurationDescriptor_GetTotalLength(pConfiguration);
                terminateWithNull = ((length % pDevice->bMaxPacketSize0) == 0);
            }
            USBD_Write(0,
                       pConfiguration,
                       length,
                       terminateWithNull ? TerminateCtrlInWithNull : 0,
                       0);
            break;

        case USBGenericDescriptor_DEVICEQUALIFIER:
            TRACE_INFO_WP("Qua ");

            // Check if descriptor exists
            if (!pQualifier) {

                USBD_Stall(0);
            }
            else {

                // Adjust length and send descriptor
                if (length > USBGenericDescriptor_GetLength((USBGenericDescriptor *) pQualifier)) {

                    length = USBGenericDescriptor_GetLength((USBGenericDescriptor *) pQualifier);
                }
                USBD_Write(0, pQualifier, length, 0, 0);
            }
            break;

        case USBGenericDescriptor_OTHERSPEEDCONFIGURATION:
            TRACE_INFO_WP("OSC ");

            // Check if descriptor exists
            if (!pOtherSpeed) {

                USBD_Stall(0);
            }
            else {

                // Adjust length and send descriptor
                if (length > USBConfigurationDescriptor_GetTotalLength(pOtherSpeed)) {

                    length = USBConfigurationDescriptor_GetTotalLength(pOtherSpeed);
                    terminateWithNull = ((length % pDevice->bMaxPacketSize0) == 0);
                }
                USBD_Write(0,
                           pOtherSpeed,
                           length,
                           terminateWithNull ? TerminateCtrlInWithNull : 0,
                           0);
            }
            break;

        case USBGenericDescriptor_STRING:
            TRACE_INFO_WP("Str%d ", indexRDesc);

            // Check if descriptor exists
            if (indexRDesc > numStrings) {

                USBD_Stall(0);
            }
            else {

                pString = pStrings[indexRDesc];

                // Adjust length and send descriptor
                if (length > USBGenericDescriptor_GetLength(pString)) {

                    length = USBGenericDescriptor_GetLength(pString);
                    terminateWithNull = ((length % pDevice->bMaxPacketSize0) == 0);
                }
                USBD_Write(0,
                           pString,
                           length,
                           terminateWithNull ? TerminateCtrlInWithNull : 0,
                           0);
            }
            break;

        default:
            TRACE_WARNING(
                      "USBDDriver_GetDescriptor: Unknown descriptor type (%d)\n\r",
                      type);
            USBD_Stall(0);
    }
}

//------------------------------------------------------------------------------
/// Sets the active setting of the given interface if the configuration supports
/// it; otherwise, the control pipe is STALLed. If the setting of an interface
/// changes.
/// \parma pDriver  Pointer to a USBDDriver instance.
/// \parma infnum  Interface number.
/// \parma setting  New active setting for the interface.
//------------------------------------------------------------------------------
static void SetInterface(
    USBDDriver *pDriver,
    unsigned char infnum,
    unsigned char setting)
{
    // Make sure alternate settings are supported
    if (!pDriver->pInterfaces) {

        USBD_Stall(0);
    }
    else {

        // Change the current setting of the interface and trigger the callback 
        // if necessary
        if (pDriver->pInterfaces[infnum] != setting) {

            pDriver->pInterfaces[infnum] = setting;
            // USBDDriverCallbacks_InterfaceSettingChanged(infnum, setting);
        }

        // Acknowledge the request
        USBD_Write(0, 0, 0, 0, 0);
    }
}

//------------------------------------------------------------------------------
/// Sends the currently active setting of the given interface to the USB
/// host. If alternate settings are not supported, this function STALLs the
/// control pipe.
/// \param pDriver  Pointer to a USBDDriver instance.
/// \param infnum  Interface number.
//------------------------------------------------------------------------------
static void GetInterface(
    const USBDDriver *pDriver,
    unsigned char infnum)
{
    // Make sure alternate settings are supported, or STALL the control pipe
    if (!pDriver->pInterfaces) {

        USBD_Stall(0);
    }
    else {

        // Sends the current interface setting to the host
        USBD_Write(0, &(pDriver->pInterfaces[infnum]), 1, 0, 0);
    }
}

#if defined(CHIP_USB_UDPHS) || defined(CHIP_USB_OTGHS)
//------------------------------------------------------------------------------
// Performs the selected test on the USB device (high-speed only).
// \param test  Test selector value.
//------------------------------------------------------------------------------
static void USBDDriver_Test(unsigned char test)
{
    TRACE_DEBUG("UDPHS_Test\n\r");

    // the lower byte of wIndex must be zero
    // the most significant byte of wIndex is used to specify the specific test mode
    switch (test) {
        case USBFeatureRequest_TESTPACKET:
            //Test mode Test_Packet: 
            //Upon command, a port must repetitively transmit the following test packet until
            //the exit action is taken. This enables the testing of rise and fall times, eye 
            //patterns, jitter, and any other dynamic waveform specifications.
            //The test packet is made up by concatenating the following strings. 
            //(Note: For J/K NRZI data, and for NRZ data, the bit on the left is the first one 
            //transmitted. �S� indicates that a bit stuff occurs, which inserts an �extra� NRZI data bit. 
            //�* N� is used to indicate N occurrences of a string of bits or symbols.)
            //A port in Test_Packet mode must send this packet repetitively. The inter-packet timing 
            //must be no less than the minimum allowable inter-packet gap as defined in Section 7.1.18 and 
            //no greater than 125 us.
            // Send ZLP
            USBD_Test(USBFeatureRequest_TESTSENDZLP);
            // Tst PACKET
            USBD_Test(USBFeatureRequest_TESTPACKET);
            while (1);
            //break; not reached

        case USBFeatureRequest_TESTJ:
            //Test mode Test_J:
            //Upon command, a port�s transceiver must enter the high-speed J state and remain in that
            //state until the exit action is taken. This enables the testing of the high output drive
            //level on the D+ line.
            // Send ZLP
            USBD_Test(USBFeatureRequest_TESTSENDZLP);
            // Tst J
            USBD_Test(USBFeatureRequest_TESTJ);
            while (1);
            //break; not reached

        case USBFeatureRequest_TESTK:
            //Test mode Test_K:
            //Upon command, a port�s transceiver must enter the high-speed K state and remain in
            //that state until the exit action is taken. This enables the testing of the high output drive
            //level on the D- line.
            // Send a ZLP
            USBD_Test(USBFeatureRequest_TESTSENDZLP);
            USBD_Test(USBFeatureRequest_TESTK);
            while (1);
            //break; not reached

        case USBFeatureRequest_TESTSE0NAK:
            //Test mode Test_SE0_NAK:
            //Upon command, a port�s transceiver must enter the high-speed receive mode
            //and remain in that mode until the exit action is taken. This enables the testing
            //of output impedance, low level output voltage, and loading characteristics.
            //In addition, while in this mode, upstream facing ports (and only upstream facing ports)
            //must respond to any IN token packet with a NAK handshake (only if the packet CRC is
            //determined to be correct) within the normal allowed device response time. This enables testing of
            //the device squelch level circuitry and, additionally, provides a general purpose stimulus/response
            //test for basic functional testing.
            // Send a ZLP
            USBD_Test(USBFeatureRequest_TESTSENDZLP);
            // Test SE0_NAK
            USBD_Test(USBFeatureRequest_TESTSE0NAK);
            while (1);
            //break; not reached

        default:
            USBD_Stall(0);
            break;

    }
    // The exit action is to power cycle the device.
    // The device must be disconnected from the host
}
#endif

//------------------------------------------------------------------------------
//      Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes a USBDDriver instance with a list of descriptors. If
/// interfaces can have multiple alternate settings, an array to store the
/// current setting for each interface must be provided.
/// \param pDriver  Pointer to a USBDDriver instance.
/// \param pDescriptors  Pointer to a USBDDriverDescriptors instance.
/// \param pInterfaces  Pointer to an array for storing the current alternate
///                     setting of each interface (optional).
//------------------------------------------------------------------------------
void USBDDriver_Initialize(
    USBDDriver *pDriver,
    const USBDDriverDescriptors *pDescriptors,
    unsigned char *pInterfaces)
{

    pDriver->cfgnum = 0;
#if (BOARD_USB_BMATTRIBUTES == USBConfigurationDescriptor_SELFPOWERED_RWAKEUP) \
    || (BOARD_USB_BMATTRIBUTES == USBConfigurationDescriptor_BUSPOWERED_RWAKEUP)
    pDriver->isRemoteWakeUpEnabled = 1;
#else
    pDriver->isRemoteWakeUpEnabled = 0;
#endif

    pDriver->pDescriptors = pDescriptors;
    pDriver->pInterfaces = pInterfaces;

#if 0
    // Initialize interfaces array if not null
    if (pInterfaces != 0) {
      memset(pInterfaces, sizeof(*pInterfaces), 0); // Really wrong, but never happens :)
    }
#endif
}

//------------------------------------------------------------------------------
/// Handles the given request if it is standard, otherwise STALLs it.
/// \param pDriver  Pointer to a USBDDriver instance.
/// \param pRequest  Pointer to a USBGenericRequest instance.
//------------------------------------------------------------------------------
void USBDDriver_RequestHandler(
    USBDDriver *pDriver,
    const USBGenericRequest *pRequest)
{
    unsigned char cfgnum;
    unsigned char infnum;
    unsigned char eptnum;
    unsigned char setting;
    unsigned char type;
    unsigned char indexDesc;
    unsigned int length;
    unsigned int address;

    TRACE_INFO_WP("Std ");

    // Check request code
    switch (USBGenericRequest_GetRequest(pRequest)) {

        case USBGenericRequest_GETDESCRIPTOR:
            TRACE_INFO_WP("gDesc ");

            // Send the requested descriptor
            type = USBGetDescriptorRequest_GetDescriptorType(pRequest);
            indexDesc = USBGetDescriptorRequest_GetDescriptorIndex(pRequest);
            length = USBGenericRequest_GetLength(pRequest);
            GetDescriptor(pDriver, type, indexDesc, length);
            break;

        case USBGenericRequest_SETADDRESS:
            TRACE_INFO_WP("sAddr ");

            // Sends a zero-length packet and then set the device address
            address = USBSetAddressRequest_GetAddress(pRequest);
            USBD_Write(0, 0, 0, (TransferCallback) USBD_SetAddress, (void *) address);
            break;

        case USBGenericRequest_SETCONFIGURATION:
            TRACE_INFO_WP("sCfg ");

            // Set the requested configuration
            cfgnum = USBSetConfigurationRequest_GetConfiguration(pRequest);
            SetConfiguration(pDriver, cfgnum);
            break;

        case USBGenericRequest_GETCONFIGURATION:
            TRACE_INFO_WP("gCfg ");

            // Send the current configuration number
            GetConfiguration(pDriver);
            break;

        case USBGenericRequest_GETSTATUS:
            TRACE_INFO_WP("gSta ");
    
            // Check who is the recipient
            switch (USBGenericRequest_GetRecipient(pRequest)) {
    
                case USBGenericRequest_DEVICE:
                    TRACE_INFO_WP("Dev ");
    
                    // Send the device status
                    GetDeviceStatus(pDriver);
                    break;
    
                case USBGenericRequest_ENDPOINT:
                    TRACE_INFO_WP("Ept ");
    
                    // Send the endpoint status
                    eptnum = USBGenericRequest_GetEndpointNumber(pRequest);
                    GetEndpointStatus(eptnum);
                    break;
    
                default:
                    TRACE_WARNING(
                              "USBDDriver_RequestHandler: Unknown recipient (%d)\n\r",
                              USBGenericRequest_GetRecipient(pRequest));
                    USBD_Stall(0);
            }
            break;

        case USBGenericRequest_CLEARFEATURE:
            TRACE_INFO_WP("cFeat ");

            // Check which is the requested feature
            switch (USBFeatureRequest_GetFeatureSelector(pRequest)) {

                case USBFeatureRequest_ENDPOINTHALT:
                    TRACE_INFO_WP("Hlt ");

                    // Unhalt endpoint and send a zero-length packet
                    USBD_Unhalt(USBGenericRequest_GetEndpointNumber(pRequest));
                    USBD_Write(0, 0, 0, 0, 0);
                    break;

                case USBFeatureRequest_DEVICEREMOTEWAKEUP:
                    TRACE_INFO_WP("RmWU ");

                    // Disable remote wake-up and send a zero-length packet
                    pDriver->isRemoteWakeUpEnabled = 0;
                    USBD_Write(0, 0, 0, 0, 0);
                    break;

                default:
                    TRACE_WARNING(
                              "USBDDriver_RequestHandler: Unknown feature selector (%d)\n\r",
                              USBFeatureRequest_GetFeatureSelector(pRequest));
                    USBD_Stall(0);
            }
            break;

    case USBGenericRequest_SETFEATURE:
        TRACE_INFO_WP("sFeat ");

        // Check which is the selected feature
        switch (USBFeatureRequest_GetFeatureSelector(pRequest)) {

            case USBFeatureRequest_DEVICEREMOTEWAKEUP:
                TRACE_INFO_WP("RmWU ");

                // Enable remote wake-up and send a ZLP
                pDriver->isRemoteWakeUpEnabled = 1;
                USBD_Write(0, 0, 0, 0, 0);
                break;

            case USBFeatureRequest_ENDPOINTHALT:
                TRACE_INFO_WP("Halt ");
                // Halt endpoint
                USBD_Halt(USBGenericRequest_GetEndpointNumber(pRequest));
                USBD_Write(0, 0, 0, 0, 0);
                break;

#if defined(CHIP_USB_UDPHS) || defined(CHIP_USB_OTGHS)

            case USBFeatureRequest_TESTMODE:
                // 7.1.20 Test Mode Support
                if ((USBGenericRequest_GetType(pRequest) == USBGenericRequest_DEVICE)
                    && ((USBGenericRequest_GetIndex(pRequest) & 0x000F) == 0)) {

                    // Handle test request
                    USBDDriver_Test(USBFeatureRequest_GetTestSelector(pRequest));
                }
                else {

                    USBD_Stall(0);
                }
                break;
#endif
#if defined(CHIP_USB_OTGHS)
            case USBFeatureRequest_OTG_B_HNP_ENABLE:
                    TRACE_INFO_WP("OTG_B_HNP_ENABLE ");
                    otg_features_supported |= 1<<USBFeatureRequest_OTG_B_HNP_ENABLE;
                    USBD_Write(0, 0, 0, 0, 0);
                break;
            case USBFeatureRequest_OTG_A_HNP_SUPPORT:
                    TRACE_INFO_WP("OTG_A_HNP_SUPPORT ");
                    otg_features_supported |= 1<<USBFeatureRequest_OTG_A_HNP_SUPPORT;
                    USBD_Write(0, 0, 0, 0, 0);
                break;
            case USBFeatureRequest_OTG_A_ALT_HNP_SUPPORT:
                    TRACE_INFO_WP("OTG_A_ALT_HNP_SUPPORT ");
                    otg_features_supported |= 1<<USBFeatureRequest_OTG_A_ALT_HNP_SUPPORT;
                    USBD_Write(0, 0, 0, 0, 0);
                break;
#endif
            default:
                TRACE_WARNING(
                          "USBDDriver_RequestHandler: Unknown feature selector (%d)\n\r",
                          USBFeatureRequest_GetFeatureSelector(pRequest));
                USBD_Stall(0);
        }
        break;

    case USBGenericRequest_SETINTERFACE:
        TRACE_INFO_WP("sInterface ");

        infnum = USBInterfaceRequest_GetInterface(pRequest);
        setting = USBInterfaceRequest_GetAlternateSetting(pRequest);
        SetInterface(pDriver, infnum, setting);
        break;

    case USBGenericRequest_GETINTERFACE:
        TRACE_INFO_WP("gInterface ");

        infnum = USBInterfaceRequest_GetInterface(pRequest);
        GetInterface(pDriver, infnum);
        break;

    default:
        TRACE_WARNING(
                  "USBDDriver_RequestHandler: Unknown request code (%d)\n\r",
                  USBGenericRequest_GetRequest(pRequest));
        USBD_Stall(0);
    }
}


//------------------------------------------------------------------------------
/// Test if RemoteWakeUP feature is enabled
/// \param pDriver  Pointer to an USBDDriver instance.
/// \return 1 if remote wake up has been enabled by the host; otherwise, returns
/// 0
//------------------------------------------------------------------------------
unsigned char USBDDriver_IsRemoteWakeUpEnabled(const USBDDriver *pDriver)
{
    return pDriver->isRemoteWakeUpEnabled;
}

#if defined(CHIP_USB_OTGHS)
//------------------------------------------------------------------------------
/// Return OTG features supported
/// \return the OTG features
//------------------------------------------------------------------------------
unsigned char USBDDriver_returnOTGFeatures(void)
{
    return otg_features_supported;
}

//------------------------------------------------------------------------------
/// Clear OTG features supported
/// \return none
//------------------------------------------------------------------------------
void USBDDriver_clearOTGFeatures(void)
{
    otg_features_supported = 0;
}
#endif

