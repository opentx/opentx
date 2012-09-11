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
/// \page "NandSpareScheme"
///
/// !!!Purpose
/// 
/// NandSpareScheme layer is used to do Nandflash device's spare area operations. It is called by 
/// upper layer drivers, such as SkipBlockNandFlash layer.
///
/// !!!Usage
///
/// -# NandSpareScheme_WriteBadBlockMarker is used to mark a badblock marker inside a spare
///     area.
/// -# NandSpareScheme_ReadBadBlockMarker is used to readout the marker.
/// -# NandSpareScheme_ReadEcc is used to read out ecc from spare area using the provided
///      spare scheme.
/// -# NandSpareScheme_WriteEcc is used to write ecc to spare area using the provided
///      spare scheme.
/// -# NandSpareScheme_ReadExtra is used to read extra bytes from spare area using the provided
///      spare scheme.
/// -# NandSpareScheme_WriteExtra is used to write extra bytes to spare area using the provided
///      spare scheme.
//------------------------------------------------------------------------------


#ifndef NANDSPARESCHEME_H
#define NANDSPARESCHEME_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "NandCommon.h"

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

struct NandSpareScheme {

    unsigned char badBlockMarkerPosition;
    unsigned char numEccBytes;
    unsigned char eccBytesPositions[NandCommon_MAXSPAREECCBYTES];
    unsigned char numExtraBytes;
    unsigned char extraBytesPositions[NandCommon_MAXSPAREEXTRABYTES];
};

//------------------------------------------------------------------------------
//         Exported variables
//------------------------------------------------------------------------------

extern const struct NandSpareScheme nandSpareScheme256;
extern const struct NandSpareScheme nandSpareScheme512;
extern const struct NandSpareScheme nandSpareScheme2048;
extern const struct NandSpareScheme nandSpareScheme4096;

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

extern void NandSpareScheme_ReadBadBlockMarker(
    const struct NandSpareScheme *scheme,
    const unsigned char *spare,
    unsigned char *marker);

extern void NandSpareScheme_WriteBadBlockMarker(
    const struct NandSpareScheme *scheme,
    unsigned char *spare,
    unsigned char marker);

extern void NandSpareScheme_ReadEcc(
    const struct NandSpareScheme *scheme,
    const unsigned char *spare,
    unsigned char *ecc);

extern void NandSpareScheme_WriteEcc(
    const struct NandSpareScheme *scheme,
    unsigned char *spare,
    const unsigned char *ecc);

extern void NandSpareScheme_ReadExtra(
    const struct NandSpareScheme *scheme,
    const unsigned char *spare,
    void *extra,
    unsigned char size,
    unsigned char offset);

extern void NandSpareScheme_WriteExtra(
    const struct NandSpareScheme *scheme,
    unsigned char *spare,
    const void *extra,
    unsigned char size,
    unsigned char offset);

#endif //#ifndef NANDSPARESCHEME_H

