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
    Title: HIDDKeyboardInputReport implementation

    About: Purpose
        Implementation of the HIDDKeyboardInputReport class.
*/

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#include "HIDDKeyboardInputReport.h"
#include "HIDDKeyboardDriverDescriptors.h"
#include <utility/assert.h>

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes a keyboard input report instance.
/// \param report Pointer to a HIDDKeyboardInputReport instance.
//------------------------------------------------------------------------------
void HIDDKeyboardInputReport_Initialize(HIDDKeyboardInputReport *report)
{
    unsigned int i;

    report->bmModifierKeys = 0;
    for (i = 0; i < HIDDKeyboardInputReport_MAXKEYPRESSES; i++) {
    
        report->pressedKeys[i] = 0;
    }
}

//------------------------------------------------------------------------------
/// Reports a standard key as being pressed.
/// \param report Pointer to a HIDDKeyboardInputReport instance.
/// \param key Key code of the standard key.
//------------------------------------------------------------------------------
void HIDDKeyboardInputReport_PressStandardKey(HIDDKeyboardInputReport *report,
                                              unsigned char key)
{
    ASSERT(key <= HIDDKeyboardDriverDescriptors_LASTSTANDARDKEY,
           "Invalid standard key code (%d)\n\r",
           key);

    // Find first available slot
    unsigned int i = 0;
    unsigned char found = 0;
    while ((i < HIDDKeyboardInputReport_MAXKEYPRESSES) && !found) {

        // Free slot: no key referenced (code = 0) or ErrorRollOver
        if ((report->pressedKeys[i] == 0)
            || (report->pressedKeys[i] == HIDKeypad_ERRORROLLOVER)) {

            found = 1;
            report->pressedKeys[i] = key;
        }
        
        i++;
    }

    // Report ErrorRollOver in all fields if too many keys are pressed
    if (!found) {

        for (i=0; i < HIDDKeyboardInputReport_MAXKEYPRESSES; i++) {

            report->pressedKeys[i] = HIDKeypad_ERRORROLLOVER;
        }
    }
}

//------------------------------------------------------------------------------
/// Reports a standard key as not being pressed anymore.
/// \param report Pointer to a HIDDKeyboardInputReport instance.
/// \param key Key code of the standard key
//------------------------------------------------------------------------------
void HIDDKeyboardInputReport_ReleaseStandardKey(HIDDKeyboardInputReport *report,
                                                unsigned char key)
{
    ASSERT(key <= HIDDKeyboardDriverDescriptors_LASTSTANDARDKEY,
           "Invalid standard key code (%d)\n\r",
           key);

    // Look for key in array
    unsigned int i = 0;
    unsigned char found = 0;
    while ((i < HIDDKeyboardInputReport_MAXKEYPRESSES) && !found) {

        if (report->pressedKeys[i] == key) {

            found = 1;
            report->pressedKeys[i] = 0;
        }

        i++;
    }
}

//------------------------------------------------------------------------------
/// Reports a modifier key as being currently pressed.
/// \param report Pointer to a HIDDKeyboardInputReport instance.
/// \param key Key code of the modifier key.
//------------------------------------------------------------------------------
void HIDDKeyboardInputReport_PressModifierKey(HIDDKeyboardInputReport *report,
                                              unsigned char key)
{
    ASSERT((key >= HIDDKeyboardDriverDescriptors_FIRSTMODIFIERKEY)
           && (key <= HIDDKeyboardDriverDescriptors_LASTMODIFIERKEY),
           "Invalid standard key code (%d)\n\r",
           key);

    // Set corresponding bit
    unsigned char bit = key - HIDDKeyboardDriverDescriptors_FIRSTMODIFIERKEY;
    report->bmModifierKeys |= 1 << bit;
}

//------------------------------------------------------------------------------
/// Reports a modifier key as not being pressed anymore.
/// \param report Pointer to a HIDDKeyboardInputReport instance.
/// \param key Key code of the modifier key.
//------------------------------------------------------------------------------
void HIDDKeyboardInputReport_ReleaseModifierKey(HIDDKeyboardInputReport *report,
                                                unsigned char key)
{
    ASSERT((key >= HIDDKeyboardDriverDescriptors_FIRSTMODIFIERKEY)
           && (key <= HIDDKeyboardDriverDescriptors_LASTMODIFIERKEY),
           "Invalid standard key code (%d)\n\r",
           key);

    // Clear corresponding bit
    unsigned char bit = key - HIDDKeyboardDriverDescriptors_FIRSTMODIFIERKEY;
    report->bmModifierKeys &= ~(1 << bit);
}

