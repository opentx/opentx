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

/** \file */

/**
 * \page sdmmc_cmd SD/MMC Low Level Commands
 *
 * \section Purpose
 *
 * Implementation for SD/MMC low level driver. Supply a set of SD/MMC mode's
 * interface.
 *
 * \section sdmmc_prop_setup SD/MMC HW property and setup functions
 *
 * - SdmmcGetProperty()
 * - SdmmcSetBusWidth()
 * - SdmmcEnableHsMode()
 * - SdmmcSetSpeed()
 *
 * \section sdmmc_cmd_fun SD/MMC command functions
 *
 * - Functions for both SD/MMC
 *   - SdmmcPowerOn() : Perform SD/MMC power on initialization sequence.
 *   - SdmmcCmd0() : Resets all cards to idle state
 *   - SdmmcCmd2() : Asks any card to send the CID numbers on the CMD line
 *   - SdmmcCmd7() : Command toggles a card between the stand-by and transfer
 *                   states or between
 *   - SdmmcCmd9() : Addressed card sends its card-specific data (CSD) on the CMD
 *                   line
 *   - SdmmcCmd12() : Forces the card to stop transmission
 *   - SdmmcCmd13() : Addressed card sends its status register
 *   - SdmmcCmd16() : Set block length
 *   - SdmmcCmd17() : Read single block
 *   - SdmmcCmd18() : Read multiple blocks
 *   - SdmmcCmd24() : Write single block
 *   - SdmmcCmd25() : Write multiple blocks
 *   - SdmmcCmd55() : App command, should be sent before application specific
 *                    command
 *   - SdmmcRead() : Write data without any command
 *                   (usually follow SdmmcCmd18()).
 *   - SdmmcWrite() : Write data without any command
 *                    (usually follow SdmmcCmd25()).
 * - Functions for SD card.
 *   - SdCmd3() : Ask the card to publish a new relative address
 *   - SdCmd6() : SD Switch command.
 *   - SdCmd8() : Sends SD Memory Card interface condition, which includes host supply voltage
 *                information and asks the card whether card supports voltage
 *   - SdAcmd6() : Defines the data bus width
 *   - SdAcmd41() : Asks to all cards to send their operations conditions.
 *   - SdAcmd51() : Sends SD Card Configuration Register (SCR).
 * - Functions for MMC card
 *   - MmcCmd1() : MMC send operation condition command.
 *   - MmcCmd3() : Set a new relative address to MMC card.
 *   - MmcCmd6() : MMC Switch.
 *   - MmcCmd8() : Sends MMC EXT_CSD.
 */

#ifndef SDMMC_CMD_H
#define SDMMC_CMD_H

#include "hsmci/hsmci.h"

/** \addtogroup sdmmc_hal
 *@{
 */

/*----------------------------------------------------------------------------
 *         Constants
 *----------------------------------------------------------------------------*/

/** \addtogroup sdmmc_cardtype SD/MMC Card Types
 *  Here lists the SD/MMC card types.
 *  - Card Type Category Bitmap
 *    - \ref CARD_TYPE_bmHC
 *    - \ref CARD_TYPE_bmSDMMC
 *      - \ref CARD_TYPE_bmUNKNOWN
 *      - \ref CARD_TYPE_bmSD
 *      - \ref CARD_TYPE_bmMMC
 *    - \ref CARD_TYPE_bmSDIO
 *  - Card Types
 *    - \ref CARD_UNKNOWN
 *    - \ref CARD_SD
 *    - \ref CARD_SDHC
 *    - \ref CARD_MMC
 *    - \ref CARD_MMCHD
 *    - \ref CARD_SDIO
 *    - \ref CARD_SDCOMBO
 *    - \ref CARD_SDHCCOMBO
 *      @{*/
