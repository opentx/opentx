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

/** \file
 *
 *  Implement for SD/MMC low level commands.
 *
 *  \sa hsmci_module
 */

/** \addtogroup sdmmc_hal
 *@{
 */

/*----------------------------------------------------------------------------
 *         Headers
 *----------------------------------------------------------------------------*/

#include <stdint.h>
#include "board.h"
#include "hsmci/hsmci.h"
#include "sdmmc_cmd.h"
#include "pmc/pmc.h"
#include "debug.h"

#define assert(...)

/*----------------------------------------------------------------------------
 *         Local constants
 *----------------------------------------------------------------------------*/

/** Return halfword(16-bit) count from byte count */
#define toHWCOUNT(byteCnt) (((byteCnt)&0x1) ? (((byteCnt)/2)+1) : ((byteCnt)/2))
/** Return word(32-bit) count from byte count */
#define toWCOUNT(byteCnt)  (((byteCnt)&0x3) ? (((byteCnt)/4)+1) : ((byteCnt)/4))


/** Bit mask for status register errors. */
#define STATUS_ERRORS ((uint32_t)(HSMCI_SR_UNRE  \
                       | HSMCI_SR_OVRE \
                       | HSMCI_SR_ACKRCVE \
                       /*| HSMCI_SR_BLKOVRE*/ \
                       | HSMCI_SR_CSTOE \
                       | HSMCI_SR_DTOE \
                       | HSMCI_SR_DCRCE \
                       | HSMCI_SR_RTOE \
                       | HSMCI_SR_RENDE \
                       | HSMCI_SR_RCRCE \
                       | HSMCI_SR_RDIRE \
                       | HSMCI_SR_RINDE))

/** Bit mask for response errors */
#define STATUS_ERRORS_RESP ((uint32_t)(HSMCI_SR_CSTOE \
                            | HSMCI_SR_RTOE \
                            | HSMCI_SR_RENDE \
                            | HSMCI_SR_RCRCE \
                            | HSMCI_SR_RDIRE \
                            | HSMCI_SR_RINDE))

/** Bit mask for data errors */
#define STATUS_ERRORS_DATA ((uint32_t)(HSMCI_SR_UNRE \
                            | HSMCI_SR_OVRE \
                            /*| HSMCI_SR_BLKOVRE*/ \
                            /*| HSMCI_SR_CSTOE*/ \
                            | HSMCI_SR_DTOE \
                            | HSMCI_SR_DCRCE))


/*---------------------------------------------------------------------------
 * Special command
 *---------------------------------------------------------------------------*/

/** MCI Perform SD/MMC Initialize sequence */
#define SDMMC_POWER_ON_INIT (0 | HSMCI_CMDR_TRCMD_NO_DATA \
                               | HSMCI_CMDR_SPCMD_INIT \
                               | HSMCI_CMDR_OPDCMD )

/*---------------------------------------------------------------------------
 * Command Classes
 *---------------------------------------------------------------------------*/

/*
 * Class 0, 2, 4, 5, 7 and 8 are mandatory and shall be supported by
 * all SD Memory Cards.
 */

/* Basic Commands (class 0) */

/** Cmd0 MCI + SPI, bc */
#define SDMMC_GO_IDLE_STATE     (0 | HSMCI_CMDR_TRCMD_NO_DATA \
                                   | HSMCI_CMDR_SPCMD_STD )
/** Cmd1 SDSPI + MMC, bcr, R3 */
#define MMC_SEND_OP_COND        (1 | HSMCI_CMDR_TRCMD_NO_DATA \
                                   | HSMCI_CMDR_SPCMD_STD \
                                   | HSMCI_CMDR_RSPTYP_48_BIT \
                                   | HSMCI_CMDR_OPDCMD )
/** Cmd2 MCI, bcr, R2 */
#define SDMMC_ALL_SEND_CID      (2 | HSMCI_CMDR_TRCMD_NO_DATA \
                                   | HSMCI_CMDR_SPCMD_STD \
                                   | HSMCI_CMDR_RSPTYP_136_BIT \
                                   | HSMCI_CMDR_OPDCMD )
/** Cmd3 MCI SD, bcr, R6 */
#define SD_SEND_RELATIVE_ADDR   (3 | HSMCI_CMDR_TRCMD_NO_DATA \
                                   | HSMCI_CMDR_SPCMD_STD \
                                   | HSMCI_CMDR_RSPTYP_48_BIT \
                                   | HSMCI_CMDR_OPDCMD \
                                   | HSMCI_CMDR_MAXLAT )
/** Cmd3 MCI MMC, ac, R1 */
#define MMC_SET_RELATIVE_ADDR   (3 | HSMCI_CMDR_TRCMD_NO_DATA \
                                   | HSMCI_CMDR_SPCMD_STD \
                                   | HSMCI_CMDR_RSPTYP_48_BIT \
                                   | HSMCI_CMDR_OPDCMD \
                                   | HSMCI_CMDR_MAXLAT )

/** Cmd4 MCI, bc */
#define SDMMC_SET_DSR           (4 | HSMCI_CMDR_TRCMD_NO_DATA \
                                   | HSMCI_CMDR_SPCMD_STD \
                                   | HSMCI_CMDR_RSPTYP_NORESP \
                                   | HSMCI_CMDR_MAXLAT )
/** Cmd5 MMC, ac, R1b */
#define MMC_SLEEP_AWAKE         (5 | HSMCI_CMDR_TRCMD_NO_DATA \
                                   | HSMCI_CMDR_SPCMD_STD \
                                   | HSMCI_CMDR_RSPTYP_R1B \
                                   | HSMCI_CMDR_MAXLAT )
/** Cmd6 MMC, ac, R1b */
#define MMC_SWITCH              (6 | HSMCI_CMDR_TRCMD_NO_DATA \
                                   | HSMCI_CMDR_SPCMD_STD \
                                   | HSMCI_CMDR_RSPTYP_R1B \
                                   | HSMCI_CMDR_MAXLAT )
/** Cmd6 SD, adtc, R1 */
#define SD_SWITCH_FUNC          (6 | HSMCI_CMDR_TRCMD_START_DATA \
                                   | HSMCI_CMDR_TRTYP_SINGLE \
                                   | HSMCI_CMDR_TRDIR \
                                   | HSMCI_CMDR_SPCMD_STD \
                                   | HSMCI_CMDR_RSPTYP_48_BIT \
                                   | HSMCI_CMDR_MAXLAT )
/** Cmd7 MCI, ac, R1/R1b */
#define SDMMC_SELECT_CARD       (7 | HSMCI_CMDR_TRCMD_NO_DATA \
                                   | HSMCI_CMDR_SPCMD_STD \
                                   | HSMCI_CMDR_RSPTYP_R1B \
                                   | HSMCI_CMDR_MAXLAT )
/** Cmd7 MCI, ac */
#define SDMMC_DESELECT_CARD     (7 | HSMCI_CMDR_TRCMD_NO_DATA \
                                   | HSMCI_CMDR_SPCMD_STD \
                                   | HSMCI_CMDR_RSPTYP_NORESP \
                                   | HSMCI_CMDR_MAXLAT )
/** Cmd8 SD + SPI, bcr, R7 */
#define SD_SEND_IF_COND         (8 | HSMCI_CMDR_TRCMD_NO_DATA \
                                   | HSMCI_CMDR_SPCMD_STD \
                                   | HSMCI_CMDR_RSPTYP_48_BIT \
                                   | HSMCI_CMDR_OPDCMD \
                                   | HSMCI_CMDR_MAXLAT )
/** Cmd8 MMC, adtc, R1 */
#define MMC_SEND_EXT_CSD        (8 | HSMCI_CMDR_TRCMD_START_DATA \
                                   | HSMCI_CMDR_TRTYP_SINGLE \
                                   | HSMCI_CMDR_TRDIR \
                                   | HSMCI_CMDR_SPCMD_STD \
                                   | HSMCI_CMDR_RSPTYP_48_BIT \
                                   | HSMCI_CMDR_MAXLAT )
/** Cmd9 MCI + SPI, ac, R2 */
#define SDMMC_SEND_CSD          (9 | HSMCI_CMDR_TRCMD_NO_DATA \
                                   | HSMCI_CMDR_SPCMD_STD \
                                   | HSMCI_CMDR_RSPTYP_136_BIT \
                                   | HSMCI_CMDR_MAXLAT )
/** Cmd10 MCI + SPI, ac, R2 */
#define SDMMC_SEND_CID          (10| HSMCI_CMDR_TRCMD_NO_DATA \
                                   | HSMCI_CMDR_SPCMD_STD \
                                   | HSMCI_CMDR_RSPTYP_136_BIT \
                                   | HSMCI_CMDR_MAXLAT )

