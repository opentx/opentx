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

#include "sdspi.h"
//#include <utility/assert.h>
//#include <utility/trace.h>
//#include <board.h>
//#include <crc7.h>
//#include <crc-itu-t.h>
//#include <crc16.h>
//#include <crc-ccitt.h>
#include <string.h>

//------------------------------------------------------------------------------
//         Macros
//------------------------------------------------------------------------------

/// Transfer is pending.
#define SDSPI_STATUS_PENDING      1
/// Transfer has been aborted because an error occured.
#define SDSPI_STATUS_ERROR        2

/// SPI driver is currently in use.
#define SDSPI_ERROR_LOCK    1

// Data Tokens
#define SDSPI_START_BLOCK_1 0xFE  // Single/Multiple read, single write
#define SDSPI_START_BLOCK_2 0xFC  // Multiple block write
#define SDSPI_STOP_TRAN     0xFD  // Cmd12

//------------------------------------------------------------------------------
//         Exported functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// Initializes the SD Spi structure and the corresponding SPI hardware.
/// \param pSpid  Pointer to a Spid instance.
/// \param pSpiHw  Associated SPI peripheral.
/// \param spiId  SPI peripheral identifier.
//------------------------------------------------------------------------------
void SDSPI_Configure(SdSpi *pSdSpi,
                     AT91PS_SPI pSpiHw,
                     unsigned char spiId)
{
    // Initialize the SPI structure
    pSdSpi->pSpiHw = pSpiHw;
    pSdSpi->spiId = spiId;
    pSdSpi->semaphore = 1;

    // Enable the SPI clock
    // BSS replaced by below AT91C_BASE_PMC->PMC_PCER = (1 << pSdSpi->spiId);
    PMC->PMC_PCER0 = (1 << pSdSpi->spiId);
    
    // Execute a software reset of the SPI twice
    pSpiHw->SPI_CR = SPI_CR_SWRST; // BSS AT91C_SPI_SWRST;
    pSpiHw->SPI_CR = SPI_CR_SWRST; // BSS AT91C_SPI_SWRST;

    // Configure SPI in Master Mode with No CS selected !!!
    pSpiHw->SPI_MR = SPI_MR_MSTR | SPI_MR_MODFDIS | SPI_MR_PCS; // BSS AT91C_SPI_MSTR | AT91C_SPI_MODFDIS  | AT91C_SPI_PCS;

    // Disables the receiver PDC transfer requests
    // Disables the transmitter PDC transfer requests.
    pSpiHw->SPI_PTCR = SPI_PTCR_RXTDIS | SPI_PTCR_TXTDIS; // BSS AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS;

    // Enable the SPI
    pSpiHw->SPI_CR = SPI_CR_SPIEN; // BSS AT91C_SPI_SPIEN;

    // Disable the SPI clock
    // BSS replaced by below AT91C_BASE_PMC->PMC_PCDR = (1 << pSdSpi->spiId);
    PMC->PMC_PCDR0 = (1 << pSdSpi->spiId);
}

//------------------------------------------------------------------------------
/// Configures the parameters for the device corresponding to the cs.
/// \param pSdSpi  Pointer to a SdSpi instance.
/// \param cs  number corresponding to the SPI chip select.
/// \param csr  SPI_CSR value to setup.
//------------------------------------------------------------------------------
void SDSPI_ConfigureCS(SdSpi *pSdSpi, unsigned char cs, unsigned int csr)
{
    unsigned int spiMr;
    AT91S_SPI *pSpiHw = pSdSpi->pSpiHw;

    // Enable the SPI clock
    // BSS below AT91C_BASE_PMC->PMC_PCER = (1 << pSdSpi->spiId);
    PMC->PMC_PCER0 = (1 << pSdSpi->spiId);

    //TRACE_DEBUG("CSR[%d]=0x%8X\n\r", cs, csr);
    pSpiHw->SPI_CSR[cs] = csr;

//jcb to put in sendcommand
    // Write to the MR register
    spiMr = pSpiHw->SPI_MR;
    spiMr |= SPI_MR_PCS; // TODO AT91C_SPI_PCS;
    spiMr &= ~((1 << cs) << 16);
    pSpiHw->SPI_MR = spiMr;

    // Disable the SPI clock
    // BSS below AT91C_BASE_PMC->PMC_PCDR = (1 << pSdSpi->spiId);
    PMC->PMC_PCDR0 = (1 << pSdSpi->spiId);
}