#define CARD_TYPE_bmHC           (1 << 0)   /**< Bit for High-Capacity(Density) */
#define CARD_TYPE_bmSDMMC        (0x3 << 1) /**< Bits mask for SD/MMC */
#define CARD_TYPE_bmUNKNOWN      (0x0 << 1) /**< Bits for Unknown card */
#define CARD_TYPE_bmSD           (0x1 << 1) /**< Bits for SD */
#define CARD_TYPE_bmMMC          (0x2 << 1) /**< Bits for MMC */
#define CARD_TYPE_bmSDIO         (1 << 3)   /**< Bit for SDIO */
/** Card can not be identified */
#define CARD_UNKNOWN    (0)
/** SD Card (0x2) */
#define CARD_SD         (CARD_TYPE_bmSD)
/** SD High Capacity Card (0x3) */
#define CARD_SDHC       (CARD_TYPE_bmSD|CARD_TYPE_bmHC)
/** MMC Card (0x4) */
#define CARD_MMC        (CARD_TYPE_bmMMC)
/** MMC High-Density Card (0x5) */
#define CARD_MMCHD      (CARD_TYPE_bmMMC|CARD_TYPE_bmHC)
/** SDIO only card (0x8) */
#define CARD_SDIO       (CARD_TYPE_bmSDIO)
/** SDIO Combo, with SD embedded (0xA) */
#define CARD_SDCOMBO    (CARD_TYPE_bmSDIO|CARD_SD)
/** SDIO Combo, with SDHC embedded (0xB) */
#define CARD_SDHCCOMBO  (CARD_TYPE_bmSDIO|CARD_SDHC)
/**     @}*/

/** \addtogroup sdmmc_rc SD/MMC Return Codes
 *  Here lists the SD/MMC driver return codes.
 *  - \ref SDMMC_ERROR
 *  - \ref SDMMC_ERROR_NORESPONSE
 *  - \ref SDMMC_ERROR_NOT_INITIALIZED
 *  - \ref SDMMC_ERROR_BUSY
 *  - \ref SDMMC_ERROR_PARAM
 *  - \ref SDMMC_ERROR_NOT_SUPPORT
 *  - \ref SDMMC_OK
 *  - \ref SDMMC_ERROR_LOCKED
 *      @{*/
/** No error */
#define SDMMC_OK                    0
/** The driver is locked. */
#define SDMMC_ERROR_LOCKED          1
/** There was an error with the SD driver. */
#define SDMMC_ERROR                 2
/** The SD card did not answer the command. */
#define SDMMC_ERROR_NORESPONSE      3
/** The SD card is not initialized. */
#define SDMMC_ERROR_NOT_INITIALIZED 4
/** The SD card is busy. */
#define SDMMC_ERROR_BUSY            5
/** The input parameter error */
#define SDMMC_ERROR_PARAM           6
/** The operation is not supported. */
#define SDMMC_ERROR_NOT_SUPPORT     0xFE
/**     @}*/

/** \addtogroup sdmmc_busmode SD/MMC Bus Modes
 *  Here lists the SD/MMC driver bus modes.
 *  - \ref SDMMC_BUS_1_BIT
 *  - \ref SDMMC_BUS_4_BIT
 *  - \ref SDMMC_BUS_8_BIT
 *      @{*/
/** Support 1-bit bus mode */
#define SDMMC_BUS_1_BIT             0x0UL
/** Support 4-bit bus mode */
#define SDMMC_BUS_4_BIT             0x1UL
/** Support 8-bit bus mode */
#define SDMMC_BUS_8_BIT             0x2UL
/**     @}*/

/** SD/MMC card block size in bytes. */
#define SDMMC_BLOCK_SIZE            512
/** SD/MMC card block size binary shift value. */
#define SDMMC_BLOCK_SIZE_SHIFT      9

/** SD/MMC command status: ready */
#define SDMMC_CMD_READY             0
/** SD/MMC command status: waiting command end */
#define SDMMC_CMD_PENDING           1
/** SD/MMC command status: error */
#define SDMMC_CMD_ERROR             2

/** \addtogroup sdmmc_properties SD/MMC Property Codes
 *  Here lists the property codes for SD/MMC low level driver to report to lib.
 *  - \ref SDMMC_PROP_BUS_MODE
 *  - \ref SDMMC_PROP_HS_MODE
 *  - \ref SDMMC_PROP_BOOT_MODE
 *      @{*/
