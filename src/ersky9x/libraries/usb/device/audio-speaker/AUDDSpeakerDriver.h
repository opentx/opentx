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

/**
 \unit

 !Purpose

    Definition of a USB Audio Speaker Driver with two playback channels and one
    record channel.

 !Usage

    -# Enable and setup USB related pins (see pio & board.h).
    -# Configure the USB Audio Speaker driver using
       AUDDSpeakerDriver_Initialize
    -# To get %audio stream frames from host, use
       AUDDSpeakerDriver_Read
    -# To send %audio sampling stream to host, use
       AUDDSpeakerDriver_Write

*/

#ifndef AUDDSPEAKERDRIVER_H
#define AUDDSPEAKERDRIVER_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include <board.h>
#include <usb/common/core/USBGenericRequest.h>
#include <usb/device/core/USBD.h>

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "Audio Speaker stream information"
///
/// This page lists codes for USB Audio Speaker stream information.
///
/// !Code
/// - AUDDSpeakerDriver_SAMPLERATE
/// - AUDDSpeakerDriver_NUMCHANNELS
/// - AUDDSpeakerDriver_BYTESPERSAMPLE
/// - AUDDSpeakerDriver_BITSPERSAMPLE
/// - AUDDSpeakerDriver_SAMPLESPERFRAME
/// - AUDDSpeakerDriver_BYTESPERFRAME

#if defined(at91sam7s)
 /// Sample rate in Hz.
 #define AUDDSpeakerDriver_SAMPLERATE        32000
 /// Number of channels in audio stream.
 #define AUDDSpeakerDriver_NUMCHANNELS       1
 /// Number of bytes in one sample.
 #define AUDDSpeakerDriver_BYTESPERSAMPLE    2
#else
 /// Sample rate in Hz.
 #define AUDDSpeakerDriver_SAMPLERATE        48000
 /// Number of channels in audio stream.
 #define AUDDSpeakerDriver_NUMCHANNELS       2
 /// Number of bytes in one sample.
 #define AUDDSpeakerDriver_BYTESPERSAMPLE    2
#endif
/// Number of bits in one sample.
#define AUDDSpeakerDriver_BITSPERSAMPLE     (AUDDSpeakerDriver_BYTESPERSAMPLE * 8)
/// Number of bytes in one USB subframe.
#define AUDDSpeakerDriver_BYTESPERSUBFRAME  (AUDDSpeakerDriver_NUMCHANNELS * \
                                             AUDDSpeakerDriver_BYTESPERSAMPLE)
/// Number of samples in one USB frame.
#define AUDDSpeakerDriver_SAMPLESPERFRAME   (AUDDSpeakerDriver_SAMPLERATE / 1000 \
                                             * AUDDSpeakerDriver_NUMCHANNELS)
/// Number of bytes in one USB frame.
#define AUDDSpeakerDriver_BYTESPERFRAME     (AUDDSpeakerDriver_SAMPLESPERFRAME * \
                                             AUDDSpeakerDriver_BYTESPERSAMPLE)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "Audio Speaker Channel Numbers"
///
/// This page lists codes for USB Audio Speaker channel numbers.
///
/// !Playback channel numbers
/// - AUDDSpeakerDriver_MASTERCHANNEL
/// - AUDDSpeakerDriver_LEFTCHANNEL
/// - AUDDSpeakerDriver_RIGHTCHANNEL
///
/// !Record channel number
/// - AUDDSpeakerDriver_RECCHANNEL

/// Master channel of playback.
#define AUDDSpeakerDriver_MASTERCHANNEL     0
/// Front left channel of playback.
#define AUDDSpeakerDriver_LEFTCHANNEL       1
/// Front right channel of playback.
#define AUDDSpeakerDriver_RIGHTCHANNEL      2
/// Channel of record.
#define AUDDSpeakerDriver_RECCHANNEL        0
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

extern void AUDDSpeakerDriver_Initialize();

extern void AUDDSpeakerDriver_RequestHandler(const USBGenericRequest *request);

extern unsigned char AUDDSpeakerDriver_Read(void *buffer,
                                            unsigned int length,
                                            TransferCallback callback,
                                            void *argument);

#endif //#ifndef AUDDSPEAKERDRIVER_H

//#ifndef SPEAKER_DRIVER_H
//#define SPEAKER_DRIVER_H
//
////------------------------------------------------------------------------------
////         Definitions
////------------------------------------------------------------------------------
//
///*! Sampling frequency in Hz */
//#define SPEAKER_SAMPLERATE          48000
///*! Number of samples in one isochronous packet (1ms frame) */
//#define SPEAKER_SAMPLESPERPACKET     (SPEAKER_SAMPLERATE / 1000)
///*! Size of one sample (in bytes) */
//#define SPEAKER_SAMPLESIZE          2
///*! Number of channels */
//#define SPEAKER_NUMCHANNELS         2
///*! Size of one frame (number of bytes sent for one sample on all channels) */
//#define SPEAKER_FRAMESIZE           (SPEAKER_SAMPLESIZE * SPEAKER_NUMCHANNELS)
///*! Required bit rate given the sample frequency, sample size and number of
//    channels. */
//#define SPEAKER_BITRATE             (SPEAKER_SAMPLERATE * SPEAKER_FRAMESIZE)
///*! Size of one isochronous packet */
//#define SPEAKER_PACKETSIZE          (SPEAKER_SAMPLESPERPACKET * SPEAKER_FRAMESIZE)
//
////------------------------------------------------------------------------------
////         Structures
////------------------------------------------------------------------------------
///*!
//    Holds the speaker driver state.
// */
//typedef struct {
//
//    S_std_class   standardDriver;
//
//    unsigned char isOutStreamEnabled;
//    unsigned char isChannelMuted[SPEAKER_NUMCHANNELS+1];
//
//    Callback_f    outStreamStatusChanged;
//    Callback_f    outStreamMuteChanged;
//
//} __attribute__((packed)) S_speaker;
//
////------------------------------------------------------------------------------
////         Exported functions
////------------------------------------------------------------------------------
//
//extern void SPK_Init(S_speaker *speakerDriver, const S_usb *usbDriver);
//extern void SPK_SetCallbacks(S_speaker *speakerDriver,
//                             Callback_f outStreamStatusChanged,
//                             Callback_f outStreamMuteChanged);
//extern void SPK_RequestHandler(S_speaker *speakerDriver);
//extern char SPK_Read(S_speaker *speakerDriver,
//                     void *buffer,
//                     unsigned int length,
//                     Callback_f callback,
//                     void *argument);
//
//#endif //#ifndef SPEAKER_DRIVER_H
//