/** Cmd12 MCI + SPI, ac, R1b (MMC read R1?) */
#define SDMMC_STOP_TRANSMISSION (12| HSMCI_CMDR_TRCMD_STOP_DATA \
                                   | HSMCI_CMDR_SPCMD_STD \
                                   /*| HSMCI_CMDR_SPCMD_SYNC*/ \
                                   | HSMCI_CMDR_RSPTYP_R1B \
                                   | HSMCI_CMDR_MAXLAT )
#define SDMMC_STOP_READ         (12| HSMCI_CMDR_RSPTYP_48_BIT \
                                   | HSMCI_CMDR_SPCMD_STD \
                                   | HSMCI_CMDR_TRCMD_STOP_DATA \
                                   | HSMCI_CMDR_MAXLAT )
#define SDMMC_STOP_WRITE        (12| HSMCI_CMDR_RSPTYP_R1B \
                                   | HSMCI_CMDR_SPCMD_STD \
                                   | HSMCI_CMDR_TRCMD_STOP_DATA \
                                   | HSMCI_CMDR_MAXLAT )
/** Cmd13 MCI + SPI, ac, R1 */
#define SDMMC_SEND_STATUS       (13| HSMCI_CMDR_TRCMD_NO_DATA \
                                   | HSMCI_CMDR_SPCMD_STD \
                                   | HSMCI_CMDR_RSPTYP_48_BIT \
                                   | HSMCI_CMDR_MAXLAT )
/** Cmd14 MMC, adtc, R1 */
#define MMC_BUSTEST_R           (14| HSMCI_CMDR_TRCMD_START_DATA \
                                   | HSMCI_CMDR_TRTYP_SINGLE \
                                   | HSMCI_CMDR_TRDIR \
                                   | HSMCI_CMDR_RSPTYP_48_BIT ) \
                                   | HSMCI_CMDR_MAXLAT )
/** Cmd15 MCI, ac */
#define SDMMC_GO_INACTIVE_STATE (15| HSMCI_CMDR_RSPTYP_NORESP )

/** Cmd19 MMC, adtc, R1 */
#define MMC_BUSTEST_W           (19| HSMCI_CMDR_TRCMD_START_DATA \
                                   | HSMCI_CMDR_TRTYP_SINGLE \
                                   | HSMCI_CMDR_RSPTYP_48_BIT \
                                   | HSMCI_CMDR_MAXLAT )

/*-------------------------------------------------
 * Class 1 commands: Stream oriented Read commands
 *-------------------------------------------------*/

/** Cmd11 MMC, adtc, R1 */
#define MMC_READ_DAT_UNTIL_STOP (11| HSMCI_CMDR_SPCMD_STD \
                                   | HSMCI_CMDR_RSPTYP_48_BIT \
                                   | HSMCI_CMDR_TRCMD_START_DATA \
                                   | HSMCI_CMDR_TRTYP_STREAM \
                                   | HSMCI_CMDR_TRDIR_READ \
                                   | HSMCI_CMDR_MAXLAT )

/*-------------------------------------------------
 * Class 2 commands: Block oriented Read commands
 *-------------------------------------------------*/

/** Cmd16, ac, R1 */
#define SDMMC_SET_BLOCKLEN          (16| HSMCI_CMDR_TRCMD_NO_DATA \
                                       | HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_MAXLAT_64 )
/** Cmd17, adtc, R1 */
#define SDMMC_READ_SINGLE_BLOCK     (17| HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_TRCMD_START_DATA \
                                       | HSMCI_CMDR_TRTYP_SINGLE \
                                       | HSMCI_CMDR_TRDIR_READ \
                                       | HSMCI_CMDR_MAXLAT)
/** Cmd18, adtc, R1 */
#define SDMMC_READ_MULTIPLE_BLOCK   (18| HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_TRCMD_START_DATA \
                                       | HSMCI_CMDR_TRTYP_MULTIPLE \
                                       | HSMCI_CMDR_TRDIR_READ \
                                       | HSMCI_CMDR_MAXLAT)

/*-------------------------------------------------
 * Class 3 commands: Stream write commands
 *-------------------------------------------------*/

#define MMC_WRITE_DAT_UNTIL_STOP    (20| HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_TRCMD_START_DATA \
                                       | HSMCI_CMDR_TRTYP_STREAM \
                                       | HSMCI_CMDR_TRDIR_WRITE \
                                       | HSMCI_CMDR_MAXLAT )

/*-------------------------------------------------
 * Class 4 commands: Block oriented write commands
 *-------------------------------------------------*/

/** Cmd23, MMC, ac, R1 */
#define MMC_SET_BLOCK_COUNT         (23| HSMCI_CMDR_TRCMD_NO_DATA \
                                       | HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_MAXLAT )
/** Cmd24, adtc, R1 */
#define SDMMC_WRITE_BLOCK           (24| HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_TRCMD_START_DATA \
                                       | HSMCI_CMDR_TRTYP_MULTIPLE \
                                       | HSMCI_CMDR_TRDIR_WRITE \
                                       | HSMCI_CMDR_MAXLAT)
/** Cmd25, adtc, R1 */
#define SDMMC_WRITE_MULTIPLE_BLOCK  (25| HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_TRCMD_START_DATA \
                                       | HSMCI_CMDR_TRTYP_MULTIPLE \
                                       | HSMCI_CMDR_TRDIR_WRITE \
                                       | HSMCI_CMDR_MAXLAT)
/** Cmd26, MMC, adtc, R1 */
#define MMC_PROGRAM_CID             (26| HSMCI_CMDR_RSPTYP_48_BIT )
/** Cmd27, adtc, R1 */
#define SDMMC_PROGRAM_CSD           (27| HSMCI_CMDR_RSPTYP_48_BIT )

/*------------------------------------------------
 * Class 8 commands: Application specific commands
 *------------------------------------------------*/

/** Cmd55, SD, ac, R1 */
#define SDMMC_APP_CMD               (55| HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_TRCMD_NO_DATA \
                                       | HSMCI_CMDR_MAXLAT)
/** Cmd 56, SD, adtc, R1 */
#define SDMMC_GEN_CMD               (56| HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_TRCMD_NO_DATA \
                                       | HSMCI_CMDR_MAXLAT)

/*------------------------------------------------
 * Class 9 commands: I/O mode commands
 *------------------------------------------------*/

/** Cmd39, MMC, ac, R4 */
#define MMC_FAST_IO                 (39| HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_TRCMD_NO_DATA \
                                       | HSMCI_CMDR_MAXLAT )
/** Cmd40, MMC, bcr, R5 */
#define MMC_GO_IRQ_STATE            (40| HSMCI_CMDR_SPCMD_IT_CMD \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_TRCMD_NO_DATA \
                                       | HSMCI_CMDR_OPDCMD_OPENDRAIN \
                                       | HSMCI_CMDR_MAXLAT )

/*------------------------------------------------
 * SD Application Specific commands:
 *------------------------------------------------*/

/** ACMD6, ac, R1 */
#define SD_SET_BUS_WIDTH            (6 | HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_TRCMD_NO_DATA \
                                       | HSMCI_CMDR_MAXLAT)
/** ACMD13, adtc, R1 */
#define SD_SD_STATUS                (13| HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_TRCMD_START_DATA \
                                       | HSMCI_CMDR_TRTYP_SINGLE \
                                       | HSMCI_CMDR_TRDIR_READ \
                                       | HSMCI_CMDR_MAXLAT)
/** ACMD22, adtc, R1 */
#define SD_SEND_NUM_WR_BLOCKS       (22| HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_TRCMD_NO_DATA \
                                       | HSMCI_CMDR_MAXLAT)
/** ACMD23, ac, R1 */
#define SD_SET_WR_BLK_ERASE_COUNT   (23| HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_TRCMD_NO_DATA \
                                       | HSMCI_CMDR_MAXLAT)
/** ACMD41, bcr, R3 */
#define SD_SD_SEND_OP_COND          (41| HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_TRCMD_NO_DATA \
                                       /*| HSMCI_CMDR_OPDCMD_OPENDRAIN */ )
/** ACMD42, ac, R1 */
#define SD_SET_CLR_CARD_DETECT      (42| HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_TRCMD_NO_DATA \
                                       | HSMCI_CMDR_MAXLAT)
/** ACMD51, adtc, R1 */
#define SD_SEND_SCR                 (51| HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_TRCMD_START_DATA \
                                       | HSMCI_CMDR_TRDIR_READ \
                                       | HSMCI_CMDR_TRTYP_SINGLE \
                                       | HSMCI_CMDR_MAXLAT)

/*------------------------------------------------
 * SDIO commands:
 *------------------------------------------------*/

