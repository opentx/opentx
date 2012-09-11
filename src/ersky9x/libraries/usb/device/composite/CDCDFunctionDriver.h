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

#ifndef CDCDFUNCTIONDRIVER_H
#define CDCDFUNCTIONDRIVER_H

//-----------------------------------------------------------------------------
//         Headers
//-----------------------------------------------------------------------------

#include <usb/device/core/USBD.h>
#include <usb/device/core/USBDDriver.h>
#include <usb/common/cdc/CDCLineCoding.h>

//-----------------------------------------------------------------------------
//         Definitions
//-----------------------------------------------------------------------------

/// Indicates the receiver carrier signal is present.
#define CDCD_STATE_RXDRIVER         (1 << 0)
/// Indicates the transmission carrier signal is present.
#define CDCD_STATE_TXCARRIER        (1 << 1)
/// Indicates a break has been detected.
#define CDCD_STATE_BREAK            (1 << 2)
/// Indicates a ring signal has been detected.
#define CDCD_STATE_RINGSIGNAL       (1 << 3)
/// Indicates a framing error has occured.
#define CDCD_STATE_FRAMING          (1 << 4)
/// Indicates a parity error has occured.
#define CDCD_STATE_PARITY           (1 << 5)
/// Indicates a data overrun error has occured.
#define CDCD_STATE_OVERRUN          (1 << 6)

//-----------------------------------------------------------------------------
//         Structs
//-----------------------------------------------------------------------------

/// CDC Serial port struct
typedef struct _CDCDSerialPort{

    /// USB interface settings
    unsigned char interfaceNum;
    unsigned char interruptInEndpoint;
    unsigned char bulkInEndpoint;
    unsigned char bulkOutEndpoint;
    /// serial port settings
    CDCLineCoding lineCoding;
    unsigned char isCarrierActivated;
    unsigned short serialState;

} CDCDSerialPort;

//-----------------------------------------------------------------------------
//         Macros
//-----------------------------------------------------------------------------

#define CDCDFunctionDriver_ConfigurePort(pPort,ifNum,intIN,bulkIN,bulkOUT) \
{\
    (pPort)->interfaceNum=(ifNum);(pPort)->interruptInEndpoint=(intIN); \
    (pPort)->bulkInEndpoint=(bulkIN);(pPort)->bulkOutEndpoint=(bulkOUT); \
}

//-----------------------------------------------------------------------------
//         Callbacks
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//         Exported functions
//-----------------------------------------------------------------------------

//- Function API for composite device
extern void CDCDFunctionDriver_Initialize(USBDDriver * pUsbdDriver,
                                          CDCDSerialPort * pCdcPorts,
                                          unsigned char numPorts);

extern unsigned char CDCDFunctionDriver_RequestHandler(
    const USBGenericRequest * request);

//- CDC Serial Port API
extern unsigned char CDCDSerialDriver_Write(
    unsigned char port,
    void *data,
    unsigned int size,
    TransferCallback callback,
    void *argument);

extern unsigned char CDCDSerialDriver_Read(
    unsigned char port,
    void *data,
    unsigned int size,
    TransferCallback callback,
    void *argument);

extern unsigned short CDCDSerialDriver_GetSerialState(unsigned char port);

extern void CDCDSerialDriver_SetSerialState(
    unsigned char port,
    unsigned short serialState);


#endif // #define CDCDFUNCTIONDRIVER_H

