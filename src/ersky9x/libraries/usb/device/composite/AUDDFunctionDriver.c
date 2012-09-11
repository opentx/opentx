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

#if defined(usb_CDCAUDIO) || defined(usb_HIDAUDIO)

//-----------------------------------------------------------------------------
//      Headers
//-----------------------------------------------------------------------------

// GENERAL
#include <utility/trace.h>
#include <utility/assert.h>
#include <utility/led.h>
// USB
#include <usb/device/core/USBD.h>
// AUDIO
#include <usb/common/audio/AUDGenericRequest.h>
#include <usb/common/audio/AUDFeatureUnitRequest.h>

#include "AUDDFunctionDriver.h"
#include "AUDDFunctionDriverDescriptors.h"

//-----------------------------------------------------------------------------
//         Internal variables
//-----------------------------------------------------------------------------

/// USB audio speaker driver instance.
static AUDDSpeakerChannel auddSpeakerChannels[AUDD_NUMCHANNELS+1];
/// Intermediate storage variable for the mute status of a channel.
static unsigned char muted;

//-----------------------------------------------------------------------------
//         Internal functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Changes the mute status of the given channel accordingly.
/// \param channel Number of the channel whose mute status has changed.
//-----------------------------------------------------------------------------
static void AUDD_MuteReceived(unsigned char channel)
{
    AUDDSpeakerChannel *pChannel = &auddSpeakerChannels[channel];
    if (muted) {
    
        if (!pChannel->muted) {

            pChannel->muted = 1;
            AUDDSpeakerChannel_MuteChanged(pChannel, 1);
        }
    }
    else {

        if (pChannel->muted) {
            
            pChannel->muted = 0;
            AUDDSpeakerChannel_MuteChanged(pChannel, 0);
        }
    }

    USBD_Write(0, 0, 0, 0, 0);
}

//-----------------------------------------------------------------------------
/// Sets the current value of a particular Feature control of a channel.
/// \param channel Number of the channel whose feature will change.
/// \param control The feature control that will change.
/// \param length The feature data size.
//-----------------------------------------------------------------------------
static void AUDD_SetFeatureCurrentValue(unsigned char channel,
                                        unsigned char control,
                                        unsigned short length)
{
    TRACE_INFO_WP("sFeature ");
    TRACE_DEBUG("\b(CS%d, CN%d, L%d) ", control, channel, length);

    // Check the the requested control is supported
    // Mute control on master channel
    if ((control == AUDFeatureUnitRequest_MUTE)
        && (channel < (AUDD_NUMCHANNELS+1))
        && (length == 1)) {

        unsigned int argument = channel; // Avoids compiler warning
        USBD_Read(0, // Endpoint #0
                  &muted,
                  sizeof(muted),
                  (TransferCallback) AUDD_MuteReceived,
                  (void *) argument);
    }
    // Control/channel combination not supported
    else {

        USBD_Stall(0);
    }
}

//-----------------------------------------------------------------------------
/// Sends the current value of a particular channel Feature to the USB host.
/// \param channel Number of the channel whose feature will be sent.
/// \param control The feature control that will be sent.
/// \param length The feature data size.
//-----------------------------------------------------------------------------
static void AUDD_GetFeatureCurrentValue(unsigned char channel,
                                        unsigned char control,
                                        unsigned char length)
{
    TRACE_INFO_WP("gFeature ");
    TRACE_DEBUG("\b(CS%d, CN%d, L%d) ", control, channel, length);

    // Check that the requested control is supported
    // Master channel mute control
    if ((control == AUDFeatureUnitRequest_MUTE)
        && (channel < (AUDD_NUMCHANNELS+1))
        && (length == 1)) {

        muted = auddSpeakerChannels[channel].muted;
        USBD_Write(0, &muted, sizeof(muted), 0, 0);
    }
    else {

        USBD_Stall(0);
    }
}

//-----------------------------------------------------------------------------
//         Exported functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/// Initializes an USB audio speaker function driver
//-----------------------------------------------------------------------------
void AUDDFunctionDriver_Initialize()
{
    auddSpeakerChannels[0].muted = 0;
    auddSpeakerChannels[0].number = AUDD_MASTERCHANNEL;
    auddSpeakerChannels[1].muted = 0;
    auddSpeakerChannels[1].number = AUDD_LEFTCHANNEL;
    auddSpeakerChannels[2].muted = 0;
    auddSpeakerChannels[2].number = AUDD_RIGHTCHANNEL;

    // Initialize the third LED to indicate when the audio interface is active
    LED_Configure(USBD_LEDOTHER);
}