/** SD/MMC Low Level Property: Bus mode */
#define SDMMC_PROP_BUS_MODE       0
/** SD/MMC Low Level Property: High-speed mode */
#define SDMMC_PROP_HS_MODE        1
/** SD/MMC Low Level Property: Boot mode */
#define SDMMC_PROP_BOOT_MODE      2
/**     @}*/

/*----------------------------------------------------------------------------
 *         Types
 *----------------------------------------------------------------------------*/

/** SD/MMC end-of-transfer callback function. */
typedef void (*SdmmcCallback)(uint8_t status, void *pArg);

/**
 * SD/MMC enumeration data buffers.
 */
typedef struct _SdEnumData {
    uint32_t cid[4]; /**< Card IDentification register */
    uint32_t csd[4]; /**< Card-Specific Data register */
    /** SD SCR (64 bit) + status (512 bit) or
        MMC EXT_CSD(512 bytes) register */
    uint32_t extData[512 / 4];
} SdEnumData;

/**
 * SD SPI mode extended functions.
 */
typedef struct _SdSpiFunctions {
    void *fCmd58;       /**< SD SPI Read OCR */
    void *fCmd59;       /**< SD SPI CRC on/off control */
    void *fStopToken;   /**< SD SPI Stop token */
    void *fDecodeResp;  /**< SD SPI response decoding */
} SdSpiFunctions;

/**
 * Sdmmc command operation settings.
 */
typedef union _SdmmcCmdOperation {
    uint8_t bVal;
    struct {
        uint8_t powerON:1, /**< Do power on initialize */
                sendCmd:1, /**< Send SD/MMC command */
                xfrData:2, /**< Send/Stop data transfer */
                respType:3,/**< Response type */
                crcON:1;   /**< CRC is used (SPI only) */
    } bmBits;
} SdmmcCmdOp;
#define SDMMC_CMD_bmPOWERON     (0x1     )
#define SDMMC_CMD_bmCOMMAND     (0x1 << 1)
#define SDMMC_CMD_bmDATAMASK    (0x3 << 2)
#define SDMMC_CMD_bmNODATA      (0x0 << 2)
#define SDMMC_CMD_RX             0x1
#define SDMMC_CMD_bmDATARX      (0x1 << 2)
#define SDMMC_CMD_TX             0x2
#define SDMMC_CMD_bmDATATX      (0x2 << 2)
#define SDMMC_CMD_bmSTOPXFR     (0x2 << 2)
#define SDMMC_CMD_bmRESPMASK    (0x7 << 4)
#define SDMMC_CMD_bmCRC         (0x1 << 7)
/* Do power on initialize */
#define SDMMC_CMD_POWERONINIT   (SDMMC_CMD_bmPOWERON)
/* Data only, read */
#define SDMMC_CMD_DATARX        (SDMMC_CMD_bmDATARX)
/* Data only, write */
#define SDMMC_CMD_DATATX        (SDMMC_CMD_bmDATATX)
/* Command without data */
#define SDMMC_CMD_CNODATA(R)    ( SDMMC_CMD_bmCOMMAND \
                                |(((R)&0x7)<<4))
/* Command with data, read */
#define SDMMC_CMD_CDATARX(R)    ( SDMMC_CMD_bmCOMMAND \
                                | SDMMC_CMD_bmDATARX \
                                | (((R)&0x7)<<4))
/* Command with data, write */
#define SDMMC_CMD_CDATATX(R)    ( SDMMC_CMD_bmCOMMAND \
                                | SDMMC_CMD_bmDATATX \
                                | (((R)&0x7)<<4))
/* Send Stop token for SPI */
#define SDMMC_CMD_STOPTOKEN     (SDMMC_CMD_bmSTOPXFR)

/**
 * Sdmmc command.
 */
typedef struct _SdmmcCommand {
    /** Optional user-provided callback function. */
    SdmmcCallback callback;
    /** Optional argument to the callback function. */
    void *pArg;
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
    /** Command argument. */
    uint32_t   arg;
    /**< Command index */
    uint8_t    cmd;
    /**< Command operation settings */
    SdmmcCmdOp cmdOp;
    /**< Command return status */
    uint8_t    status;
    /**< Command state */
    volatile uint8_t state;
} SdmmcCommand;

