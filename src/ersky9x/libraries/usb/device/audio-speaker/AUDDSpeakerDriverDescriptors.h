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

 !!!Purpose

    Declaration of the descriptors required by a USB audio speaker driver.

 !!!Usage

    -# Initialize a USBDDriver instance using the
       auddSpeakerDriverDescriptors list.
*/

#ifndef AUDDSPEAKERDRIVERDESCRIPTORS_H
#define AUDDSPEAKERDRIVERDESCRIPTORS_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include <board.h>
#include <usb/device/core/USBDDriverDescriptors.h>

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "Audio Speaker Endpoint Numbers"
///
/// This page lists the endpoint number settings for USB Audio Speaker device.
///
/// !Endpoints
/// - AUDDSpeakerDriverDescriptors_DATAOUT
/// - AUDDSpeakerDriverDescriptors_DATAIN
/// - AUDDSpeakerDriverDescriptors_FS_INTERVAL
/// - AUDDSpeakerDriverDescriptors_HS_INTERVAL
///
/// \note for UDP, uses IN EPs that support double buffer; for UDPHS, uses
///       IN EPs that support DMA and High bandwidth.

#if defined(at91sam7s) || defined(at91sam9xe)
    /// Data out endpoint number.
    #define AUDDSpeakerDriverDescriptors_DATAOUT        0x01
    /// Data in endpoint number.
    #define AUDDSpeakerDriverDescriptors_DATAIN         0x02
#elif defined(CHIP_USB_UDP)
    /// Data out endpoint number.
    #define AUDDSpeakerDriverDescriptors_DATAOUT        0x04
    /// Data in endpoint number.
    #define AUDDSpeakerDriverDescriptors_DATAIN         0x05
#else
    /// Data out endpoint number.
    #define AUDDSpeakerDriverDescriptors_DATAOUT        0x05
    /// Data in endpoint number.
    #define AUDDSpeakerDriverDescriptors_DATAIN         0x06
#endif

/// Endpoint polling interval 2^(x-1) * 125us
#define AUDDSpeakerDriverDescriptors_HS_INTERVAL        0x04
/// Endpoint polling interval 2^(x-1) * ms
#define AUDDSpeakerDriverDescriptors_FS_INTERVAL        0x01
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "Audio Speaker Interface IDs"
///
/// This page lists the interface numbers for USB Audio Speaker device.
///
/// !Interfaces
/// - AUDDSpeakerDriverDescriptors_CONTROL
/// - AUDDSpeakerDriverDescriptors_STREAMING
/// - AUDDSpeakerDriverDescriptors_STREAMINGIN

/// Audio control interface ID.
#define AUDDSpeakerDriverDescriptors_CONTROL            0
/// Audio streaming interface ID (OUT, for playback).
#define AUDDSpeakerDriverDescriptors_STREAMING          1
/// Audio streaming interface ID (IN, for record).
#define AUDDSpeakerDriverDescriptors_STREAMINGIN        2
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "Audio Speaker Entity IDs"
///
/// This page lists the entity IDs for USB Audio Speaker device.
///
/// !Entities
/// - AUDDSpeakerDriverDescriptors_INPUTTERMINAL
/// - AUDDSpeakerDriverDescriptors_OUTPUTTERMINAL
/// - AUDDSpeakerDriverDescriptors_FEATUREUNIT
/// - AUDDSpeakerDriverDescriptors_INPUTTERMINAL_REC
/// - AUDDSpeakerDriverDescriptors_OUTPUTTERMINAL_REC
/// - AUDDSpeakerDriverDescriptors_FEATUREUNIT_REC

/// Playback input terminal ID.
#define AUDDSpeakerDriverDescriptors_INPUTTERMINAL      0
/// Playback output terminal ID.
#define AUDDSpeakerDriverDescriptors_OUTPUTTERMINAL     1
/// Playback feature unit ID.
#define AUDDSpeakerDriverDescriptors_FEATUREUNIT        2
/// Record input terminal ID.
#define AUDDSpeakerDriverDescriptors_INPUTTERMINAL_REC  3
/// Record output terminal ID.
#define AUDDSpeakerDriverDescriptors_OUTPUTTERMINAL_REC 4
/// Record feature unit ID
#define AUDDSpeakerDriverDescriptors_FEATUREUNIT_REC    5
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Exported variables
//------------------------------------------------------------------------------

extern const USBDDriverDescriptors auddSpeakerDriverDescriptors;

#endif //#ifndef AUDDSPEAKERDRIVERDESCRIPTORS_H