//-----------------------------------------------------------------------------
/// Handles AUDIO-specific USB requests sent by the host
/// \param request Pointer to a USBGenericRequest instance.
/// \return 0 if the request is Unsupported, 1 if the request handled.
//-----------------------------------------------------------------------------
unsigned char AUDDFunctionDriver_RequestHandler(
    const USBGenericRequest *request)
{
    unsigned char entity;
    unsigned char interface;

    // Check if the request is supported
    switch (USBGenericRequest_GetRequest(request)) {

        case AUDGenericRequest_SETCUR:
            TRACE_INFO_WP(
                      "sCur(0x%04X) ",
                      USBGenericRequest_GetIndex(request));

            // Check the target interface and entity
            entity = AUDGenericRequest_GetEntity(request);
            interface = AUDGenericRequest_GetInterface(request);
            if ((entity == AUDD_Descriptors_FEATUREUNIT)
                && (interface == AUDD_Descriptors_CONTROL)) {

                AUDD_SetFeatureCurrentValue(
                    AUDFeatureUnitRequest_GetChannel(request),
                    AUDFeatureUnitRequest_GetControl(request),
                    USBGenericRequest_GetLength(request));
            }
            else {

                TRACE_WARNING(
                          "AUDDSpeakerDriver_RequestHandler: Unsupported entity/interface combination (0x%04X)\n\r",
                          USBGenericRequest_GetIndex(request));
                USBD_Stall(0);
            }
            break;

        case AUDGenericRequest_GETCUR:
            TRACE_INFO_WP(
                      "gCur(0x%04X) ",
                      USBGenericRequest_GetIndex(request));

            // Check the target interface and entity
            entity = AUDGenericRequest_GetEntity(request);
            interface = AUDGenericRequest_GetInterface(request);
            if ((entity == AUDD_Descriptors_FEATUREUNIT)
                && (interface == AUDD_Descriptors_CONTROL)) {

                AUDD_GetFeatureCurrentValue(
                    AUDFeatureUnitRequest_GetChannel(request),
                    AUDFeatureUnitRequest_GetControl(request),
                    USBGenericRequest_GetLength(request));
            }
            else {

                TRACE_WARNING(
                          "AUDDSpeakerDriver_RequestHandler: Unsupported entity/interface combination (0x%04X)\n\r",
                          USBGenericRequest_GetIndex(request));
                USBD_Stall(0);
            }
            break;

        default:
            return 0;
    }
    return 1;
}

//-----------------------------------------------------------------------------
/// Invoked whenever the active setting of an interface is changed by the
/// host. Changes the status of the third LED accordingly.
/// \param interface Interface number.
/// \param setting Newly active setting.
//-----------------------------------------------------------------------------
void AUDDFunctionCallbacks_InterfaceSettingChanged(unsigned char interface,
                                                   unsigned char setting)
{
    if ((interface == AUDD_Descriptors_STREAMING) && (setting == 0)) {

        LED_Clear(USBD_LEDOTHER);
    }
    else {

        LED_Set(USBD_LEDOTHER);
    }
}

//-----------------------------------------------------------------------------
/// Reads incoming audio data sent by the USB host into the provided buffer.
/// When the transfer is complete, an optional callback function is invoked.
/// \param buffer Pointer to the data storage buffer.
/// \param length Size of the buffer in bytes.
/// \param callback Optional callback function.
/// \param argument Optional argument to the callback function.
/// \return <USBD_STATUS_SUCCESS> if the transfer is started successfully;
///         otherwise an error code.
//-----------------------------------------------------------------------------
unsigned char AUDDSpeakerDriver_Read(void *buffer,
                                     unsigned int length,
                                     TransferCallback callback,
                                     void *argument)
{
    return USBD_Read(AUDD_Descriptors_DATAOUT,
                     buffer,
                     length,
                     callback,
                     argument);
}

#endif // (AUDIO defined)