/**
 * \typedef SdCard
 * Sdcard driver structure. It holds the current command being processed and
 * the SD card address.
 */
typedef struct _SdCard
{
    /** Pointer to the underlying HW driver. */
    void *pSdDriver;
    /** Pointer to the extension data for SPI mode */
    void *pSpiExt;

    /** Card IDentification (CID register) */
    uint32_t cid[4];
    /** Card-specific data (CSD register) */
    uint32_t csd[4];
    /** SD SCR(64 bit), Status(512 bit) or
        MMC EXT_CSD(512 bytes) register */
    uint32_t extData[512 / 4];

    /** Card TRANS_SPEED: Max supported transfer speed */
    uint32_t transSpeed;

    /** Card total size */
    uint32_t totalSize;
    /** Card total number of blocks */
    uint32_t blockNr;

    /** Card option command support list */
    uint32_t optCmdBitMap;
    /** Previous access block number for memory. */
    uint32_t preBlock;
    /** Previous access block number for SDIO. */
    uint32_t preSdioBlock;

    /** SD card current access speed. */
    uint32_t accSpeed;
    /** SD card current access address. */
    uint16_t cardAddress;
    /** Card type */
    uint8_t cardType;
    /** Card access bus mode */
    uint8_t busMode;
    /** Card access slot */
    uint8_t cardSlot;
    /** Card State */
    uint8_t state;
} SdCard;


/** \addtogroup sdmmc_struct_cmdarg SD/MMC command arguments
 *  Here lists the command arguments for SD/MMC.
 *  - CMD6 Argument
 *    - \ref _MmcCmd6Arg "MMC CMD6"
 *    - \ref _SdCmd6Arg  "SD CMD6"
 *  - \ref _SdioCmd52Arg CMD52
 *  - \ref _SdioCmd53Arg CMD53
 *      @{*/
/**
 * \typedef MmcCmd6Arg
 * Argument for MMC CMD6
 */
typedef struct _MmcCmd6Arg
{
    uint8_t access;
    uint8_t index;
    uint8_t value;
    uint8_t cmdSet;
} MmcCmd6Arg, MmcSwitchArg;

/**
 * \typedef SdCmd6Arg
 * Argument for SD CMD6
 */
typedef struct _SdCmd6Arg
{
    uint32_t accessMode:4,  /**< [ 3: 0] function group 1, access mode */
             command:4,     /**< [ 7: 4] function group 2, command system */
             reserveFG3:4,  /**< [11: 8] function group 3, 0xF or 0x0 */
             reserveFG4:4,  /**< [15:12] function group 4, 0xF or 0x0 */
             reserveFG5:4,  /**< [19:16] function group 5, 0xF or 0x0 */
             reserveFG6:4,  /**< [23:20] function group 6, 0xF or 0x0 */
             reserved:7,    /**< [30:24] reserved 0 */
             mode:1;        /**< [31   ] Mode, 0: Check, 1: Switch */
} SdCmd6Arg, SdSwitchArg;

/**
 * \typedef SdioCmd52Arg
 * Argument for SDIO CMD52
 */
typedef struct _SdioCmd52Arg
{
    uint32_t data:8,        /**< [ 7: 0] data for writing */
             stuff0:1,      /**< [    8] reserved */
             regAddress:17, /**< [25: 9] register address */
             stuff1:1,      /**< [   26] reserved */
             rawFlag:1,     /**< [   27] Read after Write flag */
             functionNum:3, /**< [30:28] Number of the function */
             rwFlag:1;      /**< [   31] Direction, 1:write, 0:read. */
} SdioCmd52Arg, SdioRwDirectArg;
/**
 * \typedef SdioCmd53Arg
 * Argument for SDIO CMD53
 */
