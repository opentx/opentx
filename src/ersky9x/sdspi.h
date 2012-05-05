/* ----------------------------------------------------------------------------
 *         ATMEL Microcontroller Software Support
 * ----------------------------------------------------------------------------
 * Copyright (c) 2008, Atmel Corporation

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
/// \page "sdspi"
///
/// !Purpose
///  
/// sdcard spi-bus driver
///
/// !Usage
///
/// -# SDSPI_Configure: Initializes the SD Spi structure and the corresponding SPI hardware
/// -# SDSPI_ConfigureCS : Configures the parameters for the device corresponding to the cs
/// -# SDSPI_Read: Read data on SPI data bus
/// -# SDSPI_Write : Write data on SPI data bus
/// -# SDSPI_SendCommand : Starts a SPI master transfer
/// -# SDSPI_StopTranToken : Send stop transfer data token
//------------------------------------------------------------------------------

#ifndef SDSPI_H
#define SDSPI_H

//------------------------------------------------------------------------------
//         Headers
//------------------------------------------------------------------------------

#ifdef REVA
#include "AT91SAM3S2.h"
#else
#include "AT91SAM3S4.h"
#endif

//#include <board.h>
//#include <spi/spi.h>

/// SPI CSR value
#define SDSPI_CSR(scbr) ( AT91C_SPI_CPOL  \
                        | AT91C_SPI_BITS_8 \
                        | (((scbr)<<  8) & AT91C_SPI_SCBR) \
                        | ( (0x08 << 16) & AT91C_SPI_DLYBS)\
                        | ( (0x01 << 24) & AT91C_SPI_DLYBCT) )

/// Calculates the value of the SCBR field of the Chip Select Register given
/// MCK and SPCK.
#define SPID_CSR_SCBR(mck, spck)    ((((mck) / (spck)) << 8) & AT91C_SPI_SCBR)

/// Calculates the value of the DLYBS field of the Chip Select Register given
/// the delay in ns and MCK.
#define SPID_CSR_DLYBS(mck, delay) \
    ((((((delay) * ((mck) / 1000000)) / 1000) + 1)  << 16) & AT91C_SPI_DLYBS)

/// Calculates the value of the DLYBCT field of the Chip Select Register given
/// the delay in ns and MCK.
#define SPID_CSR_DLYBCT(mck, delay) \
    ((((((delay) / 32 * ((mck) / 1000000)) / 1000) + 1) << 24) & AT91C_SPI_DLYBCT)

#define AT45_CSR(mck, spck) \
    (AT91C_SPI_NCPHA | SPID_CSR_DLYBCT(mck, 250) \
     | SPID_CSR_DLYBS(mck, 250) | SPID_CSR_SCBR(mck, spck))


/// Start new data transfer
#define SPI_NEW_TRANSFER        0
/// Continue data transfer
#define SPI_CONTINUE_TRANSFER   1

/// SD end-of-transfer callback function.
typedef void (*SdSpiCallback)(unsigned char status, void *pCommand);

//------------------------------------------------------------------------------
/// SPI Transfer Request prepared by the application upper layer. This structure
/// is sent to the SDSPI_SendCommand function to start the transfer. At the end of
/// the transfer, the callback is invoked by the interrupt handler.
//------------------------------------------------------------------------------
typedef struct _SdSpiCmd {

    /// Command status.
    volatile char status;
    /// Command code.
    unsigned int cmd;
    /// Command argument.
    unsigned int arg;
    /// Data buffer.
    unsigned char *pData;
    /// Size of data buffer in bytes.
    unsigned short blockSize;
    /// Number of blocks to be transfered
    unsigned short nbBlock;
    /// Indicate if continue to transfer data
    unsigned char conTrans;
    /// Indicates if the command is a read operation.
    unsigned char isRead;
    /// Response buffer.
    unsigned int  *pResp;
    /// Size of SD card response in bytes.
    unsigned char  resType;
    /// Optional user-provided callback function.
    SdSpiCallback callback;
    /// Optional argument to the callback function.
    void *pArg;

} SdSpiCmd;

/// Above should be put into SD card header file.

//------------------------------------------------------------------------------
/// SPI driver structure. Holds the internal state of the SPI driver and
/// prevents parallel access to a SPI peripheral.
//------------------------------------------------------------------------------
typedef struct {

    /// Pointer to a SPI peripheral.
    AT91S_SPI *pSpiHw;
    /// SPI peripheral identifier.
    unsigned char spiId;
    /// Pointer to currently executing command.
    SdSpiCmd *pCommand;
    /// Mutex.
    volatile char semaphore;

} SdSpi;

extern void SDSPI_Configure(SdSpi *pSdSpi,AT91PS_SPI pSpiHw,unsigned char spiId);

extern void SDSPI_SetSpeed(SdSpi *pSdSpi, unsigned int spiSpeed);

extern unsigned char SDSPI_SendCommand(SdSpi *pSdSpi, SdSpiCmd *pSdSpiCmd);

extern void SDSPI_Handler(SdSpi *pSdSpi);

extern unsigned char SDSPI_IsTxComplete(SdSpiCmd *pSdSpiCmd);

extern unsigned char SDSPI_IsBusy(SdSpi *pSdSpi);

extern unsigned char SDSPI_NCS(SdSpi *pSdSpi);

extern void SDSPI_Close(SdSpi *pSdSpi);

extern void SDSPI_ConfigureCS(SdSpi *pSdSpi, unsigned char cs, unsigned int csr);

extern unsigned char  SDSPI_StopTranToken(SdSpi *pSdSpi);

extern unsigned char SDSPI_Wait(SdSpi *pSdSpi, unsigned int cycles);

extern unsigned char SDSPI_WaitDataBusy(SdSpi *pSdSpi);

#endif

