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

        Manipulation of the channels of an USB audio speaker device.

    !Usage

        -# Initialize a AUDDSpeakerChannel instance using
           AUDDSpeakerChannel_Initialize.
        -# Retrieves the current status of a channel with the
           AUDDSpeakerChannel_IsMuted method.
        -# Re-implement the AUDDSpeakerChannel_MuteChanged callback to get
           notified when the status of a channel changes.
*/

#ifndef AUDDSPEAKERCHANNEL_H
#define AUDDSPEAKERCHANNEL_H

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Modelizes a channel of an USB audio speaker device.
//------------------------------------------------------------------------------
typedef struct {

    /// Zero-based channel number in the audio function.
    unsigned char number;
    /// Indicates if the channel is currently muted.
    unsigned char muted;

} AUDDSpeakerChannel;

//------------------------------------------------------------------------------
//         Callbacks
//------------------------------------------------------------------------------

extern void AUDDSpeakerChannel_MuteChanged(AUDDSpeakerChannel *channel,
                                           unsigned char muted);

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

extern void AUDDSpeakerChannel_Initialize(AUDDSpeakerChannel *channel,
                                          unsigned char number,
                                          unsigned char muted);

extern unsigned char AUDDSpeakerChannel_GetNumber(
    const AUDDSpeakerChannel *channel);

extern void AUDDSpeakerChannel_Mute(AUDDSpeakerChannel *channel);

extern void AUDDSpeakerChannel_Unmute(AUDDSpeakerChannel *channel);

extern unsigned char AUDDSpeakerChannel_IsMuted(
    const AUDDSpeakerChannel *channel);

#endif //#ifndef AUDDSPEAKERCHANNEL_H