typedef struct _SdioCmd53Arg {
    uint32_t count:9,       /**< [ 8: 0] Byte mode: number of bytes to transfer,
                                                   0 cause 512 bytes transfer.
                                         Block mode: number of blocks to transfer,
                                                    0 set count to infinite. */
             regAddress:17, /**< [25: 9] Start Address I/O register */
             opCode:1,      /**< [   26] 1:Incrementing address, 0: fixed */
             blockMode:1,   /**< [   27] (Optional) 1:block mode */
             functionNum:3, /**< [30:28] Number of the function */
             rwFlag:1;      /**< [   31] Direction, 1:WR, 0:RD */
} SdioCmd53Arg, SdioRwExtArg;
/**     @}*/


/** \addtogroup sdmmc_resp_struct SD/MMC Responses Structs
 *  Here lists the command responses for SD/MMC.
 *  - \ref _SdmmcR1 "R1"
 *  - \ref _SdmmcR3 "R3"
 *  - \ref _MmcR4 "MMC R4", \ref _SdioR4 "SDIO R4"
 *  - \ref _SdmmcR5 "R5"
 *  - \ref _SdR6 "R6"
 *  - \ref _SdR7 "R7"
 *      @{*/
/**
 * Response R1 (normal response command)
 */
typedef struct _SdmmcR1
{
    uint32_t cardStatus;    /**< [32: 0] Response card status flags */
} SdmmcR1, SdmmcR1b;

/**
 * Response R3 (OCR)
 */
typedef struct _SdmmcR3
{
    uint32_t OCR;           /**< [32: 0] OCR register */
} SdmmcR3;

/**
 * Response R4 (MMC Fast I/O CMD39)
 */
typedef struct _MmcR4
{
    uint32_t regData:8,     /**< [ 7: 0] Read register contents */
             regAddr:7,     /**< [14: 8] Register address */
             status:1,      /**< [   15] Status */
             RCA:16;        /**< [31:16] RCA */
} MmcR4;

/**
 * Response R4 (SDIO), no CRC.
 */
typedef struct _SdioR4
{
    uint32_t OCR:24,            /**< [23: 0]       Operation Conditions Register */
             reserved:3,        /**< [26:24]       Reserved */
             memoryPresent:1,   /**< [   27] MP    Set to 1 if contains
                                                   SD memory */
             nbIoFunction:3,    /**< [30:28] NF    Total number of I/O functions
                                                   supported */
             C:1;               /**< [   31] IORDY Set to 1 if card is ready */
} SdioR4;

/**
 * Response R5 (MMC Interrupt request CMD40 / SDIO CMD52)
 */
typedef struct _SdmmcR5
{
    uint32_t data:8,        /**< [ 7: 0] Response data */
             response:8,    /**< [15: 8] Response status flags */
             RCA:16;        /**< [31:16] (MMC) Winning card RCA */
} SdmmcR5;

/**
 * Response R6 (SD RCA)
 */
typedef struct _SdR6
{
    uint32_t status:16,     /**< [15: 0] Response status */
             RCA:16;        /**< [31:16] New published RCA */
} SdR6;
/**
 * Response R7 (Card interface condition)
 */
typedef struct _SdR7 {
    uint32_t checkPatten:8, /**< [ 7: 0] Echo-back of check pattern */
             voltage:4,     /**< [11: 8] Voltage accepted */
             reserved:20;   /**< [31:12] reserved bits */
} SdR7;

/**     @}*/

/*----------------------------------------------------------------------------
 *         Exported functions
 *----------------------------------------------------------------------------*/
