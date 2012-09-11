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

#include "USBD.h"
#include "USBDCallbacks.h"
#include "USBDDriver.h"
#include <board.h>
#include <pio/pio.h>
#include <utility/trace.h>
#include <utility/led.h>
#include <usb/common/core/USBConfigurationOTG.h>
#include <usb/common/core/USBEndpointDescriptor.h>
#include <usb/common/core/USBGenericRequest.h>
#include <usb/common/core/USBFeatureRequest.h>
#include <pmc/pmc.h>

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

/// Maximum number of endpoints interrupts.
#define NUM_IT_MAX       \
   (AT91C_BASE_OTGHS->OTGHS_IPFEATURES & AT91C_OTGHS_EPT_NBR_MAX)
/// Maximum number of endpoint DMA interrupts
#define NUM_IT_MAX_DMA   \
   ((AT91C_BASE_OTGHS->OTGHS_IPFEATURES & AT91C_OTGHS_DMA_CHANNEL_NBR)>>4)
/// Bits that should be shifted to access DMA control bits.
#define SHIFT_DMA        24
/// Bits that should be shifted to access interrupt bits.
#define SHIFT_INTERUPT   12

/// Max size of the FMA FIFO
#define DMA_MAX_FIFO_SIZE    32768

#define EPT_VIRTUAL_SIZE      8192

//------------------------------------------------------------------------------
/// \page "Endpoint states"
/// This page lists the endpoint states.
/// !States
//  - UDP_ENDPOINT_DISABLED
//  - UDP_ENDPOINT_HALTED
//  - UDP_ENDPOINT_IDLE
//  - UDP_ENDPOINT_SENDING
//  - UDP_ENDPOINT_RECEIVING

/// Endpoint states: Endpoint is disabled
#define UDP_ENDPOINT_DISABLED       0
/// Endpoint states: Endpoint is halted (i.e. STALLs every request)
#define UDP_ENDPOINT_HALTED         1
/// Endpoint states: Endpoint is idle (i.e. ready for transmission)
#define UDP_ENDPOINT_IDLE           2
/// Endpoint states: Endpoint is sending data
#define UDP_ENDPOINT_SENDING        3
/// Endpoint states: Endpoint is receiving data
#define UDP_ENDPOINT_RECEIVING      4
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//      Structures
//------------------------------------------------------------------------------

/// Describes an ongoing transfer on a UDP endpoint.
typedef struct
{
    /// Pointer to a data buffer used for emission/reception.
    char             *pData;
    /// Number of bytes which have been written into the UDP internal FIFO
    /// buffers.
    volatile int     buffered;
    /// Number of bytes which have been sent/received.
    volatile int     transferred;
    /// Number of bytes which have not been buffered/transferred yet.
    volatile int     remaining;
    /// Optional callback to invoke when the transfer completes.
    volatile TransferCallback fCallback;
    /// Optional argument to the callback function.
    void             *pArgument;
} Transfer;

//------------------------------------------------------------------------------
/// Describes the state of an endpoint of the UDP controller.
//------------------------------------------------------------------------------
typedef struct
{
    /// Current endpoint state.
    volatile unsigned char  state;
    /// Current reception bank (0 or 1).
    unsigned char  bank;
    /// Maximum packet size for the endpoint.
    unsigned short size;
    /// Describes an ongoing transfer (if current state is either
    ///  <UDP_ENDPOINT_SENDING> or <UDP_ENDPOINT_RECEIVING>)
    Transfer       transfer;
    /// Special case for send a ZLP
    unsigned char  sendZLP;
} Endpoint;

//------------------------------------------------------------------------------
//         Internal variables
//------------------------------------------------------------------------------

/// Holds the internal state for each endpoint of the UDP.
static Endpoint      endpoints[CHIP_USB_NUMENDPOINTS];
/// Device current state.
static unsigned char deviceState;
/// Indicates the previous device state
static unsigned char previousDeviceState;

/// 7.1.20 Test Mode Support
/// Test codes for the USB HS test mode.
static const char test_packet_buffer[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,                // JKJKJKJK * 9
    0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,                     // JJKKJJKK * 8
    0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,                     // JJJJKKKK * 8
    0xFE,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // JJJJJJJKKKKKKK * 8
    0x7F,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,                          // JJJJJJJK * 8
    0xFC,0x7E,0xBF,0xDF,0xEF,0xF7,0xFB,0xFD,0x7E                 // {JKKKKKKK * 10}, JK
};

//------------------------------------------------------------------------------
//      Internal Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Enable UDPHS clock
//------------------------------------------------------------------------------
static inline void OTGHS_EnableUsbClock( void )
{

}

//------------------------------------------------------------------------------
/// Disable UDPHS clock
//------------------------------------------------------------------------------
static inline void OTGHS_DisableUsbClock( void )
{

}

//------------------------------------------------------------------------------
/// Enables the transceiver of the USB controller
//------------------------------------------------------------------------------
static void OTGHS_EnableTransceiver(void)
{
    AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_OTGPADE;
}

//------------------------------------------------------------------------------
/// Disables the transceiver of the USB controller associated with the specified
/// USB driver
//------------------------------------------------------------------------------
static void OTGHS_DisableTransceiver(void)
{
    AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_OTGPADE;
}

//------------------------------------------------------------------------------
/// Handles a completed transfer on the given endpoint, invoking the
/// configured callback if any.
/// \param bEndpoint Number of the endpoint for which the transfer has completed.
/// \param bStatus   Status code returned by the transfer operation
//------------------------------------------------------------------------------
static void OTGHS_EndOfTransfer( unsigned char bEndpoint, char bStatus )
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);
    Transfer *pTransfer = &(pEndpoint->transfer);

    // Check that endpoint was sending or receiving data
    if( (pEndpoint->state == UDP_ENDPOINT_RECEIVING)
     || (pEndpoint->state == UDP_ENDPOINT_SENDING) ) {

        TRACE_DEBUG_WP("Eo");
        if(pEndpoint->state == UDP_ENDPOINT_SENDING) {
            pEndpoint->sendZLP = 0;
        }
        // Endpoint returns in Idle state
        pEndpoint->state = UDP_ENDPOINT_IDLE;

        // Invoke callback is present
        if (pTransfer->fCallback != 0) {

            ((TransferCallback) pTransfer->fCallback)
                (pTransfer->pArgument,
                 bStatus,
                 pTransfer->transferred,
                 pTransfer->remaining + pTransfer->buffered);
        }
        else {
            TRACE_DEBUG_WP("No callBack\n\r");
        }
    }
}

//------------------------------------------------------------------------------
/// Transfers a data payload from the current tranfer buffer to the endpoint
/// FIFO
/// \param bEndpoint Number of the endpoint which is sending data.
//------------------------------------------------------------------------------
static void OTGHS_WritePayload( unsigned char bEndpoint )
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);
    Transfer *pTransfer = &(pEndpoint->transfer);
    char     *pFifo;
    signed int   size;
    unsigned int dCtr;

    pFifo = (char*)((unsigned int *)AT91C_BASE_OTGHS_EPTFIFO + (EPT_VIRTUAL_SIZE * bEndpoint));

    // Get the number of bytes to send
    size = pEndpoint->size;
    if (size > pTransfer->remaining) {

        size = pTransfer->remaining;
    }

    // Update transfer descriptor information
    pTransfer->buffered += size;
    pTransfer->remaining -= size;

    // Write packet in the FIFO buffer
    dCtr = 0;
    while (size > 0) {

        pFifo[dCtr] = *(pTransfer->pData);
        pTransfer->pData++;
        size--;
        dCtr++;
    }
}

//------------------------------------------------------------------------------
/// Transfers a data payload from an endpoint FIFO to the current transfer buffer
/// \param bEndpoint   Endpoint number.
/// \param wPacketSize Size of received data packet
//------------------------------------------------------------------------------
static void OTGHS_ReadPayload( unsigned char bEndpoint, int wPacketSize )
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);
    Transfer *pTransfer = &(pEndpoint->transfer);
    char     *pFifo;
    unsigned char dBytes=0;

    pFifo = (char*)((unsigned int *)AT91C_BASE_OTGHS_EPTFIFO + (EPT_VIRTUAL_SIZE * bEndpoint));

    // Check that the requested size is not bigger than the remaining transfer
    if (wPacketSize > pTransfer->remaining) {

        pTransfer->buffered += wPacketSize - pTransfer->remaining;
        wPacketSize = pTransfer->remaining;
    }

    // Update transfer descriptor information
    pTransfer->remaining -= wPacketSize;
    pTransfer->transferred += wPacketSize;

    // Retrieve packet
    while (wPacketSize > 0) {

        *(pTransfer->pData) = pFifo[dBytes];
        pTransfer->pData++;
        wPacketSize--;
        dBytes++;
    }
}

