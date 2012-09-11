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
//      Headers
//------------------------------------------------------------------------------

#include <board.h>

//- HID
#include <usb/device/hid-keyboard/HIDDKeyboardInputReport.h>
#include <usb/device/hid-keyboard/HIDDKeyboardOutputReport.h>
#include <usb/common/hid/HIDGenericDescriptor.h>
#include <usb/common/hid/HIDDeviceDescriptor.h>
#include <usb/common/hid/HIDInterfaceDescriptor.h>
#include <usb/common/hid/HIDDescriptor.h>
#include <usb/common/hid/HIDReport.h>
#include <usb/common/hid/HIDGenericDesktop.h>
#include <usb/common/hid/HIDLeds.h>
#include <usb/common/hid/HIDKeypad.h>
#include "HIDDFunctionDriverDescriptors.h"

//-----------------------------------------------------------------------------
//         Definitions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//         Macros
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//         Internal structures
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Internal variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Exported variables
//------------------------------------------------------------------------------

/// Report descriptor used by the HID function driver.
const unsigned char hiddReportDescriptor[] = {

    HIDReport_GLOBAL_USAGEPAGE + 1, HIDGenericDesktop_PAGEID,
    HIDReport_LOCAL_USAGE + 1, HIDGenericDesktop_KEYBOARD,
    HIDReport_COLLECTION + 1, HIDReport_COLLECTION_APPLICATION,

        // Input report: modifier keys
        HIDReport_GLOBAL_REPORTSIZE + 1, 1,
        HIDReport_GLOBAL_REPORTCOUNT + 1, 8,
        HIDReport_GLOBAL_USAGEPAGE + 1, HIDKeypad_PAGEID,
        HIDReport_LOCAL_USAGEMINIMUM + 1,
            HIDD_Descriptors_FIRSTMODIFIERKEY,
        HIDReport_LOCAL_USAGEMAXIMUM + 1,
            HIDD_Descriptors_LASTMODIFIERKEY,
        HIDReport_GLOBAL_LOGICALMINIMUM + 1, 0,
        HIDReport_GLOBAL_LOGICALMAXIMUM + 1, 1,
        HIDReport_INPUT + 1, HIDReport_VARIABLE,

        // Input report: standard keys
        HIDReport_GLOBAL_REPORTCOUNT + 1, 3,
        HIDReport_GLOBAL_REPORTSIZE + 1, 8,
        HIDReport_GLOBAL_LOGICALMINIMUM + 1,
            HIDD_Descriptors_FIRSTSTANDARDKEY,
        HIDReport_GLOBAL_LOGICALMAXIMUM + 1,
            HIDD_Descriptors_LASTSTANDARDKEY,
        HIDReport_GLOBAL_USAGEPAGE + 1, HIDKeypad_PAGEID,
        HIDReport_LOCAL_USAGEMINIMUM + 1,
            HIDD_Descriptors_FIRSTSTANDARDKEY,
        HIDReport_LOCAL_USAGEMAXIMUM + 1,
            HIDD_Descriptors_LASTSTANDARDKEY,
        HIDReport_INPUT + 1, 0 /* Data array */,

        // Output report: LEDs
        HIDReport_GLOBAL_REPORTCOUNT + 1, 3,
        HIDReport_GLOBAL_REPORTSIZE + 1, 1,
        HIDReport_GLOBAL_USAGEPAGE + 1, HIDLeds_PAGEID,
        HIDReport_GLOBAL_LOGICALMINIMUM + 1, 0,
        HIDReport_GLOBAL_LOGICALMAXIMUM + 1, 1,
        HIDReport_LOCAL_USAGEMINIMUM + 1, HIDLeds_NUMLOCK,
        HIDReport_LOCAL_USAGEMAXIMUM + 1, HIDLeds_SCROLLLOCK,
        HIDReport_OUTPUT + 1, HIDReport_VARIABLE,

        // Output report: padding
        HIDReport_GLOBAL_REPORTCOUNT + 1, 1,
        HIDReport_GLOBAL_REPORTSIZE + 1, 5,
        HIDReport_OUTPUT + 1, HIDReport_CONSTANT,

    HIDReport_ENDCOLLECTION
};
