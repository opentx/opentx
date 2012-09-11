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
//         Headers
//------------------------------------------------------------------------------

#include <board.h>
#include <pio/pio.h>
#include <utility/trace.h>
#include <stdio.h>
#include "USBH.h"
#include <usb/host/ohci/ohci.h>


//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

/// HcFmInterval Register:
/// FrameInterval specifies the interval between 2 consecutive SOFs in bit times
/// 1/12 = 0,08333333
/// 1(ms) / 0,0833333 = 12
/// FrameInterval = 12x1000 = 12000
#define FRAMEINTERVAL    12000
/// FSLargestDataPacket
/// This field specifies a value which is loaded into the Largest
/// Data Packet Counter at the beginning of each frame.

/// overhead.
/// The value of MAXIMUM_OVERHEAD below is 210 bit times.
#define MAXIMUM_OVERHEAD 210
/// FSLargestDataPacket initializes a counter within the Host Controller that is
/// used to determine if a transaction on USB can be completed before EOF
/// processing must start.
/// It is a function of the new FrameInterval and is calculated by subtracting
/// from FrameInterval the maximum number of bit times for transaction overhead
/// on USB and the number of bit times needed for EOF processing, then 
/// multiplying the result by 6/7 to account for the worst case bit stuffing 
#define FSLARGESTDATAPACKET (((FRAMEINTERVAL-MAXIMUM_OVERHEAD) * 6) / 7)
#define OHCI_FMINTERVAL ((FSLARGESTDATAPACKET << 16) | FRAMEINTERVAL)

/// HcPeriodicStart Register
// The HcPeriodicStart register has a 14-bit programmable value which 
// determines when is the earliest time HC should start processing the 
// periodic list.
/// Set HcPeriodicStart to a value that is 90% of the value in FrameInterval 
/// field of the HcFmInterval register.
#define OHCI_PRDSTRT    (FRAMEINTERVAL*90/100)


//------------------------------------------------------------------------------
//      Internal Functions
//------------------------------------------------------------------------------

#ifdef AT91C_BASE_UHPHS_OHCI
#define AT91C_ID_UHP           AT91C_ID_UHPHS
#define AT91C_BASE_UHP         AT91C_BASE_UHPHS_OHCI
#define UHP_HcRhPortStatus     UHPHS_OHCI_HcRhPortStatus
#define UHP_HcControlHeadED    UHPHS_OHCI_HcControlHeadED
#define UHP_HcControlCurrentED UHPHS_OHCI_HcControlCurrentED
#define UHP_HcBulkDoneHead     UHPHS_OHCI_HcBulkDoneHead
#define UHP_HcControl          UHPHS_OHCI_HcControl
#define UHP_HcRhStatus         UHPHS_OHCI_HcRhStatus
#define UHP_HcHCCA             UHPHS_OHCI_HcHCCA
#define UHP_HcFmInterval       UHPHS_OHCI_HcFmInterval
#define UHP_HcPeriodicStart    UHPHS_OHCI_HcPeriodicStart
#endif

//------------------------------------------------------------------------------
/// Enable UHP clock
//------------------------------------------------------------------------------
static inline void UHP_EnablePeripheralClock(void)
{
    AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_UHP);
}

//------------------------------------------------------------------------------
/// Disable UHP clock
//------------------------------------------------------------------------------
static inline void UHP_DisablePeripheralClock( void )
{
    AT91C_BASE_PMC->PMC_PCDR = (1 << AT91C_ID_UHP);
}

//------------------------------------------------------------------------------
/// Enables the 48MHz USB clock.
//------------------------------------------------------------------------------
static inline void UHP_EnableUsbClock(void)
{
#ifdef AT91C_PMC_USBS_USB_UPLL
    AT91C_BASE_PMC->PMC_USB = AT91C_PMC_USBS_USB_UPLL | AT91C_PMC_USBDIV_10;
#endif
#ifdef AT91C_CKGR_UPLLEN_ENABLED
    AT91C_BASE_PMC->PMC_UCKR = AT91C_CKGR_UPLLEN_ENABLED;
#endif
    AT91C_BASE_PMC->PMC_SCER = AT91C_PMC_UHP;
}

