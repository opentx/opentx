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

#ifndef AUDDLOOPRECDESCRIPTORS_H
#define AUDDLOOPRECDESCRIPTORS_H

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
/// - AUDDLoopRecDriverDescriptors_DATAOUT
/// - AUDDLoopRecDriverDescriptors_DATAIN
/// - AUDDLoopRecDriverDescriptors_HS_INTERVAL
/// - AUDDLoopRecDriverDescriptors_FS_INTERVAL

#if defined(at91sam7s) || defined(at91sam9xe)
    /// Data out endpoint number, size 64B.
    #define AUDDLoopRecDriverDescriptors_DATAOUT        0x01
    /// Data in endpoint number, size 64B
    #define AUDDLoopRecDriverDescriptors_DATAIN         0x02
#elif defined(CHIP_USB_UDP)
    /// Data out endpoint number, size 192B.
    #define AUDDLoopRecDriverDescriptors_DATAOUT        0x04
    /// Data in endpoint number, size 192B.
    #define AUDDLoopRecDriverDescriptors_DATAIN         0x05
#elif defined(at91sam9m10ek)
    /// Data out endpoint number, size 192B.
    #define AUDDLoopRecDriverDescriptors_DATAOUT        0x01
    /// Data in endpoint number, size 192B
    #define AUDDLoopRecDriverDescriptors_DATAIN         0x06
#else
    /// Data out endpoint number, size 192B.
    #define AUDDLoopRecDriverDescriptors_DATAOUT        0x05
    /// Data in endpoint number, size 192B
    #define AUDDLoopRecDriverDescriptors_DATAIN         0x06
#endif

/// Endpoint polling interval 2^(x-1) * 125us
#define AUDDLoopRecDriverDescriptors_HS_INTERVAL        0x04
/// Endpoint polling interval 2^(x-1) * ms
#define AUDDLoopRecDriverDescriptors_FS_INTERVAL        0x01
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "Audio Speaker Interface IDs"
///
/// This page lists the interface numbers for USB Audio Speaker device.
///
/// !Interfaces
/// - AUDDLoopRecDriverDescriptors_CONTROL
/// - AUDDLoopRecDriverDescriptors_STREAMING
/// - AUDDLoopRecDriverDescriptors_STREAMINGIN

/// Audio control interface ID.
#define AUDDLoopRecDriverDescriptors_CONTROL            0
/// Audio streaming interface ID (OUT, for playback).
#define AUDDLoopRecDriverDescriptors_STREAMING          1
/// Audio streaming interface ID (IN, for record).
#define AUDDLoopRecDriverDescriptors_STREAMINGIN        2
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "Audio Speaker Entity IDs"
///
/// This page lists the entity IDs for USB Audio Speaker device.
///
/// !Entities
/// - AUDDLoopRecDriverDescriptors_INPUTTERMINAL
/// - AUDDLoopRecDriverDescriptors_OUTPUTTERMINAL
/// - AUDDLoopRecDriverDescriptors_FEATUREUNIT
/// - AUDDLoopRecDriverDescriptors_INPUTTERMINAL_REC
/// - AUDDLoopRecDriverDescriptors_OUTPUTTERMINAL_REC
/// - AUDDLoopRecDriverDescriptors_FEATUREUNIT_REC

/// Playback input terminal ID.
#define AUDDLoopRecDriverDescriptors_INPUTTERMINAL      0
/// Playback output terminal ID.
#define AUDDLoopRecDriverDescriptors_OUTPUTTERMINAL     1
/// Playback feature unit ID.
#define AUDDLoopRecDriverDescriptors_FEATUREUNIT        2
/// Record input terminal ID.
#define AUDDLoopRecDriverDescriptors_INPUTTERMINAL_REC  3
/// Record output terminal ID.
#define AUDDLoopRecDriverDescriptors_OUTPUTTERMINAL_REC 4
/// Record feature unit ID
#define AUDDLoopRecDriverDescriptors_FEATUREUNIT_REC    5
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Exported variables
//------------------------------------------------------------------------------

extern const USBDDriverDescriptors auddLoopRecDriverDescriptors;

#endif //#ifndef AUDDLOOPRECDESCRIPTORS_H