//------------------------------------------------------------------------------
/// Received SETUP packet from endpoint 0 FIFO
/// \param pRequest Generic USB SETUP request sent over Control endpoints
//------------------------------------------------------------------------------
static void OTGHS_ReadRequest( USBGenericRequest *pRequest )
{
    unsigned int *pData = (unsigned int *)pRequest;
    unsigned int fifo;

    fifo = (AT91C_BASE_OTGHS_EPTFIFO->OTGHS_READEPT0[0]);
    *pData = fifo;
    fifo = (AT91C_BASE_OTGHS_EPTFIFO->OTGHS_READEPT0[0]);
    pData++;
    *pData = fifo;
    //TRACE_ERROR("SETUP: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n\r", pData[0],pData[1],pData[2],pData[3],pData[4],pData[5],pData[6],pData[7]);
}

//------------------------------------------------------------------------------
/// Reset all endpoint transfer descriptors
//------------------------------------------------------------------------------
static void OTGHS_ResetEndpoints( void )
{
    Endpoint *pEndpoint;
    Transfer *pTransfer;

    unsigned char bEndpoint;

    // Reset the transfer descriptor of every endpoint
    for( bEndpoint = 0; bEndpoint < CHIP_USB_NUMENDPOINTS; bEndpoint++ ) {

        pEndpoint = &(endpoints[bEndpoint]);
        pTransfer = &(pEndpoint->transfer);

        // Reset endpoint transfer descriptor
        pTransfer->pData = 0;
        pTransfer->transferred = -1;
        pTransfer->buffered = -1;
        pTransfer->remaining = -1;
        pTransfer->fCallback = 0;
        pTransfer->pArgument = 0;

        // Reset endpoint state
        pEndpoint->bank = 0;
        pEndpoint->state = UDP_ENDPOINT_DISABLED;
        // Reset ZLP
        pEndpoint->sendZLP = 0;
    }
}


//------------------------------------------------------------------------------
/// Disable all endpoints (except control endpoint 0), aborting current 
/// transfers if necessary
//------------------------------------------------------------------------------
static void OTGHS_DisableEndpoints( void )
{
    unsigned char bEndpoint;

    // Disable each endpoint, terminating any pending transfer


    // Control endpoint 0 is not disabled
    for( bEndpoint = 1; bEndpoint < CHIP_USB_NUMENDPOINTS; bEndpoint++ ) {

        OTGHS_EndOfTransfer( bEndpoint, USBD_STATUS_ABORTED );
        endpoints[bEndpoint].state = UDP_ENDPOINT_DISABLED;
    }
}


//------------------------------------------------------------------------------
/// Endpoint interrupt handler.
/// Handle IN/OUT transfers, received SETUP packets and STALLing
/// \param bEndpoint Index of endpoint
//------------------------------------------------------------------------------
static void OTGHS_EndpointHandler( unsigned char bEndpoint )
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);
    Transfer *pTransfer = &(pEndpoint->transfer);
    unsigned int   status = AT91C_BASE_OTGHS->OTGHS_DEVEPTISR[bEndpoint];
    unsigned short wPacketSize;
    USBGenericRequest request;

    TRACE_DEBUG_WP("E%d ", bEndpoint);
    TRACE_DEBUG_WP("st:0x%X ", status);

    // Handle interrupts
    // IN packet sent
    if((AT91C_BASE_OTGHS->OTGHS_DEVEPTIMR[bEndpoint] & AT91C_OTGHS_TXINI)
    && (status & AT91C_OTGHS_TXINI )) {

        TRACE_DEBUG_WP("Wr ");

        // Check that endpoint was in Sending state
        if( pEndpoint->state == UDP_ENDPOINT_SENDING ) {

            if (pTransfer->buffered > 0) {
                pTransfer->transferred += pTransfer->buffered;
                pTransfer->buffered = 0;
            }

            if(  ((pTransfer->buffered)==0)
               &&((pTransfer->transferred)==0)
               &&((pTransfer->remaining)==0)
               &&(pEndpoint->sendZLP == 0)) {
                pEndpoint->sendZLP = 1;
            }

            // End of transfer ?
            if( (pTransfer->remaining > 0)
              ||(pEndpoint->sendZLP == 1)) {

                pEndpoint->sendZLP = 2;
                // Transfer remaining data
                TRACE_DEBUG_WP(" %d ", pEndpoint->size);
                // Send next packet
                OTGHS_WritePayload(bEndpoint);

                // Send Token IN
                AT91C_BASE_OTGHS->OTGHS_DEVEPTICR[bEndpoint] = AT91C_OTGHS_TXINI;
                // For a non-control endpoint, the FIFOCON bit must be cleared
                // to start the transfer
                if ((AT91C_OTGHS_EPT_TYPE & AT91C_BASE_OTGHS->OTGHS_DEVEPTCFG[bEndpoint])
                                             != AT91C_OTGHS_EPT_TYPE_CTL_EPT) {
                    // Send IN
                    AT91C_BASE_OTGHS->OTGHS_DEVEPTIDR[bEndpoint] = AT91C_OTGHS_FIFOCON;
                }
            }
            else {

                TRACE_DEBUG_WP("\n\r0pTransfer->buffered %d \n\r", pTransfer->buffered);
                TRACE_DEBUG_WP("0pTransfer->transferred %d \n\r", pTransfer->transferred);
                TRACE_DEBUG_WP("0pTransfer->remaining %d \n\r", pTransfer->remaining);

                TRACE_DEBUG_WP(" %d ", pTransfer->transferred);

                // Disable interrupt if this is not a control endpoint
                if ((AT91C_OTGHS_EPT_TYPE & AT91C_BASE_OTGHS->OTGHS_DEVEPTCFG[bEndpoint])
                    != AT91C_OTGHS_EPT_TYPE_CTL_EPT) {

                    AT91C_BASE_OTGHS->OTGHS_DEVIDR = 1<<SHIFT_INTERUPT<<bEndpoint;

                }
                AT91C_BASE_OTGHS->OTGHS_DEVEPTIDR[bEndpoint] = AT91C_OTGHS_TXINI;
                OTGHS_EndOfTransfer(bEndpoint, USBD_STATUS_SUCCESS);
                pEndpoint->sendZLP = 0;
            }
        }
        else {
              TRACE_DEBUG("Error Wr %d", pEndpoint->sendZLP);
        }
    }

    // OUT packet received
    if( AT91C_OTGHS_RXOUT == (status & AT91C_OTGHS_RXOUT) ) {

        // Check that the endpoint is in Receiving state
        if (pEndpoint->state != UDP_ENDPOINT_RECEIVING) {

            // Check if an ACK has been received on a Control endpoint
            if( ((AT91C_OTGHS_EPT_TYPE & AT91C_BASE_OTGHS->OTGHS_DEVEPTCFG[bEndpoint])
                  == AT91C_OTGHS_EPT_TYPE_CTL_EPT)
             && (0 == (status & AT91C_OTGHS_BYCT)) ) {

                // Control endpoint, 0 bytes received
                // Acknowledge the data and finish the current transfer
                TRACE_DEBUG_WP("Ack ");
                AT91C_BASE_OTGHS->OTGHS_DEVEPTICR[bEndpoint] = AT91C_OTGHS_RXOUT;
                AT91C_BASE_OTGHS->OTGHS_DEVEPTIDR[bEndpoint] = AT91C_OTGHS_RXOUT;
                //OTGHS_EndOfTransfer(bEndpoint, USBD_STATUS_SUCCESS);
            }
            // Check if the data has been STALLed
            else if( AT91C_OTGHS_STALL == (status & AT91C_OTGHS_STALL)) {

                // Discard STALLed data
                TRACE_DEBUG_WP("Discard ");
                AT91C_BASE_OTGHS->OTGHS_DEVEPTICR[bEndpoint] = AT91C_OTGHS_RXOUT;
            }
            else {
                // NAK the data
                TRACE_DEBUG_WP("Nak ");
                AT91C_BASE_OTGHS->OTGHS_DEVIDR = 1<<SHIFT_INTERUPT<<bEndpoint;
            }
        }
        else {

            // Endpoint is in Read state
            // Retrieve data and store it into the current transfer buffer
            wPacketSize = (unsigned short) ((status >> 20) & 0x7FF);

            //TRACE_ERROR_WP("out:%d ", wPacketSize);
            OTGHS_ReadPayload(bEndpoint, wPacketSize);
            AT91C_BASE_OTGHS->OTGHS_DEVEPTICR[bEndpoint] = AT91C_OTGHS_RXOUT;
            if((AT91C_OTGHS_EPT_TYPE & AT91C_BASE_OTGHS->OTGHS_DEVEPTCFG[bEndpoint])
                  != AT91C_OTGHS_EPT_TYPE_CTL_EPT) {
                AT91C_BASE_OTGHS->OTGHS_DEVEPTIDR[bEndpoint] = AT91C_OTGHS_FIFOCON;
            }

            // Check if the transfer is finished
            if ((pTransfer->remaining == 0) || (wPacketSize < pEndpoint->size)) {

                AT91C_BASE_OTGHS->OTGHS_DEVEPTIDR[bEndpoint] = AT91C_OTGHS_RXOUT;

                // Disable interrupt if this is not a control endpoint
                if ((AT91C_OTGHS_EPT_TYPE & AT91C_BASE_OTGHS->OTGHS_DEVEPTCFG[bEndpoint])
                    != AT91C_OTGHS_EPT_TYPE_CTL_EPT) {

                    AT91C_BASE_OTGHS->OTGHS_DEVIDR = 1<<SHIFT_INTERUPT<<bEndpoint;
                }
                //TRACE_ERROR_WP("Y ");
                OTGHS_EndOfTransfer(bEndpoint, USBD_STATUS_SUCCESS);
            }
        }
    }

    // STALL sent
    if( AT91C_OTGHS_STALL == (status & AT91C_OTGHS_STALL) ) {

        TRACE_WARNING( "Sta 0x%X [%d] ", status, bEndpoint);

        // Acknowledge the stall flag
        AT91C_BASE_OTGHS->OTGHS_DEVEPTICR[bEndpoint] = AT91C_OTGHS_STALL;

        // If the endpoint is not halted, clear the STALL condition
        if (pEndpoint->state != UDP_ENDPOINT_HALTED) {

            TRACE_WARNING("_ " );
            AT91C_BASE_OTGHS->OTGHS_DEVEPTIDR[bEndpoint] = AT91C_OTGHS_STALLRQ;
        }
    }

    // SETUP packet received
    if( AT91C_OTGHS_RXSTP == (status & AT91C_OTGHS_RXSTP) )  {

        TRACE_DEBUG_WP("Stp ");

        // If a transfer was pending, complete it
        // Handles the case where during the status phase of a control write
        // transfer, the host receives the device ZLP and ack it, but the ack
        // is not received by the device
        if ((pEndpoint->state == UDP_ENDPOINT_RECEIVING)
            || (pEndpoint->state == UDP_ENDPOINT_SENDING)) {

            OTGHS_EndOfTransfer(bEndpoint, USBD_STATUS_SUCCESS);
        }

        // Copy the setup packet
        OTGHS_ReadRequest(&request);

        // Acknowledge setup packet
        AT91C_BASE_OTGHS->OTGHS_DEVEPTICR[bEndpoint] = AT91C_OTGHS_RXSTP;

        // Forward the request to the upper layer
        USBDCallbacks_RequestReceived(&request);
    }
}