/** SDIO CMD5, R4 */
#define SDIO_SEND_OP_COND           (5 | HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_TRCMD_NO_DATA \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_OPDCMD )

/** SDIO CMD52, R5 */
#define SDIO_IO_RW_DIRECT           (52| HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_TRCMD_NO_DATA \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_MAXLAT)

/** SDIO CMD53, R5 */
#define SDIO_IO_RW_EXTENDED         (53| HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_TRCMD_START_DATA \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_MAXLAT )


/*---------------------------------------------------------------------------
 *         Local types
 *---------------------------------------------------------------------------*/

/** Transfer parameters */
typedef struct _MciTransfer {
    uint8_t  *pData;        /** Pointer to data buffer */
    uint32_t size;          /** Total number of bytes to transfer */
    uint32_t transferred;   /** Number of bytes finished */
    uint16_t fifoed;        /** bytes in FIFO */
    uint8_t  isRx;          /** 1 for RX */
    uint8_t  reserved;
} MciTransfer;

/*----------------------------------------------------------------------------
 *         Local variable
 *----------------------------------------------------------------------------*/

/** Tx/Rx parameters */
MciTransfer hsmciXfr;

static MciCmd mciCmd;


/*---------------------------------------------------------------------------
 *         Internal functions
 *---------------------------------------------------------------------------*/


#ifdef HSMCI_WRITE_FIFO
/**
 * Write data to FIFO
 * \param pHw  Pointer to HSMCI register base.
 * \param pXfr Pointer to transfer instance.
 * \param size Size of data in bytes.
 */
static uint8_t FIFO_Write(Hsmci *pHw, MciTransfer *pXfr, uint32_t size)
{
    volatile uint32_t *pFIFO = (volatile uint32_t*)((uint32_t)pHw + 0x200);
    register uint32_t c4, c1;
    uint32_t *pData = (uint32_t*)&pXfr->pData[pXfr->transferred];

    if (size > pXfr->size - pXfr->transferred)
        size = pXfr->size - pXfr->transferred;

    if (size == 0)
        return 0;

    pXfr->fifoed += size;

    /* convert bytes to DW */
    if (size & 0x3) size += 4;
    size /= 4;

    c4 = size >> 2;
    c1 = size & 0x3;
    for (;c4; c4 --) {
        *pFIFO ++ = *pData ++;
        *pFIFO ++ = *pData ++;
        *pFIFO ++ = *pData ++;
        *pFIFO ++ = *pData ++;
    }
    for (;c1; c1 --) {
        *pFIFO ++ = *pData ++;
    }

    return 0;
}
#endif
#ifdef HSMCI_READ_FIFO
/**
 * Read data from FIFO
 * \param pHw  Pointer to HSMCI register base.
 * \param pXfr Pointer to transfer instance.
 * \param size Size of data in DW.
 */
static uint8_t FIFO_Read(Hsmci *pHw, MciTransfer *pXfr, uint32_t size)
{
    volatile uint32_t *pFIFO = (volatile uint32_t*)((uint32_t)pHw + 0x200);
    register uint32_t c4, c1;
    uint32_t *pData = (uint32_t*)&pXfr->pData[pXfr->transferred];

    if (size > pXfr->size - pXfr->transferred)
        size = pXfr->size - pXfr->transferred;

    if (size == 0)
        return 0;

    pXfr->fifoed -= size;
    pXfr->transferred += size;

    /* convert bytes to DW */
    if (size & 0x3) size += 4;
    size /= 4;

    c4 = size >> 2;
    c1 = size & 0x3;
    for (;c4; c4 --) {
        *pData ++ = *pFIFO ++;
        *pData ++ = *pFIFO ++;
        *pData ++ = *pFIFO ++;
        *pData ++ = *pFIFO ++;
    }
    for (;c1; c1 --) {
        *pData ++ = *pFIFO ++;
    }

    return 0;
}
#endif

/*----------------------------------------------------------------------------
 *         Local functions
 *----------------------------------------------------------------------------*/

/**
 * Reset the MciCmd
 */
static void ResetMciCommand(MciCmd *pCommand)
{
    pCommand->cmd       = 0;
    pCommand->arg       = 0;
    pCommand->pData     = 0;
    pCommand->blockSize = 0;
    pCommand->nbBlock   = 0;
    pCommand->pResp     = 0;
    pCommand->callback  = 0;
    pCommand->pArg      = 0;
    pCommand->resType   = 0;
    pCommand->tranType  = 0;
    pCommand->busyCheck = 0;
    pCommand->status    = 0;
    pCommand->state     = 0;
}

/**
 * Starts a MCI  transfer. This is a non blocking function. It will return
 * as soon as the transfer is started.
 * Return 0 if successful; otherwise returns MCI_ERROR_LOCK if the driver is
 * already in use.
 * \param pMci  Pointer to an MCI driver instance.
 * \param pCommand  Pointer to the command to execute.
 */
uint8_t Sdmmc_SendCommand(Mcid*pMci, MciCmd *pCommand)
{
    Hsmci* pMciHw = pMci->pMciHw;
    uint32_t mciIer = 0, mciMr;
    uint32_t transSize;

    assert(pMci);
    assert(pMciHw);
    assert(pCommand);

    /* Try to acquire the MCI semaphore */
    if (pMci->semaphore == 0)
    {
        return SDMMC_ERROR_BUSY;
    }
    pMci->semaphore--;

    /* Command is now being executed */
    pMci->pCommand = pCommand;
    pCommand->state = SDMMC_CMD_PENDING;

    /* Enable the MCI peripheral clock */
    PMC_EnablePeripheral(pMci->mciId);

#if 1
    /* Wait for NOTBUSY (DATA bus IDLE) */
    if (pCommand->tranType == MCI_STOP_TRANSFER)
    {
        volatile uint32_t busyWait = 0x00001000;

        while ( busyWait-- )
        {
          uint32_t sr = pMciHw->HSMCI_SR;

          if ((sr & HSMCI_SR_NOTBUSY) && ((sr & HSMCI_SR_DTIP) == 0))
          {
              break;
          }
        }
    }
#endif

    /* Disable MCI */
    MCI_Disable(pMciHw);

    /* Prepare Default Mode register value */
    mciMr = pMciHw->HSMCI_MR & (~(HSMCI_MR_WRPROOF
                                  |HSMCI_MR_RDPROOF
                                  |HSMCI_MR_BLKLEN_Msk
                                  |HSMCI_MR_PDCMODE
                                  |HSMCI_MR_FBYTE));

    /* Stop transfer: idle the bus */
    if (pCommand->tranType == MCI_STOP_TRANSFER)
    {
        pMciHw->HSMCI_MR = mciMr;
        mciIer = HSMCI_IER_XFRDONE | STATUS_ERRORS_RESP;
    }
    /* No data transfer: stop at the end of the command */
    else if (pCommand->tranType == MCI_NO_TRANSFER)
    {
        pMciHw->HSMCI_MR = mciMr;
        mciIer = HSMCI_IER_XFRDONE | STATUS_ERRORS_RESP;
        /* R3 response, no CRC check */
        if (pCommand->resType == 3)
        {
            mciIer &= ~((uint32_t)HSMCI_IER_RCRCE);
        }
    }
    /* Data command but no following:
       Start an infinite block transfer */
    else
        if (pCommand->nbBlock == 0)
    {
        /* Block size... Data length 0: no PDC */
        pMciHw->HSMCI_MR = mciMr | HSMCI_MR_WRPROOF
                                 | HSMCI_MR_RDPROOF;
        pMciHw->HSMCI_BLKR = (pCommand->blockSize << 16);
        mciIer = HSMCI_IER_CMDRDY | STATUS_ERRORS_RESP;
    }
    /* Command with DATA stage */
    else
    {
        /* Check data size */
        if (pCommand->nbBlock == 0)
        {
            TRACE_ERROR("MCICmd: At least one block required\n\r");
            return SDMMC_ERROR_PARAM;
        }

        /* Number of blocks to transfer */
        if (   pCommand->tranType == MCI_START_WRITE
            || pCommand->tranType == MCI_START_READ)
        {
            /* Set number of blocks */
            pMciHw->HSMCI_BLKR = (pCommand->blockSize << 16)
                               | (pCommand->nbBlock   <<  0);
        }

        /* Force byte transfer */
        transSize = (pCommand->nbBlock * pCommand->blockSize);
        if (pCommand->blockSize & 0x3)
        {
            mciMr |= HSMCI_MR_FBYTE;
        }
        else
        {
            transSize = toWCOUNT(transSize);
        }

        /* Set block size & enable PDC */
        pMciHw->HSMCI_MR = mciMr | HSMCI_MR_WRPROOF
                                 | HSMCI_MR_RDPROOF
                                 | HSMCI_MR_PDCMODE
                                 | (pCommand->blockSize << 16);

        /* Update PDC for write */
        if (   pCommand->tranType == MCI_START_WRITE
            || pCommand->tranType == MCI_WRITE )
        {
            pMciHw->HSMCI_TPR = (uint32_t)pCommand->pData;
            pMciHw->HSMCI_TCR = transSize;
            pMciHw->HSMCI_TNCR = 0;

            if (pCommand->tranType == MCI_START_WRITE)
            {
                mciIer = (uint32_t)(HSMCI_IER_XFRDONE | STATUS_ERRORS_DATA);
            }
            else
            {
                mciIer = (uint32_t)(HSMCI_IER_TXBUFE | STATUS_ERRORS_DATA);
            }
        }
        /* Update PDC for read */
        else
        {
            pMciHw->HSMCI_RPR = (uint32_t)pCommand->pData;
            pMciHw->HSMCI_RCR = transSize;
            pMciHw->HSMCI_RNCR = 0;
            pMciHw->HSMCI_PTCR = HSMCI_PTCR_RXTEN;
            if (pCommand->tranType == MCI_START_READ)
            {
                mciIer = (uint32_t)(HSMCI_IER_XFRDONE | STATUS_ERRORS_DATA);
            }
            else
            {
                mciIer = (uint32_t)(HSMCI_IER_RXBUFF | STATUS_ERRORS_DATA);
            }
        }
    }

    /* Enable MCI */
    MCI_Enable(pMciHw);

    /* Send the command */
    if ( (pCommand->tranType != MCI_WRITE && pCommand->tranType != MCI_READ) || (pCommand->blockSize == 0) )
    {
        pMciHw->HSMCI_ARGR = pCommand->arg;
        pMciHw->HSMCI_CMDR = pCommand->cmd;
    }

    /* Start sending PDC */
    if (pCommand->blockSize > 0
        && (pCommand->tranType == MCI_START_WRITE
            || (pCommand->tranType == MCI_WRITE)) )
    {
        pMciHw->HSMCI_PTCR = HSMCI_PTCR_TXTEN;
    }

    /* Ignore CRC error for R3 & R4 */
    if (pCommand->resType == 3 || pCommand->resType == 4)
    {
        mciIer &= ~(uint32_t)(HSMCI_IER_RCRCE);
    }

    /* Ignore data error on stop transfer command :) */
    if (pCommand->tranType == MCI_STOP_TRANSFER)
    {
        mciIer &= ~(STATUS_ERRORS_DATA);
    }

    /* Interrupt enable shall be done after PDC TXTEN and RXTEN */
    pMciHw->HSMCI_IER = mciIer;

    return 0;
}

