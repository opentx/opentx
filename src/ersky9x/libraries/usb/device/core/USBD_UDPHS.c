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

#include "USBD.h"
#include "USBDCallbacks.h"
#include "USBDDriver.h"
#include <board.h>
#include <pio/pio.h>
#include <utility/trace.h>
#include <utility/led.h>
#include <usb/common/core/USBEndpointDescriptor.h>
#include <usb/common/core/USBGenericRequest.h>
#include <usb/common/core/USBFeatureRequest.h>

#include <stdio.h>

//------------------------------------------------------------------------------
//         Definitions
//------------------------------------------------------------------------------

/// Maximum number of endpoints interrupts.
#define NUM_IT_MAX       \
    (AT91C_BASE_UDPHS->UDPHS_IPFEATURES & AT91C_UDPHS_EPT_NBR_MAX)
/// Maximum number of endpoint DMA interrupts
#define NUM_IT_MAX_DMA   \
    ((AT91C_BASE_UDPHS->UDPHS_IPFEATURES & AT91C_UDPHS_DMA_CHANNEL_NBR)>>4)
/// Bits that should be shifted to access DMA control bits.
#define SHIFT_DMA        24
/// Bits that should be shifted to access interrupt bits.
#define SHIFT_INTERUPT    8

/// Max size of the FMA FIFO
#define DMA_MAX_FIFO_SIZE     65536

#define EPT_VIRTUAL_SIZE      16384

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
/// Endpoint states: Endpoint is sending MBL
#define UDP_ENDPOINT_SENDINGM       5
/// Endpoint states: Endpoint is receiving MBL
#define UDP_ENDPOINT_RECEIVINGM     6
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//      Macros
//------------------------------------------------------------------------------

/// Check if DMA is used to transfer data
#define UDP_IsDmaUsed(ep)   (CHIP_USB_ENDPOINTS_DMA(ep))

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

/// Describes Multi Buffer List transfer on a UDP endpoint.
typedef struct {
    /// Pointer to frame list
    USBDTransferBuffer *pMbl;
    /// Pointer to last loaded buffer
    USBDTransferBuffer *pLastLoaded;
    /// List size
    unsigned short      listSize;
    /// Current processing frame
    unsigned short      currBuffer;
    /// First freed frame to re-use
    unsigned short      freedBuffer;
    /// Frame setting, circle frame list
    unsigned char       circList;
    /// All buffer listed is used
    unsigned char       allUsed;
    /// Optional callback to invoke when the transfer completes.
    MblTransferCallback fCallback;
    /// Optional argument to the callback function.
    void                *pArgument;
} MblTransfer;

/// Describes DMA Link List transfer on a UDPHS endpoint.
typedef struct {
    /// Pointer to frame list
    USBDDmaDescriptor *pMbl;
    /// Pointer to current buffer
    USBDDmaDescriptor *pCurrBuffer;
    /// Pointer to freed buffer
    USBDDmaDescriptor *pFreedBuffer;
    /// List size
    unsigned short      listSize;
    /// Frame setting, circle frame list
    unsigned char       circList;
    /// All buffer listed is used
    unsigned char       allUsed;
    /// Optional callback to invoke when the transfer completes.
    MblTransferCallback fCallback;
    /// Optional argument to the callback function.
    void                *pArgument;
} DmaLlTransfer;

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
    union {
        Transfer       singleTransfer;
        MblTransfer    mblTransfer;
        DmaLlTransfer  dmaTransfer;
    } transfer;
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

// Force HS
static const unsigned char forceUsbFS = 0;

//------------------------------------------------------------------------------
//      Internal Functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Disables the BIAS of the USB controller
//------------------------------------------------------------------------------
static inline void UDPHS_DisableBIAS( void )
{
    AT91C_BASE_PMC->PMC_UCKR &= ~AT91C_CKGR_BIASEN_ENABLED;
}

//------------------------------------------------------------------------------
/// Enables the BIAS of the USB controller
//------------------------------------------------------------------------------
static inline void UDPHS_EnableBIAS( void )
{
    UDPHS_DisableBIAS();
    AT91C_BASE_PMC->PMC_UCKR |= AT91C_CKGR_BIASEN_ENABLED;
}

//------------------------------------------------------------------------------
/// Enable UDPHS clock
//------------------------------------------------------------------------------
static inline void UDPHS_EnableUsbClock( void )
{
#if !defined (PMC_BY_HARD)
    AT91C_BASE_PMC->PMC_PCER = (1 << AT91C_ID_UDPHS);
    // Enable 480MHZ
    //AT91C_BASE_CKGR->CKGR_UCKR |= (AT91C_CKGR_PLLCOUNT & (3 << 20)) | AT91C_CKGR_UPLLEN;
    AT91C_BASE_CKGR->CKGR_UCKR |= ((0xf << 20) & (3 << 20)) | AT91C_CKGR_UPLLEN;
    // Wait until UTMI PLL is locked
    while ((AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCKU) == 0);
#endif
}

//------------------------------------------------------------------------------
/// Disable UDPHS clock
//------------------------------------------------------------------------------
static inline void UDPHS_DisableUsbClock( void )
{
#if !defined (PMC_BY_HARD)
    AT91C_BASE_PMC->PMC_PCDR = (1 << AT91C_ID_UDPHS);
    // 480MHZ
    AT91C_BASE_CKGR->CKGR_UCKR &= ~AT91C_CKGR_UPLLEN;
#endif
}

//------------------------------------------------------------------------------
/// Handles a completed transfer on the given endpoint, invoking the
/// configured callback if any.
/// \param bEndpoint Number of the endpoint for which the transfer has completed.
/// \param bStatus   Status code returned by the transfer operation
//------------------------------------------------------------------------------
static void UDPHS_EndOfTransfer( unsigned char bEndpoint, char bStatus )
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);
    Transfer *pTransfer = (Transfer*)&(pEndpoint->transfer);

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
    else if ( (pEndpoint->state == UDP_ENDPOINT_RECEIVINGM)
            || (pEndpoint->state == UDP_ENDPOINT_SENDINGM) ) {

        MblTransfer*        pMblt = (MblTransfer*)&(pEndpoint->transfer);
        MblTransferCallback fCallback;
        void*               pArg;

        TRACE_DEBUG_WP("EoMT ");

      #ifdef DMA
      { DmaLlTransfer *pDmat = (DmaLlTransfer*)&(pEndpoint->transfer);
        fCallback = UDP_IsDmaUsed(bEndpoint) ?
                        pDmat->fCallback : pMblt->fCallback;
        pArg = UDP_IsDmaUsed(bEndpoint) ?
                        pDmat->pArgument : pMblt->pArgument;
      }
      #else
        fCallback = pMblt->fCallback;
        pArg      = pMblt->pArgument;
      #endif

        // Endpoint returns in Idle state
        pEndpoint->state = UDP_ENDPOINT_IDLE;
        // Invoke callback
        if (fCallback != 0) {

            (fCallback)(pArg, bStatus, 0);
        }
        else {
            TRACE_DEBUG_WP("NoCB ");
        }
    }
}

//------------------------------------------------------------------------------
/// Clears the correct RX flag in endpoint status register
/// \param bEndpoint Index of endpoint
//------------------------------------------------------------------------------
static void UDPHS_ClearRxFlag( unsigned char bEndpoint )
{
    AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCLRSTA = AT91C_UDPHS_RX_BK_RDY;
}