//------------------------------------------------------------------------------
/// Use PDC for SPI data transfer.
/// Return 0 if no error, otherwise return error status.
/// \param pSdSpi  Pointer to a SdSpi instance.
/// \param pData  Data pointer.
/// \param size  Data transfer byte count.
//------------------------------------------------------------------------------
unsigned char SDSPI_PDC(SdSpi *pSdSpi, unsigned char *pData, unsigned int size)
{
    AT91PS_SPI pSpiHw = pSdSpi->pSpiHw;
    unsigned int spiIer;

    if (pSdSpi->semaphore == 0) {
        TRACE_DEBUG("No semaphore\n\r");
        return SDSPI_ERROR_LOCK;
    }
    pSdSpi->semaphore--;

    // Enable the SPI clock
    // BSS below AT91C_BASE_PMC->PMC_PCER = (1 << pSdSpi->spiId);
    PMC->PMC_PCER0 = (1 << pSdSpi->spiId);

    // Disable transmitter and receiver
    pSpiHw->SPI_PTCR = SPI_PTCR_RXTDIS | SPI_PTCR_TXTDIS; // BSS AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS;

    // Receive Pointer Register
    pSpiHw->SPI_RPR = (int)pData;
    // Receive Counter Register
    pSpiHw->SPI_RCR = size;
    // Transmit Pointer Register
    pSpiHw->SPI_TPR = (int) pData;
    // Transmit Counter Register
    pSpiHw->SPI_TCR = size;

    spiIer = SPI_IER_RXBUFF; // TODO AT91C_SPI_RXBUFF;

    // Enable transmitter and receiver
    pSpiHw->SPI_PTCR = SPI_PTCR_RXTEN | SPI_PTCR_TXTEN; // BSS AT91C_PDC_RXTEN | AT91C_PDC_TXTEN;

    // Interrupt enable shall be done after PDC TXTEN and RXTEN
    pSpiHw->SPI_IER = spiIer;

    return 0;
}

//! Should be moved to a new file
//------------------------------------------------------------------------------
/// Read data on SPI data bus; 
/// Returns 1 if read fails, returns 0 if no error.
/// \param pSdSpi  Pointer to a SD SPI driver instance.
/// \param pData  Data pointer.
/// \param size Data size.
//------------------------------------------------------------------------------
unsigned char SDSPI_Read(SdSpi *pSdSpi, unsigned char *pData, unsigned int size)
{
    unsigned char error;

    // MOSI should hold high during read, or there will be wrong data in received data.
    memset(pData, 0xff, size);

    error = SDSPI_PDC(pSdSpi, pData, size);

    while(SDSPI_IsBusy(pSdSpi) == 1);

    if( error == 0 ) {
        return 0;
    }
    else {
        TRACE_DEBUG("PB SDSPI_Read\n\r");
        return 1;
    }
}