/**
 * Returns 1 if the given MCI transfer is complete; otherwise returns 0.
 * \param pMci  Pointer to a Mcid instance.
 */
uint8_t Sdmmc_IsCommandComplete( Mcid *pMci )
{
    MciCmd *pCommand = pMci->pCommand;

    if ( pMci->semaphore == 0 )
    {
        return 0;
    }

    if ( pCommand )
    {
        if ( pCommand->state == SDMMC_CMD_PENDING )
        {
            return 0;
        }
    }
    return 1;
}

/**
 * Sends the current SD card driver command to the card.
 * Returns 0 if successful; Otherwise, returns the transfer status code or
 * SDMMC_ERROR if there was a problem with the SD transfer.
 * \param pSd  Pointer to a SdCard driver instance.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
static uint8_t SendMciCommand(SdCard *pSd, SdmmcCallback fCallback)
{
    Mcid   *pMcid    = pSd->pSdDriver;
    MciCmd *pCommand = &(mciCmd);
    uint8_t error;

    //TRACE_INFO("MCI Cmd: %x\n\r", pCommand->cmd);

    /* Fill callback */
    pCommand->callback = fCallback;
    pCommand->pArg     = pSd;

    /* Send command */
    error = Sdmmc_SendCommand(pMcid, pCommand);
    if (error) {
        TRACE_ERROR("Sdmmc_SendCommand(%x): %d\n\r", pCommand->cmd, error);
        return SDMMC_ERROR;
    }

    /* Wait for command to complete (if no callback defined) */
    if (pCommand->callback == 0) {
        while (!Sdmmc_IsCommandComplete(pMcid));
        return pCommand->status;
    }

    return 0;
}


/*---------------------------------------------------------------------------
 *      Exported functions
 *---------------------------------------------------------------------------*/


/**
 * Initialization delay: The maximum of 1 msec, 74 clock cycles and supply ramp
 * up time.
 * Returns the command transfer result (see SendMciCommand).
 * \param pSd  Pointer to a SdCard driver instance.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
uint8_t SdmmcPowerOn(SdCard *pSd, SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);
    uint32_t response;
    uint8_t error;

    TRACE_DEBUG( "PowerON()\n\r" ) ;
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = SDMMC_POWER_ON_INIT;
    pCommand->pResp = &response;

    /* Send command */
    error =  SendMciCommand(pSd, fCallback);
    return error;
}

/**
 * Resets all cards to idle state
 * \param pSd  Pointer to a SdCard driver instance.
 * \param arg  Argument used.
 * \return the command transfer result (see SendMciCommand).
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
uint8_t SdmmcCmd0(SdCard *pSd, uint32_t arg, SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);
    uint8_t error;

    TRACE_DEBUG("Cmd0()\n\r");
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = SDMMC_GO_IDLE_STATE;
    pCommand->arg = arg;

    /* Send command */
    error = SendMciCommand(pSd, fCallback);
    return error;
}

/**
 * MMC send operation condition command.
 * Sends host capacity support information and activates the card's
 * initialization process.
 * Returns the command transfer result (see SendMciCommand).
 * \param pSd  Pointer to a SdCard driver instance.
 * \param pOCR Pointer to fill OCR value to send and get.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
uint8_t MmcCmd1(SdCard *pSd, uint32_t* pOCR, SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);
    uint8_t error;

    TRACE_DEBUG("Cmd1()\n\r");
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = MMC_SEND_OP_COND;
    pCommand->arg = *pOCR;
    pCommand->resType = 3;
    pCommand->pResp = pOCR;

    /* send command */
    error = SendMciCommand(pSd, fCallback);
    return error;
}

/**
 * Asks any card to send the CID numbers
 * on the CMD line (any card that is
 * connected to the host will respond)
 * Returns the command transfer result (see SendMciCommand).
 * \param pSd  Pointer to a SD card driver instance.
 * \param pCID Pointer to buffer for storing the CID numbers.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
uint8_t SdmmcCmd2(SdCard *pSd, uint32_t *pCID, SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);

    TRACE_DEBUG("Cmd2()\n\r");
    ResetMciCommand(pCommand);

    /* Fill the command information */
    pCommand->cmd = SDMMC_ALL_SEND_CID;
    pCommand->resType = 2;
    pCommand->pResp = pSd->cid;

    /* Send the command */
    return SendMciCommand(pSd, fCallback);
}

/**
 * Ask the card to publish a new relative address (RCA)
 * Returns the command transfer result (see SendMciCommand).
 * \param pSd   Pointer to a SD card driver instance.
 * \param pRsp  Pointer to buffer to fill response (address on 31:16).
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
uint8_t SdCmd3(SdCard *pSd, uint32_t *pRsp, SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);
    uint8_t error;

    TRACE_DEBUG("Cmd3()\n\r");
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = SD_SEND_RELATIVE_ADDR;
    pCommand->resType = 6;
    pCommand->pResp = pRsp;

    /* Send command */
    error = SendMciCommand(pSd, fCallback);
    return error;
}

/**
 * Assign relative address to the card
 * Returns the command transfer result (see SendMciCommand).
 * \param pSd       Pointer to a SD card driver instance.
 * \param cardAddr  Card Relative Address value.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
uint8_t MmcCmd3(SdCard *pSd, uint16_t cardAddr, SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);
    uint32_t response;
    uint8_t error;

    TRACE_DEBUG("Cmd3()\n\r");
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = MMC_SET_RELATIVE_ADDR;
    pCommand->arg = (cardAddr << 16);
    pCommand->resType = 6;
    pCommand->pResp = &response;

    /* Send command */
    error = SendMciCommand(pSd, fCallback);
    return error;
}

/**
 * Command toggles a card between the
 * stand-by and transfer states or between
 * the programming and disconnect states.
 * Returns the command transfer result (see SendMciCommand).
 * \param pSd       Pointer to a SD card driver instance.
 * \param cardAddr  Relative Card Address (0 deselects all).
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */

// extern uint8_t CoTickDelay(uint32_t ticks);
uint8_t SdmmcCmd7(SdCard *pSd, uint16_t cardAddr, SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);

    // CoTickDelay(1);
    TRACE_DEBUG("Cmd7(%d)\n\r", cardAddr);
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = cardAddr ? SDMMC_SELECT_CARD : SDMMC_DESELECT_CARD;
    pCommand->arg = cardAddr << 16;
    pCommand->resType = 1;

    /* Send command */
    return SendMciCommand(pSd, fCallback);
}

/**
 * SDIO SEND OPERATION CONDITION (OCR) command.
 * Sends host capacity support information and acrivates the card's
 * initialization process.
 * \return The command transfer result (see SendMciCommand).
 * \param pSd     Pointer to a SD/MMC card driver instance.
 * \param pIoData Pointer to data sent as well as response buffer (32bit).
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
uint8_t SdioCmd5(SdCard *pSd, uint32_t *pIoData, SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);

    TRACE_DEBUG("Cmd5()\n\r");
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = SDIO_SEND_OP_COND;
    pCommand->arg = *pIoData;
    pCommand->resType = 4;
    pCommand->pResp = pIoData;

    /* Send command */
    return SendMciCommand(pSd, fCallback);
}

/**
 * Switches the mode of operation of the selected card.
 * CMD6 is valid under the "trans" state.
 * \return The command transfer result (see SendMciCommand).
 * \param  pSd         Pointer to a SD/MMC card driver instance.
 * \param  pSwitchArg  Pointer to a MmcCmd6Arg instance.
 * \param  pStatus     Pointer to where the 512bit status is returned.
 * \param  pResp       Pointer to where the response is returned.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
uint8_t SdCmd6(SdCard *pSd,
               const void * pSwitchArg,
               uint32_t   * pStatus,
               uint32_t   * pResp,
               SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);
    uint8_t error;
    SdCmd6Arg * pSdSwitch;

    assert(pSd);
    assert(pSwitchArg);

    TRACE_DEBUG("CMD6()\n\r");
    ResetMciCommand(pCommand);

    pSdSwitch = (SdCmd6Arg*)pSwitchArg;
    pCommand->cmd = SD_SWITCH_FUNC;
    pCommand->resType = 1;
    pCommand->arg =   (pSdSwitch->mode << 31)
                    | (pSdSwitch->reserved << 30)
                    | (pSdSwitch->reserveFG6 << 20)
                    | (pSdSwitch->reserveFG5 << 16)
                    | (pSdSwitch->reserveFG4 << 12)
                    | (pSdSwitch->reserveFG3 <<  8)
                    | (pSdSwitch->command << 4)
                    | (pSdSwitch->accessMode << 0);
    if (pStatus) {
        pCommand->blockSize = 512 / 8;
        pCommand->nbBlock = 1;
        pCommand->pData = (uint8_t*)pStatus;
        pCommand->tranType = MCI_START_READ;
    }
    pCommand->pResp = pResp;

    error = SendMciCommand(pSd, fCallback);
    return error;
}

/**
 * Switches the mode of operation of the selected card or
 * Modifies the EXT_CSD registers.
 * CMD6 is valid under the "trans" state.
 * \return The command transfer result (see SendMciCommand).
 * \param  pSd         Pointer to a SD/MMC card driver instance.
 * \param  pSwitchArg  Pointer to a MmcCmd6Arg instance.
 * \param  pResp       Pointer to where the response is returned.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
uint8_t MmcCmd6(SdCard *pSd,
                const void * pSwitchArg,
                uint32_t   * pResp,
                SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);
    uint8_t error;
    MmcCmd6Arg * pMmcSwitch;

    assert(pSd);
    assert(pSwitchArg);

    TRACE_DEBUG("CMD6()\n\r");
    ResetMciCommand(pCommand);

    pMmcSwitch = (MmcCmd6Arg*)pSwitchArg;
    pCommand->cmd = MMC_SWITCH;
    pCommand->resType = 1;
    pCommand->arg =   (pMmcSwitch->access << 24)
                    | (pMmcSwitch->index  << 16)
                    | (pMmcSwitch->value  <<  8)
                    | (pMmcSwitch->cmdSet <<  0);
    pCommand->pResp = pResp;

    error = SendMciCommand(pSd, fCallback);
    return error;
}

/**
 * Sends SD Memory Card interface condition, which includes host supply
 * voltage information and asks the card whether card supports voltage.
 * Should be performed at initialization time to detect the card type.
 * \param pSd             Pointer to a SD card driver instance.
 * \param supplyVoltage   Expected supply voltage(SD).
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 * \return 0 if successful;
 *         otherwise returns SD_ERROR_NORESPONSE if the card did not answer
 *         the command, or SDMMC_ERROR.
 */
uint8_t SdCmd8(SdCard *pSd,
               uint8_t supplyVoltage,
               SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);
    uint32_t response;
    uint8_t error;

    TRACE_DEBUG("Cmd8()\n\r");
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = SD_SEND_IF_COND;
    pCommand->arg = (supplyVoltage << 8) | (0xAA);
    pCommand->resType = 7;
    pCommand->pResp = &response;

    /* Send command */
    error = SendMciCommand(pSd, fCallback);

    /* Return if there is callback */
    if (fCallback)
        return error;

    /* Check result */
    if (error == SDMMC_ERROR_NORESPONSE) {
        return SDMMC_ERROR_NORESPONSE;
    }
    /* SD_R7
     * Bit 0 - 7: check pattern (echo-back)
     * Bit 8 -11: voltage accepted
     */
    else if (!error &&
            ((response & 0x00000FFF) == ((supplyVoltage << 8) | 0xAA))) {
        return 0;
    }
    else {
        return SDMMC_ERROR;
    }
}

/**
 * SEND_EXT_CSD, to get EXT_CSD register as a block of data.
 * Valid under "trans" state.
 * \param pSd   Pointer to a SD card driver instance.
 * \param pEXT  512 byte buffer pointer for EXT_CSD data.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 * \return 0 if successful;
 *         otherwise returns SD_ERROR_NORESPONSE if the card did not answer
 *         the command, or SDMMC_ERROR.
 */
uint8_t MmcCmd8(SdCard *pSd,
                uint8_t* pEXT,
                SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);
    uint8_t error;

    TRACE_DEBUG("Cmd8()\n\r");
    ResetMciCommand(pCommand);

    pCommand->cmd = MMC_SEND_EXT_CSD;
    pCommand->resType = 1;

    pCommand->blockSize = 512;
    pCommand->nbBlock = 1;
    pCommand->pData = pEXT;

    pCommand->tranType = MCI_START_READ;

    /* Send command */
    error = SendMciCommand(pSd, fCallback);
    return error;
}

/**
 * Addressed card sends its card-specific
 * data (CSD) on the CMD line.
 * Returns the command transfer result (see SendMciCommand).
 * \param pSd       Pointer to a SD card driver instance.
 * \param cardAddr  Card Relative Address.
 * \param pCSD      Pointer to buffer for CSD data.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
uint8_t SdmmcCmd9(SdCard *pSd,
                  uint16_t cardAddr,
                  uint32_t *pCSD,
                  SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);
    uint8_t error;

    TRACE_DEBUG("Cmd9()\n\r");
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = SDMMC_SEND_CSD;
    pCommand->arg = cardAddr << 16;
    pCommand->resType = 2;
    pCommand->pResp = pCSD;

    /* Send command */
    error = SendMciCommand(pSd, fCallback);
    return error;
}

/**
 * Forces the card to stop transmission
 * \param pSd      Pointer to a SD card driver instance.
 * \param pStatus  Pointer to a status variable.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
uint8_t SdmmcCmd12(SdCard *pSd,
                   uint32_t *pStatus,
                   SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);
    uint8_t error;

    TRACE_DEBUG("Cmd12()\n\r");
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = SDMMC_STOP_TRANSMISSION;
    pCommand->tranType = MCI_STOP_TRANSFER;
    pCommand->resType = 1;
    pCommand->busyCheck = 1;
    pCommand->pResp = pStatus;

    /* Send command */
    error = SendMciCommand(pSd, fCallback);
    return error;
}

/**
 * Addressed card sends its status register.
 * Returns the command transfer result (see SendMciCommand).
 * \param pSd       Pointer to a SD card driver instance.
 * \param cardAddr  Card Relative Address.
 * \param pStatus   Pointer to a status variable.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
uint8_t SdmmcCmd13(SdCard *pSd,
                   uint16_t cardAddr,
                   uint32_t *pStatus,
                   SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);
    uint8_t error;

    TRACE_DEBUG("Cmd13()\n\r");
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = SDMMC_SEND_STATUS;
    pCommand->arg = cardAddr << 16;
    pCommand->resType = 1;
    pCommand->pResp = pStatus;

    /* Send command */
    error = SendMciCommand(pSd, fCallback);

    return error;
}