//------------------------------------------------------------------------------
/// Update multi-buffer-transfer descriptors.
/// \param pTransfer Pointer to instance MblTransfer.
/// \param size      Size of bytes that processed.
/// \param forceEnd  Force the buffer END.
/// \return 1 if current buffer ended.
//------------------------------------------------------------------------------
static char UDPHS_MblUpdate(MblTransfer *pTransfer,
                            USBDTransferBuffer * pBi,
                            unsigned short size,
                            unsigned char forceEnd)
{
    // Update transfer descriptor
    pBi->remaining -= size;
    // Check if current buffer ended
    if (pBi->remaining == 0 || forceEnd) {

        // Process to next buffer
        if ((++ pTransfer->currBuffer) == pTransfer->listSize) {
            if (pTransfer->circList) {
                pTransfer->currBuffer = 0;
            }
            else {
                pTransfer->allUsed = 1;
            }
        }
        // All buffer in the list is processed
        if (pTransfer->currBuffer == pTransfer->freedBuffer) {
            pTransfer->allUsed = 1;
        }
        // Continue transfer, prepare for next operation
        if (pTransfer->allUsed == 0) {
            pBi = &pTransfer->pMbl[pTransfer->currBuffer];
            pBi->buffered   = 0;
            pBi->transferred = 0;
            pBi->remaining   = pBi->size;
        }
        return 1;
    }
    return 0;
}
/*
void UDPHS_WriteFifo(volatile char* pFifo,
                     unsigned char* pBuffer,
                     unsigned int   size)
{
    register unsigned int c8 = size >> 3;
    register unsigned int c1 = size & 0x7;
    for (; c8; c8 --) {
        *pFifo = *(pBuffer ++);
        *pFifo = *(pBuffer ++);
        *pFifo = *(pBuffer ++);
        *pFifo = *(pBuffer ++);

        *pFifo = *(pBuffer ++);
        *pFifo = *(pBuffer ++);
        *pFifo = *(pBuffer ++);
        *pFifo = *(pBuffer ++);
    }
    for (; c1; c1 --) {
        *pFifo = *(pBuffer ++);
    }
}
*/
//------------------------------------------------------------------------------
/// Transfers a data payload from the current tranfer buffer to the endpoint
/// FIFO
/// \param bEndpoint Number of the endpoint which is sending data.
//------------------------------------------------------------------------------
static char UDPHS_MblWriteFifo(unsigned char bEndpoint)
{
    Endpoint    *pEndpoint   = &(endpoints[bEndpoint]);
    MblTransfer *pTransfer   = (MblTransfer*)&(pEndpoint->transfer);
    USBDTransferBuffer *pBi = &(pTransfer->pMbl[pTransfer->currBuffer]);
    volatile char *pFifo;
    signed int size;

    volatile unsigned char * pBytes;
    volatile char bufferEnd = 1;

    // Get the number of bytes to send
    size = pEndpoint->size;
    if (size > pBi->remaining) {

        size = pBi->remaining;
    }

    TRACE_DEBUG_WP("w%d.%d ", pTransfer->currBuffer, size);
    if (size == 0) {

        return 1;
    }

    pTransfer->pLastLoaded = pBi;
    pBytes = &(pBi->pBuffer[pBi->transferred + pBi->buffered]);
    pBi->buffered += size;
    bufferEnd = UDPHS_MblUpdate(pTransfer, pBi, size, 0);

    // Write packet in the FIFO buffer
    pFifo = (char*)((unsigned int *)AT91C_BASE_UDPHS_EPTFIFO
                                        + (EPT_VIRTUAL_SIZE * bEndpoint));
    if (size) {
        signed int c8 = size >> 3;
        signed int c1 = size & 0x7;
        //printf("%d[%x] ", pBi->transferred, pBytes);
        for (; c8; c8 --) {
            *(pFifo++) = *(pBytes ++);
            *(pFifo++) = *(pBytes ++);
            *(pFifo++) = *(pBytes ++);
            *(pFifo++) = *(pBytes ++);

            *(pFifo++) = *(pBytes ++);
            *(pFifo++) = *(pBytes ++);
            *(pFifo++) = *(pBytes ++);
            *(pFifo++) = *(pBytes ++);
        }
        for (; c1; c1 --) {
            *(pFifo++) = *(pBytes ++);
        }
    }
    return bufferEnd;
}

//------------------------------------------------------------------------------
/// Transfers a data payload from the current tranfer buffer to the endpoint
/// FIFO
/// \param bEndpoint Number of the endpoint which is sending data.
//------------------------------------------------------------------------------
static void UDPHS_WritePayload( unsigned char bEndpoint )
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);
    Transfer *pTransfer = (Transfer*)&(pEndpoint->transfer);
    char     *pFifo;
    signed int   size;
    unsigned int dCtr;

    pFifo = (char*)((unsigned int *)AT91C_BASE_UDPHS_EPTFIFO + (EPT_VIRTUAL_SIZE * bEndpoint));

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
static void UDPHS_ReadPayload( unsigned char bEndpoint, int wPacketSize )
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);
    Transfer *pTransfer = (Transfer*)&(pEndpoint->transfer);
    char     *pFifo;
    unsigned char dBytes=0;

    pFifo = (char*)((unsigned int *)AT91C_BASE_UDPHS_EPTFIFO + (EPT_VIRTUAL_SIZE * bEndpoint));

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
static void UDPHS_ReadRequest( USBGenericRequest *pRequest )
{
    unsigned int *pData = (unsigned int *)pRequest;
    unsigned int fifo;

    fifo = (AT91C_BASE_UDPHS_EPTFIFO->UDPHS_READEPT0[0]);
    *pData = fifo;
    fifo = (AT91C_BASE_UDPHS_EPTFIFO->UDPHS_READEPT0[0]);
    pData++;
    *pData = fifo;
    //TRACE_ERROR("SETUP: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n\r", pData[0],pData[1],pData[2],pData[3],pData[4],pData[5],pData[6],pData[7]);
}