//------------------------------------------------------------------------------
//      Interrupt service routine
//------------------------------------------------------------------------------
#ifdef DMA
//----------------------------------------------------------------------------
/// Endpoint DMA interrupt handler.
/// This function (ISR) handles dma interrupts
/// \param bEndpoint Index of endpoint
//----------------------------------------------------------------------------
static void OTGHS_DmaHandler( unsigned char bEndpoint )
{
    Endpoint     *pEndpoint = &(endpoints[bEndpoint]);
    Transfer     *pTransfer = &(pEndpoint->transfer);
    int           justTransferred;
    unsigned int  status;
    unsigned char result = USBD_STATUS_SUCCESS;

    status = AT91C_BASE_OTGHS->OTGHS_DEVDMA[bEndpoint].OTGHS_DEVDMASTATUS;
    TRACE_DEBUG_WP("Dma Ept%d ", bEndpoint);

    // Disable DMA interrupt to avoid receiving 2 interrupts (B_EN and TR_EN)
    AT91C_BASE_OTGHS->OTGHS_DEVDMA[bEndpoint].OTGHS_DEVDMACONTROL &=
        ~(unsigned int)(AT91C_OTGHS_END_TR_EN | AT91C_OTGHS_END_B_EN);

    AT91C_BASE_OTGHS->OTGHS_DEVIDR = (1<<SHIFT_DMA<<bEndpoint);

    if( AT91C_OTGHS_END_BF_ST == (status & AT91C_OTGHS_END_BF_ST) ) {

        TRACE_DEBUG_WP("EndBuffer ");

        // BUFF_COUNT holds the number of untransmitted bytes.
        // BUFF_COUNT is equal to zero in case of good transfer
        justTransferred = pTransfer->buffered
                                 - ((status & (unsigned int)AT91C_OTGHS_BUFF_COUNT) >> 16);
        pTransfer->transferred += justTransferred;

        pTransfer->buffered = ((status & (unsigned int)AT91C_OTGHS_BUFF_COUNT) >> 16);

        pTransfer->remaining -= justTransferred;

        TRACE_DEBUG_WP("\n\r1pTransfer->buffered %d \n\r", pTransfer->buffered);
        TRACE_DEBUG_WP("1pTransfer->transferred %d \n\r", pTransfer->transferred);
        TRACE_DEBUG_WP("1pTransfer->remaining %d \n\r", pTransfer->remaining);

        if( (pTransfer->remaining + pTransfer->buffered) > 0 ) {

            // Prepare an other transfer
            if( pTransfer->remaining > DMA_MAX_FIFO_SIZE ) {

                pTransfer->buffered = DMA_MAX_FIFO_SIZE;    
            }
            else {
                pTransfer->buffered = pTransfer->remaining;
            }

            AT91C_BASE_OTGHS->OTGHS_DEVDMA[bEndpoint].OTGHS_DEVDMAADDRESS = 
                (unsigned int)((pTransfer->pData) + (pTransfer->transferred));

            // Clear unwanted interrupts
            AT91C_BASE_OTGHS->OTGHS_DEVDMA[bEndpoint].OTGHS_DEVDMASTATUS;

            // Enable DMA endpoint interrupt
            AT91C_BASE_OTGHS->OTGHS_DEVIER = (1 << SHIFT_DMA << bEndpoint);
            // DMA config for receive the good size of buffer, or an error buffer

            AT91C_BASE_OTGHS->OTGHS_DEVDMA[bEndpoint].OTGHS_DEVDMACONTROL = 0; // raz
            AT91C_BASE_OTGHS->OTGHS_DEVDMA[bEndpoint].OTGHS_DEVDMACONTROL =
                                     ( ((pTransfer->buffered << 16) & AT91C_OTGHS_BUFF_COUNT)
                                       | AT91C_OTGHS_END_TR_EN
                                       | AT91C_OTGHS_END_TR_IT
                                       | AT91C_OTGHS_END_B_EN
                                       | AT91C_OTGHS_END_BUFFIT
                                       | AT91C_OTGHS_CHANN_ENB );
        }
    }
    else if( AT91C_OTGHS_END_TR_ST == (status & AT91C_OTGHS_END_TR_ST) ) {

        TRACE_DEBUG_WP("EndTransf ");

        pTransfer->transferred = pTransfer->buffered
                                 - ((status & (unsigned int)AT91C_OTGHS_BUFF_COUNT) >> 16);
        pTransfer->remaining = 0;
        TRACE_DEBUG_WP("\n\r0pTransfer->buffered %d \n\r", pTransfer->buffered);
        TRACE_DEBUG_WP("0pTransfer->transferred %d \n\r", pTransfer->transferred);
        TRACE_DEBUG_WP("0pTransfer->remaining %d \n\r", pTransfer->remaining);
    }
    else {

        TRACE_ERROR("OTGHS_DmaHandler: Error (0x%08X)\n\r", status);
        result = USBD_STATUS_ABORTED;
    }

    // Invoke callback
    if( pTransfer->remaining == 0 ) {

        TRACE_DEBUG_WP("EOT ");
        OTGHS_EndOfTransfer(bEndpoint, result);
    }
}
#endif

