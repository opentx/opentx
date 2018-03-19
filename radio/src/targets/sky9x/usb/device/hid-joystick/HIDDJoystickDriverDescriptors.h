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

 Definitions of the descriptors required by the HID device joystick
 driver.

 !!!Usage
 -# Use the hiddJoystickDriverDescriptors variable to initialize a
    USBDDriver instance.
 -# Send hiddReportDescriptor to the host when a GET_DESCRIPTOR request
    for the report descriptor is received.
*/

#ifndef HIDDJOYSTICKDRIVERDESCRIPTORS_H
#define HIDDJOYSTICKDRIVERDESCRIPTORS_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include <usb/device/core/USBDDriverDescriptors.h>

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------
/*
    Constants: Endpoints
        HIDDJoystickDriverDescriptors_INTERRUPTIN - Interrupt IN endpoint number.
        HIDDJoystickDriverDescriptors_INTERRUPTIN_POLLING - Interrupt IN endpoint
            polling rate (in milliseconds).
*/
#define HIDDJoystickDriverDescriptors_INTERRUPTIN           1
#define HIDDJoystickDriverDescriptors_INTERRUPTIN_POLLING   10

//------------------------------------------------------------------------------
//         Exported variables
//------------------------------------------------------------------------------
/*
    Variables: HID joystick driver descriptors
        hiddJoystickDriverDescriptors - List of descriptors used by the HID
            joystick driver.
        hiddReportDescriptor - Report descriptor used by the driver.
*/
extern USBDDriverDescriptors hiddJoystickDriverDescriptors;
extern const unsigned char hiddReportDescriptor[];

#endif //#ifndef HIDDJOYSTICKDRIVERDESCRIPTORS_H