//------------------------------------------------------------------------------
/// Reset all endpoint transfer descriptors
//------------------------------------------------------------------------------
static void UDPHS_ResetEndpoints( void )
{
    Endpoint *pEndpoint;
    Transfer *pTransfer;
    unsigned char bEndpoint;

    // Reset the transfer descriptor of every endpoint
    for( bEndpoint = 0; bEndpoint < CHIP_USB_NUMENDPOINTS; bEndpoint++ ) {

        pEndpoint = &(endpoints[bEndpoint]);
        pTransfer = (Transfer*)&(pEndpoint->transfer);

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
static void UDPHS_DisableEndpoints( void )
{
    unsigned char bEndpoint;

    // Disable each endpoint, terminating any pending transfer
    // Control endpoint 0 is not disabled
    for( bEndpoint = 1; bEndpoint < CHIP_USB_NUMENDPOINTS; bEndpoint++ ) {

        UDPHS_EndOfTransfer( bEndpoint, USBD_STATUS_ABORTED );
        endpoints[bEndpoint].state = UDP_ENDPOINT_DISABLED;
    }
}

//------------------------------------------------------------------------------
/// Endpoint interrupt handler.
/// Handle IN/OUT transfers, received SETUP packets and STALLing
/// \param bEndpoint Index of endpoint
//------------------------------------------------------------------------------
static void UDPHS_EndpointHandler( unsigned char bEndpoint )
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);
    Transfer *pTransfer = (Transfer*)&(pEndpoint->transfer);
    MblTransfer *pMblt  = (MblTransfer*)&(pEndpoint->transfer);
    unsigned int   status = AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTSTA;
    unsigned short wPacketSize;
    USBGenericRequest request;

    TRACE_DEBUG_WP("E%d ", bEndpoint);
    TRACE_DEBUG_WP("st:0x%X ", status);

    // Handle interrupts
    // IN packet sent
    if( (AT91C_UDPHS_TX_PK_RDY == (AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCTL & AT91C_UDPHS_TX_PK_RDY))
     && (0 == (status & AT91C_UDPHS_TX_PK_RDY )) ) {

        TRACE_DEBUG_WP("Wr ");

        // Check that endpoint was sending multi-buffer-list
        if( pEndpoint->state == UDP_ENDPOINT_SENDINGM ) {

            USBDTransferBuffer * pMbli = pMblt->pLastLoaded;

            TRACE_DEBUG_WP("TxM%d.%d,%d ",
                pMblt->allUsed, pMbli->buffered, pMbli->remaining);

            // End of transfer ?
            if (pMblt->allUsed && pMbli->remaining == 0) {

                pMbli->transferred += pMbli->buffered;
                pMbli->buffered = 0;

                // Disable interrupt
                AT91C_BASE_UDPHS->UDPHS_IEN &= ~(1<<SHIFT_INTERUPT<<bEndpoint);
                AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCTLDIS =
                                                        AT91C_UDPHS_TX_PK_RDY;
                UDPHS_EndOfTransfer(bEndpoint, USBD_STATUS_SUCCESS);
            }
            else {

                // Transfer remaining data
                TRACE_DEBUG_WP("%d ", pEndpoint->size);

                if (pMbli->buffered  > pEndpoint->size) {
                    pMbli->transferred += pEndpoint->size;
                    pMbli->buffered -= pEndpoint->size;
                }
                else {
                    pMbli->transferred += pMbli->buffered;
                    pMbli->buffered  = 0;
                }

                // Send next packet
                UDPHS_MblWriteFifo(bEndpoint);
                AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTSETSTA =
                                                            AT91C_UDPHS_TX_PK_RDY;

                if (pMbli->remaining == 0 && pMbli->buffered == 0) {
                    
                    if (pMblt->fCallback) {
                        ((MblTransferCallback) pTransfer->fCallback)
                            (pTransfer->pArgument,
                             USBD_STATUS_PARTIAL_DONE,
                             1);
                    }
                }
            }
        }

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
                TRACE_DEBUG_WP("\n\r1pTransfer->buffered %d \n\r", pTransfer->buffered);
                TRACE_DEBUG_WP("1pTransfer->transferred %d \n\r", pTransfer->transferred);
                TRACE_DEBUG_WP("1pTransfer->remaining %d \n\r", pTransfer->remaining);

                // Transfer remaining data
                TRACE_DEBUG_WP(" %d ", pEndpoint->size);

                // Send next packet
                UDPHS_WritePayload(bEndpoint);
                AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTSETSTA = AT91C_UDPHS_TX_PK_RDY;
            }
            else {
                TRACE_DEBUG_WP("\n\r0pTransfer->buffered %d \n\r", pTransfer->buffered);
                TRACE_DEBUG_WP("0pTransfer->transferred %d \n\r", pTransfer->transferred);
                TRACE_DEBUG_WP("0pTransfer->remaining %d \n\r", pTransfer->remaining);

                TRACE_DEBUG_WP(" %d ", pTransfer->transferred);

                // Disable interrupt if this is not a control endpoint
                if( AT91C_UDPHS_EPT_TYPE_CTL_EPT != (AT91C_UDPHS_EPT_TYPE&(AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCFG)) ) {

                    AT91C_BASE_UDPHS->UDPHS_IEN &= ~(1<<SHIFT_INTERUPT<<bEndpoint);
                }
                AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCTLDIS = AT91C_UDPHS_TX_PK_RDY;

                UDPHS_EndOfTransfer(bEndpoint, USBD_STATUS_SUCCESS);
                pEndpoint->sendZLP = 0;
            }
        }
        else {

              TRACE_DEBUG("Error Wr %d", pEndpoint->sendZLP);
        }
    }

    // OUT packet received
    if( AT91C_UDPHS_RX_BK_RDY == (status & AT91C_UDPHS_RX_BK_RDY) ) {

        TRACE_DEBUG_WP("Rd ");

        // Check that the endpoint is in Receiving state
        if (pEndpoint->state != UDP_ENDPOINT_RECEIVING) {

            // Check if an ACK has been received on a Control endpoint
            if( (0 == (AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCFG & AT91C_UDPHS_EPT_TYPE))
             && (0 == (status & AT91C_UDPHS_BYTE_COUNT)) ) {

                // Control endpoint, 0 bytes received
                // Acknowledge the data and finish the current transfer
                TRACE_DEBUG_WP("Ack ");
                UDPHS_ClearRxFlag(bEndpoint);
                UDPHS_EndOfTransfer(bEndpoint, USBD_STATUS_SUCCESS);
                //todo remove endoftranfer and test
            }
            // Check if the data has been STALLed
            else if( AT91C_UDPHS_FRCESTALL == (status & AT91C_UDPHS_FRCESTALL)) {

                // Discard STALLed data
                TRACE_DEBUG_WP("Discard ");
                UDPHS_ClearRxFlag(bEndpoint);
            }
            // NAK the data
            else {

                TRACE_DEBUG_WP("Nak ");
                AT91C_BASE_UDPHS->UDPHS_IEN &= ~(1<<SHIFT_INTERUPT<<bEndpoint);
            }
        }
        else {

            // Endpoint is in Read state
            // Retrieve data and store it into the current transfer buffer
            wPacketSize = (unsigned short)((status & AT91C_UDPHS_BYTE_COUNT)>>20);

            TRACE_DEBUG_WP("%d ", wPacketSize);
            UDPHS_ReadPayload(bEndpoint, wPacketSize);
            UDPHS_ClearRxFlag(bEndpoint);

            // Check if the transfer is finished
            if ((pTransfer->remaining == 0) || (wPacketSize < pEndpoint->size)) {

                AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCTLDIS = AT91C_UDPHS_RX_BK_RDY;

                // Disable interrupt if this is not a control endpoint
                if( AT91C_UDPHS_EPT_TYPE_CTL_EPT != (AT91C_UDPHS_EPT_TYPE & (AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCFG)) ) {

                    AT91C_BASE_UDPHS->UDPHS_IEN &= ~(1<<SHIFT_INTERUPT<<bEndpoint);
                }
                UDPHS_EndOfTransfer(bEndpoint, USBD_STATUS_SUCCESS);
            }
        }
    }


    // STALL sent
    if( AT91C_UDPHS_STALL_SNT == (status & AT91C_UDPHS_STALL_SNT) ) {

        TRACE_WARNING( "Sta 0x%X [%d] ", status, bEndpoint);

        // Acknowledge the stall flag
        AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCLRSTA = AT91C_UDPHS_STALL_SNT;

        // If the endpoint is not halted, clear the STALL condition
        if (pEndpoint->state != UDP_ENDPOINT_HALTED) {

            TRACE_WARNING( "_ " );
            AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCLRSTA = AT91C_UDPHS_FRCESTALL;
        }
    }

    // SETUP packet received
    if( AT91C_UDPHS_RX_SETUP == (status & AT91C_UDPHS_RX_SETUP) )  {

        TRACE_DEBUG_WP("Stp ");

        // If a transfer was pending, complete it
        // Handles the case where during the status phase of a control write
        // transfer, the host receives the device ZLP and ack it, but the ack
        // is not received by the device
        if ((pEndpoint->state == UDP_ENDPOINT_RECEIVING)
            || (pEndpoint->state == UDP_ENDPOINT_SENDING)) {

            UDPHS_EndOfTransfer(bEndpoint, USBD_STATUS_SUCCESS);
        }
        // Copy the setup packet
        UDPHS_ReadRequest(&request);

        // Acknowledge setup packet
        AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCLRSTA = AT91C_UDPHS_RX_SETUP;

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
static void UDPHS_DmaHandler( unsigned char bEndpoint )
{
    Endpoint     *pEndpoint = &(endpoints[bEndpoint]);
    Transfer     *pTransfer = (Transfer*)&(pEndpoint->transfer);
    int           justTransferred;
    unsigned int  status;
    unsigned char result = USBD_STATUS_SUCCESS;

    status = AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMASTATUS;
    TRACE_DEBUG_WP("DmaE%d,%x ", bEndpoint, status);

    if (pEndpoint->state == UDP_ENDPOINT_SENDINGM) {

        DmaLlTransfer *pDmat   = (DmaLlTransfer*)&(pEndpoint->transfer);
        USBDDmaDescriptor *pDi = pDmat->pCurrBuffer;
        USBDDmaDescriptor *pNi = (USBDDmaDescriptor*)AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMANXTDSC;
        unsigned char bufCnt = 0;

        if (AT91C_UDPHS_END_BF_ST == (status & AT91C_UDPHS_END_BF_ST)) {

            // Stop loading descriptors
            AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMACONTROL &=
                                                      ~(AT91C_UDPHS_LDNXT_DSC);

            //printf("%x,%x|",
            //    (char)status,
            //    (short)AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMANXTDSC);

            while( pDi->pNxtDesc != pNi ) {

                // All descriptor processed
                if (pDi == 0)  break;
                if (pDi->used) break;

                // Current descriptor processed
                /*
                pDi->ctrlSettings = 0
                                    //|AT91C_UDPHS_CHANN_ENB
                                    //| AT91C_UDPHS_LDNXT_DSC
                                    //| AT91C_UDPHS_END_TR_EN
                                    //| AT91C_UDPHS_END_TR_IT
                                    //| AT91C_UDPHS_END_B_EN
                                    //| AT91C_UDPHS_END_BUFFIT
                                    //| AT91C_UDPHS_DESC_LD_IT
                                    ; */
                pDi->used = 1;      // Flag as used
                bufCnt ++;
                pDi = pDi->pNxtDesc;
            }
            pDmat->pCurrBuffer = pDi;

            if (bufCnt) {

                if (pDmat->fCallback) {

                    pDmat->fCallback(pDmat->pArgument,
                                    ((result == USBD_STATUS_SUCCESS) ? 
                                        USBD_STATUS_PARTIAL_DONE : result),
                                    bufCnt);
                }
            }

            if (   (pNi == 0)
                || (pNi->used && pNi) ) {

                // Disable DMA endpoint interrupt
                AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMACONTROL = 0;
                AT91C_BASE_UDPHS->UDPHS_IEN &= ~(1 << SHIFT_DMA << bEndpoint);

                TRACE_DEBUG_WP("EoMDT ");
                //printf("E:N%x,C%d ", (short)pNi, bufCnt);

                UDPHS_EndOfTransfer(bEndpoint, result);
            }
            else {

                AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMACONTROL |= 
                                                            (AT91C_UDPHS_LDNXT_DSC);
            }
        }
        else {
        
            TRACE_ERROR("UDPHS: Dma(0x%X)\n\r", status);
            result = USBD_STATUS_ABORTED;
            UDPHS_EndOfTransfer(bEndpoint, result);
        }
        return;
    }
    else if (pEndpoint->state == UDP_ENDPOINT_RECEIVINGM) {
        return;
    }

    // Disable DMA interrupt to avoid receiving 2 interrupts (B_EN and TR_EN)
    AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMACONTROL &=
        ~(AT91C_UDPHS_END_TR_EN | AT91C_UDPHS_END_B_EN);

    AT91C_BASE_UDPHS->UDPHS_IEN &= ~(1 << SHIFT_DMA << bEndpoint);

    if( AT91C_UDPHS_END_BF_ST == (status & AT91C_UDPHS_END_BF_ST) ) {

        TRACE_DEBUG_WP("EndBuffer ");

        // BUFF_COUNT holds the number of untransmitted bytes.
        // BUFF_COUNT is equal to zero in case of good transfer
        justTransferred = pTransfer->buffered
                                 - ((status & AT91C_UDPHS_BUFF_COUNT) >> 16);
        pTransfer->transferred += justTransferred;

        pTransfer->buffered = ((status & AT91C_UDPHS_BUFF_COUNT) >> 16);

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

            AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMAADDRESS = 
                (unsigned int)((pTransfer->pData) + (pTransfer->transferred));

            // Clear unwanted interrupts
            AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMASTATUS;

            // Enable DMA endpoint interrupt
            AT91C_BASE_UDPHS->UDPHS_IEN |= (1 << SHIFT_DMA << bEndpoint);
            // DMA config for receive the good size of buffer, or an error buffer

            AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMACONTROL = 0; // raz
            AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMACONTROL =
                                     ( ((pTransfer->buffered << 16) & AT91C_UDPHS_BUFF_COUNT)
                                       | AT91C_UDPHS_END_TR_EN
                                       | AT91C_UDPHS_END_TR_IT
                                       | AT91C_UDPHS_END_B_EN
                                       | AT91C_UDPHS_END_BUFFIT
                                       | AT91C_UDPHS_CHANN_ENB );
        }
    }
    else if( AT91C_UDPHS_END_TR_ST == (status & AT91C_UDPHS_END_TR_ST) ) {

        TRACE_DEBUG_WP("EndTransf ");

        pTransfer->transferred = pTransfer->buffered
                                 - ((status & AT91C_UDPHS_BUFF_COUNT) >> 16);
        pTransfer->remaining = 0;
        TRACE_DEBUG_WP("\n\r0pTransfer->buffered %d \n\r", pTransfer->buffered);
        TRACE_DEBUG_WP("0pTransfer->transferred %d \n\r", pTransfer->transferred);
        TRACE_DEBUG_WP("0pTransfer->remaining %d \n\r", pTransfer->remaining);
    }
    else {

        TRACE_ERROR("UDPHS_DmaHandler: Error (0x%08X)\n\r", status);
        result = USBD_STATUS_ABORTED;
    }

    // Invoke callback
    if( pTransfer->remaining == 0 ) {

        TRACE_DEBUG_WP("EOT ");
        UDPHS_EndOfTransfer(bEndpoint, result);
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

    // Get interrupts status
    status = AT91C_BASE_UDPHS->UDPHS_INTSTA & AT91C_BASE_UDPHS->UDPHS_IEN;

    // Handle all UDPHS interrupts
    TRACE_DEBUG_WP("H");
    while (status != 0) {

        // Start Of Frame (SOF)
        if ((status & AT91C_UDPHS_IEN_SOF) != 0) {

            TRACE_DEBUG_WP("SOF ");

            // Invoke the SOF callback
            //USB_StartOfFrameCallback(pUsb);

            // Acknowledge interrupt
            AT91C_BASE_UDPHS->UDPHS_CLRINT = AT91C_UDPHS_IEN_SOF;
            status &= ~AT91C_UDPHS_IEN_SOF;
        }
        // Suspend
        // This interrupt is always treated last (hence the '==')
        else if (status == AT91C_UDPHS_DET_SUSPD) {

            TRACE_DEBUG_WP("S");

            // The device enters the Suspended state
            // MCK + UDPCK must be off
            // Pull-Up must be connected
            // Transceiver must be disabled

            LED_Clear(USBD_LEDUSB);

            UDPHS_DisableBIAS();

            // Enable wakeup
            AT91C_BASE_UDPHS->UDPHS_IEN |= AT91C_UDPHS_WAKE_UP | AT91C_UDPHS_ENDOFRSM;
            AT91C_BASE_UDPHS->UDPHS_IEN &= ~AT91C_UDPHS_DET_SUSPD;

            // Acknowledge interrupt
            AT91C_BASE_UDPHS->UDPHS_CLRINT = AT91C_UDPHS_DET_SUSPD | AT91C_UDPHS_WAKE_UP;
            previousDeviceState = deviceState;
            deviceState = USBD_STATE_SUSPENDED;
            UDPHS_DisableUsbClock();

            // Invoke the Suspend callback
            USBDCallbacks_Suspended();
        }
        // Resume
        else if( ((status & AT91C_UDPHS_WAKE_UP) != 0)      // line activity
              || ((status & AT91C_UDPHS_ENDOFRSM) != 0))  { // pc wakeup
            {
                // Invoke the Resume callback
                USBDCallbacks_Resumed();

                TRACE_DEBUG_WP("R");

                UDPHS_EnableUsbClock();
                UDPHS_EnableBIAS();

                // The device enters Configured state
                // MCK + UDPCK must be on
                // Pull-Up must be connected
                // Transceiver must be enabled

                deviceState = previousDeviceState;

                AT91C_BASE_UDPHS->UDPHS_CLRINT = AT91C_UDPHS_WAKE_UP | AT91C_UDPHS_ENDOFRSM | AT91C_UDPHS_DET_SUSPD;

                AT91C_BASE_UDPHS->UDPHS_IEN |= AT91C_UDPHS_ENDOFRSM | AT91C_UDPHS_DET_SUSPD;
                AT91C_BASE_UDPHS->UDPHS_CLRINT = AT91C_UDPHS_WAKE_UP | AT91C_UDPHS_ENDOFRSM;
                AT91C_BASE_UDPHS->UDPHS_IEN &= ~AT91C_UDPHS_WAKE_UP;
            }
        }
        // End of bus reset
        else if ((status & AT91C_UDPHS_ENDRESET) == AT91C_UDPHS_ENDRESET) {

//            TRACE_DEBUG_WP("EoB ");

            // The device enters the Default state
            deviceState = USBD_STATE_DEFAULT;
            //      MCK + UDPCK are already enabled
            //      Pull-Up is already connected
            //      Transceiver must be enabled
            //      Endpoint 0 must be enabled

            UDPHS_ResetEndpoints();
            UDPHS_DisableEndpoints();
            USBD_ConfigureEndpoint(0);

            // Flush and enable the Suspend interrupt
            AT91C_BASE_UDPHS->UDPHS_CLRINT = AT91C_UDPHS_WAKE_UP | AT91C_UDPHS_DET_SUSPD;

            //// Enable the Start Of Frame (SOF) interrupt if needed
            //if (pCallbacks->startOfFrame != 0)
            //{
            //    AT91C_BASE_UDPHS->UDPHS_IEN |= AT91C_UDPHS_IEN_SOF;
            //}

            // Invoke the Reset callback
            USBDCallbacks_Reset();

            // Acknowledge end of bus reset interrupt
            AT91C_BASE_UDPHS->UDPHS_CLRINT = AT91C_UDPHS_ENDRESET;

            AT91C_BASE_UDPHS->UDPHS_IEN |= AT91C_UDPHS_DET_SUSPD;
        }
        // Handle upstream resume interrupt
        else if (status & AT91C_UDPHS_UPSTR_RES) {

            TRACE_DEBUG_WP("ExtRes ");

            // - Acknowledge the IT
            AT91C_BASE_UDPHS->UDPHS_CLRINT = AT91C_UDPHS_UPSTR_RES;
        }
        // Endpoint interrupts
        else {
#ifndef DMA
            // Handle endpoint interrupts
            for (numIT = 0; numIT < NUM_IT_MAX; numIT++) {

                if ((status & (1 << SHIFT_INTERUPT << numIT)) != 0) {

                    UDPHS_EndpointHandler(numIT);
                }
            }
#else
            // Handle endpoint control interrupt
            if ((status & (1 << SHIFT_INTERUPT << 0)) != 0) {

                UDPHS_EndpointHandler( 0 );
            }
            else {

                numIT = 1;
                while((status&(0x7E<<SHIFT_DMA)) != 0) {

                    // Check if endpoint has a pending interrupt
                    if ((status & (1 << SHIFT_DMA << numIT)) != 0) {

                        UDPHS_DmaHandler(numIT);
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
        status = AT91C_BASE_UDPHS->UDPHS_INTSTA & AT91C_BASE_UDPHS->UDPHS_IEN;

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
    //unsigned char bInterval = 0;
    unsigned char bNbTrans  = 1;
    unsigned char bSizeEpt  = 0;
    unsigned char bHs = ((AT91C_BASE_UDPHS->UDPHS_INTSTA & AT91C_UDPHS_SPEED)
                                                                           > 0);

    // NULL descriptor -> Control endpoint 0
    if (pDescriptor == 0) {

        bEndpoint = 0;
        pEndpoint = &(endpoints[bEndpoint]);
        bType = USBEndpointDescriptor_CONTROL;
        bEndpointDir = 0;
        pEndpoint->size = CHIP_USB_ENDPOINTS_MAXPACKETSIZE(0);
        pEndpoint->bank = CHIP_USB_ENDPOINTS_BANKS(0);
    }
    else  {

        // The endpoint number
        bEndpoint = USBEndpointDescriptor_GetNumber(pDescriptor);
        pEndpoint = &(endpoints[bEndpoint]);
        // Transfer type: Control, Isochronous, Bulk, Interrupt
        bType = USBEndpointDescriptor_GetType(pDescriptor);
        // interval
        //bInterval = USBEndpointDescriptor_GetInterval(pDescriptor);
        // Direction, ignored for control endpoints
        bEndpointDir = USBEndpointDescriptor_GetDirection(pDescriptor);
        pEndpoint->size = USBEndpointDescriptor_GetMaxPacketSize(pDescriptor);
        pEndpoint->bank = CHIP_USB_ENDPOINTS_BANKS(bEndpoint);

        // Convert descriptor value to EP configuration
        if (bHs) {  // HS Interval, *125us

            // MPS: Bit12,11 specify NB_TRANS
            bNbTrans = ((pEndpoint->size >> 11) & 0x3);
            if (bNbTrans == 3)
                bNbTrans = 1;
            else
                bNbTrans ++;

            // Mask, bit 10..0 is the size
            pEndpoint->size &= 0x3FF;
        }
    }

    // Abort the current transfer is the endpoint was configured and in
    // Write or Read state
    if( (pEndpoint->state == UDP_ENDPOINT_RECEIVING)
     || (pEndpoint->state == UDP_ENDPOINT_SENDING)
     || (pEndpoint->state == UDP_ENDPOINT_RECEIVINGM)
     || (pEndpoint->state == UDP_ENDPOINT_SENDINGM) ) {

        UDPHS_EndOfTransfer(bEndpoint, USBD_STATUS_RESET);
    }
    pEndpoint->state = UDP_ENDPOINT_IDLE;

    // Disable endpoint
    AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCTLDIS = AT91C_UDPHS_SHRT_PCKT
                                                           | AT91C_UDPHS_BUSY_BANK
                                                           | AT91C_UDPHS_NAK_OUT
                                                           | AT91C_UDPHS_NAK_IN
                                                           | AT91C_UDPHS_STALL_SNT
                                                           | AT91C_UDPHS_RX_SETUP
                                                           | AT91C_UDPHS_TX_PK_RDY
                                                           | AT91C_UDPHS_TX_COMPLT
                                                           | AT91C_UDPHS_RX_BK_RDY
                                                           | AT91C_UDPHS_ERR_OVFLW
                                                           | AT91C_UDPHS_MDATA_RX
                                                           | AT91C_UDPHS_DATAX_RX
                                                           | AT91C_UDPHS_NYET_DIS
                                                           | AT91C_UDPHS_INTDIS_DMA
                                                           | AT91C_UDPHS_AUTO_VALID
                                                           | AT91C_UDPHS_EPT_DISABL;

    // Reset Endpoint Fifos
    AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCLRSTA = AT91C_UDPHS_TOGGLESQ | AT91C_UDPHS_FRCESTALL;
    AT91C_BASE_UDPHS->UDPHS_EPTRST = 1<<bEndpoint;

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

    // Configure endpoint
    if (bType == USBEndpointDescriptor_CONTROL) {

        // Enable endpoint IT for control endpoint
        AT91C_BASE_UDPHS->UDPHS_IEN |= (1<<SHIFT_INTERUPT<<bEndpoint);
    }


    AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCFG = bSizeEpt 
                                                        | ( bEndpointDir << 3) 
                                                        | ( bType << 4) 
                                                        | ((pEndpoint->bank) << 6)
                                                        | ( bNbTrans << 8)
                                                        ;

    while( (signed int)AT91C_UDPHS_EPT_MAPD != (signed int)((AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCFG) & AT91C_UDPHS_EPT_MAPD) ) {

        // resolved by clearing the reset IT in good place
        TRACE_ERROR("PB bEndpoint: 0x%X\n\r", bEndpoint);
        TRACE_ERROR("PB bSizeEpt: 0x%X\n\r", bSizeEpt);
        TRACE_ERROR("PB bEndpointDir: 0x%X\n\r", bEndpointDir);
        TRACE_ERROR("PB bType: 0x%X\n\r", bType);
        TRACE_ERROR("PB pEndpoint->bank: 0x%X\n\r", pEndpoint->bank);
        TRACE_ERROR("PB UDPHS_EPTCFG: 0x%X\n\r", AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCFG);
        for(;;);
    }

    if (bType == USBEndpointDescriptor_CONTROL) {

        AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCTLENB = AT91C_UDPHS_RX_BK_RDY 
                                                               | AT91C_UDPHS_RX_SETUP
                                                               | AT91C_UDPHS_EPT_ENABL;
    }
    else {
#ifndef DMA
        AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCTLENB = AT91C_UDPHS_EPT_ENABL;
#else
        AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCTLENB = AT91C_UDPHS_AUTO_VALID 
                                                               | AT91C_UDPHS_EPT_ENABL;
#endif
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
    Transfer *pTransfer = (Transfer*)&(pEndpoint->transfer);

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
    if(AT91C_UDPHS_EPT_TYPE_CTL_EPT == (AT91C_UDPHS_EPT_TYPE&(AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCFG)))  {
#endif
        // Enable endpoint IT
        AT91C_BASE_UDPHS->UDPHS_IEN |= (1 << SHIFT_INTERUPT << bEndpoint);
        AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCTLENB = AT91C_UDPHS_TX_PK_RDY;

#ifdef DMA
    }
    else {
        if( CHIP_USB_ENDPOINTS_DMA(bEndpoint) == 0 ) {
            TRACE_FATAL("Endpoint has no DMA\n\r");
        }
        if( pTransfer->remaining == 0 ) {
            // DMA not handle ZLP
            AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTSETSTA = AT91C_UDPHS_TX_PK_RDY;
            // Enable endpoint IT
            AT91C_BASE_UDPHS->UDPHS_IEN |= (1 << SHIFT_INTERUPT << bEndpoint);
            AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCTLENB = AT91C_UDPHS_TX_PK_RDY;
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

            TRACE_DEBUG_WP("\n\r_WR[%x]:%d ", pTransfer->pData, pTransfer->remaining );
            TRACE_DEBUG_WP("B:%d ", pTransfer->buffered );
            TRACE_DEBUG_WP("T:%d ", pTransfer->transferred );

            AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMAADDRESS = (unsigned int)(pTransfer->pData);

            // Clear unwanted interrupts
            AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMASTATUS;
            // Enable DMA endpoint interrupt
            AT91C_BASE_UDPHS->UDPHS_IEN |= (1 << SHIFT_DMA << bEndpoint);
            // DMA config
            AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMACONTROL = 0; // raz
            AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMACONTROL =
                                              ( ((pTransfer->buffered << 16) & AT91C_UDPHS_BUFF_COUNT)
                                                | AT91C_UDPHS_END_B_EN
                                                | AT91C_UDPHS_END_BUFFIT
                                                | AT91C_UDPHS_CHANN_ENB );
        }
    }
#endif

    return USBD_STATUS_SUCCESS;
}

//------------------------------------------------------------------------------
/// Sends data frames through a USB endpoint. Sets up the transfer descriptor
/// list, writes one or two data payloads (depending on the number of FIFO bank
/// for the endpoint) and then starts the actual transfer. The operation invoke
/// a callback with RC USBD_STATUS_PARTIAL_DONE each time when a buffer is
/// transferred and is complete when all the data has been sent.
///
/// *If the size of the frame is greater than the size of the endpoint
///  (or twice the size if the endpoint has two FIFO banks), then the buffer
///  must be kept allocated until the frame is finished*. This means that
///  it is not possible to declare it on the stack (i.e. as a local variable
///  of a function which returns after starting a transfer).
///
/// \param bEndpoint Endpoint number.
/// \param pMbl Pointer to a frame list with the data to send.
///             Two different list is supported, for EPs that support DMA,
///             the frame is described by USBDDmaDescriptor link list,
///             for EPs that uses FIFO the frame is described by
///             USBDTransferBuffer array.
///             For DMA transfer, the size of each frame is limited by the
///             max size of one DMA transfer. When using DMA, the frame list
///             size and size of each frame should be larger so that more
///             efficiency is achieved and CPU could have more time on
///             handler of each buffer end event.
/// \param wListSize Size of the frame list.
/// \param bCircList Circle the list.
/// \param wStartNdx Starting buffer index in the list.
/// \param fCallback Optional callback function to invoke when the transfer is
///        complete.
/// \param pArgument Optional argument to the callback function.
/// \return USBD_STATUS_SUCCESS if the transfer has been started;
///         otherwise, the corresponding error status code.
/// \see USBDDmaDescriptor, USBDTransferBuffer
//------------------------------------------------------------------------------
char USBD_MblWrite( unsigned char       bEndpoint,
                    void                *pMbl,
                    unsigned short      wListSize,
                    unsigned char       bCircList,
                    unsigned short      wStartNdx,
                    MblTransferCallback fCallback,
                    void                *pArgument )
{
    Endpoint    *pEndpoint = &(endpoints[bEndpoint]);
    MblTransfer *pTransfer = (MblTransfer*)&(pEndpoint->transfer);
    unsigned short i;

    // EP0 is not suitable for Mbl
    if (bEndpoint == 0) {

        return USBD_STATUS_INVALID_PARAMETER;
    }

    // Check that the endpoint is in Idle state
    if (pEndpoint->state != UDP_ENDPOINT_IDLE) {

        //return USBD_STATUS_LOCKED;
    }
    // Setup state
    pEndpoint->state = UDP_ENDPOINT_SENDINGM;

    TRACE_DEBUG_WP("WriteM%d(0x%x,%d) ", bEndpoint, pMbl, wListSize);

    // Start from first if not circled list
    if (!bCircList) wStartNdx = 0;

#ifdef DMA
    if (UDP_IsDmaUsed(bEndpoint)) {

        DmaLlTransfer *pDmallt  = (DmaLlTransfer*)&(pEndpoint->transfer);
        USBDDmaDescriptor * pDi = (USBDDmaDescriptor*)pMbl;

        // DMA config
        AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMACONTROL = 0;
        AT91C_BASE_UDPHS->UDPHS_IEN &= ~(1 << SHIFT_DMA << bEndpoint);

        // Setup the transfer descriptor
        pDmallt->pMbl         = (USBDDmaDescriptor*)pMbl;
        pDmallt->listSize     = wListSize;
        pDmallt->fCallback    = fCallback;
        pDmallt->pArgument    = pArgument;
        pDmallt->circList     = bCircList;
        pDmallt->allUsed      = 0;

        // Check all buffers, fill settings
        for (i = 0; i < wListSize - 1; i ++) {

            if (pDi->bufferLength > DMA_MAX_FIFO_SIZE) {

                TRACE_ERROR("DMA buffer size %d exceeds max %d\n\r",
                    pDi->bufferLength, DMA_MAX_FIFO_SIZE);
                return USBD_STATUS_INVALID_PARAMETER;
            }

            // Channel control value
            pDi->ctrlSettings = AT91C_UDPHS_CHANN_ENB
                                | AT91C_UDPHS_LDNXT_DSC
                                | AT91C_UDPHS_END_TR_EN
                                //| AT91C_UDPHS_END_TR_IT
                                | AT91C_UDPHS_END_B_EN
                                | AT91C_UDPHS_END_BUFFIT
                                | AT91C_UDPHS_DESC_LD_IT
                                ;
            pDi->used = 0;
            pDi = pDi->pNxtDesc;
        }

        // Circled list check
        pDi->used = 0;
        if (bCircList) {

            pDi->pNxtDesc = pDmallt->pMbl;
            pDi->ctrlSettings = AT91C_UDPHS_CHANN_ENB
                                | AT91C_UDPHS_LDNXT_DSC
                                | AT91C_UDPHS_END_TR_EN
                                //| AT91C_UDPHS_END_TR_IT
                                | AT91C_UDPHS_END_B_EN
                                | AT91C_UDPHS_END_BUFFIT
                                //| AT91C_UDPHS_DESC_LD_IT
                                ;
        }
        else {

            pDi->pNxtDesc = 0;
            pDi->ctrlSettings = AT91C_UDPHS_CHANN_ENB
                                //| AT91C_UDPHS_LDNXT_DSC
                                | AT91C_UDPHS_END_TR_EN
                                //| AT91C_UDPHS_END_TR_IT
                                | AT91C_UDPHS_END_B_EN
                                | AT91C_UDPHS_END_BUFFIT
                                //| AT91C_UDPHS_DESC_LD_IT
                                ;
        }

        // Start from 'wStartNdx'th descriptor
        pDi = (USBDDmaDescriptor*)pMbl;
        for (;wStartNdx; wStartNdx --) {
            pDi = pDi->pNxtDesc;
        }
        pDmallt->pCurrBuffer  = (USBDDmaDescriptor*)pDi;
        pDmallt->pFreedBuffer = (USBDDmaDescriptor*)pDi;

        // Disable DMA when interrupt happenned
        AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCTLENB = AT91C_UDPHS_INTDIS_DMA;

        // Clear unwanted interrupts
        AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMASTATUS;
        // Enable DMA endpoint interrupt
        AT91C_BASE_UDPHS->UDPHS_IEN |= (1 << SHIFT_DMA << bEndpoint);
        AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMANXTDSC  = (unsigned int)pMbl;
        AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMACONTROL = AT91C_UDPHS_LDNXT_DSC
                                                                //| AT91C_UDPHS_END_TR_EN
                                                                //| AT91C_UDPHS_END_TR_IT
                                                                //| AT91C_UDPHS_END_B_EN
                                                                //| AT91C_UDPHS_END_BUFFIT
                                                                //| AT91C_UDPHS_DESC_LD_IT
                                                                ;

        return USBD_STATUS_SUCCESS;
    }
#endif

    // DMA is not used
    {
        unsigned char nbBanks = CHIP_USB_ENDPOINTS_BANKS(bEndpoint);

        // Setup the transfer descriptor
        pTransfer->pMbl        = (USBDTransferBuffer*)pMbl;
        pTransfer->listSize    = wListSize;
        pTransfer->fCallback   = fCallback;
        pTransfer->pArgument   = pArgument;
        pTransfer->currBuffer  = wStartNdx;
        pTransfer->freedBuffer = 0;
        pTransfer->pLastLoaded = &(((USBDTransferBuffer*)pMbl)[wStartNdx]);
        pTransfer->circList    = bCircList;
        pTransfer->allUsed     = 0;
        
        // Clear all buffer
        for (i = 0; i < wListSize; i ++) {

            pTransfer->pMbl[i].transferred = 0;
            pTransfer->pMbl[i].buffered    = 0;
            pTransfer->pMbl[i].remaining   = pTransfer->pMbl[i].size;
        }

        // Fill data into FIFO
        for (;
            nbBanks && pTransfer->pMbl[pTransfer->currBuffer].remaining;
            nbBanks --) {

            UDPHS_MblWriteFifo(bEndpoint);
            AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTSETSTA =
                                                    AT91C_UDPHS_TX_PK_RDY;
        }

        // Enable interrup
        AT91C_BASE_UDPHS->UDPHS_IEN |= (1 << SHIFT_INTERUPT << bEndpoint);
        AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCTLENB = 0
                                                   //| AT91C_UDPHS_TX_COMPLT
                                                   | AT91C_UDPHS_TX_PK_RDY
                                                   ;
        return USBD_STATUS_SUCCESS;
    }
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
    Transfer *pTransfer = (Transfer*)&(pEndpoint->transfer);
  
    // Return if the endpoint is not in IDLE state
    if (pEndpoint->state != UDP_ENDPOINT_IDLE) {

        return USBD_STATUS_LOCKED;
    }

    TRACE_DEBUG_WP("Read%d(%d) ", bEndpoint, dLength);

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
    if(AT91C_UDPHS_EPT_TYPE_CTL_EPT == (AT91C_UDPHS_EPT_TYPE&(AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCFG))) {
#endif
        // Control endpoint
        // Enable endpoint IT
        AT91C_BASE_UDPHS->UDPHS_IEN |= (1 << SHIFT_INTERUPT << bEndpoint);
        AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCTLENB = AT91C_UDPHS_RX_BK_RDY;
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

        AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMAADDRESS = (unsigned int)(pTransfer->pData);

        // Clear unwanted interrupts
        AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMASTATUS;

        // Enable DMA endpoint interrupt
        AT91C_BASE_UDPHS->UDPHS_IEN |= (1 << SHIFT_DMA << bEndpoint);

        TRACE_DEBUG_WP("\n\r_RR:%d ", pTransfer->remaining );
        TRACE_DEBUG_WP("B:%d ", pTransfer->buffered );
        TRACE_DEBUG_WP("T:%d ", pTransfer->transferred );

        // DMA config
        AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMACONTROL = 0; // raz
        AT91C_BASE_UDPHS->UDPHS_DMA[bEndpoint].UDPHS_DMACONTROL =
                                 ( ((pTransfer->buffered << 16) & AT91C_UDPHS_BUFF_COUNT)
                                   | AT91C_UDPHS_END_TR_EN
                                   | AT91C_UDPHS_END_TR_IT
                                   | AT91C_UDPHS_END_B_EN
                                   | AT91C_UDPHS_END_BUFFIT
                                   | AT91C_UDPHS_CHANN_ENB );
    }
#endif

    return USBD_STATUS_SUCCESS;
}

//------------------------------------------------------------------------------
/// Reuse first used/released buffer with new buffer address and size to be used
/// in transfer again. Only valid when frame list is ringed. Can be used for
/// both read & write.
/// \param bEndpoint  Endpoint number.
/// \param pNewBuffer Pointer to new buffer with data to send (0 to keep last).
/// \param wNewSize   Size of the data buffer
//------------------------------------------------------------------------------
char USBD_MblReuse( unsigned char  bEndpoint,
                    unsigned char  *pNewBuffer,
                    unsigned short wNewSize )
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);
    MblTransfer *pTransfer = (MblTransfer*)&(pEndpoint->transfer);
    USBDTransferBuffer *pBi = &(pTransfer->pMbl[pTransfer->freedBuffer]);

    // Only for Multi-buffer-circle list
    if (bEndpoint != 0
        && (pEndpoint->state == UDP_ENDPOINT_RECEIVINGM
            || pEndpoint->state == UDP_ENDPOINT_SENDINGM)) {
    }
    else {

        return USBD_STATUS_WRONG_STATE;
    }

    TRACE_DEBUG_WP("MblReuse(%d), st%x, circ%d\n\r",
        bEndpoint, pEndpoint->state, pTransfer->circList);

#ifdef DMA
    if (UDP_IsDmaUsed(bEndpoint)) {

        DmaLlTransfer *pDmallt = (DmaLlTransfer*)&(pEndpoint->transfer);
        USBDDmaDescriptor *pDi  = pDmallt->pFreedBuffer;

        // ONLY for circled list
        if (!pDmallt->circList) {

            return USBD_STATUS_WRONG_STATE;
        }

        // Check if there is freed buffer
        if ((pDi == 0) || (pDi && !pDi->used) ) {

            return USBD_STATUS_LOCKED;
        }

        // Update transfer information
        pDmallt->pFreedBuffer = pDi->pNxtDesc;
        if (pNewBuffer) {

            pDi->pDataAddr    = pNewBuffer;
            pDi->bufferLength = wNewSize;
        }
        /*
        pDi->ctrlSettings = AT91C_UDPHS_CHANN_ENB
                            | AT91C_UDPHS_LDNXT_DSC
                            | AT91C_UDPHS_END_TR_EN
                            //| AT91C_UDPHS_END_TR_IT
                            | AT91C_UDPHS_END_B_EN
                            | AT91C_UDPHS_END_BUFFIT
                            | AT91C_UDPHS_DESC_LD_IT
                            ; */
        pDi->used = 0;

        pDmallt->allUsed = 0;

        return USBD_STATUS_SUCCESS;
    }
#endif

    // DMA is not used
    {

        // ONLY for circled list
        if (!pTransfer->circList) {

            return USBD_STATUS_WRONG_STATE;
        }

        // Check if there is freed buffer
        if (pTransfer->freedBuffer == pTransfer->currBuffer
            && !pTransfer->allUsed) {

            return USBD_STATUS_LOCKED;
        }

        // Update transfer information
        if ((++ pTransfer->freedBuffer) == pTransfer->listSize)
            pTransfer->freedBuffer = 0;
        if (pNewBuffer) {

            pBi->pBuffer = pNewBuffer;
            pBi->size    = wNewSize;
        }
        pBi->buffered   = 0;
        pBi->transferred = 0;
        pBi->remaining   = pBi->size;

        // At least one buffer is not processed
        pTransfer->allUsed = 0;
    }

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
    // Check that endpoint is enabled and not already in Halt state
    if( (pEndpoint->state != UDP_ENDPOINT_DISABLED)
     && (pEndpoint->state != UDP_ENDPOINT_HALTED) ) {

        TRACE_INFO("Halt%d ", bEndpoint);

        // Abort the current transfer if necessary
        UDPHS_EndOfTransfer(bEndpoint, USBD_STATUS_ABORTED);

        // Put endpoint into Halt state
        AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTSETSTA = AT91C_UDPHS_FRCESTALL;
        pEndpoint->state = UDP_ENDPOINT_HALTED;

#ifdef DMA
        // Test if endpoint type control
        if(AT91C_UDPHS_EPT_TYPE_CTL_EPT == (AT91C_UDPHS_EPT_TYPE&(AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCFG)))  {
#endif
            // Enable the endpoint interrupt
            AT91C_BASE_UDPHS->UDPHS_IEN |= (1<<SHIFT_INTERUPT<<bEndpoint);
#ifdef DMA
        }
        else {
            // Enable IT DMA
            AT91C_BASE_UDPHS->UDPHS_IEN |= (1<<SHIFT_DMA<<bEndpoint);
        }
#endif
   }
}

//------------------------------------------------------------------------------
/// Clears the Halt feature on the given endpoint.
/// \param bEndpoint Index of endpoint
//------------------------------------------------------------------------------
void USBD_Unhalt( unsigned char bEndpoint )
{
    Endpoint *pEndpoint = &(endpoints[bEndpoint]);

    // Check if the endpoint is enabled
    if (pEndpoint->state != UDP_ENDPOINT_DISABLED) {

        TRACE_DEBUG_WP("Unhalt%d ", bEndpoint);

        // Return endpoint to Idle state
        pEndpoint->state = UDP_ENDPOINT_IDLE;

        // Clear FORCESTALL flag
        AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTCLRSTA = AT91C_UDPHS_TOGGLESQ | AT91C_UDPHS_FRCESTALL;

        // Reset Endpoint Fifos
        AT91C_BASE_UDPHS->UDPHS_EPTRST = (1<<bEndpoint);
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

    if( AT91C_UDPHS_SPEED == (AT91C_BASE_UDPHS->UDPHS_INTSTA & AT91C_UDPHS_SPEED) )
    {
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

    AT91C_BASE_UDPHS->UDPHS_EPT[bEndpoint].UDPHS_EPTSETSTA = AT91C_UDPHS_FRCESTALL;

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
        UDPHS_EnableUsbClock();

        // Activates a remote wakeup
        AT91C_BASE_UDPHS->UDPHS_CTRL |= AT91C_UDPHS_REWAKEUP;

        while ((AT91C_BASE_UDPHS->UDPHS_CTRL&AT91C_UDPHS_REWAKEUP) == AT91C_UDPHS_REWAKEUP) {

            TRACE_DEBUG_WP("W");
        }
        UDPHS_EnableBIAS();
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
    AT91C_BASE_UDPHS->UDPHS_CTRL &= ~AT91C_UDPHS_DEV_ADDR; // RAZ Address
    AT91C_BASE_UDPHS->UDPHS_CTRL |= address | AT91C_UDPHS_FADDR_EN;

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
        UDPHS_DisableEndpoints();
    }
}

//------------------------------------------------------------------------------
/// Enables the pull-up on the D+ line to connect the device to the USB.
//------------------------------------------------------------------------------
void USBD_Connect( void )
{
    TRACE_DEBUG_WP("Conn ");
#if defined(CHIP_USB_PULLUP_INTERNAL)
    AT91C_BASE_UDPHS->UDPHS_CTRL &= ~AT91C_UDPHS_DETACH;   // Pull Up on DP
    AT91C_BASE_UDPHS->UDPHS_CTRL |= AT91C_UDPHS_PULLD_DIS; // Disable Pull Down

#elif defined(CHIP_USB_PULLUP_INTERNAL_BY_MATRIX)
    TRACE_DEBUG_WP("PUON 1\n\r");
    AT91C_BASE_MATRIX->MATRIX_USBPCR |= AT91C_MATRIX_USBPCR_PUON;

#elif defined(CHIP_USB_PULLUP_EXTERNAL)

#ifdef PIN_USB_PULLUP
    const Pin pinPullUp = PIN_USB_PULLUP;
    if( pinPullUp.attribute == PIO_OUTPUT_0 ) {

        PIO_Set(&pinPullUp);
    }
    else {

        PIO_Clear(&pinPullUp);
    }
#else
    #error unsupported now
#endif

#elif !defined(CHIP_USB_PULLUP_ALWAYSON)
    #error Unsupported pull-up type.

#endif
}

//------------------------------------------------------------------------------
/// Disables the pull-up on the D+ line to disconnect the device from the bus.
//------------------------------------------------------------------------------
void USBD_Disconnect( void )
{
    TRACE_DEBUG_WP("Disc ");

#if defined(CHIP_USB_PULLUP_INTERNAL)
    AT91C_BASE_UDPHS->UDPHS_CTRL |= AT91C_UDPHS_DETACH; // detach
    AT91C_BASE_UDPHS->UDPHS_CTRL &= ~AT91C_UDPHS_PULLD_DIS; // Enable Pull Down

#elif defined(CHIP_USB_PULLUP_INTERNAL_BY_MATRIX)
    AT91C_BASE_MATRIX->MATRIX_USBPCR &= ~AT91C_MATRIX_USBPCR_PUON;

#elif defined(CHIP_USB_PULLUP_EXTERNAL)

#ifdef PIN_USB_PULLUP
    const Pin pinPullUp = PIN_USB_PULLUP;
    if (pinPullUp.attribute == PIO_OUTPUT_0) {

        PIO_Clear(&pinPullUp);
    }
    else {

        PIO_Set(&pinPullUp);
    }
#else
    #error unsupported now
#endif

#elif !defined(CHIP_USB_PULLUP_ALWAYSON)
    #error Unsupported pull-up type.

#endif

    // Device returns to the Powered state
    if (deviceState > USBD_STATE_POWERED) {    

        deviceState = USBD_STATE_POWERED;
    }
    if (previousDeviceState > USBD_STATE_POWERED) {

        previousDeviceState = USBD_STATE_POWERED;
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

    AT91C_BASE_UDPHS->UDPHS_IEN &= ~AT91C_UDPHS_DET_SUSPD; // remove suspend for TEST
    AT91C_BASE_UDPHS->UDPHS_TST |= AT91C_UDPHS_SPEED_CFG_HS; // force High Speed (remove suspend)

    switch( bIndex ) {

        case USBFeatureRequest_TESTPACKET:
            TRACE_DEBUG_WP("TEST_PACKET ");

            AT91C_BASE_UDPHS->UDPHS_DMA[1].UDPHS_DMACONTROL = 0;
            AT91C_BASE_UDPHS->UDPHS_DMA[2].UDPHS_DMACONTROL = 0;

            // Configure endpoint 2, 64 bytes, direction IN, type BULK, 1 bank
            AT91C_BASE_UDPHS->UDPHS_EPT[2].UDPHS_EPTCFG = AT91C_UDPHS_EPT_SIZE_64 | AT91C_UDPHS_EPT_DIR_IN | AT91C_UDPHS_EPT_TYPE_BUL_EPT | AT91C_UDPHS_BK_NUMBER_1;
            while( (signed int)(AT91C_BASE_UDPHS->UDPHS_EPT[2].UDPHS_EPTCFG & AT91C_UDPHS_EPT_MAPD) != (signed int)AT91C_UDPHS_EPT_MAPD ) {}

            AT91C_BASE_UDPHS->UDPHS_EPT[2].UDPHS_EPTCTLENB =  AT91C_UDPHS_EPT_ENABL;

            // Write FIFO
            pFifo = (char*)((unsigned int *)(AT91C_BASE_UDPHS_EPTFIFO->UDPHS_READEPT0) + (EPT_VIRTUAL_SIZE * 2));
            for( i=0; i<sizeof(test_packet_buffer); i++) {
                pFifo[i] = test_packet_buffer[i];
            }
            // Tst PACKET
            AT91C_BASE_UDPHS->UDPHS_TST |= AT91C_UDPHS_TST_PKT;
            // Send packet
            AT91C_BASE_UDPHS->UDPHS_EPT[2].UDPHS_EPTSETSTA = AT91C_UDPHS_TX_PK_RDY;
            break;

        case USBFeatureRequest_TESTJ:
            TRACE_DEBUG_WP("TEST_J ");
            AT91C_BASE_UDPHS->UDPHS_TST = AT91C_UDPHS_TST_J;
            break;

        case USBFeatureRequest_TESTK:
            TRACE_DEBUG_WP("TEST_K ");
            AT91C_BASE_UDPHS->UDPHS_TST = AT91C_UDPHS_TST_K;
            break;

        case USBFeatureRequest_TESTSE0NAK:
            TRACE_DEBUG_WP("TEST_SEO_NAK ");
            AT91C_BASE_UDPHS->UDPHS_IEN = 0;  // for test
            break;

        case USBFeatureRequest_TESTSENDZLP:
            //while( 0 != (AT91C_BASE_UDPHS->UDPHS_EPT[0].UDPHS_EPTSTA & AT91C_UDPHS_TX_PK_RDY ) ) {}
            AT91C_BASE_UDPHS->UDPHS_EPT[0].UDPHS_EPTSETSTA = AT91C_UDPHS_TX_PK_RDY;
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
    unsigned char i;

    TRACE_DEBUG_WP("USBD Init()\n\r");

    // Reset endpoint structures
    UDPHS_ResetEndpoints();

    // Enables the USB Clock
    UDPHS_EnableUsbClock();

    // Configure the pull-up on D+ and disconnect it
#if defined(CHIP_USB_PULLUP_INTERNAL)
    AT91C_BASE_UDPHS->UDPHS_CTRL |= AT91C_UDPHS_DETACH; // detach
    AT91C_BASE_UDPHS->UDPHS_CTRL |= AT91C_UDPHS_PULLD_DIS; // Disable Pull Down

#elif defined(CHIP_USB_PULLUP_INTERNAL_BY_MATRIX)
    TRACE_DEBUG_WP("PUON 0\n\r");
    AT91C_BASE_MATRIX->MATRIX_USBPCR &= ~AT91C_MATRIX_USBPCR_PUON;

#elif defined(CHIP_USB_PULLUP_EXTERNAL)
#ifdef PIN_USB_PULLUP
    const Pin pinPullUp = PIN_USB_PULLUP;
    PIO_Configure(&pinPullUp, 1);
    if (pinPullUp.attribute == PIO_OUTPUT_0) {

        PIO_Clear(&pinPullUp);
    }
    else {

        PIO_Set(&pinPullUp);
    }
#else
    #error unsupported now
#endif
#elif !defined(CHIP_USB_PULLUP_ALWAYSON)
    #error Unsupported pull-up type.

#endif

    // Reset and enable IP UDPHS
    AT91C_BASE_UDPHS->UDPHS_CTRL &= ~AT91C_UDPHS_EN_UDPHS;
    AT91C_BASE_UDPHS->UDPHS_CTRL |= AT91C_UDPHS_EN_UDPHS;
    // Enable and disable of the transceiver is automaticaly done by the IP.

    // With OR without DMA !!!
    // Initialization of DMA
    for( i=1; i<=((AT91C_BASE_UDPHS->UDPHS_IPFEATURES & AT91C_UDPHS_DMA_CHANNEL_NBR)>>4); i++ ) {

        // RESET endpoint canal DMA:
        // DMA stop channel command
        AT91C_BASE_UDPHS->UDPHS_DMA[i].UDPHS_DMACONTROL = 0;  // STOP command

        // Disable endpoint
        AT91C_BASE_UDPHS->UDPHS_EPT[i].UDPHS_EPTCTLDIS = AT91C_UDPHS_SHRT_PCKT
                                                       | AT91C_UDPHS_BUSY_BANK
                                                       | AT91C_UDPHS_NAK_OUT
                                                       | AT91C_UDPHS_NAK_IN
                                                       | AT91C_UDPHS_STALL_SNT
                                                       | AT91C_UDPHS_RX_SETUP
                                                       | AT91C_UDPHS_TX_PK_RDY
                                                       | AT91C_UDPHS_TX_COMPLT
                                                       | AT91C_UDPHS_RX_BK_RDY
                                                       | AT91C_UDPHS_ERR_OVFLW
                                                       | AT91C_UDPHS_MDATA_RX
                                                       | AT91C_UDPHS_DATAX_RX
                                                       | AT91C_UDPHS_NYET_DIS
                                                       | AT91C_UDPHS_INTDIS_DMA
                                                       | AT91C_UDPHS_AUTO_VALID
                                                       | AT91C_UDPHS_EPT_DISABL;

        // Clear status endpoint
        AT91C_BASE_UDPHS->UDPHS_EPT[i].UDPHS_EPTCLRSTA = AT91C_UDPHS_TOGGLESQ
                                                       | AT91C_UDPHS_FRCESTALL
                                                       | AT91C_UDPHS_RX_BK_RDY
                                                       | AT91C_UDPHS_TX_COMPLT
                                                       | AT91C_UDPHS_RX_SETUP
                                                       | AT91C_UDPHS_STALL_SNT
                                                       | AT91C_UDPHS_NAK_IN
                                                       | AT91C_UDPHS_NAK_OUT;

        // Reset endpoint config
        AT91C_BASE_UDPHS->UDPHS_EPT[i].UDPHS_EPTCTLENB = 0;

        // Reset DMA channel (Buff count and Control field)
        AT91C_BASE_UDPHS->UDPHS_DMA[i].UDPHS_DMACONTROL = AT91C_UDPHS_LDNXT_DSC;  // NON STOP command

        // Reset DMA channel 0 (STOP)
        AT91C_BASE_UDPHS->UDPHS_DMA[i].UDPHS_DMACONTROL = 0;  // STOP command

        // Clear DMA channel status (read the register for clear it)
        AT91C_BASE_UDPHS->UDPHS_DMA[i].UDPHS_DMASTATUS = AT91C_BASE_UDPHS->UDPHS_DMA[i].UDPHS_DMASTATUS;

    }

    AT91C_BASE_UDPHS->UDPHS_TST = forceUsbFS ? AT91C_UDPHS_SPEED_CFG_FS : 0;
    AT91C_BASE_UDPHS->UDPHS_IEN = 0;
    AT91C_BASE_UDPHS->UDPHS_CLRINT = AT91C_UDPHS_UPSTR_RES
                                   | AT91C_UDPHS_ENDOFRSM
                                   | AT91C_UDPHS_WAKE_UP
                                   | AT91C_UDPHS_ENDRESET
                                   | AT91C_UDPHS_IEN_SOF
                                   | AT91C_UDPHS_MICRO_SOF
                                   | AT91C_UDPHS_DET_SUSPD;
    
    // Device is in the Attached state
    deviceState = USBD_STATE_SUSPENDED;
    previousDeviceState = USBD_STATE_POWERED;

    // Enable interrupts
    AT91C_BASE_UDPHS->UDPHS_IEN = AT91C_UDPHS_ENDOFRSM
                                | AT91C_UDPHS_WAKE_UP
                                | AT91C_UDPHS_DET_SUSPD;

    // Disable USB clocks
    UDPHS_DisableUsbClock();

    // Configure interrupts
    USBDCallbacks_Initialized();
}

//------------------------------------------------------------------------------
/// Configure USB Speed, should be invoked before USB attachment.
/// \param forceFS Force to use FS mode.
//------------------------------------------------------------------------------
void USBD_ConfigureSpeed(unsigned char forceFS)
{
    if (forceFS) {
        AT91C_BASE_UDPHS->UDPHS_TST |=  AT91C_UDPHS_SPEED_CFG_FS;
    }
    else {
        AT91C_BASE_UDPHS->UDPHS_TST &= ~AT91C_UDPHS_SPEED_CFG_FS;
    }
}


//------------------------------------------------------------------------------
/// Returns the current state of the USB device.
/// \return Device current state.
//------------------------------------------------------------------------------
unsigned char USBD_GetState( void )
{
    return deviceState;
}