//------------------------------------------------------------------------------
//      Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// USB device interrupt handler
/// Manages device resume, suspend, end of bus reset. 
/// Forwards endpoint interrupts to the appropriate handler.
//------------------------------------------------------------------------------
void USBD_IrqHandler(void)
{
    unsigned int  status;
    unsigned char numIT;

    if (deviceState >= USBD_STATE_POWERED) {

        LED_Set(USBD_LEDUSB);
    }

#ifndef OTG_PROJECT 
    // Get interrupts status
    status = AT91C_BASE_OTGHS->OTGHS_SR & AT91C_BASE_OTGHS->OTGHS_CTRL & 0xFF;
    while (status != 0) {
        //TRACE_ERROR_WP("~");
        if((status&AT91C_OTGHS_VBUSTI)==AT91C_OTGHS_VBUSTI) {
            TRACE_DEBUG_WP("__VBus\n\r");

            USBD_Connect();

            // Acknowledge the interrupt
            AT91C_BASE_OTGHS->OTGHS_SCR = AT91C_OTGHS_VBUSTI;
        }

        // Don't treat others interrupt for this time
        AT91C_BASE_OTGHS->OTGHS_SCR = AT91C_OTGHS_IDT    | AT91C_OTGHS_SRP 
                              | AT91C_OTGHS_VBERR  | AT91C_OTGHS_BCERR
                              | AT91C_OTGHS_ROLEEX | AT91C_OTGHS_HNPERR
                              | AT91C_OTGHS_STO;

        AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_IDT; 
    
        status = AT91C_BASE_OTGHS->OTGHS_SR & AT91C_BASE_OTGHS->OTGHS_CTRL & 0xFF;
    }
#endif

    // Get OTG Device interrupts status
    status = AT91C_BASE_OTGHS->OTGHS_DEVISR & AT91C_BASE_OTGHS->OTGHS_DEVIMR;
    //TRACE_ERROR_WP("S=0x%X\n\r", AT91C_BASE_OTGHS->OTGHS_DEVISR );
    //TRACE_ERROR_WP("M=0x%X\n\r", AT91C_BASE_OTGHS->OTGHS_DEVIMR );
    while (status != 0) {
        //TRACE_ERROR_WP("=");
        // Start Of Frame (SOF)
        if((status&AT91C_OTGHS_SOF)==AT91C_OTGHS_SOF) {
            TRACE_DEBUG_WP("SOF ");

            // Invoke the SOF callback
            //USB_StartOfFrameCallback();

            // Acknowledge interrupt
            AT91C_BASE_OTGHS->OTGHS_DEVICR = AT91C_OTGHS_SOF;
            status &= ~(unsigned int)AT91C_OTGHS_SOF;
        }

        // Suspend
        // This interrupt is always treated last (hence the '==')
        else if (status == AT91C_OTGHS_SUSP) {

            TRACE_DEBUG_WP("S");

            // The device enters the Suspended state
            //      MCK + UDPCK must be off
            //      Pull-Up must be connected
            //      Transceiver must be disabled

            LED_Clear(USBD_LEDUSB);

            // Enable wakeup
            AT91C_BASE_OTGHS->OTGHS_DEVIER = AT91C_OTGHS_EORST | AT91C_OTGHS_WAKEUP | AT91C_OTGHS_EORSM;

            // Acknowledge interrupt
            AT91C_BASE_OTGHS->OTGHS_DEVICR = AT91C_OTGHS_SUSP;
            previousDeviceState = deviceState;
            deviceState = USBD_STATE_SUSPENDED;
            OTGHS_DisableTransceiver();
            OTGHS_DisableUsbClock();
            // Invoke the Suspend callback
            USBDCallbacks_Suspended();
        }

        // Resume
        else if( ((status & AT91C_OTGHS_WAKEUP) != 0)    // line activity
              || ((status & AT91C_OTGHS_EORSM) != 0))  { // pc wakeup

            // Invoke the Resume callback
            USBDCallbacks_Resumed();

            TRACE_DEBUG_WP("R");

            OTGHS_EnableUsbClock();
            OTGHS_EnableTransceiver();

            // The device enters Configured state
            //      MCK + UDPCK must be on
            //      Pull-Up must be connected
            //      Transceiver must be enabled

            deviceState = previousDeviceState;

            AT91C_BASE_OTGHS->OTGHS_DEVICR = 
                (AT91C_OTGHS_WAKEUP | AT91C_OTGHS_EORSM | AT91C_OTGHS_SUSP);
            AT91C_BASE_OTGHS->OTGHS_DEVIER = (AT91C_OTGHS_EORST | AT91C_OTGHS_SUSP);
            AT91C_BASE_OTGHS->OTGHS_DEVICR = (AT91C_OTGHS_WAKEUP | AT91C_OTGHS_EORSM);
            AT91C_BASE_OTGHS->OTGHS_DEVIDR = AT91C_OTGHS_WAKEUP;

        }

        // End of bus reset
        else if ((status & AT91C_OTGHS_EORST) == AT91C_OTGHS_EORST) {

            TRACE_DEBUG_WP("EoB ");

            // The device enters the Default state
            deviceState = USBD_STATE_DEFAULT;
            //      MCK + UDPCK are already enabled
            //      Pull-Up is already connected
            //      Transceiver must be enabled
            //      Endpoint 0 must be enabled

            OTGHS_EnableTransceiver();
            USBDDriver_clearOTGFeatures();

            // The device leaves the Address & Configured states
            OTGHS_ResetEndpoints();
            OTGHS_DisableEndpoints();
            USBD_ConfigureEndpoint(0);

            // Flush and enable the Suspend interrupt
            AT91C_BASE_OTGHS->OTGHS_DEVICR = AT91C_OTGHS_WAKEUP | AT91C_OTGHS_SUSP;

            //// Enable the Start Of Frame (SOF) interrupt if needed
            //if (pCallbacks->startOfFrame != 0)
            //{
            //    AT91C_BASE_OTGHS->OTGHS_DEVIER |= AT91C_OTGHS_SOF;
            //}

            // Invoke the Reset callback
            USBDCallbacks_Reset();

            // Acknowledge end of bus reset interrupt
            AT91C_BASE_OTGHS->OTGHS_DEVICR = AT91C_OTGHS_EORST;
        }

        // Handle upstream resume interrupt
        else if (status & AT91C_OTGHS_UPRSM) {

            TRACE_DEBUG_WP("ExtRes ");

            // - Acknowledge the IT
            AT91C_BASE_OTGHS->OTGHS_DEVICR = AT91C_OTGHS_UPRSM;
        }

        // Endpoint interrupts
        else {
#ifndef DMA

            // Handle endpoint interrupts
            for (numIT = 0; numIT < NUM_IT_MAX; numIT++) {

                if ((status & (1 << SHIFT_INTERUPT << numIT)) != 0) {

                    OTGHS_EndpointHandler(numIT);
                }
            }
#else
            // Handle endpoint control interrupt
            if ((status & (1 << SHIFT_INTERUPT << 0)) != 0) {

                OTGHS_EndpointHandler( 0 );
            }
            else {

                numIT = 1;
                while((status&(0x7E<<SHIFT_DMA)) != 0) {

                    // Check if endpoint has a pending interrupt
                    if ((status & (1 << SHIFT_DMA << numIT)) != 0) {

                        OTGHS_DmaHandler(numIT);
                        status &= ~(1 << SHIFT_DMA << numIT);
                        if (status != 0) {

                            TRACE_INFO_WP("\n\r  - ");
                        }
                    }
                    numIT++;
                }
            }
#endif
        }

        // Retrieve new interrupt status
        status = AT91C_BASE_OTGHS->OTGHS_DEVISR & AT91C_BASE_OTGHS->OTGHS_DEVIMR;

        TRACE_DEBUG_WP("\n\r");

        if (status != 0) {

            TRACE_DEBUG_WP("  - ");
        }
    }

    if (deviceState >= USBD_STATE_POWERED) {

        LED_Clear(USBD_LEDUSB);
    }
}

//------------------------------------------------------------------------------
/// Configure an endpoint with the provided endpoint descriptor
/// \param pDdescriptor Pointer to the endpoint descriptor
//------------------------------------------------------------------------------
void USBD_ConfigureEndpoint(const USBEndpointDescriptor *pDescriptor)