/**
 * In the case of a Standard Capacity SD Memory Card, this command sets the
 * block length (in bytes) for all following block commands
 * (read, write, lock).
 * Default block length is fixed to 512 Bytes.
 * Set length is valid for memory access commands only if partial block read
 * operation are allowed in CSD.
 * In the case of a High Capacity SD Memory Card, block length set by CMD16
 * command does not affect the memory read and write commands. Always 512
 * Bytes fixed block length is used. This command is effective for LOCK_UNLOCK
 * command. In both cases, if block length is set larger than 512Bytes, the
 * card sets the BLOCK_LEN_ERROR bit.
 * \param pSd  Pointer to a SD card driver instance.
 * \param blockLength  Block length in bytes.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
uint8_t SdmmcCmd16(SdCard *pSd, uint16_t blockLength, SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);
    uint8_t error;

    TRACE_DEBUG("Cmd16()\n\r");
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = SDMMC_SET_BLOCKLEN;
    pCommand->arg = blockLength;
    pCommand->resType = 1;

    /* Send command */
    error = SendMciCommand(pSd, fCallback);

    return error;
}

/**
 * Read single block command
 * \param pSd  Pointer to a SD card driver instance.
 * \param blockSize Block size (shall be set to 512 in case of high capacity).
 * \param pData     Pointer to the DW aligned buffer to be filled.
 * \param address   Data Address on SD/MMC card.
 * \param pStatus   Pointer response buffer as status return.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
uint8_t SdmmcCmd17(SdCard *pSd,
                   uint16_t blockSize,
                   uint8_t *pData,
                   uint32_t address,
                   uint32_t *pStatus,
                   SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);
    uint8_t error;

    assert(pSd);

    TRACE_DEBUG("Cmd17()\n\r");
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = SDMMC_READ_SINGLE_BLOCK;
    pCommand->arg = address;
    pCommand->resType = 1;
    pCommand->pResp = pStatus;

    pCommand->blockSize = blockSize;
    pCommand->nbBlock = 1;
    pCommand->pData = pData;

    pCommand->tranType = MCI_START_READ;

    /* Send command */
    error = SendMciCommand(pSd, fCallback);
    return error;
}

/**
 * Continously transfers datablocks from card to host until interrupted by a
 * STOP_TRANSMISSION command.
 * \param pSd       Pointer to a SD card driver instance.
 * \param blockSize Block size (shall be set to 512 in case of high capacity).
 * \param nbBlocks  Number of blocks to send.
 * \param pData     Pointer to the DW aligned buffer to be filled.
 * \param address   Data Address on SD/MMC card.
 * \param pStatus   Pointer to the response status.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
uint8_t SdmmcCmd18(SdCard *pSd,
                   uint16_t blockSize,
                   uint16_t nbBlocks,
                   uint8_t *pData,
                   uint32_t address,
                   uint32_t *pStatus,
                   SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);
    uint8_t error;

    TRACE_DEBUG("Cmd18()\n\r");
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = SDMMC_READ_MULTIPLE_BLOCK;
    pCommand->arg = address;
    pCommand->resType = 1;
    pCommand->pResp = pStatus;

    pCommand->blockSize = blockSize;
    pCommand->nbBlock = nbBlocks;
    pCommand->pData = pData;

    pCommand->tranType = MCI_START_READ;

    /* Send command */
    error = SendMciCommand(pSd, fCallback);
    return error;
}

/**
 * Write single block command
 * \param pSd  Pointer to a SD card driver instance.
 * \param blockSize Block size (shall be set to 512 in case of high capacity).
 * \param pData     Pointer to the DW aligned buffer to be filled.
 * \param address   Data Address on SD/MMC card.
 * \param pStatus   Pointer to response buffer as status.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
uint8_t SdmmcCmd24(SdCard *pSd,
                   uint16_t blockSize,
                   uint8_t *pData,
                   uint32_t address,
                   uint32_t *pStatus,
                   SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);
    uint8_t error;

    assert(pSd);

    TRACE_DEBUG("Cmd24()\n\r");
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = SDMMC_WRITE_BLOCK;
    pCommand->arg = address;
    pCommand->resType = 1;
    pCommand->pResp = pStatus;

    pCommand->blockSize = blockSize;
    pCommand->nbBlock = 1;
    pCommand->pData = pData;

    pCommand->tranType = MCI_START_WRITE;

    /* Send command */
    error = SendMciCommand(pSd, fCallback);
    return error;
}

/**
 * Write multiple block command
 * \param pSd  Pointer to a SD card driver instance.
 * \param blockSize Block size (shall be set to 512 in case of high capacity).
 * \param nbBlock   Number of blocks to send.
 * \param pData     Pointer to the DW aligned buffer to be filled.
 * \param address   Data Address on SD/MMC card.
 * \param pStatus   Pointer to the response buffer as status.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
uint8_t SdmmcCmd25(SdCard *pSd,
                   uint16_t blockSize,
                   uint16_t nbBlock,
                   uint8_t *pData,
                   uint32_t address,
                   uint32_t *pStatus,
                   SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);
    uint8_t error;

    TRACE_DEBUG("Cmd25()\n\r");
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = SDMMC_WRITE_MULTIPLE_BLOCK;
    pCommand->arg = address;
    pCommand->resType = 1;
    pCommand->pResp = pStatus;

    pCommand->blockSize = blockSize;
    pCommand->nbBlock = nbBlock;
    pCommand->pData = pData;

    pCommand->tranType = MCI_START_WRITE;
    pCommand->busyCheck = 1;

    /* Send command */
    error = SendMciCommand(pSd, fCallback);
    return error;
}

/**
 * SDIO IO_RW_DIRECT command, response R5.
 * \return the command transfer result (see SendMciCommand).
 * \param pSd       Pointer to a SD card driver instance.
 * \param pIoData   Pointer to input argument (\ref SdioRwDirectArg) and
 *                  response (\ref SdmmcR5) buffer.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
uint8_t SdioCmd52(SdCard *pSd, uint32_t* pIoData,
                  SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);
    //SdioRwDirectArg *pArg = (SdioRwDirectArg*)pIoData;

    TRACE_DEBUG("Cmd52()\n\r");
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = SDIO_IO_RW_DIRECT;
    #if 1
    pCommand->arg = *pIoData;
    #else
    pCommand->arg = pArg->data               /* data            7: 0 */
                  | (pArg->regAddress << 9)  /* address        25: 9 */
                  | (pArg->rawFlag << 27)    /* ReadAfterWrite    27 */
                  | (pArg->functionNum << 28)/* FunctionNumber 30:28 */
                  | (pArg->rwFlag << 31);    /* Wr/Rd          31    */
    #endif
    pCommand->resType = 5;
    pCommand->pResp = (uint32_t*)pIoData;

    /* Send command */
    return SendMciCommand(pSd, fCallback);
}

/**
 * SDIO IO_RW_EXTENDED command, response R5.
 * \param pSd       Pointer to a SD card driver instance.
 * \param pArgResp  Pointer to input argument (\ref SdioRwExtArg)
 *                  and response (\ref SdmmcR5) buffer.
 * \param pData     Pointer to data buffer to transfer.
 * \param size      Transfer data size.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 * \param pArg Callback argument.
 */
uint8_t SdioCmd53(SdCard *pSd,
                  uint32_t* pArgResp,
                  uint8_t * pData, uint32_t size,
                  SdmmcCallback fCallback,
                  void* pArg)
{
    MciCmd *pCommand = &(mciCmd);
    SdioRwExtArg *pCmdArg = (SdioRwExtArg*)pArgResp;
    uint8_t error;

    TRACE_DEBUG( "Cmd53()\n\r" ) ;
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = SDIO_IO_RW_EXTENDED;
    /* Write/Read operation */
    if (pCmdArg->rwFlag) {
        pCommand->tranType = MCI_START_WRITE;
    }
    else {
        pCommand->cmd |= HSMCI_CMDR_TRDIR;
        pCommand->tranType = MCI_START_READ;
    }
    /* Block/Byte mode */
    if (pCmdArg->blockMode) {
        pCommand->cmd |= HSMCI_CMDR_TRTYP_BLOCK
                       | HSMCI_CMDR_TRCMD_START_DATA;
        pCommand->blockSize = SDMMC_BLOCK_SIZE;
    }
    else {
        pCommand->cmd |= HSMCI_CMDR_TRTYP_BYTE
                       | HSMCI_CMDR_TRCMD_START_DATA;
        pCommand->blockSize = 1;
    }
    #if 1
    pCommand->arg = *pArgResp;
    #else
    pCommand->arg = pCmdArg->count              /* data count        8: 0 */
                  | (pCmdArg->regAddress << 9)  /* address          27: 9 */
                  | (pCmdArg->opCode << 26)     /* address Inc/Fixed   26 */
                  | (pCmdArg->blockMode << 27)  /* block mode          27 */
                  | (pCmdArg->functionNum << 28)/* FunctionNumber   30:28 */
                  | (pCmdArg->rwFlag << 31);    /* Wr/Rd            31    */
    #endif
    pCommand->resType = 5;
    pCommand->pResp = pArgResp;
    pCommand->pData = pData;
    pCommand->nbBlock = size;
    /* Callback and its arguments */
    pCommand->callback = fCallback;
    pCommand->pArg     = pArg;

    /* Send command */
    error = Sdmmc_SendCommand(pSd->pSdDriver, pCommand);
    if (fCallback == 0 && error == 0) {
        while(!Sdmmc_IsCommandComplete(pSd->pSdDriver));
    }
    return error;
}