//------------------------------------------------------------------------------
/// Write data on SPI data bus; 
/// Returns 1 if write fails, returns 0 if no error.
/// \param pSdSpi  Pointer to a SD SPI driver instance.
/// \param pData  Data pointer.
/// \param size Data size.
//------------------------------------------------------------------------------
unsigned char SDSPI_Write(SdSpi *pSdSpi, unsigned char *pData, unsigned int size)
{
    unsigned char error;

    error = SDSPI_PDC(pSdSpi, pData, size);

    while(SDSPI_IsBusy(pSdSpi) == 1);

    if( error == 0 ) {
        return 0;
    }
    else {
        TRACE_DEBUG("PB SDSPI_Write\n\r");
        return 1;
    }
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
unsigned char SDSPI_WaitDataBusy(SdSpi *pSdSpi)
{
    unsigned char busyData;

    SDSPI_Read(pSdSpi, &busyData, 1);

    if (busyData != 0xff) {
        return 1;
    }
    else {
        return 0;
    }
}

/* Table for CRC-7 (polynomial x^7 + x^3 + 1) */
const uint8_t crc7_syndrome_table[256] = {
        0x00, 0x09, 0x12, 0x1b, 0x24, 0x2d, 0x36, 0x3f,
        0x48, 0x41, 0x5a, 0x53, 0x6c, 0x65, 0x7e, 0x77,
        0x19, 0x10, 0x0b, 0x02, 0x3d, 0x34, 0x2f, 0x26,
        0x51, 0x58, 0x43, 0x4a, 0x75, 0x7c, 0x67, 0x6e,
        0x32, 0x3b, 0x20, 0x29, 0x16, 0x1f, 0x04, 0x0d,
        0x7a, 0x73, 0x68, 0x61, 0x5e, 0x57, 0x4c, 0x45,
        0x2b, 0x22, 0x39, 0x30, 0x0f, 0x06, 0x1d, 0x14,
        0x63, 0x6a, 0x71, 0x78, 0x47, 0x4e, 0x55, 0x5c,
        0x64, 0x6d, 0x76, 0x7f, 0x40, 0x49, 0x52, 0x5b,
        0x2c, 0x25, 0x3e, 0x37, 0x08, 0x01, 0x1a, 0x13,
        0x7d, 0x74, 0x6f, 0x66, 0x59, 0x50, 0x4b, 0x42,
        0x35, 0x3c, 0x27, 0x2e, 0x11, 0x18, 0x03, 0x0a,
        0x56, 0x5f, 0x44, 0x4d, 0x72, 0x7b, 0x60, 0x69,
        0x1e, 0x17, 0x0c, 0x05, 0x3a, 0x33, 0x28, 0x21,
        0x4f, 0x46, 0x5d, 0x54, 0x6b, 0x62, 0x79, 0x70,
        0x07, 0x0e, 0x15, 0x1c, 0x23, 0x2a, 0x31, 0x38,
        0x41, 0x48, 0x53, 0x5a, 0x65, 0x6c, 0x77, 0x7e,
        0x09, 0x00, 0x1b, 0x12, 0x2d, 0x24, 0x3f, 0x36,
        0x58, 0x51, 0x4a, 0x43, 0x7c, 0x75, 0x6e, 0x67,
        0x10, 0x19, 0x02, 0x0b, 0x34, 0x3d, 0x26, 0x2f,
        0x73, 0x7a, 0x61, 0x68, 0x57, 0x5e, 0x45, 0x4c,
        0x3b, 0x32, 0x29, 0x20, 0x1f, 0x16, 0x0d, 0x04,
        0x6a, 0x63, 0x78, 0x71, 0x4e, 0x47, 0x5c, 0x55,
        0x22, 0x2b, 0x30, 0x39, 0x06, 0x0f, 0x14, 0x1d,
        0x25, 0x2c, 0x37, 0x3e, 0x01, 0x08, 0x13, 0x1a,
        0x6d, 0x64, 0x7f, 0x76, 0x49, 0x40, 0x5b, 0x52,
        0x3c, 0x35, 0x2e, 0x27, 0x18, 0x11, 0x0a, 0x03,
        0x74, 0x7d, 0x66, 0x6f, 0x50, 0x59, 0x42, 0x4b,
        0x17, 0x1e, 0x05, 0x0c, 0x33, 0x3a, 0x21, 0x28,
        0x5f, 0x56, 0x4d, 0x44, 0x7b, 0x72, 0x69, 0x60,
        0x0e, 0x07, 0x1c, 0x15, 0x2a, 0x23, 0x38, 0x31,
        0x46, 0x4f, 0x54, 0x5d, 0x62, 0x6b, 0x70, 0x79
};

// BSS crc functions added
static inline uint8_t crc7_byte(uint8_t crc, uint8_t data)
{
  return crc7_syndrome_table[(crc << 1) ^ data];
}

uint8_t crc7(uint8_t crc, const uint8_t *buffer, size_t len)
{
        while (len--)
                crc = crc7_byte(crc, *buffer++);
        return crc;
}

//------------------------------------------------------------------------------
/// Convert SD MCI command to a SPI mode command token.
/// \param pCmdToken Pointer to the SD command token.
/// \param arg    SD command argument
//------------------------------------------------------------------------------
void SDSPI_MakeCmd(unsigned char *pCmdToken, unsigned int arg)
{
    unsigned char sdCmdNum;
    unsigned char crc = 0;
    unsigned char crcPrev = 0;

    sdCmdNum = 0x3f & *pCmdToken;
    *pCmdToken = sdCmdNum | 0x40;
    *(pCmdToken+1) = (arg >> 24) & 0xff;
    *(pCmdToken+2) = (arg >> 16) & 0xff;
    *(pCmdToken+3) = (arg >> 8) & 0xff;
    *(pCmdToken+4) = arg & 0xff;

    crc = crc7(crcPrev, (unsigned char *)(pCmdToken), 5);

    *(pCmdToken+5) = (crc << 1) | 1;
}

//------------------------------------------------------------------------------
/// Get response after send SD command.
/// Return 0 if no error, otherwise indicate an error.
/// \param pSdSpi Pointer to the SD SPI instance.
/// \param pCommand  Pointer to the SD command
//------------------------------------------------------------------------------
unsigned char SDSPI_GetCmdResp(SdSpi *pSdSpi, SdSpiCmd *pCommand)
{
    unsigned char resp[8];  // response 
    unsigned char error;
    unsigned int  respRetry = 8; //NCR max 8, refer to card datasheet

    memset(resp, 0, 8);

    // Wait for response start bit. 
    do {
        error = SDSPI_Read(pSdSpi, &resp[0], 1);
        if (error) {
            TRACE_DEBUG("\n\rpb SDSPI_GetCmdResp: 0x%X\n\r", error);
            return error;
        }
        if ((resp[0]&0x80) == 0) {
            break;
        }
        respRetry--;
    } while(respRetry > 0);

    switch (pCommand->resType) {
        case 1:
        *(pCommand->pResp) = resp[0];
        break;

        case 2:
        error = SDSPI_Read(pSdSpi, &resp[1], 1);
        if (error) {
            return error;
        }
        *(pCommand->pResp) = resp[0]
                          | (resp[1] << 8);
        break;

        // Response 3, get OCR
        case 3:
        error = SDSPI_Read(pSdSpi, &resp[1], 4);
        if (error) {
            return error;
        }
        *(pCommand->pResp) = resp[0] 
                          | (resp[1] << 8) 
                          | (resp[2] << 16)
                          | (resp[3] << 24);
        *(pCommand->pResp+1) = resp[4];
        break;

        case 7:
        TRACE_DEBUG("case 7\n\r");
        error = SDSPI_Read(pSdSpi, &resp[1], 4);
        if (error) {
            return error;
        }
        *(pCommand->pResp) = resp[0]
                          | (resp[1] << 8) 
                          | (resp[2] << 16)
                          | (resp[3] << 24);
        *(pCommand->pResp+1) = resp[4];
        break;

        default:
        TRACE_DEBUG("PB default\n\r");
        break;
    }

    return 0;
}

//------------------------------------------------------------------------------
/// Get response after send data.
/// Return 0 if no error, otherwise indicate an error.
/// \param pSdSpi Pointer to the SD SPI instance.
/// \param pCommand  Pointer to the SD command
//------------------------------------------------------------------------------
unsigned char SDSPI_GetDataResp(SdSpi *pSdSpi, SdSpiCmd *pCommand)
{
    unsigned char resp = 0;  // response 
    unsigned char error;
    unsigned int respRetry = 18; //NCR max 8, refer to card datasheet

    // Wait for response start bit. 
    do {
        error = SDSPI_Read(pSdSpi, &resp, 1);
        if (error) {
            return error;
        }
        if (((resp & 0x11) == 0x1) || ((resp & 0xf0) == 0))
            break;

        respRetry--;
    } while(respRetry > 0);
    //TRACE_DEBUG("SDSPI_GetDataResp 0x%X\n\r",resp);
    return resp;
}

// BSS crc16 function added

const uint16_t crc_itu_t_table[256] = {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
        0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef,
        0x1231, 0x0210, 0x3273, 0x2252, 0x52b5, 0x4294, 0x72f7, 0x62d6,
        0x9339, 0x8318, 0xb37b, 0xa35a, 0xd3bd, 0xc39c, 0xf3ff, 0xe3de,
        0x2462, 0x3443, 0x0420, 0x1401, 0x64e6, 0x74c7, 0x44a4, 0x5485,
        0xa56a, 0xb54b, 0x8528, 0x9509, 0xe5ee, 0xf5cf, 0xc5ac, 0xd58d,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76d7, 0x66f6, 0x5695, 0x46b4,
        0xb75b, 0xa77a, 0x9719, 0x8738, 0xf7df, 0xe7fe, 0xd79d, 0xc7bc,
        0x48c4, 0x58e5, 0x6886, 0x78a7, 0x0840, 0x1861, 0x2802, 0x3823,
        0xc9cc, 0xd9ed, 0xe98e, 0xf9af, 0x8948, 0x9969, 0xa90a, 0xb92b,
        0x5af5, 0x4ad4, 0x7ab7, 0x6a96, 0x1a71, 0x0a50, 0x3a33, 0x2a12,
        0xdbfd, 0xcbdc, 0xfbbf, 0xeb9e, 0x9b79, 0x8b58, 0xbb3b, 0xab1a,
        0x6ca6, 0x7c87, 0x4ce4, 0x5cc5, 0x2c22, 0x3c03, 0x0c60, 0x1c41,
        0xedae, 0xfd8f, 0xcdec, 0xddcd, 0xad2a, 0xbd0b, 0x8d68, 0x9d49,
        0x7e97, 0x6eb6, 0x5ed5, 0x4ef4, 0x3e13, 0x2e32, 0x1e51, 0x0e70,
        0xff9f, 0xefbe, 0xdfdd, 0xcffc, 0xbf1b, 0xaf3a, 0x9f59, 0x8f78,
        0x9188, 0x81a9, 0xb1ca, 0xa1eb, 0xd10c, 0xc12d, 0xf14e, 0xe16f,
        0x1080, 0x00a1, 0x30c2, 0x20e3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83b9, 0x9398, 0xa3fb, 0xb3da, 0xc33d, 0xd31c, 0xe37f, 0xf35e,
        0x02b1, 0x1290, 0x22f3, 0x32d2, 0x4235, 0x5214, 0x6277, 0x7256,
        0xb5ea, 0xa5cb, 0x95a8, 0x8589, 0xf56e, 0xe54f, 0xd52c, 0xc50d,
        0x34e2, 0x24c3, 0x14a0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
        0xa7db, 0xb7fa, 0x8799, 0x97b8, 0xe75f, 0xf77e, 0xc71d, 0xd73c,
        0x26d3, 0x36f2, 0x0691, 0x16b0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xd94c, 0xc96d, 0xf90e, 0xe92f, 0x99c8, 0x89e9, 0xb98a, 0xa9ab,
        0x5844, 0x4865, 0x7806, 0x6827, 0x18c0, 0x08e1, 0x3882, 0x28a3,
        0xcb7d, 0xdb5c, 0xeb3f, 0xfb1e, 0x8bf9, 0x9bd8, 0xabbb, 0xbb9a,
        0x4a75, 0x5a54, 0x6a37, 0x7a16, 0x0af1, 0x1ad0, 0x2ab3, 0x3a92,
        0xfd2e, 0xed0f, 0xdd6c, 0xcd4d, 0xbdaa, 0xad8b, 0x9de8, 0x8dc9,
        0x7c26, 0x6c07, 0x5c64, 0x4c45, 0x3ca2, 0x2c83, 0x1ce0, 0x0cc1,
        0xef1f, 0xff3e, 0xcf5d, 0xdf7c, 0xaf9b, 0xbfba, 0x8fd9, 0x9ff8,
        0x6e17, 0x7e36, 0x4e55, 0x5e74, 0x2e93, 0x3eb2, 0x0ed1, 0x1ef0
};

static inline uint16_t crc_itu_t_byte(uint16_t crc, const uint8_t data)
{
  return (crc << 8) ^ crc_itu_t_table[((crc >> 8) ^ data) & 0xff];
}

uint16_t crc_itu_t(uint16_t crc, const uint8_t *buffer, size_t len)
{
        while (len--)
                crc = crc_itu_t_byte(crc, *buffer++);
        return crc;
}

//------------------------------------------------------------------------------
/// Starts a SPI master transfer. This is a non blocking function. It will
/// return as soon as the transfer is started.
/// Returns 0 if the transfer has been started successfully; otherwise returns
/// error.
/// \param pSdSpi  Pointer to a SdSpi instance.
/// \param pCommand Pointer to the SPI command to execute.
//------------------------------------------------------------------------------
unsigned char SDSPI_SendCommand(SdSpi *pSdSpi, SdSpiCmd *pCommand)
{
    // BSS AT91S_SPI *pSpiHw = pSdSpi->pSpiHw;
    unsigned char CmdToken[6];
    unsigned char *pData;
    unsigned int blockSize;
    unsigned int i;
    unsigned char error;
    unsigned char dataHeader;
    unsigned int dataRetry1 = 100;
    unsigned int dataRetry2 = 100;
    unsigned char crc[2];
    unsigned char crcPrev = 0;
    unsigned char crcPrev2 = 0;

    SANITY_CHECK(pSdSpi);
    // BSS SANITY_CHECK(pSpiHw);
    SANITY_CHECK(pCommand);

    CmdToken[0] = pCommand->cmd & 0x3F;
    pData = pCommand->pData;
    blockSize = pCommand->blockSize;

    SDSPI_MakeCmd((unsigned char *)&CmdToken, pCommand->arg);

    // Command is now being executed
    pSdSpi->pCommand = pCommand;
    pCommand->status = SDSPI_STATUS_PENDING;

    // Send the command
    if((pCommand->conTrans == SPI_NEW_TRANSFER) || (blockSize == 0)) {

        for(i = 0; i < 6; i++) {
            error = SDSPI_Write(pSdSpi, &CmdToken[i], 1);
            if (error) {
                TRACE_DEBUG("Error: %d\n\r", error);
                return error;
            }
        }
        // Specific for Cmd12()
        if ((pCommand->cmd & 0x3F) == 12) {
            if( 1 == SDSPI_Wait(pSdSpi, 2) ) {
                TRACE_DEBUG("Pb Send command 12\n\r");
            }
        }
        if (pCommand->pResp) {
            error = SDSPI_GetCmdResp(pSdSpi, pCommand);
            if (error) {
                TRACE_DEBUG("Error: %d\n\r", error);
                return error;
            }
        }
    }

    if( (blockSize > 0) && (pCommand->nbBlock == 0) ) {
        pCommand->nbBlock = 1;
    }

    // For data block operations
    while (pCommand->nbBlock > 0) {

        // If data block size is invalid, return error
        if (blockSize == 0) {
            TRACE_DEBUG("Block Size = 0\n\r");
            return 1;
        }

        // DATA transfer from card to host
        if (pCommand->isRead) {
            do {
                SDSPI_Read(pSdSpi, &dataHeader, 1);
                dataRetry1 --;
                if (dataHeader == SDSPI_START_BLOCK_1) {
                    break;
                }
                else if((dataHeader & 0xf0) == 0x00) {
                    pCommand->status = SDSPI_STATUS_ERROR;
                    TRACE_DEBUG("Data Error 0x%X!\n\r", dataHeader);
                    return 1;
                }
            } while(dataRetry1 > 0);

            if (dataRetry1 == 0) {
                TRACE_DEBUG("Timeout dataretry1\n\r");
                return 1;
            }

            SDSPI_Read(pSdSpi, pData, blockSize);

            // Specific for Cmd9()
            if ((pCommand->cmd & 0x3f) != 0x9) {

                SDSPI_Read(pSdSpi, crc, 2);
#ifdef SDSPI_CRC_ON
                // Check data CRC
                TRACE_DEBUG("Check Data CRC\n\r");
                crcPrev = 0;
                crcPrev2 = 0;
                if (crc[0] != ((crc_itu_t(crcPrev, pData, blockSize) & 0xff00) >> 8 )
                 || crc[1] !=  (crc_itu_t(crcPrev2, pData, blockSize) & 0xff)) {
                    TRACE_ERROR("CRC error 0x%X 0x%X 0x%X\n\r", \
                        crc[0], crc[1], crc_itu_t(pData, blockSize));
                    return 1;
                }
#endif
            }
        }

        // DATA transfer from host to card
        else {
            SDSPI_NCS(pSdSpi);
            if ((pCommand->conTrans == SPI_CONTINUE_TRANSFER) || ((pCommand->cmd & 0x3f) == 25)) {
                dataHeader = SDSPI_START_BLOCK_2;
            }
            else {
                dataHeader = SDSPI_START_BLOCK_1;
            }

            crcPrev = 0;
            crc[0] = (crc_itu_t(crcPrev, pData, blockSize) & 0xff00) >> 8;
            crcPrev2 = 0;
            crc[1] = (crc_itu_t(crcPrev2, pData, blockSize) & 0xff);
            SDSPI_Write(pSdSpi, &dataHeader, 1);
            SDSPI_Write(pSdSpi, pData, blockSize);
            SDSPI_Write(pSdSpi, crc, 2);

            // If status bits in data response is not "data accepted", return error
            if ((SDSPI_GetDataResp(pSdSpi, pCommand) & 0xe) != 0x4) {
                TRACE_ERROR("Write resp error!\n\r");
                return 1;
            }

            do {
                if (SDSPI_WaitDataBusy(pSdSpi) == 0) {
                    break;
                }
                dataRetry2--;
            } while(dataRetry2 > 0);
        }
        pData += blockSize;
        pCommand->nbBlock--;
    }

    if (pCommand->status == SDSPI_STATUS_PENDING) {
        pCommand->status = 0;
    }

    //TRACE_DEBUG("end SDSPI_SendCommand\n\r");
    return 0;
}
//!

//------------------------------------------------------------------------------
/// The SPI_Handler must be called by the SPI Interrupt Service Routine with the
/// corresponding Spi instance.
/// The SPI_Handler will unlock the Spi semaphore and invoke the upper application 
/// callback.
/// \param pSdSpi  Pointer to a SdSpi instance.
//------------------------------------------------------------------------------
void SDSPI_Handler(SdSpi *pSdSpi)
{
    SdSpiCmd *pCommand = pSdSpi->pCommand;
    AT91S_SPI *pSpiHw = pSdSpi->pSpiHw;
    volatile unsigned int spiSr;

    // Read the status register
    spiSr = pSpiHw->SPI_SR;
    if(spiSr & SPI_SR_RXBUFF/*BSS AT91C_SPI_RXBUFF*/) {

        if (pCommand->status == SDSPI_STATUS_PENDING) {
            pCommand->status = 0;
        }
        // Disable transmitter and receiver
        pSpiHw->SPI_PTCR = SPI_PTCR_RXTDIS | SPI_PTCR_TXTDIS; // BSS AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS;

        // Disable the SPI clock
        // BSS below AT91C_BASE_PMC->PMC_PCDR = (1 << pSdSpi->spiId);
        PMC->PMC_PCDR0 = (1 << pSdSpi->spiId);

        // Disable buffer complete interrupt
        pSpiHw->SPI_IDR = SPI_IDR_RXBUFF | SPI_IDR_ENDTX; // BSS AT91C_SPI_RXBUFF | AT91C_SPI_ENDTX;

        // Release the SPI semaphore
        pSdSpi->semaphore++;
    }

    // Invoke the callback associated with the current command
    if (pCommand && pCommand->callback) {
        pCommand->callback(0, pCommand);
    }
}

//------------------------------------------------------------------------------
/// Returns 1 if the given SPI transfer is complete; otherwise returns 0.
/// \param pCommand  Pointer to a SdSpiCmd instance.
//------------------------------------------------------------------------------
unsigned char SDSPI_IsTxComplete(SdSpiCmd *pCommand)
{
    if (pCommand->status != SDSPI_STATUS_PENDING) {
        if (pCommand->status != 0){
            TRACE_DEBUG("SPI_IsTxComplete %d\n\r", pCommand->status);
        }
        return 1;
    }
    else {
        return 0;
    }
}

//------------------------------------------------------------------------------
/// Close a SPI driver instance and the underlying peripheral.
/// \param pSdSpi  Pointer to a SD SPI driver instance.
//------------------------------------------------------------------------------
void SDSPI_Close(SdSpi *pSdSpi)
{
    AT91S_SPI *pSpiHw = pSdSpi->pSpiHw;

    SANITY_CHECK(pSdSpi);
    SANITY_CHECK(pSpiHw);

    // Enable the SPI clock
    // BSS below AT91C_BASE_PMC->PMC_PCER = (1 << pSdSpi->spiId);
    PMC->PMC_PCER0 = (1 << pSdSpi->spiId);

    // Disable the PDC transfer    
    pSpiHw->SPI_PTCR = SPI_PTCR_RXTDIS | SPI_PTCR_TXTDIS; // BSS AT91C_PDC_RXTDIS | AT91C_PDC_TXTDIS;

    // Enable the SPI
    pSpiHw->SPI_CR = SPI_CR_SPIDIS; // BSS AT91C_SPI_SPIDIS;

    // Disable the SPI clock
    // BSS below AT91C_BASE_PMC->PMC_PCDR = (1 << pSdSpi->spiId);
    PMC->PMC_PCDR0 = (1 << pSdSpi->spiId);

    // Disable all the interrupts
    pSpiHw->SPI_IDR = 0xFFFFFFFF;
}

//------------------------------------------------------------------------------
/// Returns 1 if the SPI driver is currently busy programming; 
/// otherwise returns 0.
/// \param pSdSpi  Pointer to a SD SPI driver instance.
//------------------------------------------------------------------------------
unsigned char SDSPI_IsBusy(SdSpi *pSdSpi)
{
    if (pSdSpi->semaphore == 0) {
        return 1;
    }
    else {
        return 0;
    }
}

//------------------------------------------------------------------------------
/// Wait several cycles on SPI bus; 
/// Returns 0 to indicates no error, otherwise return 1.
/// \param pSdSpi  Pointer to a SD SPI driver instance.
/// \param cycles  Wait data cycles.
//------------------------------------------------------------------------------
unsigned char SDSPI_Wait(SdSpi *pSdSpi, unsigned int cycles)
{
    unsigned int i = cycles;
    unsigned char data = 0xff;

    for (; i > 0; i--) {
        if (SDSPI_Read(pSdSpi, &data, 1)) {
            return 1;
        }
    }
    return 0;
}

//------------------------------------------------------------------------------
/// Send stop transfer data token;
/// Returns 0 to indicates no error, otherwise return 1.
/// \param pSdSpi  Pointer to a SD SPI driver instance.
//------------------------------------------------------------------------------
unsigned char SDSPI_StopTranToken(SdSpi *pSdSpi)
{
    unsigned char stopToken = SDSPI_STOP_TRAN;

    TRACE_DEBUG("SDSPI_StopTranToken\n\r");
    return SDSPI_Write(pSdSpi, &stopToken, 1);
}

//------------------------------------------------------------------------------
/// Wait, SD card Ncs cycles; 
/// Returns 0 to indicates no error, otherwise return 1.
/// \param pSdSpi  Pointer to a SD SPI driver instance.
//------------------------------------------------------------------------------
unsigned char SDSPI_NCS(SdSpi *pSdSpi)
{
    unsigned int i;
    unsigned char ncs;

    for(i = 0; i < 15; i++) {
        ncs = 0xff;
        if (SDSPI_Write(pSdSpi, &ncs, 1)) {
            return 1;
        }
    }
    return 0;
}