{
    Endpoint *pEndpoint;
    unsigned char bEndpoint;
    unsigned char bType;
    unsigned char bEndpointDir;
    unsigned char bSizeEpt = 0;

    // NULL descriptor -> Control endpoint 0
    if (pDescriptor == 0) {

        bEndpoint = 0;
        pEndpoint = &(endpoints[bEndpoint]);
        bType = USBEndpointDescriptor_CONTROL;
        bEndpointDir = 0;
        pEndpoint->size = CHIP_USB_ENDPOINTS_MAXPACKETSIZE(0);
        pEndpoint->bank = CHIP_USB_ENDPOINTS_BANKS(0);
    }
    else {

        // The endpoint number
        bEndpoint = USBEndpointDescriptor_GetNumber(pDescriptor);
        pEndpoint = &(endpoints[bEndpoint]);
        // Transfer type: Control, Isochronous, Bulk, Interrupt
        bType = USBEndpointDescriptor_GetType(pDescriptor);
        // Direction, ignored for control endpoints
        bEndpointDir = USBEndpointDescriptor_GetDirection(pDescriptor);
        pEndpoint->size = USBEndpointDescriptor_GetMaxPacketSize(pDescriptor);
        pEndpoint->bank = CHIP_USB_ENDPOINTS_BANKS(bEndpoint);
    }

    // Abort the current transfer is the endpoint was configured and in
    // Write or Read state
    if( (pEndpoint->state == UDP_ENDPOINT_RECEIVING)
     || (pEndpoint->state == UDP_ENDPOINT_SENDING) ) {

        OTGHS_EndOfTransfer(bEndpoint, USBD_STATUS_RESET);
    }
    pEndpoint->state = UDP_ENDPOINT_IDLE;

    // Disable endpoint
    AT91C_BASE_OTGHS->OTGHS_DEVEPTIDR[bEndpoint] = AT91C_OTGHS_TXINI    
                                                 | AT91C_OTGHS_RXOUT    
                                                 | AT91C_OTGHS_RXSTP    
                                                 | AT91C_OTGHS_NAKOUT   
                                                 | AT91C_OTGHS_NAKIN    
                                                 | AT91C_OTGHS_OVERFL   
                                                 | AT91C_OTGHS_STALL    
                                                 | AT91C_OTGHS_SHRTPACK 
                                                 | AT91C_OTGHS_MADATA   
                                                 | AT91C_OTGHS_DATAX    
                                                 | AT91C_OTGHS_ERRTRANS 
                                                 | AT91C_OTGHS_NBUSYBK  
                                                 | AT91C_OTGHS_FIFOCON  
                                                 | AT91C_OTGHS_EPDISHDMA
                                                 | AT91C_OTGHS_NYETDIS  
                                                 | AT91C_OTGHS_STALLRQ;

    // Reset Endpoint Fifos
    AT91C_BASE_OTGHS->OTGHS_DEVEPT |= (1<<bEndpoint<<16);
    AT91C_BASE_OTGHS->OTGHS_DEVEPT &= ~(1<<bEndpoint<<16);

    // Configure endpoint
    if( pEndpoint->size <= 8 )  {
        bSizeEpt = 0;
    } 
    else if ( pEndpoint->size <= 16 ) {
        bSizeEpt = 1;
    }
    else if ( pEndpoint->size <= 32 ) {
        bSizeEpt = 2;
    }
    else if ( pEndpoint->size <= 64 ) {
        bSizeEpt = 3;
    }
    else if ( pEndpoint->size <= 128 ) {
        bSizeEpt = 4;
    }
    else if ( pEndpoint->size <= 256 ) {
        bSizeEpt = 5;
    }
    else if ( pEndpoint->size <= 512 )  {
        bSizeEpt = 6;
    }
    else if ( pEndpoint->size <= 1024 ) {
        bSizeEpt = 7;
    } //else {
    //  sizeEpt = 0; // control endpoint
    //}

    // Enable endpoint
    AT91C_BASE_OTGHS->OTGHS_DEVEPT |= (1<<bEndpoint);

    // Configure endpoint
    if (bType == USBEndpointDescriptor_CONTROL) {

        // Enable endpoint IT for control endpoint
        AT91C_BASE_OTGHS->OTGHS_DEVIER = 1<<SHIFT_INTERUPT<<bEndpoint;
    }

    AT91C_BASE_OTGHS->OTGHS_DEVEPTCFG[bEndpoint] = (bSizeEpt << 4) 
                                                 | (bEndpointDir << 8) 
                                                 | (bType << 11) 
                                                 | (((pEndpoint->bank)-1) << 2);

    if (bType == USBEndpointDescriptor_CONTROL) {

        AT91C_BASE_OTGHS->OTGHS_DEVEPTIER[bEndpoint] = AT91C_OTGHS_RXSTP;
    }
#ifdef DMA
    else {
        AT91C_BASE_OTGHS->OTGHS_DEVEPTCFG[bEndpoint] |= AT91C_OTGHS_AUTOSW;
    }
#endif

    AT91C_BASE_OTGHS->OTGHS_DEVEPTIDR[bEndpoint] = AT91C_OTGHS_NYETDIS;// with nyet
    //AT91C_BASE_OTGHS->OTGHS_DEVEPTIER[bEndpoint] = AT91C_OTGHS_NYETDIS;  // without nyet

    // Check if the configuration is ok
    AT91C_BASE_OTGHS->OTGHS_DEVEPTCFG[bEndpoint] |= AT91C_OTGHS_ALLOC;
    if((AT91C_BASE_OTGHS->OTGHS_DEVEPTISR[bEndpoint]&AT91C_OTGHS_CFGOK)==0) {

        TRACE_ERROR("PB bEndpoint: 0x%X\n\r", bEndpoint);
        TRACE_ERROR("PB bSizeEpt: 0x%X\n\r", bSizeEpt);
        TRACE_ERROR("PB bEndpointDir: 0x%X\n\r", bEndpointDir);
        TRACE_ERROR("PB bType: 0x%X\n\r", bType);
        TRACE_ERROR("PB pEndpoint->bank: 0x%X\n\r", pEndpoint->bank);
        TRACE_ERROR("PB OTGHS_EPTCFG: 0x%X\n\r", AT91C_BASE_OTGHS->OTGHS_DEVEPTCFG[bEndpoint]);
        for(;;);
    }
}

//------------------------------------------------------------------------------
/// Sends data through an USB endpoint (IN)
/// Sets up the transfer descriptor, write one or two data payloads
/// (depending on the number of FIFO banks for the endpoint) and then
/// starts the actual transfer. The operation is complete when all
/// the data has been sent.
/// \param bEndpoint Index of endpoint
/// \param *pData  Data to be written
/// \param dLength Data length to be send
/// \param fCallback  Callback to be call after the success command
/// \param *pArgument Callback argument
/// \return USBD_STATUS_LOCKED or USBD_STATUS_SUCCESS
//------------------------------------------------------------------------------
char USBD_Write( unsigned char    bEndpoint,
                 const void       *pData,
                 unsigned int     dLength,
                 TransferCallback fCallback,
                 void             *pArgument )
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);
    Transfer *pTransfer = &(pEndpoint->transfer);
//unsigned char i;
//unsigned char * data;

    // Return if the endpoint is not in IDLE state
    if (pEndpoint->state != UDP_ENDPOINT_IDLE)  {

        return USBD_STATUS_LOCKED;
    }

    TRACE_DEBUG_WP("Write%d(%d) ", bEndpoint, dLength);
    pEndpoint->sendZLP = 0;
    // Setup the transfer descriptor
    pTransfer->pData = (void *) pData;
    pTransfer->remaining = dLength;
    pTransfer->buffered = 0;
    pTransfer->transferred = 0;
    pTransfer->fCallback = fCallback;
    pTransfer->pArgument = pArgument;

    // Send one packet
    pEndpoint->state = UDP_ENDPOINT_SENDING;

#ifdef DMA
    // Test if endpoint type control
    if (AT91C_OTGHS_EPT_TYPE_CTL_EPT == (AT91C_OTGHS_EPT_TYPE & AT91C_BASE_OTGHS->OTGHS_DEVEPTCFG[bEndpoint])) {
#endif
        // Enable endpoint IT
        AT91C_BASE_OTGHS->OTGHS_DEVIER = (1<<SHIFT_INTERUPT<<bEndpoint);
        AT91C_BASE_OTGHS->OTGHS_DEVEPTIER[bEndpoint] = AT91C_OTGHS_TXINI;

#ifdef DMA
    }
    else {
        if( CHIP_USB_ENDPOINTS_DMA(bEndpoint) == 0 ) {
            TRACE_FATAL("Endpoint has no DMA\n\r");
        }
        if( pTransfer->remaining == 0 ) {

            // DMA not handle ZLP
            AT91C_BASE_OTGHS->OTGHS_DEVEPTICR[bEndpoint] = AT91C_OTGHS_TXINI;
            // For a non-control endpoint, the FIFOCON bit must be cleared
            // to start the transfer
            if ((AT91C_OTGHS_EPT_TYPE & AT91C_BASE_OTGHS->OTGHS_DEVEPTCFG[bEndpoint])
                != AT91C_OTGHS_EPT_TYPE_CTL_EPT) {

                AT91C_BASE_OTGHS->OTGHS_DEVEPTIDR[bEndpoint] = AT91C_OTGHS_FIFOCON;
            }            
            AT91C_BASE_OTGHS->OTGHS_DEVEPTIDR[bEndpoint] = AT91C_OTGHS_TXINI;

            // Enable endpoint IT
            AT91C_BASE_OTGHS->OTGHS_DEVIER = (1<<SHIFT_INTERUPT<<bEndpoint);
        }
        else {
            // Others endpoints (not control)
            if( pTransfer->remaining > DMA_MAX_FIFO_SIZE ) {

                // Transfer the max
                pTransfer->buffered = DMA_MAX_FIFO_SIZE;    
            }
            else {
                // Transfer the good size
                pTransfer->buffered = pTransfer->remaining;
            }

            TRACE_DEBUG_WP("\n\r_WR:%d ", pTransfer->remaining );
            TRACE_DEBUG_WP("B:%d ", pTransfer->buffered );
            TRACE_DEBUG_WP("T:%d ", pTransfer->transferred );

            AT91C_BASE_OTGHS->OTGHS_DEVDMA[bEndpoint].OTGHS_DEVDMAADDRESS = (unsigned int)(pTransfer->pData);

            // Clear unwanted interrupts
            AT91C_BASE_OTGHS->OTGHS_DEVDMA[bEndpoint].OTGHS_DEVDMASTATUS;

            // Enable DMA endpoint interrupt
            AT91C_BASE_OTGHS->OTGHS_DEVIER = (1<<SHIFT_DMA<<bEndpoint);
            // DMA config
            AT91C_BASE_OTGHS->OTGHS_DEVDMA[bEndpoint].OTGHS_DEVDMACONTROL = 0; // raz
            AT91C_BASE_OTGHS->OTGHS_DEVDMA[bEndpoint].OTGHS_DEVDMACONTROL = 
                 (((pTransfer->buffered<<16)&AT91C_OTGHS_BUFF_LENGTH)
                   | AT91C_OTGHS_END_B_EN
                   | AT91C_OTGHS_END_BUFFIT
                   | AT91C_OTGHS_CHANN_ENB);
        }
    }