/**
 * Indicates to the card that the next command is an application specific
 * command rather than a standard command.
 * \return the command transfer result (see SendMciCommand).
 * \param pSd       Pointer to a SD card driver instance.
 * \param cardAddr  Card Relative Address.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
uint8_t SdmmcCmd55(SdCard *pSd, uint16_t cardAddr, SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);
    uint8_t error;

    TRACE_DEBUG( "Cmd55()\n\r" ) ;
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = SDMMC_APP_CMD;
    //if (addr == 0)  pCommand->cmd |= HSMCI_CMDR_OPDCMD;
    pCommand->arg = cardAddr << 16;
    pCommand->resType = 1;

    /* Send command */
    error = SendMciCommand(pSd, fCallback);
    return error;
}

/**
 * Defines the data bus width (00=1bit or 10=4 bits bus) to be used for data
 * transfer.
 * The allowed data bus widths are given in SCR register.
 * Should be invoked after SdmmcCmd55().
 * \param pSd     Pointer to a SD card driver instance.
 * \param arg     Argument for this command.
 * \param pStatus Pointer to buffer for command response as status.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 * \return the command transfer result (see SendMciCommand).
 */
uint8_t SdAcmd6(SdCard *pSd,
                uint32_t arg,
                uint32_t *pStatus,
                SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);

    TRACE_DEBUG( "Acmd6()\n\r" ) ;
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = SD_SET_BUS_WIDTH;
    pCommand->arg = arg;
    pCommand->resType = 1;
    pCommand->pResp = pStatus;

    /* Send command */
    return SendMciCommand(pSd, fCallback);
}

/**
 * The SD Status contains status bits that are related to the SD memory Card
 * proprietary features and may be used for future application-specific usage.
 * Can be sent to a card only in 'tran_state'.
 * Should be invoked after SdmmcCmd55().
 * \param pSd       Pointer to a SD card driver instance.
 * \param pSdSTAT   Pointer to buffer for SD STATUS data.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 * \return the command transfer result (see SendMciCommand).
 */
uint8_t SdAcmd13(SdCard *pSd,
                 uint32_t * pSdSTAT,
                 SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);
    uint8_t error;

    TRACE_DEBUG( "Acmd13()\n\r" ) ;
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = SD_SD_STATUS;
    pCommand->resType = 1;

    pCommand->blockSize = 512 / 8;
    pCommand->nbBlock = 1;
    pCommand->pData = (uint8_t*)pSdSTAT;

    pCommand->tranType = MCI_START_READ;

    /* Send command */
    error = SendMciCommand(pSd, fCallback);
    return error;
}

/**
 * Asks to all cards to send their operations conditions.
 * Returns the command transfer result (see SendMciCommand).
 * Should be invoked after SdmmcCmd55().
 * \param pSd   Pointer to a SD card driver instance.
 * \param pOpIo Pointer to argument that should be sent and OCR content.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
uint8_t SdAcmd41(SdCard *pSd, uint32_t* pOpIo, SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);

    assert(pSd);
    assert(pOpIo);

    TRACE_DEBUG( "Acmd41()\n\r" ) ;
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = SD_SD_SEND_OP_COND;
    pCommand->arg = *pOpIo;
    pCommand->resType = 3;
    pCommand->pResp = pOpIo;

    return SendMciCommand(pSd, fCallback);
}

/**
 * SD Card Configuration Register (SCR) provides information on the SD Memory
 * Card's special features that were configured into the given card. The size
 * of SCR register is 64 bits.
 * Should be invoked after SdmmcCmd55().
 * \param pSd   Pointer to a SD card driver instance.
 * \param pSCR  Pointer to buffer for SCR content.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 *                  Callback argument is fixed to a pointer to SdCard instance.
 */
uint8_t SdAcmd51(SdCard *pSd, uint32_t * pSCR, SdmmcCallback fCallback)
{
    MciCmd *pCommand = &(mciCmd);
    uint8_t error;

    assert(pSd);

    TRACE_DEBUG( "Acmd51()\n\r" ) ;
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->cmd = SD_SEND_SCR;
    pCommand->resType = 1;

    pCommand->blockSize = 64 / 8;
    pCommand->nbBlock = 1;
    pCommand->pData = (uint8_t*)pSCR;

    pCommand->tranType = MCI_START_READ;

    /* Send command */
    error = SendMciCommand(pSd, fCallback);
    return error;
}

/**
 * Continue to transfer datablocks from card to host until interrupted by a
 * STOP_TRANSMISSION command.
 * \param pSd  Pointer to a SD card driver instance.
 * \param blockSize Block size (shall be set to 512 in case of high capacity).
 * \param nbBlock   Number of blocks to send.
 * \param pData     Pointer to the application buffer to be filled.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 * \param pArg Callback argument.
 */
uint8_t SdmmcRead(SdCard   *pSd,
                  uint16_t blockSize,
                  uint16_t nbBlock,
                  uint8_t  *pData,
                  SdmmcCallback fCallback,
                  void* pArg)
{
    MciCmd *pCommand = &(mciCmd);
    uint8_t error;

    TRACE_DEBUG( "Read()\n\r" ) ;
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->blockSize = blockSize;
    pCommand->nbBlock   = nbBlock;
    pCommand->pData     = pData;

    pCommand->tranType = MCI_READ;

    pCommand->callback = fCallback;
    pCommand->pArg     = pArg;

    /* Send command */
    error = Sdmmc_SendCommand(pSd->pSdDriver, pCommand);
    if (fCallback == 0 && error == 0) {
        while(!Sdmmc_IsCommandComplete(pSd->pSdDriver));
    }
    return error;
}

/**
 * Continue to transfer datablocks from host to card until interrupted by a
 * STOP_TRANSMISSION command.
 * \param pSd  Pointer to a SD card driver instance.
 * \param blockSize Block size (shall be set to 512 in case of high capacity).
 * \param nbBlock   Number of blocks to send.
 * \param pData  Pointer to the application buffer to be filled.
 * \param fCallback Pointer to optional callback invoked on command end.
 *                  NULL:    Function return until command finished.
 *                  Pointer: Return immediately and invoke callback at end.
 * \param pArg Callback argument.
 */
uint8_t SdmmcWrite(SdCard   *pSd,
                   uint16_t blockSize,
                   uint16_t nbBlock,
                   const uint8_t *pData,
                   SdmmcCallback fCallback,
                   void* pArg)
{
    MciCmd *pCommand = &(mciCmd);
    uint8_t error;

    TRACE_DEBUG( "Write()\n\r" ) ;
    ResetMciCommand(pCommand);

    /* Fill command information */
    pCommand->blockSize = blockSize;
    pCommand->nbBlock   = nbBlock;
    pCommand->pData     = (uint8_t*)pData;

    pCommand->tranType = MCI_WRITE;
    pCommand->busyCheck = 1;

    pCommand->callback = fCallback;
    pCommand->pArg     = pArg;

    /* Send command */
    error = Sdmmc_SendCommand(pSd->pSdDriver, pCommand);
    if (fCallback == 0 && error == 0) {
        while(!Sdmmc_IsCommandComplete(pSd->pSdDriver));
    }
    return error;
}

/**
 * Return different properties of the SD/MMC low level driver.
 * \param pSd       Pointer to \ref SdCard instance.
 * \param property  Property code (\ref sdmmc_properties) that should return.
 * \param pExtData  Pointer to additional data for the property.
 */
uint32_t SdmmcGetProperty(SdCard * pSd,
                          uint32_t property,
                          void * pExtData)
{
    switch(property) {
        case SDMMC_PROP_BUS_MODE:
            return SDMMC_BUS_4_BIT;
        case SDMMC_PROP_HS_MODE:
            return 1;
        case SDMMC_PROP_BOOT_MODE:
            return 0;
    }
    return 0;
}

