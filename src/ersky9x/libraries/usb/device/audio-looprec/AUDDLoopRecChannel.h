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

        Manipulation of the channels of an USB audio LoopRec device.

    !Usage

        -# Initialize a AUDDLoopRecChannel instance using
           AUDDLoopRecChannel_Initialize.
        -# Retrieves the current status of a channel with the
           AUDDLoopRecChannel_IsMuted method.
        -# Re-implement the AUDDLoopRecChannel_MuteChanged callback to get
           notified when the status of a channel changes.
*/

#ifndef AUDDLOOPRECCHANNEL_H
#define AUDDLOOPRECCHANNEL_H

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Modelizes a channel of an USB audio LoopRec device.
//------------------------------------------------------------------------------
typedef struct {

    /// Zero-based channel number in the audio function.
    unsigned char number;
    /// Indicates if the channel is currently muted.
    unsigned char muted;

} AUDDLoopRecChannel;

//------------------------------------------------------------------------------
//         Callbacks
//------------------------------------------------------------------------------

extern void AUDDLoopRecChannel_MuteChanged(AUDDLoopRecChannel *channel,
                                           unsigned char muted);

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

extern void AUDDLoopRecChannel_Initialize(AUDDLoopRecChannel *channel,
                                          unsigned char number,
                                          unsigned char muted);

extern unsigned char AUDDLoopRecChannel_GetNumber(
    const AUDDLoopRecChannel *channel);

extern void AUDDLoopRecChannel_Mute(AUDDLoopRecChannel *channel);

extern void AUDDLoopRecChannel_Unmute(AUDDLoopRecChannel *channel);

extern unsigned char AUDDLoopRecChannel_IsMuted(
    const AUDDLoopRecChannel *channel);

#endif //#ifndef AUDDLOOPRECCHANNEL_H