#endif

    return USBD_STATUS_SUCCESS;
}

//------------------------------------------------------------------------------
/// Reads incoming data on an USB endpoint (OUT)
/// \param bEndpoint Index of endpoint
/// \param *pData  Data to be readen
/// \param dLength Data length to be receive
/// \param fCallback  Callback to be call after the success command
/// \param *pArgument Callback argument
/// \return USBD_STATUS_LOCKED or USBD_STATUS_SUCCESS
//------------------------------------------------------------------------------
char USBD_Read( unsigned char    bEndpoint,
                void             *pData,
                unsigned int     dLength,
                TransferCallback fCallback,
                void             *pArgument )
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);
    Transfer *pTransfer = &(pEndpoint->transfer);
  
    // Return if the endpoint is not in IDLE state
    if (pEndpoint->state != UDP_ENDPOINT_IDLE) {

        return USBD_STATUS_LOCKED;
    }

    TRACE_DEBUG_WP("Read%d(%d) ", bEndpoint, dLength);
    //TRACE_ERROR_WP("Read%d(%d) ", bEndpoint, dLength);

    // Endpoint enters Receiving state
    pEndpoint->state = UDP_ENDPOINT_RECEIVING;

    // Set the transfer descriptor
    pTransfer->pData = pData;
    pTransfer->remaining = dLength;
    pTransfer->buffered = 0;
    pTransfer->transferred = 0;
    pTransfer->fCallback = fCallback;
    pTransfer->pArgument = pArgument;

#ifdef DMA
    // Test if endpoint type control
    if (AT91C_OTGHS_EPT_TYPE_CTL_EPT == (AT91C_OTGHS_EPT_TYPE & AT91C_BASE_OTGHS->OTGHS_DEVEPTCFG[bEndpoint])) {
#endif
        // Control endpoint
        // Enable endpoint IT
        AT91C_BASE_OTGHS->OTGHS_DEVIER = (1<<SHIFT_INTERUPT<<bEndpoint);
        AT91C_BASE_OTGHS->OTGHS_DEVEPTIER[bEndpoint] = AT91C_OTGHS_RXOUT;
#ifdef DMA
    }
    else {
        if( CHIP_USB_ENDPOINTS_DMA(bEndpoint) == 0 ) {
            TRACE_FATAL("Endpoint has no DMA\n\r");
        }
        TRACE_DEBUG_WP("Read%d(%d) ", bEndpoint, dLength);

        // Others endpoints (not control)
        if( pTransfer->remaining > DMA_MAX_FIFO_SIZE ) {

            // Transfer the max
            pTransfer->buffered = DMA_MAX_FIFO_SIZE;    
        }
        else {
            // Transfer the good size
            pTransfer->buffered = pTransfer->remaining;
        }

        AT91C_BASE_OTGHS->OTGHS_DEVDMA[bEndpoint].OTGHS_DEVDMAADDRESS = (unsigned int)(pTransfer->pData);

        // Clear unwanted interrupts
        AT91C_BASE_OTGHS->OTGHS_DEVDMA[bEndpoint].OTGHS_DEVDMASTATUS;

        // Enable DMA endpoint interrupt
        AT91C_BASE_OTGHS->OTGHS_DEVIER = (1<<SHIFT_DMA<<bEndpoint);

        TRACE_DEBUG_WP("\n\r_RR:%d ", pTransfer->remaining );
        TRACE_DEBUG_WP("B:%d ", pTransfer->buffered );
        TRACE_DEBUG_WP("T:%d ", pTransfer->transferred );

        // DMA config
        AT91C_BASE_OTGHS->OTGHS_DEVDMA[bEndpoint].OTGHS_DEVDMACONTROL = 0; // raz
        AT91C_BASE_OTGHS->OTGHS_DEVDMA[bEndpoint].OTGHS_DEVDMACONTROL =
                             (((pTransfer->buffered<<16)&AT91C_OTGHS_BUFF_LENGTH)
                               | AT91C_OTGHS_END_TR_EN
                               | AT91C_OTGHS_END_TR_IT
                               | AT91C_OTGHS_END_B_EN
                               | AT91C_OTGHS_END_BUFFIT
                               | AT91C_OTGHS_CHANN_ENB);
    }
#endif

  return USBD_STATUS_SUCCESS;
}

//------------------------------------------------------------------------------
/// Put endpoint into Halt state
/// \param bEndpoint Index of endpoint
//------------------------------------------------------------------------------
void USBD_Halt( unsigned char bEndpoint )
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);

    TRACE_INFO("usbd_Halt%d ", bEndpoint);
    //TRACE_ERROR("usbd_Halt%d ", bEndpoint);

    // Check that endpoint is enabled and not already in Halt state
    if( (pEndpoint->state != UDP_ENDPOINT_DISABLED)
     && (pEndpoint->state != UDP_ENDPOINT_HALTED) ) {

        TRACE_INFO("Halt%d ", bEndpoint);

        // Abort the current transfer if necessary
        OTGHS_EndOfTransfer(bEndpoint, USBD_STATUS_ABORTED);

        pEndpoint->state = UDP_ENDPOINT_HALTED;
        // Put endpoint into Halt state
        AT91C_BASE_OTGHS->OTGHS_DEVEPTIER[bEndpoint] = AT91C_OTGHS_STALLRQ;
        AT91C_BASE_OTGHS->OTGHS_DEVEPTIER[bEndpoint] = AT91C_OTGHS_STALL;
   }
}

//------------------------------------------------------------------------------
/// Clears the Halt feature on the given endpoint.
/// \param bEndpoint Index of endpoint
//------------------------------------------------------------------------------
void USBD_Unhalt( unsigned char bEndpoint )
{
    unsigned int cfgSav;

    Endpoint *pEndpoint = &(endpoints[bEndpoint]);

    // Check if the endpoint is enabled
    if (pEndpoint->state != UDP_ENDPOINT_DISABLED) {

        TRACE_DEBUG_WP("Unhalt%d ", bEndpoint);
        //TRACE_ERROR("Unhalt%d ", bEndpoint);

        // Return endpoint to Idle state
        pEndpoint->state = UDP_ENDPOINT_IDLE;

        cfgSav = AT91C_BASE_OTGHS->OTGHS_DEVEPTCFG[bEndpoint];
 
        // Reset Endpoint Fifos
        AT91C_BASE_OTGHS->OTGHS_DEVEPT |= (1<<bEndpoint<<16);
        AT91C_BASE_OTGHS->OTGHS_DEVEPT &= ~(1<<bEndpoint<<16);

        AT91C_BASE_OTGHS->OTGHS_DEVEPTCFG[bEndpoint] = cfgSav;

        if((AT91C_BASE_OTGHS->OTGHS_DEVEPTISR[bEndpoint]&AT91C_OTGHS_CFGOK)==0) {

            TRACE_ERROR("PB bEndpoint: 0x%X\n\r", bEndpoint);
            for(;;);
        }

        // Reset data-toggle
        AT91C_BASE_OTGHS->OTGHS_DEVEPTIER[bEndpoint] = AT91C_OTGHS_RSTDT;

        // Clear FORCESTALL flag
        // Disable stall on endpoint
        AT91C_BASE_OTGHS->OTGHS_DEVEPTIDR[bEndpoint] = AT91C_OTGHS_STALLRQ;
        AT91C_BASE_OTGHS->OTGHS_DEVEPTICR[bEndpoint] = AT91C_OTGHS_STALL;
    }
}

