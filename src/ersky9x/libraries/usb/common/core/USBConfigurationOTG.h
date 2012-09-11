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
///
/// !Purpose
/// 
/// Methods and definitions for configuring OTG.
/// 
/// !Usage
///
/// -# 
/// -# 
///
//------------------------------------------------------------------------------

#ifndef USBCONFIGURATIONOTG_H
#define USBCONFIGURATIONOTG_H

#include <usb/otg/compiler.h>


/// OTG specific configuration
typedef struct {

    /// USB MODE CONFIGURATION

    /// ENABLE to activate the host software library support
    /// Possible values ENABLE or DISABLE
    unsigned char bOTGUsbHostFeature;

    /// ENABLE to activate the device software library support
    /// Possible values ENABLE or DISABLE
    unsigned char bOTGUsbDeviceFeature;
    /// OTG MODE CONFIGURATION
    /// ENABLE to enable OTG module (support A- and B- Device roles)
    /// Possible values ENABLE or DISABLE
    unsigned char bOTGFeature;

    /// For reduced host only allows to control VBUS generator with PIO PE.7
    unsigned char bOTGSoftwareVbusCtrl;

    /// Selects the messaging method for OTG No Silent Failure spec.
    ///
    /// A compliant OTG device must at least handle 3 Failure messages : 
    ///   "Device No Response", "Unsupported Device" and "Unsupported Hub"
    /// Functions must be defined for communicating messages to user (LCD display,
    /// LEDs...)
    /// The values available for this parameter are :
    ///  - OTGMSG_ALL : all messages (events and failures) are displayed
    ///     In this case, the following functions must be defined in user application firmware :
    ///       # "void Otg_messaging_init(void)", this function is called at start up to initialize the messaging peripheral
    ///       # "void Otg_output_failure_msg(U8)", displays the failure message choosen by ID number (see "usb_task.h")
    ///       # "void Otg_output_failure_clear(void)", clears the current failure message
    ///       # "void Otg_output_event_msg(U8)", displays the event message choosen by ID number
    ///       # "void Otg_output_event_clear(void)", clears the current event message
    ///  - OTGMSG_FAIL : only failure messages are displayed
    ///     In this case, the following functions must be defined in user application firmware :
    ///       # "void Otg_messaging_init(void)", this function is called at start up to initialize the messaging peripheral
    ///       # "void Otg_output_failure_msg(U8)", displays the failure message choosen by ID number (see "usb_task.h")
    ///       # "void Otg_output_failure_clear(void)", clears the current failure message
    ///  - OTGMSG_NONE  : messages are not displayed (not OTG compliant device)
    unsigned char bOTGMessagingOutput;

    /// ENABLE to make the A-Device send a SetFeature(b_hnp_enable) just after a SRP
    /// has been received
    /// This feature must be ENABLE to pass the OTG compliance program
    /// Possible values ENABLE or DISABLE
    unsigned char bOTGEnableHNPAfterSRP;
 
    /// Selects the SRP pulse for which the A Device will react and rise Vbus
    /// The value is VBUS_PULSE or DATA_PULSE
    unsigned char bOTGADevSRPReaction;

} USBGenericOTGDescriptor;

#define     OTGMSG_NONE     0   // no messages displayed
#define     OTGMSG_FAIL     1   // only failure messages displayed
#define     OTGMSG_ALL      2   // all messages displayed

#define   VBUS_PULSE    1   // no more used
#define   DATA_PULSE    0


extern USBGenericOTGDescriptor *pOTGDescriptor;

extern void OTG_Initialize(USBGenericOTGDescriptor *pOTGDesc);


#endif //#ifndef USBCONFIGURATIONOTG_H