//------------------------------------------------------------------------------
///  Disables the 48MHz USB clock.
//------------------------------------------------------------------------------
static inline void UHP_DisableUsbClock(void)
{
    AT91C_BASE_PMC->PMC_SCDR = AT91C_PMC_UHP;
#ifdef AT91C_CKGR_UPLLEN_ENABLED
    AT91C_BASE_PMC->PMC_UCKR = AT91C_CKGR_UPLLEN_ENABLED;
#endif
}

//------------------------------------------------------------------------------
//      Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Detect a connected device
//------------------------------------------------------------------------------
unsigned char USBH_IsDeviceConnectedOnPort(unsigned char portNumber)
{
  return (AT91C_BASE_UHP->UHP_HcRhPortStatus[portNumber] & 0x01);
}

//------------------------------------------------------------------------------
/// Reset the port number
//------------------------------------------------------------------------------
void USBH_ResetPort(unsigned char portNumber)
{
    // SetPortReset
    AT91C_BASE_UHP->UHP_HcRhPortStatus[portNumber] = (1 << 4);
    // Wait for the end of reset
    while (AT91C_BASE_UHP->UHP_HcRhPortStatus[portNumber] & (1 << 4));
     // SetPortEnable
    AT91C_BASE_UHP->UHP_HcRhPortStatus[portNumber] = (1 << 1);
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void USBH_program(unsigned int* pBHED, unsigned int* pBCED, OHCI_HCCA* pHCCA)
{
    // Programming the BHED
    // The HcBulkHeadED register contains the physical address of the first 
    // Endpoint Descriptor of the Bulk list.
    AT91C_BASE_UHP->UHP_HcControlHeadED = (unsigned int) pBHED;

    // Programming the BCED
    // The HcBulkCurrentED register contains the physical address of the current
    // endpoint of the Bulk list.
    AT91C_BASE_UHP->UHP_HcControlCurrentED = (unsigned int) pBCED;

    // Initializing the UHP_HcDoneHead
    AT91C_BASE_UHP->UHP_HcBulkDoneHead   = 0x00;
    pHCCA->UHP_HccaDoneHead = 0x0000;

    // Forcing UHP_Hc to Operational State
    AT91C_BASE_UHP->UHP_HcControl = 0x80;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
void USBH_enablingPort(void)    
{
    // Enabling port power
    AT91C_BASE_UHP->UHP_HcRhPortStatus[0] = 0x00000100;
    AT91C_BASE_UHP->UHP_HcRhPortStatus[1] = 0x00000100;
    AT91C_BASE_UHP->UHP_HcRhStatus = 0x00010000;
}

//------------------------------------------------------------------------------
/// Initializes the specified USB driver
/// This function initializes the current FIFO bank of endpoints,
/// configures the pull-up and VBus lines, disconnects the pull-up and
/// then trigger the Init callback.
//------------------------------------------------------------------------------
void USBH_Init(OHCI_HCCA* pHCCA)
{
    TRACE_DEBUG_WP("USBH Init()\n\r");

    UHP_EnablePeripheralClock();
    UHP_EnableUsbClock();

    // Forcing UHP_Hc to reset
    AT91C_BASE_UHP->UHP_HcControl = 0;

    // Writing the UHP_HCCA
    AT91C_BASE_UHP->UHP_HcHCCA = (unsigned int) &pHCCA;

    // Enabling list processing
    AT91C_BASE_UHP->UHP_HcControl = 0;

    // HcFmInterval register is used to control the length of USB frames
    AT91C_BASE_UHP->UHP_HcFmInterval = OHCI_FMINTERVAL;
    // The HcPeriodicStart register has a 14-bit programmable value which 
    // determines when is the earliest time HC should start processing the 
    // periodic list.
    AT91C_BASE_UHP->UHP_HcPeriodicStart = OHCI_PRDSTRT;
}

