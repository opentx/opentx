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

/** \addtogroup hsmci_module Working with HSMCI
 * The HSMCI driver provides the interface to configure and use the HSMCI
 * peripheral.
 *
 * The user needs to set the number of wait states depending on the frequency used.\n
 * Configure number of cycles for flash read/write operations in the FWS field of HSMCI_FMR.
 *
 * It offers a function to send flash command to HSMCI and waits for the
 * flash to be ready.
 *
 * To send flash command, the user could do in either of following way:
 * <ul>
 * <li>Write a correct key, command and argument in HSMCI_FCR. </li>
 * <li>Or, Use IAP (In Application Programming) function which is executed from
 * ROM directly, this allows flash programming to be done by code running in flash.</li>
 * <li>Once the command is achieved, it can be detected even by polling EEFC_FSR or interrupt.
 * </ul>
 *
 * The command argument could be a page number,GPNVM number or nothing, it depends on
 * the command itself. Some useful functions in this driver could help user tranlate physical
 * flash address into a page number and vice verse.
 *
 * For more accurate information, please look at the EEFC section of the
 * Datasheet.
 *
 * Related files :\n
 * \ref hsmci_pdc.c\n
 * \ref hsmci.h.\n
*/
/*@{*/
/*@}*/

/**
 *  \file
 */

/**
 *
 * \section Purpose
 *
 * Implement MultiMediaCard(MCI) Interface Driver
 *
 * \section Usage
 *
 * -# MCI_Init(): Initializes a MCI driver instance and the underlying
 *                peripheral.
  * -# MCI_Handler() : Interrupt handler which is called by ISR handler.
 * -# MCI_SetSpeed() : Configure the  MCI CLKDIV in the _MR register
 *                     (\ref Hsmci::HSMCI_MR).
 * -# MCI_SetBusWidth() : Configure the  MCI SDCBUS in the _SDCR register
 *                        (\ref Hsmci::HSMCI_SDCR).
 * -# MCI_EnableHsMode() : Configure the MCI HSMODE in the _CFG register
 *                         (\ref Hsmci::HSMCI_CFG).
 */

#ifndef HSMCID_H
#define HSMCID_H
/** \addtogroup sdmmc_hal
 *@{
 */

/*----------------------------------------------------------------------------
 *         Headers
 *----------------------------------------------------------------------------*/

#include <stdint.h>
#include <stdio.h>
#include "board.h"

/*----------------------------------------------------------------------------
 *         Constants
 *----------------------------------------------------------------------------*/

/* Transfer type */

/** MultiMedia Transfer type: no data */
#define MCI_NO_TRANSFER     0
/** MultiMedia Transfer type: Device to Host (read) */
#define MCI_START_READ      1
/** MultiMedia Transfer type: Host to Device (write) & check BUSY */
#define MCI_START_WRITE     2
/** Device to Host (read) without command */
#define MCI_READ            3
/** Host to Device (write) without command & check BUSY */
#define MCI_WRITE           4
/** MultiMedia Transfer type: STOP transfer */
#define MCI_STOP_TRANSFER   5

/** MCI Initialize clock 400K Hz */
#define MCI_INITIAL_SPEED   400000

/*----------------------------------------------------------------------------
 *         Types
 *----------------------------------------------------------------------------*/

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * \brief MCI Transfer Request prepared by the application upper layer.
 *
 * This structure is sent to the Sdmmc_SendCommand function to start the transfer.
 * At the end of the transfer, the callback is invoked.
 */
typedef struct _MciCmd {

    /** Command code. */
    uint32_t cmd;
    /** Command argument. */
    uint32_t arg;
    /** Data buffer, with MCI_DMA_ENABLE defined 1, the buffer can be
     * 1, 2 or 4 bytes aligned. It has to be 4 byte aligned if no DMA.
     */
    uint8_t *pData;
    /** Size of data block in bytes. */
    uint16_t blockSize;
    /** Number of blocks to be transfered */
    uint16_t nbBlock;
    /** Response buffer. */
    uint32_t  *pResp;
    /** Optional user-provided callback function. */
    void (*callback)( uint8_t status, void *pArg ) ;
    /** Optional argument to the callback function. */
    void *pArg;
    /** SD card command option. */
    uint8_t resType:7,      /** Response */
            busyCheck:1;    /** Check busy as end of command */
    /** Indicate transfer type */
    uint8_t tranType;

    /** Indicate end of transfer status */
    uint8_t status;

    /** Command state. */
    volatile uint8_t state;
} MciCmd;

/**
 *  \brief MCI Driver
 */
typedef struct
{
    /** Pointer to a MCI peripheral. */
    Hsmci *pMciHw;
    /** Pointer to currently executing command. */
    MciCmd *pCommand;
    /** MCI peripheral identifier. */
    uint8_t mciId;
    /** Mutex. */
    volatile uint8_t semaphore;
} Mcid;

/*----------------------------------------------------------------------------
 *         Exported functions
 *----------------------------------------------------------------------------*/

extern void MCI_Disable(Hsmci *pMciHw);
extern void MCI_Enable(Hsmci *pMciHw);
extern uint8_t MCI_EnableHsMode(Mcid * pMci, uint8_t hsEnable);
extern void MCI_Init( Mcid  *pMci, Hsmci *pMciHw, uint8_t mciId, uint32_t dwMCk ) ;
extern uint32_t MCI_SetBusWidth(Mcid *pMci, uint32_t busWidth);
extern uint32_t MCI_SetSpeed(Mcid *pMci, uint32_t mciSpeed, uint32_t mck);

/* pdc if used */
extern void MCI_Reset(Mcid *pMci, uint8_t keepSettings);

#ifdef __cplusplus
}
#endif

/**@}*/
#endif //#ifndef HSMCID_H