//------------------------------------------------------------------------------
/// Returns the current Halt status of an endpoint.
/// \param bEndpoint Index of endpoint
/// \return 1 if the endpoint is currently halted; otherwise 0
//------------------------------------------------------------------------------
unsigned char USBD_IsHalted( unsigned char bEndpoint )
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);
    unsigned char status = 0;

    if (pEndpoint->state == UDP_ENDPOINT_HALTED) {
        status = 1;
    }
    return( status );
}

//------------------------------------------------------------------------------
/// IS High Speed device working in High Speed ?
/// \return 1 if the device is in High Speed; otherwise 0 (Full Speed)
//------------------------------------------------------------------------------
unsigned char USBD_IsHighSpeed( void )
{
    unsigned char status = 0;

    if(AT91C_OTGHS_SPEED_SR_HS == (AT91C_BASE_OTGHS->OTGHS_SR & (0x03<<12))) {
        // High Speed
        TRACE_DEBUG_WP("High Speed\n\r");
        status = 1;
    }
    else {
        TRACE_DEBUG_WP("Full Speed\n\r");
    }
    return( status );
}

//------------------------------------------------------------------------------
/// Causes the endpoint to acknowledge the next received packet with a STALL
/// handshake.
/// Further packets are then handled normally.
/// \param bEndpoint Index of endpoint
/// \return Operation result code: USBD_STATUS_LOCKED or USBD_STATUS_SUCCESS
//------------------------------------------------------------------------------
unsigned char USBD_Stall( unsigned char bEndpoint )
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);

    // Check that endpoint is in Idle state
    if (pEndpoint->state != UDP_ENDPOINT_IDLE) {

        TRACE_WARNING("UDP_Stall: Endpoint%d locked\n\r", bEndpoint);
        return USBD_STATUS_LOCKED;
    }

    TRACE_DEBUG_WP("Stall%d ", bEndpoint);

    // Sends a STALL handshake for the next host request. 
    // A STALL handshake will be sent for each following request until a SETUP 
    // or a Clear Halt Feature occurs for this endpoint.
    AT91C_BASE_OTGHS->OTGHS_DEVEPTIER[bEndpoint] = AT91C_OTGHS_STALLRQ|AT91C_OTGHS_RXSTP;

    return USBD_STATUS_SUCCESS;
}

//------------------------------------------------------------------------------
/// Activates a remote wakeup procedure
//------------------------------------------------------------------------------
void USBD_RemoteWakeUp(void)
{
    TRACE_DEBUG_WP("Remote WakeUp\n\r");

    // Device is currently suspended
    if (deviceState == USBD_STATE_SUSPENDED) {

        TRACE_DEBUG_WP("RW\n\r");
        OTGHS_EnableUsbClock();
        OTGHS_EnableTransceiver();

        // Activates a remote wakeup
        AT91C_BASE_OTGHS->OTGHS_DEVCTRL |= AT91C_OTGHS_RMWKUP;
    }
    // Device is NOT suspended
    else {

        TRACE_WARNING("USBD_RemoteWakeUp: Device is not suspended\n\r");
    }
}

//------------------------------------------------------------------------------
/// Sets the device address
/// \param address Adress to be set
//------------------------------------------------------------------------------
void USBD_SetAddress( unsigned char address )
{
    TRACE_DEBUG_WP("SetAddr(%d) ", address);

    // Set address
    AT91C_BASE_OTGHS->OTGHS_DEVCTRL &= ~(unsigned int)AT91C_OTGHS_UADD;
    AT91C_BASE_OTGHS->OTGHS_DEVCTRL |= address & AT91C_OTGHS_UADD;
    AT91C_BASE_OTGHS->OTGHS_DEVCTRL |= AT91C_OTGHS_ADDEN;

    // If the address is 0, the device returns to the Default state
    if (address == 0) {
        deviceState = USBD_STATE_DEFAULT;
    }
    // If the address is non-zero, the device enters the Address state
    else {
        deviceState = USBD_STATE_ADDRESS;
    }
}

//------------------------------------------------------------------------------
/// Changes the device state from Address to Configured, or from Configured 
/// to Address.
/// This method directly access the last received SETUP packet to decide on 
/// what to do.
/// \param cfgnum configuration number
//------------------------------------------------------------------------------
void USBD_SetConfiguration( unsigned char cfgnum )
{
    TRACE_DEBUG_WP("SetCfg(%d) ", cfgnum);

    // Check the request
    if( cfgnum != 0 ) {

        // Enter Configured state
        deviceState = USBD_STATE_CONFIGURED;
    }
    // If the configuration number is zero, the device goes back to the Address
    // state
    else  {

        // Go back to Address state
        deviceState = USBD_STATE_ADDRESS;

        // Abort all transfers
        OTGHS_DisableEndpoints();
    }
}


//------------------------------------------------------------------------------
/// Enables the pull-up on the D+ line to connect the device to the USB.
//------------------------------------------------------------------------------
void USBD_Connect( void )
{
    TRACE_DEBUG_WP("Conn ");
#if defined(CHIP_USB_PULLUP_INTERNAL)
    AT91C_BASE_OTGHS->OTGHS_DEVCTRL &= ~(unsigned int)AT91C_OTGHS_DETACH;
#else
    #error "not defined"
#endif
}

//------------------------------------------------------------------------------
/// Disables the pull-up on the D+ line to disconnect the device from the bus.
//------------------------------------------------------------------------------
void USBD_Disconnect( void )
{
    TRACE_DEBUG_WP("Disc ");
#if defined(CHIP_USB_PULLUP_INTERNAL)
    AT91C_BASE_OTGHS->OTGHS_DEVCTRL |= AT91C_OTGHS_DETACH;

#else
    #error "not defined"
#endif
    // Device returns to the Powered state
    if (deviceState > USBD_STATE_POWERED) {    

        deviceState = USBD_STATE_POWERED;
    }
}

//------------------------------------------------------------------------------
/// Certification test for High Speed device.
/// \param bIndex Test to be done
//------------------------------------------------------------------------------
void USBD_Test( unsigned char bIndex )
{
    char          *pFifo;
    unsigned char i;

    AT91C_BASE_OTGHS->OTGHS_DEVIDR &= ~(unsigned int)AT91C_OTGHS_SUSP;
    AT91C_BASE_OTGHS->OTGHS_DEVCTRL |= AT91C_OTGHS_SPDCONF_HS; // remove suspend ?

    switch( bIndex ) {
        case USBFeatureRequest_TESTPACKET:
            TRACE_DEBUG_WP("TEST_PACKET ");

            AT91C_BASE_OTGHS->OTGHS_DEVDMA[1].OTGHS_DEVDMACONTROL = 0; // raz
            AT91C_BASE_OTGHS->OTGHS_DEVDMA[2].OTGHS_DEVDMACONTROL = 0; // raz

            // Configure endpoint 2, 64 bytes, direction IN, type BULK, 1 bank
            AT91C_BASE_OTGHS->OTGHS_DEVEPTCFG[2] = AT91C_OTGHS_EPT_SIZE_64
                                                         | AT91C_OTGHS_EPT_DIR_IN  
                                                         | AT91C_OTGHS_EPT_TYPE_BUL_EPT 
                                                         | AT91C_OTGHS_BK_NUMBER_1;
            // Check if the configuration is ok
            AT91C_BASE_OTGHS->OTGHS_DEVEPTCFG[2] |= AT91C_OTGHS_ALLOC;
            while((AT91C_BASE_OTGHS->OTGHS_DEVEPTISR[2]&AT91C_OTGHS_CFGOK)==0) {
            }

            AT91C_BASE_OTGHS->OTGHS_DEVEPT |= AT91C_OTGHS_EPEN2;

            // Write FIFO
            pFifo = (char*)((unsigned int *)AT91C_BASE_OTGHS_EPTFIFO + (EPT_VIRTUAL_SIZE * 2));
            for( i=0; i<sizeof(test_packet_buffer); i++) {
                pFifo[i] = test_packet_buffer[i];
            }
            // Tst PACKET
            AT91C_BASE_OTGHS->OTGHS_DEVCTRL |= AT91C_OTGHS_TSTPCKT;
            // Send packet
            AT91C_BASE_OTGHS->OTGHS_DEVEPTICR[2] = AT91C_OTGHS_TXINI;
            AT91C_BASE_OTGHS->OTGHS_DEVEPTIDR[2] = AT91C_OTGHS_FIFOCON;
            break;

        case USBFeatureRequest_TESTJ:
            TRACE_DEBUG_WP("TEST_J ");
            AT91C_BASE_OTGHS->OTGHS_DEVCTRL |= AT91C_OTGHS_TSTJ;
            break;

        case USBFeatureRequest_TESTK:
            TRACE_DEBUG_WP("TEST_K ");
            AT91C_BASE_OTGHS->OTGHS_DEVCTRL |= AT91C_OTGHS_TSTK;
            break;

        case USBFeatureRequest_TESTSE0NAK:
            TRACE_DEBUG_WP("TEST_SEO_NAK ");
            AT91C_BASE_OTGHS->OTGHS_DEVIDR = 0xFFFFFFFF;
            break;

        case USBFeatureRequest_TESTSENDZLP:
            //while( 0 != (AT91C_BASE_UDPHS->UDPHS_EPT[0].UDPHS_EPTSTA & AT91C_UDPHS_TX_PK_RDY ) ) {}
            AT91C_BASE_OTGHS->OTGHS_DEVEPTICR[0] = AT91C_OTGHS_TXINI;
            //while( 0 != (AT91C_BASE_UDPHS->UDPHS_EPT[0].UDPHS_EPTSTA & AT91C_UDPHS_TX_PK_RDY ) ) {}
            TRACE_DEBUG_WP("SEND_ZLP ");
            break;
    }
    TRACE_DEBUG_WP("\n\r");
}

