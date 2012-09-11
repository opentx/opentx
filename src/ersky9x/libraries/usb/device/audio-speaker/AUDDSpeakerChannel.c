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
    Title: AUDDSpeakerChannel implementation

    About: Purpose
        Implementation of the AUDDSpeakerChannel functions.
*/

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "AUDDSpeakerChannel.h"
#include <utility/trace.h>

//------------------------------------------------------------------------------
//         Callbacks
//------------------------------------------------------------------------------
/*
    Function: AUDDSpeakerChannel_MuteChanged
        Callback triggered when the mute status of a channel changes. This is
        a default implementation which does nothing and must be overriden.

    Parameters:
        channel - Pointer to a AUDDSpeakerChannel instance.
        muted - New mute status.
*/
//__attribute__ ((weak)) void AUDDSpeakerChannel_MuteChanged(
//    AUDDSpeakerChannel *channel,
//    unsigned char muted)
//{
//    TRACE_DEBUG("MuteChanged(%d, %d) ", channel->number, muted);
//}

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes the member variables of an AUDDSpeakerChannel object to the
/// given values.
/// \param channel Pointer to an AUDDSpeakerChannel instance.
/// \param number Channel number in the audio function.
/// \param muted Indicates if the channel is muted.
//------------------------------------------------------------------------------
void AUDDSpeakerChannel_Initialize(AUDDSpeakerChannel *channel,
                                   unsigned char number,
                                   unsigned char muted)
{
    channel->number = number;
    channel->muted = muted;
}

//------------------------------------------------------------------------------
/// Indicates the number of a channel.
/// \param channel Pointer to an AUDDSpeakerChannel instance.
/// \return Channel number.
//------------------------------------------------------------------------------
unsigned char AUDDSpeakerChannel_GetNumber(const AUDDSpeakerChannel *channel)
{
    return channel->number;
}

//------------------------------------------------------------------------------
/// Mutes the given channel and triggers the MuteChanged callback if
/// necessary.
/// \param channel Pointer to an AUDDSpeakerChannelInstance.
//------------------------------------------------------------------------------
void AUDDSpeakerChannel_Mute(AUDDSpeakerChannel *channel)
{
    if (!channel->muted) {

        channel->muted = 1;
        AUDDSpeakerChannel_MuteChanged(channel, 1);
    }
}

//------------------------------------------------------------------------------
/// Unmutes the given channel and triggers the MuteChanged callback if
/// necessary.
/// \param channel Pointer to an AUDDSpeakerChannelInstance.
//------------------------------------------------------------------------------
void AUDDSpeakerChannel_Unmute(AUDDSpeakerChannel *channel)
{
    if (channel->muted) {

        channel->muted = 0;
        AUDDSpeakerChannel_MuteChanged(channel, 0);
    }
}

//------------------------------------------------------------------------------
/// Indicates if the given channel is currently muted or not.
/// \param channel Pointer an AUDDSpeakerChannel instance.
/// \return 1 if the channel is muted; otherwise 0.
//------------------------------------------------------------------------------
unsigned char AUDDSpeakerChannel_IsMuted(const AUDDSpeakerChannel *channel)
{
    return channel->muted;
}

