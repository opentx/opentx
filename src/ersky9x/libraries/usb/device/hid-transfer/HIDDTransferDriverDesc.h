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

 Definitions of the descriptors required by the HID transfer %device driver.

 !!!Usage

 -# Use the hiddTransferDriverDescriptors variable to initialize a
    USBDDriver instance.
 -# Send hiddReportDescriptor to the host when a GET_DESCRIPTOR request
    for the report descriptor is received.
*/

#ifndef HIDDKEYBOARDDRIVERDESCRIPTORS_H
#define HIDDKEYBOARDDRIVERDESCRIPTORS_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include <usb/device/core/USBDDriverDescriptors.h>
#include <usb/common/hid/HIDKeypad.h>

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

/// Interrupt IN endpoint number.
#define HIDDTransferDriverDescriptors_INTERRUPTIN           1
/// Polling rate in ms
#define HIDDTransferDriverDescriptors_INTERRUPTIN_POLLING   50
/// Interrupt IN endpoint polling rate (in milliseconds).
#define HIDDTransferDriverDescriptors_INTERRUPTOUT          2
/// Polling rate in ms
#define HIDDTransferDriverDescriptors_INTERRUPTOUT_POLLING  50

/// Size of the report descriptor in bytes.
#define HIDDTransferDriverDescriptors_REPORTSIZE        32

/// Size of the input and output report, in bytes
#define HIDDTransferDriver_REPORTSIZE               32

//------------------------------------------------------------------------------
//         Exported variables
//------------------------------------------------------------------------------
/*
    Variables: HID keyboard driver descriptors
        hiddTransferDriverDescriptors - List of descriptors used by the HID
            keyboard driver.
        hiddReportDescriptor - Report descriptor used by the driver.
*/
extern USBDDriverDescriptors hiddTransferDriverDescriptors;
extern const unsigned char hiddReportDescriptor[];

#endif //#ifndef HIDDKEYBOARDDRIVERDESCRIPTORS_H

