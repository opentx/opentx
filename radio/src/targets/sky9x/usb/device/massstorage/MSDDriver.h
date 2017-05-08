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
/// \unit
/// !Purpose
/// 
/// Mass storage %device driver implementation.
/// 
/// !Usage
/// 
/// -# Enable and setup USB related pins (see pio & board.h).
/// -# Configure the memory interfaces used for Mass Storage LUNs
///    (see memories, MSDLun.h).
/// -# Configure the USB MSD %driver using MSDDriver_Initialize.
/// -# Invoke MSDDriver_StateMachine in main loop to handle all Mass Storage
///    operations.
//------------------------------------------------------------------------------

#ifndef MSDDRIVER_H
#define MSDDRIVER_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "../../../usb/device/massstorage/MSD.h"
#include "../../../usb/device/massstorage/MSDLun.h"
// #include <utility/trace.h>

//------------------------------------------------------------------------------
//      Global functions
//------------------------------------------------------------------------------

extern void MSDDriver_Initialize(MSDLun *luns, unsigned char numLuns);

extern void MSDDriver_RequestHandler(const USBGenericRequest *request);

extern void MSDDriver_StateMachine(void);

extern void MSDDriver_RemoteWakeUp(void);

extern void MSDDriver_DataCallback(unsigned char isRead,
                                   unsigned int dataLength,
                                   unsigned int nullCnt,
                                   unsigned int fullCnt);

#endif // #ifndef MSDDRIVER_H