extern uint8_t MmcCmd1(SdCard * pSd,uint32_t * pOCR,SdmmcCallback fCallback);
extern uint8_t MmcCmd3(SdCard * pSd, uint16_t cardAddr,SdmmcCallback fCallback);
extern uint8_t MmcCmd6(SdCard * pSd, const void * pSwitchArg, uint32_t * pResp,SdmmcCallback fCallback);
extern uint8_t MmcCmd8(SdCard * pSd,uint8_t * pEXT,SdmmcCallback fCallback);
extern uint8_t SdAcmd13(SdCard * pSd,uint32_t * pSdSTAT,SdmmcCallback fCallback);
extern uint8_t SdAcmd41(SdCard * pSd,uint32_t * pIo,SdmmcCallback fCallback);
extern uint8_t SdAcmd51(SdCard * pSd,uint32_t * pSCR,SdmmcCallback fCallback);
extern uint8_t SdAcmd6(SdCard * pSd, uint32_t arg, uint32_t * pStatus,SdmmcCallback fCallback);
extern uint8_t SdCmd3(SdCard * pSd,uint32_t * pRsp, SdmmcCallback fCallback);
extern uint8_t SdCmd6(SdCard * pSd, const void * pSwitchArg,uint32_t * pStatus,uint32_t * pResp, SdmmcCallback fCallback);
extern uint8_t SdCmd8(SdCard * pSd,uint8_t supplyVoltage,SdmmcCallback fCallback);
extern uint8_t SdioCmd5(SdCard * pSd, uint32_t * pIoData,SdmmcCallback fCallback);
extern uint8_t SdioCmd52(SdCard * pSd, uint32_t * pIoData,SdmmcCallback fCallback);
extern uint8_t SdioCmd53(SdCard * pSd, uint32_t * pArgResp,uint8_t * pData,uint32_t size, SdmmcCallback fCallback,void * pArg);
extern void Sdmmc_Handler(Mcid*pMci);
extern uint8_t Sdmmc_IsCommandComplete( Mcid *pMci );
extern uint8_t Sdmmc_SendCommand(Mcid*pMci, MciCmd *pCommand);
extern uint8_t SdmmcCmd0(SdCard * pSd, uint32_t arg, SdmmcCallback fCallback);
extern uint8_t SdmmcCmd12(SdCard * pSd, uint32_t * pStatus,SdmmcCallback fCallback);
extern uint8_t SdmmcCmd13(SdCard * pSd, uint16_t cardAddr, uint32_t * pStatus,SdmmcCallback fCallback);
extern uint8_t SdmmcCmd16(SdCard * pSd, uint16_t blockLength,SdmmcCallback fCallback);
extern uint8_t SdmmcCmd17(SdCard * pSd, uint16_t blockSize, uint8_t * pData, uint32_t address,
                            uint32_t * pStatus,SdmmcCallback fCallback);
extern uint8_t SdmmcCmd18( SdCard * pSd, uint16_t blockSize,uint16_t nbBlocks,uint8_t * pData,uint32_t address,
                            uint32_t * pStatus,SdmmcCallback fCallback);
extern uint8_t SdmmcCmd2(SdCard * pSd, uint32_t * pCID, SdmmcCallback fCallback);
extern uint8_t SdmmcCmd24(SdCard * pSd, uint16_t blockSize, uint8_t * pData, uint32_t address,
                            uint32_t * pStatus,SdmmcCallback fCallback);
extern uint8_t SdmmcCmd25(SdCard * pSd, uint16_t blockSize,uint16_t nbBlock,uint8_t * pData,uint32_t address,
                            uint32_t * pStatus,SdmmcCallback fCallback);
extern uint8_t SdmmcCmd55(SdCard * pSd, uint16_t cardAddr,SdmmcCallback fCallback);
extern uint8_t SdmmcCmd7(SdCard * pSd, uint16_t cardAddr, SdmmcCallback fCallback);
extern uint8_t SdmmcCmd9(SdCard * pSd, uint16_t cardAddr, uint32_t * pCSD,SdmmcCallback fCallback);
extern uint8_t SdmmcEnableHsMode(SdCard *pSd, uint8_t enable);
extern uint32_t SdmmcGetProperty(SdCard *pSd, uint32_t property, void * pExtData);
extern uint8_t SdmmcPowerOn(SdCard * pSd,SdmmcCallback fCallback);
extern uint8_t SdmmcRead(SdCard * pSd, uint16_t blockSize, uint16_t nbBlock, uint8_t * pData, SdmmcCallback fCallback,void * pArg);
extern uint32_t SdmmcSetBusWidth(SdCard * pSd, uint32_t busWidth);
extern uint8_t SdmmcSetSlot(SdCard * pSd, uint8_t slot);
extern uint32_t SdmmcSetSpeed(SdCard *pSd, uint32_t clock);
extern uint8_t SdmmcWrite(SdCard * pSd, uint16_t blockSize, uint16_t nbBlock, const uint8_t * pData, SdmmcCallback fCallback,void * pArg);

// BSS added!!!!


