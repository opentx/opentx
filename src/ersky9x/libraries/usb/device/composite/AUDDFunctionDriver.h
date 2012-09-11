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

#ifndef AUDDFUNCTIONDRIVER_H
#define AUDDFUNCTIONDRIVER_H

//-----------------------------------------------------------------------------
//         Headers
//-----------------------------------------------------------------------------

#include <usb/common/core/USBGenericRequest.h>
#include <usb/device/core/USBD.h>

//-----------------------------------------------------------------------------
//         Definitions
//-----------------------------------------------------------------------------

/// Sample rate in Hz.
#define AUDD_SAMPLERATE        48000
/// Number of channels in audio stream.
#define AUDD_NUMCHANNELS       2
/// Number of bytes in one sample.
#define AUDD_BYTESPERSAMPLE    2
/// Number of bits in one sample.
#define AUDD_BITSPERSAMPLE     (AUDD_BYTESPERSAMPLE * 8)
/// Number of samples in one USB subframe.
#define AUDD_BYTESPERSUBFRAME  (AUDD_NUMCHANNELS * AUDD_BYTESPERSAMPLE)
/// Number of samples in one USB frame.
#define AUDD_SAMPLESPERFRAME   (AUDD_SAMPLERATE / 1000 * AUDD_NUMCHANNELS)
/// Number of bytes in one USB frame.
#define AUDD_BYTESPERFRAME     (AUDD_SAMPLESPERFRAME * AUDD_BYTESPERSAMPLE)
/// Master channel.
#define AUDD_MASTERCHANNEL     0
/// Front left channel.
#define AUDD_LEFTCHANNEL       1
/// Front right channel.
#define AUDD_RIGHTCHANNEL      2

//-----------------------------------------------------------------------------
//         Structs
//-----------------------------------------------------------------------------

/// AUDIO Speaker channel struct
typedef struct {

    unsigned char number;
    unsigned char muted;

} AUDDSpeakerChannel;

//-----------------------------------------------------------------------------
//         Callbacks
//-----------------------------------------------------------------------------
extern void AUDDSpeakerChannel_MuteChanged(AUDDSpeakerChannel *channel,
                                           unsigned char muted);

extern void AUDDFunctionCallbacks_InterfaceSettingChanged(
    unsigned char interface,
    unsigned char setting);


//-----------------------------------------------------------------------------
//         Exported functions
//-----------------------------------------------------------------------------

//- Function API For composite device
extern void AUDDFunctionDriver_Initialize();

extern unsigned char AUDDFunctionDriver_RequestHandler(
    const USBGenericRequest * request);

//- AUDIO Speaker API
extern unsigned char AUDDSpeakerDriver_Read(void *buffer,
                                            unsigned int length,
                                            TransferCallback callback,
                                            void *argument);

#endif // #define AUDDFUNCTIONDRIVER_H