/**
 * Setup HW to use the desired slot.
 * \param pSd  Pointer to SdCard instance.
 * \param slot Desired slot.
 */
uint8_t SdmmcSetSlot(SdCard * pSd, uint8_t slot)
{
    return 0;
}

/**
 * Setup HW to use the desired bus width.
 * \param pSd Pointer to SdCard instance.
 * \param busWidth Desired bus width.
 */
uint32_t SdmmcSetBusWidth(SdCard * pSd, uint32_t busWidth)
{
    switch(busWidth) {
    case SDMMC_BUS_4_BIT: busWidth = HSMCI_SDCR_SDCBUS_4; break;
    case SDMMC_BUS_8_BIT: busWidth = HSMCI_SDCR_SDCBUS_8; break;
    default: busWidth = HSMCI_SDCR_SDCBUS_1;
    }
    return MCI_SetBusWidth(pSd->pSdDriver, busWidth);
}

/**
 * Setup HW to enable High-Speed mode
 * \param pSd Pointer to SdCard instance.
 * \param enable 1 to enable, 0 to disable.
 */
uint8_t SdmmcEnableHsMode(SdCard * pSd, uint8_t enable)
{
    return MCI_EnableHsMode(pSd->pSdDriver, enable);
}

/**
 * Setup HW to change to desired clock speed.
 * \param pSd Pointer to SdCard instance.
 * \param clock Clock frequence in Hz.
 * \return Actually running clock.
 */
uint32_t SdmmcSetSpeed(SdCard * pSd,uint32_t clock)
{
    return MCI_SetSpeed(pSd->pSdDriver, clock, BOARD_MCK);
}

/**
 * Processes pending events on the given MCI driver.
 * \param pMci  Pointer to a MCI driver instance.
 */
void Sdmmc_Handler(Mcid*pMci)
{
    Hsmci *pMciHw = pMci->pMciHw;
    MciCmd *pCommand = pMci->pCommand;
    volatile uint32_t SR;
    volatile uint32_t maskedSR;
    volatile uint32_t mask;
    uint8_t i;

    assert(pMci);
    assert(pMciHw);
    assert(pCommand);

    /* Read the status register */
    SR = pMciHw->HSMCI_SR;
    mask = pMciHw->HSMCI_IMR;
    maskedSR = SR & mask;

    //TRACE_INFO_WP("i%dS %x\n\r", (pCommand->cmd & HSMCI_CMDR_CMDNB_Msk), SR);
    //TRACE_INFO_WP("i%dM %x\n\r", (pCommand->cmd & HSMCI_CMDR_CMDNB_Msk), maskedSR);

    /* Check if an error has occured */
    if ((maskedSR & STATUS_ERRORS) != 0)
    {
        pCommand->state = SDMMC_CMD_ERROR;
        /* Check error code */
        if ((maskedSR & STATUS_ERRORS) == HSMCI_SR_RTOE)
        {
            pCommand->status = SDMMC_ERROR_NORESPONSE;
        }
        else
        {
            TRACE_INFO( "iERR %x: C%x(%d), R%d\n\r", maskedSR, pCommand->cmd, (pCommand->cmd & HSMCI_CMDR_CMDNB_Msk), pCommand->resType ) ;
            /* If the command is SEND_OP_COND the CRC error flag is always
               present (cf: R3,R4 response, no CRC) */
            if (   ((pCommand->resType == 3)||(pCommand->resType == 4))
                && (maskedSR & STATUS_ERRORS) == HSMCI_SR_RCRCE)
            {
            }
            else
            {
                pCommand->status = SDMMC_ERROR;
            }
        }
    }
    mask &= ~STATUS_ERRORS;

    /* Check if a command has been completed */
    if (maskedSR & HSMCI_SR_CMDRDY)
    {
        /* Command OK, disable interrupt */
        pMciHw->HSMCI_IDR = HSMCI_IDR_CMDRDY;

        /* Check BUSY */
        if (pCommand->busyCheck
           && (pCommand->tranType == MCI_NO_TRANSFER
              || (pCommand->tranType == MCI_STOP_TRANSFER)) )
        {
            /* Check XFRDONE to confirm no bus action */
            if (SR & HSMCI_SR_NOTBUSY)
            {
                mask &= ~(uint32_t)HSMCI_IMR_CMDRDY;
            }
            else
            {
                pMciHw->HSMCI_IER = HSMCI_SR_NOTBUSY;
            }
        }
        else
        {
            mask &= ~(uint32_t)HSMCI_IMR_CMDRDY;
        }
    }

    /* Check if transfer stopped */
    if (maskedSR & HSMCI_SR_XFRDONE)
    {
        mask &= ~(uint32_t)HSMCI_IMR_XFRDONE;
    }

    /* Check if not busy */
    if (maskedSR & HSMCI_SR_NOTBUSY)
    {
        mask &= ~(uint32_t)HSMCI_IMR_NOTBUSY;
    }

    /* Check if data read done */
    if (maskedSR & HSMCI_SR_RXBUFF)
    {
        mask &= ~(uint32_t)HSMCI_IMR_RXBUFF;
    }

    /* Check if TX ready */
    if (maskedSR & HSMCI_SR_TXRDY)
    {
        mask &= ~(uint32_t)HSMCI_IMR_TXRDY;
    }

    /* Check if FIFO empty (TX & All data sent) */
    if (maskedSR & HSMCI_SR_FIFOEMPTY)
    {
        /* Disable FIFO EMPTY */
        pMciHw->HSMCI_IDR = HSMCI_SR_FIFOEMPTY;
        /* Byte transfer no BLKE */
        if (pMciHw->HSMCI_MR & HSMCI_MR_FBYTE)
        {
            mask &= ~(uint32_t)HSMCI_IMR_FIFOEMPTY;
        }
        /* End command until no data on bus */
        else
        {
            if ((SR & HSMCI_SR_BLKE) == 0)
            {
                pMciHw->HSMCI_IER = HSMCI_IER_BLKE;
            }
            else
            {
                mask &= ~(uint32_t)HSMCI_IMR_FIFOEMPTY;
            }
        }
    }

    /* Check if BLKE (TX & no data on bus) */
    if (maskedSR & HSMCI_SR_BLKE)
    {
        if (SR & HSMCI_SR_TXRDY )
        {
            mask &= ~(uint32_t)HSMCI_SR_BLKE;
        }
        else
        {
            pMciHw->HSMCI_IDR = HSMCI_SR_BLKE;
            pMciHw->HSMCI_IER = HSMCI_SR_TXRDY;
        }
    }

    /* Check if data write finished */
    if (maskedSR & HSMCI_SR_TXBUFE)
    {
        /* Data OK, disable interrupt */
        pMciHw->HSMCI_IDR = HSMCI_IDR_TXBUFE;

        /* End command until no data on bus */
        if ((SR & HSMCI_SR_FIFOEMPTY) == 0)
        {
            pMciHw->HSMCI_IER = HSMCI_IER_FIFOEMPTY;
        }
        else
        {
            mask &= ~(uint32_t)HSMCI_SR_TXBUFE;
        }
    }

    /* All non-error mask done, complete the command */
    if (0 == mask || pCommand->state != SDMMC_CMD_PENDING)
    {
        /* Store the card response in the provided buffer */
        if (pCommand->pResp)
        {
            uint8_t resSize;
            switch (pCommand->resType)
            {
                case 1:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                    resSize = 1;
                break;

                case 2:
                    resSize = 4;
                break;

                default:
                    resSize = 0;
                break;
            }

            for (i=0; i < resSize; i++)
            {
                pCommand->pResp[i] = pMciHw->HSMCI_RSPR[0];
            }
        }

        /* If no error occured, the transfer is successful */
        if (pCommand->state == SDMMC_CMD_PENDING)
        {
            pCommand->state = 0;
        }
        /* Error ? Reset to remove transfer statuses */
        else
        {
            MCI_Reset(pMci, 1);
        }

        /* Disable PDC */
        pMciHw->HSMCI_PTCR = HSMCI_PTCR_RXTDIS | HSMCI_PTCR_TXTDIS;

        /* Disable interrupts */
        pMciHw->HSMCI_IDR = pMciHw->HSMCI_IMR;

        /* Disable peripheral */
        PMC_DisablePeripheral(pMci->mciId);

        /* Release the semaphore */
        pMci->semaphore++;

        /* Invoke the callback associated with the current command (if any) */
        if ( pCommand->callback )
        {
            (pCommand->callback)(pCommand->status, pCommand->pArg);
        }
    }
}

/**@}*/