//------------------------------------------------------------------------------
//         Constants
//------------------------------------------------------------------------------


/// There was an error with the SD driver.
#define SD_ERROR_DRIVER          1
/// The SD card did not answer the command.
#define SD_ERROR_NORESPONSE      2
/// The SD card did not answer the command.
#define SD_ERROR_NOT_INITIALIZED 3
/// The SD card is busy
#define SD_ERROR_BUSY            4
/// The operation is not supported
#define SD_ERROR_NOT_SUPPORT     5

/// SD card block size in bytes.
#define SD_BLOCK_SIZE           512
/// SD card block size binary shift value
#define SD_BLOCK_SIZE_BIT       9

//- MMC Card Command Types
/// Broadcast commands (bc), no response
#define MMC_CCT_BC             0
/// Broadcase commands with response (bcr)
#define MMC_CCT_BCR            1
/// Addressed commands (ac), no data transfer on DAT lines
#define MMC_CCT_AC             2
/// Addressed data transfer commands (adtc), data transfer on DAT lines
#define MMC_CCT_ADTC           3

//- MMC Card Command Classes (CCC)
/// Class 0: basic
#define MMC_CCC_BASIC               0
/// Class 1: stream read
#define MMC_CCC_STREAM_READ         1
/// Class 2: block read
#define MMC_CCC_BLOCK_READ          2
/// Class 3: stream write
#define MMC_CCC_STREAM_WRITE        3
/// Class 4: block write
#define MMC_CCC_BLOCK_WRITE         4
/// Class 5: erase
#define MMC_CCC_ERASE               5
/// Class 6: write protection
#define MMC_CCC_WRITE_PROTECTION    6
/// Class 7: lock card
#define MMC_CCC_LOCK_CARD           7
/// Class 8: application specific
#define MMC_CCC_APP_SPEC            8
/// Class 9: I/O mode
#define MMC_CCC_IO_MODE             9

//- MMC/SD Card Command Response Type
/// R1 (normal response command), 48bits
#define MMC_RESP_R1                 1
#define SD_RESP_R1                  MMC_RESP_R1
/// R1b: busy signal transmitted on the data line DAT0
#define MMC_RESP_R1b                1
#define SD_RESP_R1b                 MMC_RESP_R1b
/// R2: 136bits, CID, CSD register
#define MMC_RESP_R2                 2
#define SD_RESP_R2                  MMC_RESP_R2
/// R3: 48bits, OCR
#define MMC_RESP_R3                 3
#define SD_RESP_R3                  MMC_RESP_R3
/// R4 (Fast I/O), 48bits
#define MMC_RESP_R4                 4
/// R5 (Interrupt request), 48bits
#define MMC_RESP_R5                 5
/// R6 (Published RCA response), 48bits
#define SD_RESP_R6                  6
/// R7 (Card interface condition), 48bits
#define SD_RESP_R7                  7

//- MMC Card CMD6 access mode
#define MMC_SWITCH_CMDSET           0
#define MMC_SWITCH_SETBITS          1
#define MMC_SWITCH_CLRBITS          2
#define MMC_SWITCH_WRITE            3

//-MMC Boot partition enable
/// Boot partition 1 enabled for boot
#define MMC_BOOT_PART_1             1
/// Boot partition 2 enabled for boot
#define MMC_BOOT_PART_2             2
/// User area enabled for boot
#define MMC_BOOT_PART_USER          7

//-MMC Boot partition access
/// R/W boot partition 1
#define MMC_BOOT_ACC_PART1          1
/// R/W boot partition 2
#define MMC_BOOT_ACC_PART2          2


typedef void (*SdCallback)(unsigned char status, void *pCommand);

typedef struct _SdCmd {

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
    unsigned char tranType;
    /// Indicates if the command is a read operation.
    unsigned char isRead;
    /// Response buffer.
    unsigned int  *pResp;
    /// SD card response type.
    unsigned char  resType;
    /// Optional user-provided callback function.
    SdCallback callback;
    /// Optional argument to the callback function.
    void *pArg;

} SdCmd;

/**@}*/
#endif //#ifndef SDMMC_CMD_H