//------------------------------------------------------------------------------
/// Initializes the specified USB driver
/// This function initializes the current FIFO bank of endpoints,
/// configures the pull-up and VBus lines, disconnects the pull-up and
/// then trigger the Init callback.
//------------------------------------------------------------------------------
void USBD_Init(void)
{
    // forceFS must not be used !
    unsigned int i;

    TRACE_DEBUG_WP("USBD Init()\n\r");

        // Enable USB macro
    *AT91C_OTGHS_CTRL |= AT91C_OTGHS_USBECTRL;

    // Automatic mode speed for device
    *AT91C_OTGHS_DEVCTRL &= ~(unsigned int)AT91C_OTGHS_SPDCONF_FS;  // Normal mode

    *AT91C_OTGHS_DEVCTRL &= ~(unsigned int)(  AT91C_OTGHS_LS   | AT91C_OTGHS_TSTJ
                                            | AT91C_OTGHS_TSTK | AT91C_OTGHS_TSTPCKT
                                            | AT91C_OTGHS_OPMODE2 ); // Normal mode

    AT91C_BASE_OTGHS->OTGHS_DEVCTRL = 0;
    AT91C_BASE_OTGHS->OTGHS_HSTCTRL = 0;

    // Enable OTG pad
    *AT91C_OTGHS_CTRL |= AT91C_OTGHS_OTGPADE;

    // Enable clock OTG pad
    *AT91C_OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_FRZCLKCTRL;

    //Usb_disable();
    AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_USBECTRL;
    AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_OTGPADE;
    AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_FRZCLKCTRL;
    //Usb_enable();
    AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_USBECTRL;
    AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_OTGPADE;
    AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_FRZCLKCTRL;
    //Usb_select_device();
    AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_UIDE;
    AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_UIMOD;

    // Device is in the Attached state
    deviceState = USBD_STATE_SUSPENDED;
    previousDeviceState = USBD_STATE_POWERED;

    
    PMC_EnablePeripheral(AT91C_ID_OTGHS);
     
    // Reset endpoint structures
    OTGHS_ResetEndpoints();

    // Enables the USB Clock
    OTGHS_EnableUsbClock();

    //926C
    // Enable USB macro and clear all other bit
    AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_USBECTRL;
    AT91C_BASE_OTGHS->OTGHS_CTRL = AT91C_OTGHS_USBECTRL;
 
    // Configure the pull-up on D+ and disconnect it
    USBD_Disconnect();

    // Enable clock OTG pad
    AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_FRZCLKCTRL;
   TRACE_DEBUG("AT91C_OTGHS_CTRL: 0x%X\n\r", AT91C_BASE_OTGHS->OTGHS_CTRL );

    // Clear General IT
    AT91C_BASE_OTGHS->OTGHS_SCR = 0x01FF;

    // Clear OTG Device IT
    AT91C_BASE_OTGHS->OTGHS_DEVICR = 0xFF;

    // Clear OTG Host IT
    AT91C_BASE_OTGHS->OTGHS_HSTICR = 0x7F;

    // Reset all Endpoints Fifos
    AT91C_BASE_OTGHS->OTGHS_DEVEPT |= (0x7F<<16);
    AT91C_BASE_OTGHS->OTGHS_DEVEPT &= ~(unsigned int)(0x7F<<16);

    // Disable all endpoints
    AT91C_BASE_OTGHS->OTGHS_DEVEPT &= ~(unsigned int)0x7F;

    AT91C_BASE_OTGHS->OTGHS_TSTA2 = 0;

    // Device is in the Attached state
    deviceState = USBD_STATE_SUSPENDED;
    previousDeviceState = USBD_STATE_POWERED;

    // Automatic mode speed for device
    AT91C_BASE_OTGHS->OTGHS_DEVCTRL &= ~(unsigned int)AT91C_OTGHS_SPDCONF_FS;
    // Force Full Speed mode for device
    //*AT91C_OTGHS_DEVCTRL = AT91C_OTGHS_SPDCONF_FS;
    // Force High Speed mode for device
    //*AT91C_OTGHS_DEVCTRL = AT91C_OTGHS_SPDCONF_HS;

    AT91C_BASE_OTGHS->OTGHS_DEVCTRL &= ~(unsigned int)(  AT91C_OTGHS_LS 
                                                       | AT91C_OTGHS_TSTJ
                                                       | AT91C_OTGHS_TSTK
                                                       | AT91C_OTGHS_TSTPCKT
                                                       | AT91C_OTGHS_OPMODE2 );


    // Automatic mode speed for host
    AT91C_BASE_OTGHS->OTGHS_HSTCTRL &= ~(unsigned int)AT91C_OTGHS_SPDCONF_HST_FS;
    // Force Full Speed mode for host
    //AT91C_BASE_OTGHS->OTGHS_HSTCTRL = AT91C_OTGHS_SPDCONF_HST_FS;
    // Force High Speed mode for host
    //*AT91C_BASE_OTGHS->OTGHS_HSTCTRL = AT91C_OTGHS_SPDCONF_HST_HS;

     // Enable USB macro
    AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_USBECTRL;

    // Enable the UID pin select
    AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_UIDE;

    // Enable OTG pad
    AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_OTGPADE;

    // Enable clock OTG pad
    AT91C_BASE_OTGHS->OTGHS_CTRL &= ~(unsigned int)AT91C_OTGHS_FRZCLKCTRL;


    // With OR without DMA !!!
    // Initialization of DMA
    for( i=1; i<=(unsigned int)((AT91C_BASE_OTGHS->OTGHS_IPFEATURES & AT91C_OTGHS_DMA_CHANNEL_NBR)>>4); i++ ) {

        // RESET endpoint canal DMA:
        // DMA stop channel command
        AT91C_BASE_OTGHS->OTGHS_DEVDMA[i].OTGHS_DEVDMACONTROL = 0;  // STOP command

        // Disable endpoint
        AT91C_BASE_OTGHS->OTGHS_DEVEPTIDR[i] = 0XFFFFFFFF;

        // Reset endpoint config
        AT91C_BASE_OTGHS->OTGHS_DEVEPTCFG[i] = 0;

        // Reset DMA channel (Buff count and Control field)
        AT91C_BASE_OTGHS->OTGHS_DEVDMA[i].OTGHS_DEVDMACONTROL = 0x02;  // NON STOP command

        // Reset DMA channel 0 (STOP)
        AT91C_BASE_OTGHS->OTGHS_DEVDMA[i].OTGHS_DEVDMACONTROL = 0;  // STOP command

        // Clear DMA channel status (read the register for clear it)
        AT91C_BASE_OTGHS->OTGHS_DEVDMA[i].OTGHS_DEVDMASTATUS = AT91C_BASE_OTGHS->OTGHS_DEVDMA[i].OTGHS_DEVDMASTATUS;

    }

    
    // Configure interrupts
    USBDCallbacks_Initialized();

    AT91C_BASE_OTGHS->OTGHS_CTRL |= AT91C_OTGHS_VBUSTI;

   TRACE_DEBUG("AT91C_OTGHS_CTRL: 0x%X\n\r", AT91C_BASE_OTGHS->OTGHS_CTRL );
   TRACE_DEBUG("AT91C_OTGHS_SR: 0x%X\n\r", AT91C_BASE_OTGHS->OTGHS_SR );
  
    AT91C_BASE_OTGHS->OTGHS_DEVIER = AT91C_OTGHS_WAKEUP;
    
   TRACE_DEBUG("NUM_IT_MAX_DMA: 0x%X\n\r", NUM_IT_MAX_DMA );
   TRACE_DEBUG("NUM_IT_MAX: 0x%X\n\r", NUM_IT_MAX );

}


//------------------------------------------------------------------------------
/// Returns the current state of the USB device.
/// \return Device current state.
//------------------------------------------------------------------------------
unsigned char USBD_GetState( void )
{
    return deviceState;
}

