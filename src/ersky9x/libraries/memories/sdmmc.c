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

/** \addtogroup sdmmc_api
 *  @{
 */

/*----------------------------------------------------------------------------
 *         Headers
 *----------------------------------------------------------------------------*/


#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "board.h"
#include "sdmmc.h"
#include "debug.h"

#define assert(...)
/*----------------------------------------------------------------------------
 *         Global variables
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *         Local constants
 *----------------------------------------------------------------------------*/

/** \addtogroup sdmmc_op_state SD/MMC operation states
 *      @{*/
#define SD_STATE_IDLE        0
#define SD_STATE_INIT        1
#define SD_STATE_READY       2
#define SD_STATE_READ     0x10
#define SD_STATE_RD_RDY   0x11
#define SD_STATE_RD_BSY   0x12
#define SD_STATE_WRITE    0x20
#define SD_STATE_WR_RDY   0x21
#define SD_STATE_WR_BSY   0x22
#define SD_STATE_BOOT     0x30
/**     @}*/

/** \addtogroup sdmmc_status_bm SD/MMC Status register constants
 *      @{*/
#define STATUS_APP_CMD          (1UL << 5)
#define STATUS_SWITCH_ERROR     (1UL << 7)
#define STATUS_READY_FOR_DATA   (1UL << 8)
#define STATUS_IDLE             (0UL << 9)
#define STATUS_READY            (1UL << 9)
#define STATUS_IDENT            (2UL << 9)
#define STATUS_STBY             (3UL << 9)
#define STATUS_TRAN             (4UL << 9)
#define STATUS_DATA             (5UL << 9)
#define STATUS_RCV              (6UL << 9)
#define STATUS_PRG              (7UL << 9)
#define STATUS_DIS              (8UL << 9)
#define STATUS_STATE            (0xFUL << 9)
#define STATUS_ERASE_RESET       (1UL << 13)
#define STATUS_WP_ERASE_SKIP     (1UL << 15)
#define STATUS_CIDCSD_OVERWRITE  (1UL << 16)
#define STATUS_OVERRUN           (1UL << 17)
#define STATUS_UNERRUN           (1UL << 18)
#define STATUS_ERROR             (1UL << 19)
#define STATUS_CC_ERROR          (1UL << 20)
#define STATUS_CARD_ECC_FAILED   (1UL << 21)
#define STATUS_ILLEGAL_COMMAND   (1UL << 22)
#define STATUS_COM_CRC_ERROR     (1UL << 23)
#define STATUS_UN_LOCK_FAILED    (1UL << 24)
#define STATUS_CARD_IS_LOCKED    (1UL << 25)
#define STATUS_WP_VIOLATION      (1UL << 26)
#define STATUS_ERASE_PARAM       (1UL << 27)
#define STATUS_ERASE_SEQ_ERROR   (1UL << 28)
#define STATUS_BLOCK_LEN_ERROR   (1UL << 29)
#define STATUS_ADDRESS_MISALIGN  (1UL << 30)
#define STATUS_ADDR_OUT_OR_RANGE (1UL << 31)

#define STATUS_STOP ((uint32_t)( STATUS_CARD_IS_LOCKED \
                        | STATUS_COM_CRC_ERROR \
                        | STATUS_ILLEGAL_COMMAND \
                        | STATUS_CC_ERROR \
                        | STATUS_ERROR \
                        | STATUS_STATE \
                        | STATUS_READY_FOR_DATA ))

#define STATUS_WRITE ((uint32_t)( STATUS_ADDR_OUT_OR_RANGE \
                        | STATUS_ADDRESS_MISALIGN \
                        | STATUS_BLOCK_LEN_ERROR \
                        | STATUS_WP_VIOLATION \
                        | STATUS_CARD_IS_LOCKED \
                        | STATUS_COM_CRC_ERROR \
                        | STATUS_ILLEGAL_COMMAND \
                        | STATUS_CC_ERROR \
                        | STATUS_ERROR \
                        | STATUS_ERASE_RESET \
                        | STATUS_STATE \
                        | STATUS_READY_FOR_DATA ))

#define STATUS_READ  ((uint32_t)( STATUS_ADDR_OUT_OR_RANGE \
                        | STATUS_ADDRESS_MISALIGN \
                        | STATUS_BLOCK_LEN_ERROR \
                        | STATUS_CARD_IS_LOCKED \
                        | STATUS_COM_CRC_ERROR \
                        | STATUS_ILLEGAL_COMMAND \
                        | STATUS_CARD_ECC_FAILED \
                        | STATUS_CC_ERROR \
                        | STATUS_ERROR \
                        | STATUS_ERASE_RESET \
                        | STATUS_STATE \
                        | STATUS_READY_FOR_DATA ))

#define STATUS_SD_SWITCH ((uint32_t)( STATUS_ADDR_OUT_OR_RANGE \
                            | STATUS_CARD_IS_LOCKED \
                            | STATUS_COM_CRC_ERROR \
                            | STATUS_ILLEGAL_COMMAND \
                            | STATUS_CARD_ECC_FAILED \
                            | STATUS_CC_ERROR \
                            | STATUS_ERROR \
                            | STATUS_UNERRUN \
                            | STATUS_OVERRUN \
                            /*| STATUS_STATE*/))

#define STATUS_MMC_SWITCH ((uint32_t)( STATUS_CARD_IS_LOCKED \
                            | STATUS_COM_CRC_ERROR \
                            | STATUS_ILLEGAL_COMMAND \
                            | STATUS_CC_ERROR \
                            | STATUS_ERROR \
                            | STATUS_ERASE_RESET \
                            /*| STATUS_STATE*/ \
                            /*| STATUS_READY_FOR_DATA*/ \
                            | STATUS_SWITCH_ERROR ))
/**     @}*/

/** \addtogroup sdio_status_bm SDIO Status definitions
 *      @{*/
/** The CRC check of the previous command failed. */
#define SDIO_COM_CRC_ERROR   (1UL << 15)
/** Command not legal for the card state. */
#define SDIO_ILLEGAL_COMMAND (1UL << 14)
/** A general or an unknown error occurred during the operation. */
#define SDIO_R6_ERROR        (1UL << 13)
/** SDIO state (in R5) */
#define SDIO_R5_STATE        (3UL << 12)/**< SDIO R5 state mask */
#define SDIO_R5_STATE_DIS   (0UL << 12) /**< Disabled */
#define SDIO_R5_STATE_CMD   (1UL << 12) /**< DAT lines free */
#define SDIO_R5_STATE_TRN   (2UL << 12) /**< Transfer */
#define SDIO_R5_STATE_RFU   (3UL << 12) /**< Reserved */
#define SDIO_R5_ERROR        (1UL << 11)/**< General error */
#define SDIO_R5_FUNC_NUM     (1UL << 10)/**< Invalid function number */
#define SDIO_R5_OUT_OF_RANGE (1UL << 9) /**< Argument out of range */

/** Status bits mask for SDIO R6 */
#define STATUS_SDIO_R6  (SDIO_COM_CRC_ERROR \
                         | SDIO_ILLEGAL_COMMAND \
                         | SDIO_R6_ERROR)
/** Status bits mask for SDIO R5 */
#define STATUS_SDIO_R5  (0/*SDIO_R5_STATE*/ \
                         | SDIO_R5_ERROR \
                         | SDIO_R5_FUNC_NUM \
                         | SDIO_R5_OUT_OF_RANGE)
/**     @}*/

/** \addtogroup sdmmc_ocr_bm OCR Register definitions
 *      @{*/
#define OCR_VDD_16_17          ((uint32_t)(1 <<  4))
#define OCR_VDD_17_18          ((uint32_t)(1 <<  5))
#define OCR_VDD_18_19          ((uint32_t)(1 <<  6))
#define OCR_VDD_19_20          ((uint32_t)(1 <<  7))
#define OCR_VDD_20_21          ((uint32_t)(1 <<  8))
#define OCR_VDD_21_22          ((uint32_t)(1 <<  9))
#define OCR_VDD_22_23          ((uint32_t)(1 << 10))
#define OCR_VDD_23_24          ((uint32_t)(1 << 11))
#define OCR_VDD_24_25          ((uint32_t)(1 << 12))
#define OCR_VDD_25_26          ((uint32_t)(1 << 13))
#define OCR_VDD_26_27          ((uint32_t)(1 << 14))
#define OCR_VDD_27_28          ((uint32_t)(1 << 15))
#define OCR_VDD_28_29          ((uint32_t)(1 << 16))
#define OCR_VDD_29_30          ((uint32_t)(1 << 17))
#define OCR_VDD_30_31          ((uint32_t)(1 << 18))
#define OCR_VDD_31_32          ((uint32_t)(1 << 19))
#define OCR_VDD_32_33          ((uint32_t)(1 << 20))
#define OCR_VDD_33_34          ((uint32_t)(1 << 21))
#define OCR_VDD_34_35          ((uint32_t)(1 << 22))
#define OCR_VDD_35_36          ((uint32_t)(1 << 23))

#define SDMMC_HOST_VOLTAGE_RANGE     (OCR_VDD_27_28 +\
                                      OCR_VDD_28_29 +\
                                      OCR_VDD_29_30 +\
                                      OCR_VDD_30_31 +\
                                      OCR_VDD_31_32 +\
                                      OCR_VDD_32_33)

#define OCR_BIT2930            (3UL << 29)
#define OCR_MMC_HIGH_DENSITY   (0x2UL << 29)
#define OCR_MMC_HD             (0x2UL << 29)

#define OCR_SDIO_MP            (1UL << 27)
#define OCR_SDIO_NF            (7UL << 28)

#define OCR_SD_CCS             (1UL << 30)

#define OCR_POWER_UP_BUSY      (1UL << 31)
/**     @}*/

// Optional commands
#define SD_ACMD6_SUPPORT        ((uint32_t)1 << 0)
#define SD_ACMD13_SUPPORT       ((uint32_t)1 << 1)
#define SD_ACMD41_SUPPORT       ((uint32_t)1 << 2)
#define SD_ACMD51_SUPPORT       ((uint32_t)1 << 3)
#define SD_CMD16_SUPPORT        ((uint32_t)1 << 8)

/*----------------------------------------------------------------------------
 *         Macros
 *----------------------------------------------------------------------------*/

/** Return SD/MMC card address */
#define CARD_ADDR(pSd)          (pSd->cardAddress)

/** Return SD/MMC card block size (Default size now, 512B) */
#define BLOCK_SIZE(pSd)         (SDMMC_BLOCK_SIZE)

/** Convert block address to SD/MMC command parameter */
#define SD_ADDRESS(pSd, address) \
    ( ((pSd)->totalSize == 0xFFFFFFFF) ? \
                            (address):((address) << SDMMC_BLOCK_SIZE_SHIFT) )

/** Check if SD Spec version 1.10 or later */
#define SD_IsVer1_10(pSd) \
    ( SD_SCR_SD_SPEC(pSd) >= SD_SCR_SD_SPEC_1_10 )

/** Check if SD card support HS mode (1.10 or later) */
#define SD_IsHsModeSupported(pSd)  \
    ( SD_IsVer1_10(pSd)||(SD_CSD_STRUCTURE(pSd)>=1) )

/** Check if SD card support 4-bit mode (All SD card) */
#define SD_IsBusModeSupported(pSd) (1)

/** Check if MMC Spec version 4 */
#define MMC_IsVer4(pSd)     ( SD_CSD_SPEC_VERS(pSd) >= 4 )

/** Check if MMC CSD structure is 1.2 (3.1 or later) */
#define MMC_IsCSDVer1_2(pSd) \
    (  (SD_CSD_STRUCTURE(pSd)==2) \
     ||(SD_CSD_STRUCTURE(pSd)>2&&SD_EXTCSD_CSD_STRUCTURE(pSd)>=2) )

/** Check if MMC card support boot mode (4.3 or later) */
#define MMC_IsBootModeSupported(pSd) \
    (  (MMC_IsVer4(pSd)&&(SD_CID_CBX(pSd)==0x01)  )

/** Check if MMC card support 8-bit mode (4.0 or later) */
#define MMC_IsBusModeSupported(pSd) (MMC_IsVer4(pSd))

/** Check if MMC card support HS mode (4.0 or later) */
#define MMC_IsHsModeSupported(pSd)  \
    (MMC_IsCSDVer1_2(pSd)&&(SD_EXTCSD_CARD_TYPE(pSd)&0x2))

/*----------------------------------------------------------------------------
 *         Local variables
 *----------------------------------------------------------------------------*/

/** SD/MMC transfer rate unit codes (10K) list */
static const uint32_t sdmmcTransUnits[7] = {
    10, 100, 1000, 10000,
    0, 0, 0
};

/** SD transfer multiplier factor codes (1/10) list */
static const uint32_t sdTransMultipliers[16] = {
    0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80
};

/** MMC transfer multiplier factor codes (1/10) list */
static const uint32_t mmcTransMultipliers[16] = {
    0, 10, 12, 13, 15, 20, 26, 30, 35, 40, 45, 52, 55, 60, 70, 80
};

/*----------------------------------------------------------------------------
 *         Local functions
 *----------------------------------------------------------------------------*/

/**
 * Delay some loop
 */
static void Delay(volatile unsigned int loop)
{
    for(;loop > 0; loop --);
}


/**
 * Find SDIO ManfID, Fun0 tuple.
 * \param pSd         Pointer to \ref SdCard instance.
 * \param address     Search area start address.
 * \param size        Search area size.
 * \param pAddrManfID Pointer to ManfID address value buffer.
 * \param pAddrFunc0  Pointer to Func0 address value buffer.
 */
static uint8_t SdioFindTuples(SdCard *pSd,
                              uint32_t address, uint32_t size,
                              uint32_t *pAddrManfID,
                              uint32_t *pAddrFunc0)
{
    uint8_t error, tmp[3];
    uint32_t addr = address;
    uint8_t  flagFound = 0; /* 1:Manf, 2:Func0 */
    uint32_t addManfID = 0, addFunc0 = 0;
    for(;flagFound != 3;) {
        error = SDIO_ReadDirect(pSd, SDIO_CIA, addr, tmp, 3);
        if (error) {
            TRACE_ERROR("SdioFindTuples.RdDirect: %u\n\r", error);
            return error;
        }
        /* End */
        if (tmp[0] == CISTPL_END) break;
        /* ManfID */
        else if (tmp[0] == CISTPL_MANFID) {
            flagFound |= 1; addManfID = addr;
        }
        /* Func0 */
        else if (tmp[0] == CISTPL_FUNCE && tmp[2] == 0x00) {
            flagFound |= 2; addFunc0 = addr;
        }
        /* Tuple error ? */
        if (tmp[1] == 0) break;
        /* Next address */
        addr += (tmp[1] + 2);
        if (addr > (address + size)) break;
    }
    if (pAddrManfID) *pAddrManfID = addManfID;
    if (pAddrFunc0)  *pAddrFunc0  = addFunc0;
    return 0;
}

/**
 * \brief Decode Trans Speed Value
 * \param code The trans speed code value.
 * \param unitCodes  Unit list in 10K, 0 as unused value.
 * \param multiCodes Multiplier list in 1/10, index 1 ~ 15 is valid.
 */
static uint32_t SdmmcDecodeTransSpeed(uint32_t code,
                                      const uint32_t *unitCodes,
                                      const uint32_t *multiCodes)
{
    uint32_t speed;
    uint8_t unitI, mulI;

    /* Unit code is valid ? */
    unitI = (code & 0x7);
    if (unitCodes[unitI] == 0) return 0;

    /* Multi code is valid ? */
    mulI = (code >> 3) & 0xF;
    if (multiCodes[mulI] == 0) return 0;

    speed = unitCodes[unitI] * multiCodes[mulI];
    return speed;
}

static inline uint8_t Pon(SdCard *pSd)
{
    return SdmmcPowerOn(pSd, NULL);
}

static inline uint8_t Cmd0(SdCard *pSd, uint8_t arg)
{
    return SdmmcCmd0(pSd, arg, NULL);
}

static inline uint8_t Cmd1(SdCard *pSd, uint8_t* pHd)
{
    uint8_t error;
    uint32_t arg;
    arg = SDMMC_HOST_VOLTAGE_RANGE | OCR_MMC_HD;
    error = MmcCmd1(pSd, &arg, NULL);
    if (error) return error;
    if (arg & OCR_POWER_UP_BUSY) {
        *pHd = 0;
        if ((arg & OCR_BIT2930) == OCR_MMC_HIGH_DENSITY) {
            *pHd = 1;
        }
        return 0;
    }
    return SDMMC_ERROR_NOT_INITIALIZED;
}

static inline uint8_t Cmd2(SdCard *pSd)
{
    return SdmmcCmd2(pSd, pSd->cid, NULL);
}

static uint8_t Cmd3(SdCard *pSd)
{
    if (pSd->cardType == CARD_MMC || pSd->cardType == CARD_MMCHD) {
        uint8_t error = MmcCmd3(pSd, 1, NULL);
        if (!error) pSd->cardAddress = 1;
        return error;
    }
    else {
        uint8_t error;
        uint32_t addr;
        error = SdCmd3(pSd, &addr, NULL);
        pSd->cardAddress = (addr >> 16);
        return error;
    }
}

static uint8_t Cmd5(SdCard *pSd, uint32_t *pIo)
{
    return SdioCmd5(pSd, pIo, NULL);
}

static inline uint8_t Cmd7(SdCard *pSd, uint16_t address)
{
    return SdmmcCmd7(pSd, address, NULL);
}

static inline uint8_t Cmd9(SdCard *pSd)
{
    return SdmmcCmd9(pSd, CARD_ADDR(pSd), pSd->csd, NULL);
}

static uint8_t Cmd12(SdCard *pSd, uint32_t *pStatus)
{
    return SdmmcCmd12(pSd, pStatus, NULL);
}

static uint8_t Cmd13(SdCard *pSd, uint32_t *pStatus)
{
    return SdmmcCmd13(pSd, CARD_ADDR(pSd), pStatus, NULL);
}

static inline uint8_t Cmd16(SdCard *pSd, uint16_t blkLen)
{
    return SdmmcCmd16(pSd, blkLen, NULL);
}

static inline uint8_t Cmd17(SdCard *pSd,
                            uint8_t *pData,
                            uint32_t address,
                            uint32_t *pStatus)
{
    return SdmmcCmd17(pSd, BLOCK_SIZE(pSd), pData, address, pStatus, NULL);
}

static inline uint8_t Cmd18(SdCard *pSd,
                            uint16_t nbBlock,
                            uint8_t *pData,
                            uint32_t address,
                            uint32_t *pStatus)
{
    return SdmmcCmd18(pSd,
                      BLOCK_SIZE(pSd), nbBlock,
                      pData, address,
                      pStatus, NULL);
}

static inline uint8_t Cmd24(SdCard *pSd,
                            uint8_t *pData,
                            uint32_t address,
                            uint32_t *pStatus)
{
    return SdmmcCmd24(pSd, BLOCK_SIZE(pSd), pData, address, pStatus, NULL);
}

static inline uint8_t Cmd25(SdCard *pSd,
                            uint16_t nbBlock,
                            uint8_t *pData,
                            uint32_t address,
                            uint32_t *pStatus)
{
    return SdmmcCmd25(pSd,
                      BLOCK_SIZE(pSd), nbBlock,
                      pData, address,
                      pStatus, NULL);
}

static inline uint8_t Cmd52(SdCard *pSd,
                            uint8_t wrFlag,
                            uint8_t funcNb,
                            uint8_t rdAfterWr,
                            uint32_t addr,
                            uint32_t *pIoData)
{
    SdioCmd52Arg *pArg52 = (SdioCmd52Arg*)pIoData;
    pArg52->rwFlag = wrFlag;
    pArg52->functionNum = funcNb;
    pArg52->rawFlag = rdAfterWr;
    pArg52->regAddress = addr;
    return SdioCmd52(pSd, pIoData, NULL);
}

static inline uint8_t Cmd53(SdCard *pSd,
                            uint8_t wrFlag,
                            uint8_t funcNb,
                            uint8_t blockMode,
                            uint8_t incAddr,
                            uint32_t addr,
                            uint8_t *pIoData,
                            uint16_t len,
                            uint32_t *pArgResp,
                            SdmmcCallback fCallback,
                            void* pCbArg)
{
    uint8_t  error;
    SdioCmd53Arg* pArg53;
    pArg53 = (SdioCmd53Arg*)pArgResp;
    pArg53->rwFlag = wrFlag;
    pArg53->functionNum = funcNb;
    pArg53->blockMode = blockMode;
    pArg53->opCode = incAddr;
    pArg53->regAddress = addr;
    pArg53->count = len;
    error = SdioCmd53(pSd, pArgResp, pIoData, len, fCallback, pCbArg);
    return error;
}

/**
 * Defines the data bus width (00=1bit or 10=4 bits bus) to be used for data
 * transfer.
 * The allowed data bus widths are given in SCR register.
 * \param pSd  Pointer to a SD card driver instance.
 * \param busWidth  Bus width in bits.
 * \return the command transfer result (see SendCommand).
 */
static uint8_t Acmd6(SdCard *pSd, uint8_t busWidth)
{
    uint8_t error;
    uint32_t arg;
    error = SdmmcCmd55(pSd, CARD_ADDR(pSd), NULL);
    if (error) {
        TRACE_ERROR("Acmd6.cmd55:%d\n\r", error);
        return error;
    }
    arg = (busWidth == SDMMC_BUS_4_BIT)
        ? SD_STAT_DATA_BUS_WIDTH_4BIT : SD_STAT_DATA_BUS_WIDTH_1BIT;
    return SdAcmd6(pSd, arg, NULL, NULL);
}

/**
 * The SD Status contains status bits that are related to the SD memory Card
 * proprietary features and may be used for future application-specific usage.
 * Can be sent to a card only in 'tran_state'.
 */
static uint8_t Acmd13(SdCard *pSd, uint32_t *pSdSTAT)
{
    uint8_t error;
    error = SdmmcCmd55(pSd, CARD_ADDR(pSd), NULL);
    if (error) {
        TRACE_ERROR("Acmd13.cmd55:%d\n\r", error);
        return error;
    }
    return SdAcmd13(pSd, pSdSTAT, NULL);
}

/**
 * Asks to all cards to send their operations conditions.
 * Returns the command transfer result (see SendCommand).
 * \param pSd  Pointer to a SD card driver instance.
 * \param hcs  Shall be true if Host support High capacity.
 * \param pCCS  Set the pointed flag to 1 if hcs != 0 and SD OCR CCS flag is set.
 */
static uint8_t Acmd41(SdCard *pSd, uint8_t hcs, uint8_t *pCCS)
{
    uint8_t error;
    uint32_t arg;
    do {
        error = SdmmcCmd55(pSd, 0, NULL);
        if (error) {
            TRACE_ERROR("Acmd41.cmd55:%d\n\r", error);
            return error;
        }
        arg = SDMMC_HOST_VOLTAGE_RANGE;
        if (hcs) arg |= OCR_SD_CCS;
        error = SdAcmd41(pSd, &arg, NULL);
        if (error) {
            TRACE_ERROR("Acmd41.cmd41:%d\n\r", error);
            return error;
        }
        *pCCS = ((arg & OCR_SD_CCS)!=0);
    } while ((arg & OCR_POWER_UP_BUSY) != OCR_POWER_UP_BUSY);
    return 0;
}

/**
 * SD Card Configuration Register (SCR) provides information on the SD Memory
 * Card's special features that were configured into the given card. The size
 * of SCR register is 64 bits.
 */
static uint8_t Acmd51(SdCard* pSd, uint32_t *pSCR)
{
    uint8_t error;
    error = SdmmcCmd55(pSd, CARD_ADDR(pSd), NULL);
    if (error) {
        TRACE_ERROR("Acmd51.cmd55:%d\n\r", error);
        return error;
    }
    return SdAcmd51(pSd, pSCR, NULL);
}

/**
 * Try SW Reset several times (CMD0 with ARG 0)
 * \param pSd      Pointer to a SD card driver instance.
 * \param retry    Retry times.
 * \return 0 or MCI error code.
 */
static uint8_t SwReset(SdCard *pSd, uint32_t retry)
{
    uint32_t i;
    uint8_t error = 0;

    for (i = 0; i < retry; i ++) {
        error = Cmd0(pSd, 0);
        if (error != SDMMC_ERROR_NORESPONSE)
            break;
    }
    return error;
}

/**
 * Perform sligle block transfer
 */
static uint8_t PerformSingleTransfer(SdCard *pSd,
                                     uint32_t address,
                                     uint8_t *pData,
                                     uint8_t isRead)
{
    uint32_t status;
    uint8_t error = 0;
    /* Reset transfer state if previous in multi- mode */
    if(    (pSd->state == SD_STATE_READ)
        || (pSd->state == SD_STATE_WRITE)) {
        /* Stop transfer */
        error = Cmd12(pSd, &status);
        if (error) {
            TRACE_ERROR("SingleTx.Cmd12: st%x, er%d\n\r", pSd->state, error);
        }
        pSd->state = SD_STATE_READY;
        pSd->preBlock = 0xFFFFFFFF;
    }

    if ( isRead )
    {
        /* Wait for card to be ready for data transfers */
        do
        {
            error = Cmd13(pSd, &status);
            if (error)
            {
                TRACE_ERROR("SingleTx.RD.Cmd13: %d\n\r", error);

                return error;
            }

            if(  ((status & STATUS_STATE) == STATUS_IDLE)
               ||((status & STATUS_STATE) == STATUS_READY)
               ||((status & STATUS_STATE) == STATUS_IDENT))
            {
                TRACE_ERROR("SingleTx.mode\n\r");

                return SDMMC_ERROR_NOT_INITIALIZED;
            }

            /* If the card is in sending data state or
               in receivce data state */
            if ( ((status & STATUS_STATE) == STATUS_RCV) ||((status & STATUS_STATE) == STATUS_DATA) )
            {

                TRACE_DEBUG("SingleTx.state = 0x%X\n\r",
                            (status & STATUS_STATE) >> 9);
            }
        }

        while ( ((status & STATUS_READY_FOR_DATA) == 0) || ((status & STATUS_STATE) != STATUS_TRAN) ) ;

        assert( (status & STATUS_STATE) == STATUS_TRAN ) ; /* "SD Card can't be configured in transfer state 0x%X\n\r", (status & STATUS_STATE)>>9 */

        /* Read single block */
        error = Cmd17( pSd, pData, SD_ADDRESS(pSd,address), &status ) ;

        if ( error )
        {
            TRACE_ERROR("SingleTx.Cmd17: %d\n\r", error);
            return error;
        }

        if (status & ~(STATUS_READY_FOR_DATA | STATUS_STATE))
        {
            TRACE_ERROR("CMD17.stat: %x\n\r",
                status & ~(STATUS_READY_FOR_DATA | STATUS_STATE));
            return SDMMC_ERROR;
        }
        return error;
    }
    /* Write */
    {
        /* Wait for card to be ready for data transfers */
        do
        {
            error = Cmd13(pSd, &status);
            if (error)
            {
                TRACE_ERROR("SingleTx.WR.Cmd13: %d\n\r", error);
                return error;
            }
        }

        while ((status & STATUS_READY_FOR_DATA) == 0);

        /* Move to Sending data state */
        error = Cmd24(pSd,
                      pData, SD_ADDRESS(pSd,address),
                      &status);
        if (error)
        {
            TRACE_DEBUG("SingleTx.Cmd24: %d\n\r", error);
            return error;
        }

        if (status & (STATUS_WRITE & ~(STATUS_READY_FOR_DATA | STATUS_STATE)))
        {
            TRACE_ERROR("CMD24(0x%x).stat: %x\n\r",
                SD_ADDRESS(pSd,address),
                status & (STATUS_WRITE
                            & ~(STATUS_READY_FOR_DATA | STATUS_STATE)));
            return SDMMC_ERROR;
        }
    }

    return error;
}

/**
 * Move SD card to transfer state. The buffer size must be at
 * least 512 byte long. This function checks the SD card status register and
 * address the card if required before sending the transfer command.
 * Returns 0 if successful; otherwise returns an code describing the error.
 * \param pSd      Pointer to a SD card driver instance.
 * \param address  Address of the block to transfer.
 * \param nbBlocks Number of blocks to be transfer, 0 for infinite transfer.
 * \param pData    Data buffer whose size is at least the block size.
 * \param isRead   1 for read data and 0 for write data.
 */
static uint8_t MoveToTransferState(SdCard *pSd,
                                   uint32_t address,
                                   uint16_t nbBlocks,
                                   uint8_t *pData,
                                   uint8_t isRead)
{
    uint32_t status;
    uint8_t error;

    if ( (pSd->state == SD_STATE_READ) || (pSd->state == SD_STATE_WRITE) )
    {
        /* Stop transfer */
        error = Cmd12(pSd, &status);
        if (error)
        {
            TRACE_ERROR("MTTranState.Cmd12: st%x, er%d\n\r", pSd->state, error);
            return error;
        }
    }

    if ( isRead )
    {
        /* Wait for card to be ready for data transfers */
        do {
            error = Cmd13(pSd, &status);
            if (error)
            {
                TRACE_ERROR("MTTranState.RD.Cmd13: %d\n\r", error);
                return error;
            }

            if ( ((status & STATUS_STATE) == STATUS_IDLE) || ((status & STATUS_STATE) == STATUS_READY) || ((status & STATUS_STATE) == STATUS_IDENT) )
            {
                TRACE_ERROR("Pb Card Identification mode\n\r");
                return SDMMC_ERROR_NOT_INITIALIZED;
            }

            /* If the card is in sending data state or in receivce data state */
            if ( ((status & STATUS_STATE) == STATUS_RCV) || ((status & STATUS_STATE) == STATUS_DATA) )
            {
                TRACE_DEBUG("state = 0x%X\n\r", (status & STATUS_STATE) >> 9);
            }
        }

        while ( ((status & STATUS_READY_FOR_DATA) == 0) || ((status & STATUS_STATE) != STATUS_TRAN) ) ;

        assert( (status & STATUS_STATE) == STATUS_TRAN ) ; /* "SD Card can't be configured in transfer state 0x%X\n\r", (status & STATUS_STATE)>>9 */

        /* Move to Receiving data state */
        error = Cmd18(pSd, nbBlocks, pData, SD_ADDRESS(pSd,address), &status);

        //Cmd13(pSd, &status); //status -> 0xB00
        if (error)
        {
            TRACE_ERROR("MTTranState.Cmd18: %d\n\r", error);
            return error;
        }
        if (status & ~(STATUS_READY_FOR_DATA | STATUS_STATE)) {
            TRACE_ERROR("CMD18.stat: %x\n\r",
                status & ~(STATUS_READY_FOR_DATA | STATUS_STATE));
            return SDMMC_ERROR;
        }
    }
    else
    {
        /* Wait for card to be ready for data transfers */
        do
        {
            error = Cmd13(pSd, &status);
            if (error)
            {
                TRACE_ERROR("MoveToTransferState.WR.Cmd13: %d\n\r", error);
                return error;
            }
        }

        while ((status & STATUS_READY_FOR_DATA) == 0);
        /* Move to Sending data state */
        error = Cmd25(pSd,
                      nbBlocks,
                      pData, SD_ADDRESS(pSd,address),
                      &status);
        if (error) {
            TRACE_DEBUG("MoveToTransferState.Cmd25: %d\n\r", error);
            return error;
        }
        if (status & (STATUS_WRITE & ~(STATUS_READY_FOR_DATA | STATUS_STATE))) {
            TRACE_ERROR("CMD25(0x%x, %d).stat: %x\n\r",
                SD_ADDRESS(pSd,address), nbBlocks,
                status & (STATUS_WRITE
                            & ~(STATUS_READY_FOR_DATA | STATUS_STATE)));
            return SDMMC_ERROR;
        }
    }

    if (!error) pSd->preBlock = address + (nbBlocks-1);
    return error;
}

/**
 * Switch card state between STBY and TRAN (or CMD and TRAN)
 * \param pSd       Pointer to a SD card driver instance.
 * \param address   Card address to TRAN, 0 to STBY
 * \param statCheck Whether to check the status before CMD7.
 */
static uint8_t MmcSelectCard(SdCard *pSd, uint16_t address, uint8_t statCheck)
{
    uint8_t error;
    uint32_t  status;
    uint32_t  targetState = address ? STATUS_TRAN : STATUS_STBY;
    uint32_t  srcState    = address ? STATUS_STBY : STATUS_TRAN;

    /* At this stage the Initialization and identification process is achieved
     * The SD card is supposed to be in Stand-by State */
    while(statCheck) {
        error = Cmd13(pSd, &status);
        if (error) {
            TRACE_ERROR("MmcSelectCard.Cmd13 (%d)\n\r", error);
            return error;
        }
        if ((status & STATUS_READY_FOR_DATA)) {
            uint32_t currState = status & STATUS_STATE;
            if (currState == targetState) return 0;
            if (currState != srcState) {
                TRACE_ERROR("MmcSelectCard, wrong state %x\n\r", currState);
                return SDMMC_ERROR;
            }
            break;
        }
    }

    /* witch to TRAN mode to Select the current SD/MMC
     * so that SD ACMD6 can process or EXT_CSD can read. */
    error = Cmd7(pSd, address);
    if (error == SDMMC_ERROR_NOT_INITIALIZED && address == 0) {}
    else if (error) {
        TRACE_ERROR("MmcSelectCard.Cmd7 (%d)\n\r", error);
    }

    return error;
}

/**
 * Get MMC EXT_CSD information
 * \param pSd      Pointer to a SD card driver instance.
 */
static void MmcGetExtInformation(SdCard *pSd)
{
    uint8_t error;
    /* MMC 4.0 Higher version */
    if(SD_CSD_STRUCTURE(pSd) >= 2 && MMC_IsVer4(pSd)) {

        error = MmcCmd8(pSd, (uint8_t*)pSd->extData, NULL);
        if (error) {
            TRACE_ERROR("MmcGetExt.Cmd8: %d\n\r", error);
        }
    }
}

/**
 * Get SD/MMC memory max transfer speed in K.
 * \param pSd Pointer to a SD card driver instance.
 */
static uint32_t SdmmcGetMaxSpeed(SdCard *pSd)
{
    uint32_t speed = 0;
    if (pSd->cardType & CARD_TYPE_bmSDMMC) {
        speed = SdmmcDecodeTransSpeed(SD_CSD_TRAN_SPEED(pSd),
            sdmmcTransUnits,
            ((pSd->cardType & CARD_TYPE_bmSDMMC) == CARD_TYPE_bmSD) ?
                    sdTransMultipliers : mmcTransMultipliers);
    }
    return speed;
}

/**
 * Get SDIO max transfer speed, in K.
 * \param pSd Pointer to a SD card driver instance.
 */
static uint32_t SdioGetMaxSpeed(SdCard *pSd)
{
    uint8_t error;
    uint32_t speed = 0;
    uint32_t addr = 0;
    uint8_t  buf[6];
    if (pSd->cardType & CARD_TYPE_bmSDIO) {
        /* Check Func0 tuple in CIS area */
        error = SDIO_ReadDirect(pSd,
                                SDIO_CIA, SDIO_CIS_PTR_REG,
                                (uint8_t*)&addr, 3);
        if (error) {
            TRACE_ERROR("SdioUpdateCardInformation.RdDirect: %u\n\r", error);
            return error;
        }
        error = SdioFindTuples(pSd, addr, 256, NULL, &addr);
        if (error) {
            TRACE_ERROR("SdioUpdateCardInformation.FindTuple: %u\n\r", error);
            return error;
        }
        if (addr) {
            /* Fun0 tuple: fn0_blk_siz & max_tran_speed */
            SDIO_ReadDirect(pSd, SDIO_CIA, addr, buf, 6);
            speed = SdmmcDecodeTransSpeed(buf[5],
                                          sdmmcTransUnits,
                                          sdTransMultipliers);
        }
    }
    return speed;
}

/**
 * Get SCR and SD Status information
 * \param pSd      Pointer to a SD card driver instance.
 */
static void SdGetExtInformation(SdCard *pSd)
{
    uint8_t error;

    /* SD Status */
    if (pSd->optCmdBitMap & SD_ACMD13_SUPPORT) {
        error = Acmd13(pSd, &pSd->extData[SD_EXT_OFFSET_SD_STAT]);
        if (error) {
            TRACE_ERROR("SdGetExt.Acmd13: %d\n\r", error);
            pSd->optCmdBitMap &= ~SD_ACMD13_SUPPORT;
        }
    }

    /* SD SCR */
    error = Acmd51(pSd, &pSd->extData[SD_EXT_OFFSET_SD_SCR]);
    if (error) {
        TRACE_ERROR("SdGetExt.Acmd51: %d\n\r", error);
    }
}

/**
 * Update SD/MMC information.
 * Update CSD for card speed switch.
 * Update ExtDATA for any card function switch.
 * \param pSd      Pointer to a SD card driver instance.
 * \return error code when update CSD error.
 */
static void SdMmcUpdateInformation(SdCard *pSd,
                                   uint8_t csd,
                                   uint8_t extData)
{
    uint8_t error;

    /* Update CSD for new TRAN_SPEED value */
    if (csd) {
        MmcSelectCard(pSd, 0, 1);
        Delay(800);
        error = Cmd9(pSd);
        if (error ) {
            TRACE_ERROR("SdMmcUpdateInfo.Cmd9 (%d)\n\r", error);
            return;
        }
        error = MmcSelectCard(pSd, pSd->cardAddress, 1);
    }
    if (extData) {
        switch(pSd->cardType & CARD_TYPE_bmSDMMC) {
            case CARD_TYPE_bmSD:    SdGetExtInformation(pSd);   break;
            case CARD_TYPE_bmMMC:   MmcGetExtInformation(pSd);  break;
            default: break;
        }
    }
}

/**
 * \brief Check HS capability and enable it
 * \param pSd Pointer to SdCard instance.
 */
static uint8_t SdMmcEnableHighSpeed(SdCard *pSd)
{
    uint8_t  error;
    uint32_t status;
    uint8_t io, sd, mmc;

    io  = ((pSd->cardType & CARD_TYPE_bmSDIO) > 0);
    sd  = ((pSd->cardType & CARD_TYPE_bmSDMMC) == CARD_TYPE_bmSD);
    mmc = ((pSd->cardType & CARD_TYPE_bmSDMMC) == CARD_TYPE_bmMMC);

    /* Check host driver capability */
    if (SdmmcGetProperty(pSd, SDMMC_PROP_HS_MODE, NULL) == 0) {
        TRACE_INFO("HS Mode not supported by Host\n\r");
        return SDMMC_ERROR_NOT_SUPPORT;
    }
    /* Check MMC */
    if (mmc) {
        /* MMC card type 3 (HS) */
        //if (SD_CSD_STRUCTURE(pSd) >= 2 && (SD_EXTCSD_CARD_TYPE(pSd) & 0x2)) {
        if (MMC_IsHsModeSupported(pSd)) {
            /* Try switch to HS mode */
            MmcCmd6Arg cmd6Arg = {
                0x3,
                SD_EXTCSD_HS_TIMING_INDEX,
                SD_EXTCSD_HS_TIMING_ENABLE,
                0};
            error = MmcCmd6(pSd, &cmd6Arg, &status, NULL);
            if (error) {
                TRACE_ERROR("SdMmcEnableHS.Cmd6: %u\n\r", error);
                return SDMMC_ERROR;
            }
            else if (status & STATUS_MMC_SWITCH) {
                TRACE_ERROR("Mmc Switch HS: %x\n\r", status);
                return SDMMC_ERROR_NOT_SUPPORT;
            }
        }
        else {
            TRACE_INFO("MMC without HS support\n\r");
            return SDMMC_ERROR_NOT_SUPPORT;
        }
        TRACE_WARNING("MMC HS Enabled\n\r");
    }
    /* SD (+IO) */
    else {
        if (io) {
            /* Check CIA.HS */
            status = 0;
            error = Cmd52(pSd, 0, SDIO_CIA, 0, SDIO_HS_REG, &status);
            if (error) {
                TRACE_ERROR("SdMmcEnableHS.Cmd52: %u\n\r", error);
                return SDMMC_ERROR;
            }
            if ((status & SDIO_SHS) == 0) {
                TRACE_INFO("HS Mode not supported by SDIO\n\r");
                return SDMMC_ERROR_NOT_SUPPORT;
            }
            /* Enable HS mode */
            else {
                status = SDIO_EHS;
                error = Cmd52(pSd, 1, SDIO_CIA, 1, SDIO_HS_REG, &status);
                if (error) {
                    TRACE_ERROR("SdMmcEnableHS.Cmd52 H: %u\n\r", error);
                    return SDMMC_ERROR;
                }
                if (status & SDIO_EHS) {
                    TRACE_WARNING("SDIO HS Enabled\n\r");
                }
            }
        }
        if (sd) {
            /* Check version */
            //if (SD_SCR_SD_SPEC(pSd) >= SD_SCR_SD_SPEC_1_10) {
            if (SD_IsHsModeSupported(pSd)) {
                /* Try enable HS mode */
                SdCmd6Arg cmd6Arg = {
                    1, 0, 0xF, 0xF, 0xF, 0xF, 0, 1
                };
                uint32_t switchStatus[512/32];
                error = SdCmd6(pSd, &cmd6Arg, switchStatus, &status, NULL);
                if (error || (status & STATUS_SWITCH_ERROR)) {
                    TRACE_INFO("SD HS Fail\n\r");
                    return SDMMC_ERROR;
                }
                else if (SD_SW_STAT_FUN_GRP1_RC(switchStatus)
                                == SD_SW_STAT_FUN_GRP_RC_ERROR) {
                    TRACE_INFO("SD HS Not Supported\n\r");
                    return SDMMC_ERROR_NOT_SUPPORT;
                }
                else if (SD_SW_STAT_FUN_GRP1_BUSY(switchStatus)) {
                    TRACE_INFO("SD HS Locked\n\r");
                    return SDMMC_ERROR_BUSY;
                }
                else {
                    TRACE_WARNING("SD HS Enabled\n\r");
                }
            }
            else {
                TRACE_INFO("HS Not Supported in SD Rev 0x%x\n\r",
                    SD_SCR_SD_SPEC(pSd));
                return SDMMC_ERROR_NOT_SUPPORT;
            }
        }
    }

    /* Enable Host HS mode */
    SdmmcEnableHsMode(pSd, 1);
    return 0;
}

/**
 * \brief Check bus width capability and enable it
 */
static uint8_t SdMmcDesideBuswidth(SdCard *pSd)
{
    uint8_t  error, busWidth;
    uint32_t status;
    uint8_t mmc;

    /* Best width that the card support */
    busWidth = SdmmcGetProperty(pSd, SDMMC_PROP_BUS_MODE, NULL);

    mmc = ((pSd->cardType & CARD_TYPE_bmSDMMC) == CARD_TYPE_bmMMC);

    if (mmc) {
        /* Check MMC revision 4 or later (1/4/8 bit mode) */
        if (SD_CSD_SPEC_VERS(pSd) >= 4) {
            /* Select what's HC supported */
            MmcCmd6Arg cmd6Arg = {
                0x1, SD_EXTCSD_BUS_WIDTH_INDEX, SD_EXTCSD_BUS_WIDTH_1BIT, 0
            };
            switch(busWidth) {
                case SDMMC_BUS_4_BIT:
                    cmd6Arg.value = SD_EXTCSD_BUS_WIDTH_4BIT;
                    break;
                case SDMMC_BUS_8_BIT:
                    cmd6Arg.value = SD_EXTCSD_BUS_WIDTH_8BIT;
                    break;
                case SDMMC_BUS_1_BIT:
                    break;
            }
            error = MmcCmd6(pSd, &cmd6Arg, &status, NULL);
            if (error) {
                TRACE_ERROR("SdMmcSetBuswidth.Cmd6: %u\n\r", error);
                return SDMMC_ERROR;
            }
            else {
                if (status & STATUS_MMC_SWITCH) {
                    TRACE_ERROR("MMC Bus Switch error %x\n\r", status);
                    return SDMMC_ERROR_NOT_SUPPORT;
                }
                TRACE_WARNING("MMC Bus mode %x\n\r", busWidth);
            }
        }
        else {
            TRACE_WARNING("MMC 1-bit only\n\r");
            return SDMMC_ERROR_NOT_SUPPORT;
        }
    }
    else {
        /* SD(IO): switch to 4-bit mode ? */
        uint8_t io  = ((pSd->cardType & CARD_TYPE_bmSDIO) > 0);
        uint8_t sd  = ((pSd->cardType & CARD_TYPE_bmSDMMC) == CARD_TYPE_bmSD);
        if (busWidth == SDMMC_BUS_1_BIT)
            return SDMMC_ERROR_NOT_SUPPORT;
        /* No 8-bit mode, default to 4-bit mode */
        busWidth = SDMMC_BUS_4_BIT;

        /* SDIO */
        if (io) {
            /* SDIO 1 bit only */
            busWidth = SDMMC_BUS_1_BIT;
        }

        /* SD */
        if (sd) {
            error = Acmd6(pSd, busWidth);
            if (error) {
                TRACE_ERROR("SdMmcSetBusWidth.Acmd6: %u\n\r", error);
                return SDMMC_ERROR;
            }
            TRACE_WARNING("SD 4-bit mode\n\r");
        }
    }

    /* Switch to selected bus mode */
    pSd->busMode = busWidth;
    SdmmcSetBusWidth(pSd, busWidth);
    return 0;
}

/**
 * \brief Run the SD/MMC/SDIO Mode initialization sequence.
 * This function runs the initialization procedure and the identification
 * process. Then it leaves the card in ready state. The following procedure must
 * check the card type and continue to put the card into tran(for memory card)
 * or cmd(for io card) state for data exchange.
 * \param pSd  Pointer to a SD card driver instance.
 * \return 0 if successful; otherwise returns an \ref sdmmc_rc "SD_ERROR code".
 */
static uint8_t SdMmcIdentify(SdCard *pSd)
{
    uint8_t mem = 0, io = 0, f8 = 0, mp = 1, ccs = 0;
    uint32_t status;
    uint8_t error;
    /* Reset HC to default HS and BusMode */
    SdmmcEnableHsMode(pSd, 0);
    SdmmcSetBusWidth(pSd, SDMMC_BUS_1_BIT);
    /* Reset SDIO: CMD52, write 1 to RES bit in CCCR (bit 3 of register 6) */
    status = SDIO_RES;
    error = Cmd52(pSd, 1, SDIO_CIA, 0, SDIO_IOA_REG, &status);
    if (!error && ((status & STATUS_SDIO_R5)==0))
    {
    }
    else
    {
        if (error == SDMMC_ERROR_NORESPONSE)
        {
        }
        else
        {
            TRACE_DEBUG("SdMmcIdentify.Cmd52 fail: %u, %x\n\r", error, status);
        }
    }

    /* Reset MEM: CMD0 */
    error = SwReset(pSd, 1);
    if (error) {
        TRACE_DEBUG("SdMmcIdentify.SwReset fail: %u\n\r", error);
    }

    /* CMD8 is newly added in the Physical Layer Specification Version 2.00 to
     * support multiple voltage ranges and used to check whether the card
     * supports supplied voltage. The version 2.00 host shall issue CMD8 and
     * verify voltage before card initialization.
     * The host that does not support CMD8 shall supply high voltage range... */
    error = SdCmd8(pSd, 1, NULL);
    if (error == 0) f8 = 1;
    else if (error != SDMMC_ERROR_NORESPONSE) {
        TRACE_ERROR("SdMmcIdentify.Cmd8: %u\n\r", error);
        return SDMMC_ERROR;
    }
    /* Delay after "no response" */
    else Delay(800);

    /* CMD5 is newly added for SDIO initialize & power on */
    status = 0;
    error = Cmd5(pSd, &status);
    if (error) {
        TRACE_WARNING("SdMmcIdentify.Cmd5: %u\n\r", error);
    }
    /* Card has SDIO function */
    else if ((status & OCR_SDIO_NF) > 0) {
        unsigned int cmd5Retries = 10000;
        do {
            status &= SDMMC_HOST_VOLTAGE_RANGE;
            error = Cmd5(pSd, &status);
            if (status & OCR_POWER_UP_BUSY) break;
        } while(!error && cmd5Retries --);
        if (error) {
            TRACE_ERROR("SdMmcIdentify.Cmd5 V: %u\n\r", error);
            return SDMMC_ERROR;
        }
        io = 1;
        TRACE_INFO("SDIO\n\r");
        /* IO only ?*/
        mp = ((status & OCR_SDIO_MP) > 0);
    }
    /* Has memory: SD/MMC/COMBO */
    if (mp) {
        /* Try SD memory initialize */
        error = Acmd41(pSd, f8, &ccs);
        if (error) {
            unsigned int cmd1Retries = 10000;
            TRACE_DEBUG("SdMmcIdentify.Acmd41: %u, try MMC\n\r", error);
            /* Try MMC initialize */
            error = SwReset(pSd, 10);
            if (error) {
                TRACE_ERROR("SdMmcIdentify.Mmc.SwReset: %u\n\r", error);
                return SDMMC_ERROR;
            }
            ccs = 1;
            do { error = Cmd1(pSd, &ccs); }while(error && cmd1Retries -- > 0);
            if (error) {
                TRACE_ERROR("SdMmcIdentify.Cmd1: %u\n\r", error);
                return SDMMC_ERROR;
            }
            else if (ccs) pSd->cardType = CARD_MMCHD;
            else          pSd->cardType = CARD_MMC;
            /* MMC card identification OK */
            TRACE_INFO("MMC Card\n\r");
            return 0;
        }
        else if (ccs) { TRACE_INFO("SDHC MEM\n\r");}
        else          { TRACE_INFO("SD MEM\n\r");}
        mem = 1;
    }
    /* SD(IO) + MEM ? */
    if (!mem) {
        if (io) pSd->cardType = CARD_SDIO;
        else {
            TRACE_ERROR("Unknown card\n\r");
            return SDMMC_ERROR;
        }
    }
    /* SD(HC) combo */
    else if (io)
        pSd->cardType = ccs ? CARD_SDHCCOMBO : CARD_SDCOMBO;
    /* SD(HC) */
    else
        pSd->cardType = ccs ? CARD_SDHC : CARD_SD;

    return 0;
}

/**
 * \brief Run the SD/MMC/SDIO enumeration sequence.
 * This function runs after the initialization and identification procedure. It
 * gets all necessary information from the card and deside transfer block size,
 * clock speed and bus width. It sets the SD/MMC/SDIO card in transfer
 * (or command) state.
 * \param pSd  Pointer to a SD card driver instance.
 * \return 0 if successful; otherwise returns an \ref sdmmc_rc "SD_ERROR code".
 */
static uint8_t SdMmcEnum(SdCard *pSd)
{
    uint8_t mem , io;
    uint8_t error;
    uint32_t ioSpeed = 0, memSpeed = 0;
    uint8_t hsExec = 0, bwExec = 0;

    /* - has Memory/IO/High-Capacigy - */
    mem = ((pSd->cardType & CARD_TYPE_bmSDMMC) > 0);
    io  = ((pSd->cardType & CARD_TYPE_bmSDIO)  > 0);

    /* For MEMORY cards:
     * The host then issues the command ALL_SEND_CID (CMD2) to the card to get
     * its unique card identification (CID) number.
     * Card that is unidentified (i.e. which is in Ready State) sends its CID
     * number as the response (on the CMD line). */
    if (mem) {
        error = Cmd2(pSd);
        if (error) {
            TRACE_ERROR("SdMmcInit.cmd2(%d)\n\r", error);
            return error;
        }
    }

    /* For MEMORY and SDIO cards:
     * Thereafter, the host issues CMD3 (SEND_RELATIVE_ADDR) asks the
     * card to publish a new relative card address (RCA), which is shorter than
     * CID and which is used to address the card in the future data transfer
     * mode. Once the RCA is received the card state changes to the Stand-by
     * State. At this point, if the host wants to assign another RCA number, it
     * can ask the card to publish a new number by sending another CMD3 command
     * to the card. The last published RCA is the actual RCA number of the
     * card. */
    error = Cmd3(pSd);
    if (error) {
        TRACE_ERROR("SdMmcInit.cmd3(%d)\n\r", error);
        return error;
    }

    /* For MEMORY cards:
     * SEND_CSD (CMD9) to obtain the Card Specific Data (CSD register),
     * e.g. block length, card storage capacity, etc... */
    if (mem) {
        error = Cmd9(pSd);
        if (error) {
            TRACE_ERROR("SdMmcInit.cmd9(%d)\n\r", error);
            return error;
        }
    }

    /* Now select the card, to TRAN state */
    error = MmcSelectCard(pSd, pSd->cardAddress, 0);
    TRACE_ERROR("COUCOU");
    if (error) {
        TRACE_ERROR("SdMmcInit.SelCard(%d)\n\r", error);
        return error;
    }

    /* - Now in TRAN, obtain extended setup information - */

    /* If the card support EXT_CSD, read it! */
    TRACE_INFO("Card Type %d, CSD_STRUCTURE %d\n\r",
               pSd->cardType, SD_CSD_STRUCTURE(pSd));

    /* Get extended information of the card */
    SdMmcUpdateInformation(pSd, 0, 1);

    /* Calculate transfer speed */
    if (io)     ioSpeed = SdioGetMaxSpeed(pSd);
    if (mem)    memSpeed = SdmmcGetMaxSpeed(pSd);
    /* Combo, min speed */
    if (io && mem) {
        pSd->transSpeed = (ioSpeed > memSpeed) ? memSpeed : ioSpeed;
    }
    /* SDIO only */
    else if (io) {
        pSd->transSpeed = ioSpeed;
    }
    /* Memory card only */
    else if (mem) {
        pSd->transSpeed = memSpeed;
    }
    pSd->transSpeed *= 1000;

    /* Enable more bus width Mode */
    error = SdMmcDesideBuswidth(pSd);
    if (!error) bwExec = 1;
    else if (error != SDMMC_ERROR_NOT_SUPPORT) {
        TRACE_ERROR("SdmmcEnum.DesideBusWidth: %u\n\r", error);
        return SDMMC_ERROR;
    }

    /* Enable High-Speed Mode */
    error = SdMmcEnableHighSpeed(pSd);
    if (!error) hsExec = 1;
    else if (error != SDMMC_ERROR_NOT_SUPPORT) {
        TRACE_ERROR("SdmmcEnum.EnableHS: %u\n\r", error);
        return SDMMC_ERROR;
    }

    /* In HS mode transfer speed *2 */
    if (hsExec) pSd->transSpeed *= 2;

    /* Update card information since status changed */
    if (bwExec || hsExec) SdMmcUpdateInformation(pSd, hsExec, 1);
    return 0;
}

/*----------------------------------------------------------------------------
 *         Global functions
 *----------------------------------------------------------------------------*/

/**
 * Read Blocks of data in a buffer pointed by pData. The buffer size must be at
 * least 512 byte long. This function checks the SD card status register and
 * address the card if required before sending the read command.
 * \return 0 if successful; otherwise returns an \ref sdmmc_rc "error code".
 * \param pSd      Pointer to a SD card driver instance.
 * \param address  Address of the block to read.
 * \param pData    Data buffer whose size is at least the block size, it can
 *            be 1,2 or 4-bytes aligned when used with DMA.
 * \param length   Number of blocks to be read.
 * \param pCallback Pointer to callback function that invoked when read done.
 *                  0 to start a blocked read.
 * \param pArgs     Pointer to callback function arguments.
 */
uint8_t SD_Read(SdCard        *pSd,
                uint32_t      address,
                void          *pData,
                uint16_t      length,
                SdmmcCallback pCallback,
                void          *pArgs)
{
    uint8_t error;

    assert( pSd != NULL ) ;
    assert( pData != NULL ) ;

    if (   pSd->state != SD_STATE_READ
        || pSd->preBlock + 1 != address ) {
        /* Start infinite block reading */
        error = MoveToTransferState(pSd, address, 0, 0, 1);
    }
    else    error = 0;
    if (!error) {
        pSd->state = SD_STATE_READ;
        pSd->preBlock = address + (length - 1);
        error = SdmmcRead(pSd, BLOCK_SIZE(pSd), length, pData,
                          pCallback, pArgs);
    }
    TRACE_DEBUG("SDrd(%u,%u):%u\n\r", address, length, error);

    return 0;
}

/**
 * Write Blocks of data in a buffer pointed by pData. The buffer size must be at
 * least 512 byte long. This function checks the SD card status register and
 * address the card if required before sending the read command.
 * \return 0 if successful; otherwise returns an \ref sdmmc_rc "error code".
 * \param pSd      Pointer to a SD card driver instance.
 * \param address  Address of the block to read.
 * \param pData    Data buffer whose size is at least the block size, it can
 *            be 1,2 or 4-bytes aligned when used with DMA.
 * \param length   Number of blocks to be read.
 * \param pCallback Pointer to callback function that invoked when read done.
 *                  0 to start a blocked read.
 * \param pArgs     Pointer to callback function arguments.
 */
uint8_t SD_Write(SdCard        *pSd,
                 uint32_t      address,
                 void          *pData,
                 uint16_t      length,
                 SdmmcCallback pCallback,
                 void          *pArgs)
{
    uint8_t error = 0;

    assert( pSd != NULL ) ;

    if (   pSd->state != SD_STATE_WRITE
        || pSd->preBlock + 1 != address ) {
        /* Start infinite block writing */
        error = MoveToTransferState(pSd, address, 0, 0, 0);
    }
    if (!error) {
        pSd->state = SD_STATE_WRITE;
        error = SdmmcWrite(pSd, BLOCK_SIZE(pSd), length, pData,
                           pCallback, pArgs);
        pSd->preBlock = address + (length - 1);
    }
    TRACE_DEBUG("SDwr(%u,%u):%u\n\r", address, length, error);

    return 0;
}

/**
 * Read 1 Block of data in a buffer pointed by pData. The buffer size must be
 * one block size. This function checks the SD card status register and
 * address the card if required before sending the read command.
 * \return 0 if successful; otherwise returns an \ref sdmmc_rc "error code".
 * \param pSd  Pointer to a SD card driver instance.
 * \param address  Address of the block to read.
 * \param pData    Data buffer whose size is at least the block size.
 */
uint8_t SD_ReadBlock( SdCard *pSd, uint32_t address, uint8_t *pData )
{
    assert( pSd != NULL ) ;
    assert( pData != NULL ) ;

    TRACE_DEBUG("RdBlk(%d)\n\r", address);

    return PerformSingleTransfer(pSd, address, pData, 1);
}

/**
 * Write 1 Block of data pointed by pData. The buffer size must be
 * one block size. This function checks the SD card status register and
 * address the card if required before sending the read command.
 * \return 0 if successful; otherwise returns an \ref sdmmc_rc "error code".
 * \param pSd  Pointer to a SD card driver instance.
 * \param address  Address of block to write.
 * \param pData    Data buffer whose size is at least the block size, it can
 *                 be 1,2 or 4-bytes aligned when used with DMA.
 */
uint8_t SD_WriteBlock(SdCard *pSd,
                      uint32_t address,
                      uint8_t *pData)
{
    assert( pSd != NULL ) ;
    assert( pData != NULL ) ;

    TRACE_DEBUG("WrBlk(%d)\n\r", address);

    return PerformSingleTransfer(pSd, address, pData, 0);
}

/**
 * Read Blocks of data in a buffer pointed by pData. The buffer size must be at
 * least 512 byte long. This function checks the SD card status register and
 * address the card if required before sending the read command.
 * \return 0 if successful; otherwise returns an \ref sdmmc_rc "error code".
 * \param pSd  Pointer to a SD card driver instance.
 * \param address  Address of the block to read.
 * \param nbBlocks Number of blocks to be read.
 * \param pData    Data buffer whose size is at least the block size, it can
 *            be 1,2 or 4-bytes aligned when used with DMA.
 */
uint8_t SD_ReadBlocks(SdCard *pSd,
                      uint32_t address,
                      uint16_t nbBlocks,
                      uint8_t *pData)
{
    uint8_t error = 0;

    assert( pSd != NULL ) ;
    assert( pData != NULL ) ;
    assert( nbBlocks != NULL ) ;

    TRACE_DEBUG("RdBlks(%d,%d)\n\r", address, nbBlocks);
    while(nbBlocks --) {
        error = PerformSingleTransfer(pSd, address, pData, 1);
        if (error)
            break;
        address += 1;
        pData = &pData[512];
    }
    return error;
}

/**
 * Write Block of data pointed by pData. The buffer size must be at
 * least 512 byte long. This function checks the SD card status register and
 * address the card if required before sending the read command.
 * \return 0 if successful; otherwise returns an \ref sdmmc_rc "error code".
 * \param pSd  Pointer to a SD card driver instance.
 * \param address  Address of block to write.
 * \param nbBlocks Number of blocks to be read
 * \param pData    Data buffer whose size is at least the block size, it can
 *            be 1,2 or 4-bytes aligned when used with DMA.
 */
uint8_t SD_WriteBlocks(SdCard *pSd,
                       uint32_t address,
                       uint16_t nbBlocks,
                       uint8_t *pData)
{
    uint8_t error = 0;
    uint8_t *pB = (uint8_t*)pData;

    assert( pSd != NULL ) ;
    assert( pData != NULL ) ;
    assert( nbBlocks != NULL ) ;

    TRACE_DEBUG("WrBlks(%d,%d)\n\r", address, nbBlocks);

    while(nbBlocks --) {
        error = PerformSingleTransfer(pSd, address, pB, 0);
        if (error)
            break;
        address += 1;
        pB = &pB[512];
    }
    return error;
}

/**
 * Run the SDcard initialization sequence. This function runs the
 * initialisation procedure and the identification process, then it sets the
 * SD card in transfer state to set the block length and the bus width.
 * \return 0 if successful; otherwise returns an \ref sdmmc_rc "error code".
 * \param pSd  Pointer to a SD card driver instance.
 * \param pSdDriver  Pointer to SD driver already initialized.
 */
uint8_t SD_Init(SdCard *pSd, void *pSdDriver)
{
    uint8_t  error;
    uint32_t clock;
    uint32_t i;

    /* Initialize SdCard structure */
    pSd->pSdDriver = pSdDriver;

    pSd->transSpeed = 0;
    pSd->totalSize  = 0;
    pSd->blockNr    = 0;

    pSd->preBlock     = 0xffffffff;
    pSd->optCmdBitMap = 0xFFFFFFFF;

    pSd->accSpeed    = 0;
    pSd->cardAddress = 0;
    pSd->cardType    = CARD_UNKNOWN;
    pSd->busMode     = 0;
    pSd->cardSlot    = 0;
    pSd->state       = SD_STATE_INIT;

    /* Clear CID, CSD, EXT_CSD data */
    for (i = 0; i < 4; i ++)     pSd->cid[i] = 0;
    for (i = 0; i < 4; i ++)     pSd->csd[i] = 0;
    for (i = 0; i < 512/4; i ++) pSd->extData[i] = 0;

    /* Set low speed for device identification (LS device max speed) */
    SdmmcSetSpeed(pSd, 400000);

    /* Initialization delay: The maximum of 1 msec, 74 clock cycles and supply
     * ramp up time. Supply ramp up time provides the time that the power is
     * built up to the operating level (the bus master supply voltage) and the
     * time to wait until the SD card can accept the first command. */
    /* Power On Init Special Command */
    error = Pon(pSd);
    if (error) {
        TRACE_ERROR("SD_Init.PowON:%d\n\r", error);
        return error;
    }

    /* After power-on or CMD0, all cards?
     * CMD lines are in input mode, waiting for start bit of the next command.
     * The cards are initialized with a default relative card address
     * (RCA=0x0000) and with a default driver stage register setting
     * (lowest speed, highest driving current capability). */
    error = SdMmcIdentify(pSd);
    if (error) {
        TRACE_ERROR("SD_Init.Identify: %u\n\r", error);
        return error;
    }
    error = SdMmcEnum(pSd);
    if (error) {
        TRACE_ERROR("SD_Init.Enum: %u\n\r", error);
        return error;
    }

    /* In the case of a Standard Capacity SD Memory Card, this command sets the
     * block length (in bytes) for all following block commands
     * (read, write, lock).
     * Default block length is fixed to 512 Bytes.
     * Set length is valid for memory access commands only if partial block read
     * operation are allowed in CSD.
     * In the case of a High Capacity SD Memory Card, block length set by CMD16
     * command does not affect the memory read and write commands. Always 512
     * Bytes fixed block length is used. This command is effective for
     * LOCK_UNLOCK command.
     * In both cases, if block length is set larger than 512Bytes, the card sets
     * the BLOCK_LEN_ERROR bit. */
    if (pSd->cardType == CARD_SD) {
        error = Cmd16(pSd, SDMMC_BLOCK_SIZE);
        if (error) {
            pSd->optCmdBitMap &= ~SD_CMD16_SUPPORT;
            TRACE_INFO("SD_Init.Cmd16 (%d)\n\r", error);
            TRACE_INFO("Fail to set BLK_LEN, default is 512\n\r");
        }
    }

    /* Reset status for R/W */
    pSd->state = SD_STATE_READY;

    /* If MMC Card & get size from EXT_CSD */
    if ((pSd->cardType & CARD_TYPE_bmSDMMC) == CARD_TYPE_bmMMC
        && SD_CSD_C_SIZE(pSd) == 0xFFF) {
        pSd->blockNr = SD_EXTCSD_BLOCKNR(pSd);
        /* Block number less than 0x100000000/512 */
        if (pSd->blockNr > 0x800000)
            pSd->totalSize = 0xFFFFFFFF;
        else
            pSd->totalSize = SD_EXTCSD_TOTAL_SIZE(pSd);
    }
    /* If SD CSD v2.0 */
    else if((pSd->cardType & CARD_TYPE_bmSDMMC) == CARD_TYPE_bmSD
        && SD_CSD_STRUCTURE(pSd) >= 1) {
        pSd->blockNr   = SD_CSD_BLOCKNR_HC(pSd);
        pSd->totalSize = 0xFFFFFFFF;
    }
    /* Normal SD/MMC card */
    else if (pSd->cardType & CARD_TYPE_bmSDMMC) {
        pSd->totalSize = SD_CSD_TOTAL_SIZE(pSd);
        pSd->blockNr = SD_CSD_BLOCKNR(pSd);
    }

    if (pSd->cardType == CARD_UNKNOWN) {
        return SDMMC_ERROR_NOT_INITIALIZED;
    }
    /* Automatically select the max clock */
    clock = SdmmcSetSpeed(pSd, pSd->transSpeed);
    TRACE_WARNING_WP("-I- Set SD/MMC clock to %dK\n\r", clock/1000);
    pSd->accSpeed = clock;
    return 0;
}

/**
 * Return type of the card.
 * \param pSd Pointer to SdCard instance.
 * \sa sdmmc_cardtype
 */
uint8_t SD_GetCardType(SdCard *pSd)
{
    assert( pSd != NULL ) ;

    return pSd->cardType;
}

/**
 * Return size of the SD/MMC card, in KB.
 * \param pSd Pointer to SdCard instance.
 */
uint32_t SD_GetTotalSizeKB(SdCard *pSd)
{
    assert( pSd != NULL ) ;

    if (pSd->totalSize == 0xFFFFFFFF) {

        return pSd->blockNr / 2;
    }

    return pSd->totalSize / 1024;
}

/**
 * Return reported block size of the SD/MMC card.
 * (SD/MMC access block size is always 512B for R/W).
 * \param pSd Pointer to SdCard instance.
 */
uint32_t SD_GetBlockSize(SdCard *pSd)
{
    assert( pSd != NULL ) ;

    if (pSd->totalSize == 0xFFFFFFFF)
        return 512;
    else
        return (SD_TOTAL_SIZE(pSd) / SD_TOTAL_BLOCK(pSd));
}

/**
 * Return reported number of blocks for the SD/MMC card.
 * \param pSd Pointer to SdCard instance.
 */
uint32_t SD_GetNumberBlocks( SdCard *pSd )
{
    assert( pSd != NULL ) ;

    return pSd->blockNr ;
}

/**
 * Read one or more bytes from SDIO card, using RW_DIRECT command.
 * \param pSd         Pointer to SdCard instance.
 * \param functionNum Function number.
 * \param address     First register address to read from.
 * \param pData       Pointer to data buffer.
 * \param size        Buffer size, number of bytes to read.
 * \return 0 if successful; otherwise returns an \ref sdmmc_rc "error code".
 */
uint8_t SDIO_ReadDirect(SdCard *pSd,
                        uint8_t functionNum,
                        uint32_t address,
                        uint8_t *pData,
                        uint32_t size)
{
    uint8_t  error;
    uint32_t status;

    assert( pSd != NULL ) ;

    if (pSd->cardType & CARD_TYPE_bmSDIO) {
        if (size == 0) return SDMMC_ERROR_PARAM;
        while(size --) {
            status = 0;
            error = Cmd52(pSd, 0, functionNum, 0, address ++, &status);
            if (pData) *pData ++ = (uint8_t)status;
            if (error) {
                TRACE_ERROR("IO_RdDirect.Cmd52: %u\n\r", error);
                return SDMMC_ERROR;
            }
            else if (status & STATUS_SDIO_R5) {
                TRACE_ERROR("RD_DIRECT(%u, %u) st %x\n\r",
                    address, size, status);
                return SDMMC_ERROR;
            }
        }
    }
    else {
        return SDMMC_ERROR_NOT_SUPPORT;
    }
    return 0;
}

/**
 * Write one byte to SDIO card, using RW_DIRECT command.
 * \param pSd         Pointer to SdCard instance.
 * \param functionNum Function number.
 * \param address     Register address to write to.
 * \param dataByte    Data to write.
 * \return 0 if successful; otherwise returns an \ref sdmmc_rc "error code".
 */
uint8_t SDIO_WriteDirect(SdCard *pSd,
                         uint8_t functionNum,
                         uint32_t address,
                         uint8_t dataByte)
{
    uint8_t  error;
    uint32_t status;

    assert( pSd != NULL ) ;

    if (pSd->cardType & CARD_TYPE_bmSDIO) {
        status = dataByte;
        error = Cmd52(pSd, 1, functionNum, 0, address, &status);
        if (error) {
            TRACE_ERROR("SDIO_WrDirect.Cmd52: %u\n\r", error);
            return SDMMC_ERROR;
        }
        else if (status & STATUS_SDIO_R5) {
            TRACE_ERROR("WR_DIRECT(%u) st %x\n\r",
                address, status);
            return SDMMC_ERROR;
        }
    }
    else {
        return SDMMC_ERROR_NOT_SUPPORT;
    }
    return 0;
}

/**
 * Read byte by byte from SDIO card, using RW_EXTENDED command.
 * \param pSd            Pointer to SdCard instance.
 * \param functionNum    Function number.
 * \param address        First byte address of data in SDIO card.
 * \param isFixedAddress During transfer the data address is never increased.
 * \param pData          Pointer to data buffer.
 * \param size           Size of data to read (1 ~ 512).
 * \param fCallback      Callback function invoked when transfer finished.
 * \param pArg           Pointer to callback argument.
 * \return 0 if successful; otherwise returns an \ref sdmmc_rc "error code".
 */
uint8_t SDIO_ReadBytes(SdCard *pSd,
                       uint8_t functionNum,
                       uint32_t address,
                       uint8_t isFixedAddress,
                       uint8_t *pData,
                       uint16_t size,
                       SdmmcCallback fCallback,
                       void* pArg)
{
    uint8_t  error;
    uint32_t status;

    assert( pSd != NULL ) ;

    if (pSd->cardType & CARD_TYPE_bmSDIO) {
        if (size == 0) return SDMMC_ERROR_PARAM;
        error = Cmd53(pSd,
                      0, functionNum, 0, !isFixedAddress,
                      address, pData, size,
                      &status,
                      fCallback, pArg);
        if (error) {
            TRACE_ERROR("IO_RdBytes.Cmd53: %u\n\r", error);
            return SDMMC_ERROR;
        }
        else if (status & STATUS_SDIO_R5) {
            TRACE_ERROR("RD_EXT st %x\n\r", status);
            return SDMMC_ERROR;
        }
    }
    else {
        return SDMMC_ERROR_NOT_SUPPORT;
    }
    return 0;
}

/**
 * Write byte by byte to SDIO card, using RW_EXTENDED command.
 * \param pSd            Pointer to SdCard instance.
 * \param functionNum    Function number.
 * \param address        First byte address of data in SDIO card.
 * \param isFixedAddress During transfer the data address is never increased.
 * \param pData          Pointer to data buffer.
 * \param size           Size of data to write (1 ~ 512).
 * \param fCallback      Callback function invoked when transfer finished.
 * \param pArg           Pointer to callback argument.
 * \return 0 if successful; otherwise returns an \ref sdmmc_rc "error code".
 */
uint8_t SDIO_WriteBytes(SdCard *pSd,
                        uint8_t functionNum,
                        uint32_t address,
                        uint8_t isFixedAddress,
                        uint8_t *pData,
                        uint16_t size,
                        SdmmcCallback fCallback,
                        void* pArg)
{
    uint8_t  error;
    uint32_t status;

    assert( pSd != NULL ) ;

    if (pSd->cardType & CARD_TYPE_bmSDIO) {
        if (size == 0) return SDMMC_ERROR_PARAM;
        error = Cmd53(pSd,
                      1, functionNum, 0, !isFixedAddress,
                      address, pData, size,
                      (uint32_t*)&status,
                      fCallback, pArg);
        Delay(100);
        if (error) {
            TRACE_ERROR("IO_WrBytes.Cmd53: %u\n\r", error);
            return SDMMC_ERROR;
        }
        else if (status & STATUS_SDIO_R5) {
            TRACE_ERROR("WR_EXT st %x\n\r", status);
            return SDMMC_ERROR;
        }
    }
    else {
        return SDMMC_ERROR_NOT_SUPPORT;
    }
    return 0;
}

/**
 * Display SDIO card informations (CIS, tuple ...)
 * \param pSd Pointer to SdCard instance.
 */
void SDIO_DisplayCardInformation(SdCard * pSd)
{
    uint32_t tmp = 0, addrCIS = 0, addrManfID = 0, addrFunc0 = 0;
    uint8_t *p = (uint8_t*)&tmp;
    uint8_t buf[8];

    switch(pSd->cardType)
    {
        case CARD_SDIO:
            TRACE_INFO("** SDIO ONLY card\n\r");
            break;
        case CARD_SDCOMBO: case CARD_SDHCCOMBO:
            TRACE_INFO("** SDIO Combo card\n\r");
            break;
        default:
            TRACE_INFO("** NO SDIO\n\r");
            return;
    }
    /* CCCR */
    TRACE_INFO("====== CCCR ======\n\r");
    SDIO_ReadDirect(pSd, SDIO_CIA, SDIO_CCCR_REG, p, 1);
    TRACE_INFO(".SDIO       %02X\n\r", (tmp & SDIO_SDIO) >> 4);
    TRACE_INFO(".CCCR       %02X\n\r", (tmp & SDIO_CCCR) >> 0);
    SDIO_ReadDirect(pSd, SDIO_CIA, SDIO_SD_REV_REG, p, 1);
    TRACE_INFO(".SD         %02X\n\r", (tmp & SDIO_SD) >> 0);
    SDIO_ReadDirect(pSd, SDIO_CIA, SDIO_IOE_REG, p, 1);
    TRACE_INFO(".IOE        %02X\n\r", (tmp & SDIO_IOE) >> 0);
    SDIO_ReadDirect(pSd, SDIO_CIA, SDIO_IOR_REG, p, 1);
    TRACE_INFO(".IOR        %02X\n\r", (tmp & SDIO_IOR) >> 0);
    SDIO_ReadDirect(pSd, SDIO_CIA, SDIO_IEN_REG, p, 1);
    TRACE_INFO(".IEN        %02X\n\r", (tmp & SDIO_IEN) >> 0);
    SDIO_ReadDirect(pSd, SDIO_CIA, SDIO_INT_REG, p, 1);
    TRACE_INFO(".INT        %02X\n\r", (tmp & SDIO_INT) >> 0);
    SDIO_ReadDirect(pSd, SDIO_CIA, SDIO_BUS_CTRL_REG, p, 1);
    TRACE_INFO(".CD         %x\n\r", (tmp & SDIO_CD) >> 7);
    TRACE_INFO(".SCSI       %x\n\r", (tmp & SDIO_SCSI) >> 6);
    TRACE_INFO(".ECSI       %x\n\r", (tmp & SDIO_ECSI) >> 5);
    TRACE_INFO(".BUS_WIDTH  %x\n\r", (tmp & SDIO_BUSWIDTH) >> 0);
    SDIO_ReadDirect(pSd, SDIO_CIA, SDIO_CAP_REG, p, 1);
    TRACE_INFO(".4BLS       %x\n\r", (tmp & SDIO_4BLS) >> 7);
    TRACE_INFO(".LSC        %x\n\r", (tmp & SDIO_LSC) >> 6);
    TRACE_INFO(".E4MI       %x\n\r", (tmp & SDIO_E4MI) >> 5);
    TRACE_INFO(".S4MI       %x\n\r", (tmp & SDIO_S4MI) >> 4);
    TRACE_INFO(".SBS        %x\n\r", (tmp & SDIO_SBS) >> 3);
    TRACE_INFO(".SRW        %x\n\r", (tmp & SDIO_SRW) >> 2);
    TRACE_INFO(".SMB        %x\n\r", (tmp & SDIO_SMB) >> 1);
    TRACE_INFO(".SDC        %x\n\r", (tmp & SDIO_SDC) >> 0);
    SDIO_ReadDirect(pSd, SDIO_CIA, SDIO_CIS_PTR_REG, p, 3);
    TRACE_INFO(".CIS_PTR    %06X\n\r", tmp);
    addrCIS = tmp; tmp = 0;
    SDIO_ReadDirect(pSd, SDIO_CIA, SDIO_BUS_SUSP_REG, p, 1);
    TRACE_INFO(".BR         %x\n\r", (tmp & SDIO_BR) >> 1);
    TRACE_INFO(".BS         %x\n\r", (tmp & SDIO_BS) >> 0);
    SDIO_ReadDirect(pSd, SDIO_CIA, SDIO_FUN_SEL_REG, p, 1);
    TRACE_INFO(".DF         %x\n\r", (tmp & SDIO_DF) >> 7);
    TRACE_INFO(".FS         %x\n\r", (tmp & SDIO_FS) >> 0);
    SDIO_ReadDirect(pSd, SDIO_CIA, SDIO_EXEC_REG, p, 1);
    TRACE_INFO(".EX         %x\n\r", (tmp & SDIO_EX));
    TRACE_INFO(".EXM        %x\n\r", (tmp & SDIO_EXM) >> 0);
    SDIO_ReadDirect(pSd, SDIO_CIA, SDIO_READY_REG, p, 1);
    TRACE_INFO(".RF         %x\n\r", (tmp & SDIO_RF));
    TRACE_INFO(".RFM        %x\n\r", (tmp & SDIO_RFM) >> 0);
    SDIO_ReadDirect(pSd, SDIO_CIA, SDIO_FN0_BLKSIZ_REG, p, 2);
    TRACE_INFO(".FN0_SIZE   %d(%04X)\n\r", tmp, tmp);
    tmp = 0;
    SDIO_ReadDirect(pSd, SDIO_CIA, SDIO_POWER_REG, p, 1);
    TRACE_INFO(".EMPC       %x\n\r", (tmp & SDIO_EMPC) >> 1);
    TRACE_INFO(".SMPC       %x\n\r", (tmp & SDIO_SMPC) >> 0);
    SDIO_ReadDirect(pSd, SDIO_CIA, SDIO_HS_REG, p, 1);
    TRACE_INFO(".EHS        %x\n\r", (tmp & SDIO_EHS) >> 1);
    TRACE_INFO(".SHS        %x\n\r", (tmp & SDIO_SHS) >> 0);
    /* Metaformat */
    SdioFindTuples(pSd, addrCIS, 128, &addrManfID, &addrFunc0);
    if (addrManfID != 0) {
        SDIO_ReadDirect(pSd, SDIO_CIA, addrManfID, buf, 6);
        TRACE_INFO("==== CISTPL_MANFID ====\n\r");
        TRACE_INFO("._MANF %04X\n\r", buf[2] + (buf[3] << 8));
        TRACE_INFO("._CARD %04X\n\r", buf[4] + (buf[5] << 8));
    }
    if (addrFunc0 != 0) {
        SDIO_ReadDirect(pSd, SDIO_CIA, addrFunc0, buf, 6);
        TRACE_INFO("== CISTPL_FUNCE Fun0 ==\n\r");
        TRACE_INFO("._FN0_BLK_SIZE   %d(0x%04X)\n\r",
            buf[3] + (buf[4] << 8), buf[3] + (buf[4] << 8));
        TRACE_INFO("._MAX_TRAN_SPEED %02X\n\r", buf[5]);
    }
}

/**
 * Display the content of the CID register
 * \param pSd Pointer to SdCard instance.
 */
void SD_DisplayRegisterCID(SdCard *pSd)
{
    TRACE_INFO("======= CID =======\n\r");
    TRACE_INFO(" .MID Manufacturer ID       %02X\n\r",
        SD_CID_MID(pSd));

    TRACE_INFO(" .CBX Card/BGA              %X\n\r", SD_CID_CBX(pSd));

    TRACE_INFO(" .OID OEM/Application ID    %c%c\n\r",
        (char)SD_CID_OID_BYTE_1(pSd),
        (char)SD_CID_OID_BYTE_0(pSd));

    TRACE_INFO(" .PNM Product revision      %c%c%c%c%c\n\r",
        (char)SD_CID_PNM_BYTE_4(pSd),
        (char)SD_CID_PNM_BYTE_3(pSd),
        (char)SD_CID_PNM_BYTE_2(pSd),
        (char)SD_CID_PNM_BYTE_1(pSd),
        (char)SD_CID_PNM_BYTE_0(pSd));

    TRACE_INFO(" .PRV Product serial number %02X%04X\n\r",
         SD_CID_PRV_2(pSd),
         SD_CID_PRV_1(pSd));

    TRACE_INFO(" .MDT Manufacturing date    %04d/%02d\n\r",
        (uint16_t)SD_CID_MDT_YEAR(pSd),
        (uint8_t)SD_CID_MDT_MONTH(pSd));

    TRACE_INFO(" .CRC checksum              %02X\n\r",
         SD_CID_CRC(pSd));
}

/**
 * Display the content of the CSD register
 * \param pSd Pointer to SdCard instance.
 */
void SD_DisplayRegisterCSD(SdCard *pSd)
{
    TRACE_INFO("======== CSD ========");
    TRACE_INFO_WP("\n\r");
    TRACE_INFO(" .CSD_STRUCTURE      0x%x\r\n", SD_CSD_STRUCTURE(pSd));
    TRACE_INFO(" .SPEC_VERS          0x%x\r\n", SD_CSD_SPEC_VERS(pSd));
    TRACE_INFO(" .TAAC               0x%X\r\n", SD_CSD_TAAC(pSd)              );
    TRACE_INFO(" .NSAC               0x%X\r\n", SD_CSD_NSAC(pSd)              );
    TRACE_INFO(" .TRAN_SPEED         0x%X\r\n", SD_CSD_TRAN_SPEED(pSd)        );
    TRACE_INFO(" .CCC                0x%X\r\n", SD_CSD_CCC(pSd)               );
    TRACE_INFO(" .READ_BL_LEN        0x%X\r\n", SD_CSD_READ_BL_LEN(pSd)       );
    TRACE_INFO(" .READ_BL_PARTIAL    0x%X\r\n", SD_CSD_READ_BL_PARTIAL(pSd)   );
    TRACE_INFO(" .WRITE_BLK_MISALIGN 0x%X\r\n", SD_CSD_WRITE_BLK_MISALIGN(pSd));
    TRACE_INFO(" .READ_BLK_MISALIGN  0x%X\r\n", SD_CSD_READ_BLK_MISALIGN(pSd) );
    TRACE_INFO(" .DSR_IMP            0x%X\r\n", SD_CSD_DSR_IMP(pSd)           );
    TRACE_INFO(" .C_SIZE             0x%X\r\n", SD_CSD_C_SIZE(pSd)            );
    TRACE_INFO(" .C_SIZE_HC          0x%X\r\n", SD_CSD_C_SIZE_HC(pSd)         );
    TRACE_INFO(" .VDD_R_CURR_MIN     0x%X\r\n", SD_CSD_VDD_R_CURR_MIN(pSd)    );
    TRACE_INFO(" .VDD_R_CURR_MAX     0x%X\r\n", SD_CSD_VDD_R_CURR_MAX(pSd)    );
    TRACE_INFO(" .VDD_W_CURR_MIN     0x%X\r\n", SD_CSD_VDD_W_CURR_MIN(pSd)    );
    TRACE_INFO(" .VDD_W_CURR_MAX     0x%X\r\n", SD_CSD_VDD_W_CURR_MAX(pSd)    );
    TRACE_INFO(" .C_SIZE_MULT        0x%X\r\n", SD_CSD_C_SIZE_MULT(pSd)       );
    TRACE_INFO(" .ERASE_BLK_EN       0x%X\r\n", SD_CSD_ERASE_BLK_EN(pSd)      );
    TRACE_INFO(" .SECTOR_SIZE        0x%X\r\n", SD_CSD_SECTOR_SIZE(pSd)       );
    TRACE_INFO(" .WP_GRP_SIZE        0x%X\r\n", SD_CSD_WP_GRP_SIZE(pSd)       );
    TRACE_INFO(" .WP_GRP_ENABLE      0x%X\r\n", SD_CSD_WP_GRP_ENABLE(pSd)     );
    TRACE_INFO(" .R2W_FACTOR         0x%X\r\n", SD_CSD_R2W_FACTOR(pSd)        );
    TRACE_INFO(" .WRITE_BL_LEN       0x%X\r\n", SD_CSD_WRITE_BL_LEN(pSd)      );
    TRACE_INFO(" .WRITE_BL_PARTIAL   0x%X\r\n", SD_CSD_WRITE_BL_PARTIAL(pSd)  );
    TRACE_INFO(" .FILE_FORMAT_GRP    0x%X\r\n", SD_CSD_FILE_FORMAT_GRP(pSd)   );
    TRACE_INFO(" .COPY               0x%X\r\n", SD_CSD_COPY(pSd)              );
    TRACE_INFO(" .PERM_WRITE_PROTECT 0x%X\r\n", SD_CSD_PERM_WRITE_PROTECT(pSd));
    TRACE_INFO(" .TMP_WRITE_PROTECT  0x%X\r\n", SD_CSD_TMP_WRITE_PROTECT(pSd) );
    TRACE_INFO(" .FILE_FORMAT        0x%X\r\n", SD_CSD_FILE_FORMAT(pSd)       );
    TRACE_INFO(" .ECC                0x%X\r\n", SD_CSD_ECC(pSd)               );
    TRACE_INFO(" .CRC                0x%X\r\n", SD_CSD_CRC(pSd)               );
    TRACE_INFO(" .MULT               0x%X\r\n", SD_CSD_MULT(pSd)              );
    TRACE_INFO(" .BLOCKNR            0x%X\r\n", SD_CSD_BLOCKNR(pSd)           );
    TRACE_INFO(" .BLOCKNR_HC         0x%X\r\n", SD_CSD_BLOCKNR_HC(pSd)        );
    TRACE_INFO(" .BLOCK_LEN          0x%X\r\n", SD_CSD_BLOCK_LEN(pSd)         );
    TRACE_INFO(" .TOTAL_SIZE         0x%X\r\n", SD_CSD_TOTAL_SIZE(pSd)        );
    TRACE_INFO(" .TOTAL_SIZE_HC      0x%X\r\n", SD_CSD_TOTAL_SIZE_HC(pSd)     );
    TRACE_INFO(" -SD_TOTAL_SIZE      0x%X\r\n", SD_TOTAL_SIZE(pSd)            );
    TRACE_INFO(" -SD_TOTAL_BLOCK     0x%X\r\n", SD_TOTAL_BLOCK(pSd)           );
}

/**
 * Display the content of the EXT_CSD register
 * \param pSd Pointer to SdCard instance.
 */
void SD_DisplayRegisterECSD(SdCard *pSd)
{
    if ((pSd->cardType & CARD_TYPE_bmSDMMC) == CARD_MMC
        && MMC_IsVer4(pSd)
        && SD_CSD_STRUCTURE(pSd) >= 2) {}
    else {
        TRACE_INFO("** EXT_CSD NOT SUPPORTED\n\r");
        return;
    }
    TRACE_INFO("======= EXT_CSD =======");
    TRACE_INFO_WP("\n\r");
    TRACE_INFO(" .S_CMD_SET            : 0x%X\n\r",
        SD_EXTCSD_S_CMD_SET(pSd));
    TRACE_INFO(" .BOOT_INFO            : 0x%X\n\r",
        SD_EXTCSD_BOOT_INFO(pSd));
    TRACE_INFO(" .BOOT_SIZE_MULTI      : 0x%X\n\r",
        SD_EXTCSD_BOOT_SIZE_MULTI(pSd));
    TRACE_INFO(" .ACC_SIZE             : 0x%X\n\r",
        SD_EXTCSD_ACC_SIZE(pSd));
    TRACE_INFO(" .HC_ERASE_GRP_SIZE    : 0x%X\n\r",
        SD_EXTCSD_HC_ERASE_GRP_SIZE(pSd));
    TRACE_INFO(" .ERASE_TIMEOUT_MULT   : 0x%X\n\r",
        SD_EXTCSD_ERASE_TIMEOUT_MULT(pSd));
    TRACE_INFO(" .REL_WR_SEC_C         : 0x%X\n\r",
        SD_EXTCSD_REL_WR_SEC_C(pSd));
    TRACE_INFO(" .HC_WP_GRP_SIZE       : 0x%X\n\r",
        SD_EXTCSD_HC_WP_GRP_SIZE(pSd));
    TRACE_INFO(" .S_C_VCC              : 0x%X\n\r",
        SD_EXTCSD_S_C_VCC(pSd));
    TRACE_INFO(" .S_C_VCCQ             : 0x%X\n\r",
        SD_EXTCSD_S_C_VCCQ(pSd));
    TRACE_INFO(" .S_A_TIMEOUT          : 0x%X\n\r",
        SD_EXTCSD_S_A_TIMEOUT(pSd));
    TRACE_INFO(" .SEC_COUNT            : 0x%X\n\r",
        SD_EXTCSD_SEC_COUNT(pSd));
    TRACE_INFO(" .MIN_PERF_W_8_52      : 0x%X\n\r",
        SD_EXTCSD_MIN_PERF_W_8_52(pSd));
    TRACE_INFO(" .MIN_PERF_R_8_52      : 0x%X\n\r",
        SD_EXTCSD_MIN_PERF_R_8_52(pSd));
    TRACE_INFO(" .MIN_PERF_W_8_26_4_52 : 0x%X\n\r",
        SD_EXTCSD_MIN_PERF_W_8_26_4_52(pSd));
    TRACE_INFO(" .MIN_PERF_R_8_26_4_52 : 0x%X\n\r",
        SD_EXTCSD_MIN_PERF_R_8_26_4_52(pSd));
    TRACE_INFO(" .MIN_PERF_W_4_26      : 0x%X\n\r",
        SD_EXTCSD_MIN_PERF_W_4_26(pSd));
    TRACE_INFO(" .MIN_PERF_R_4_26      : 0x%X\n\r",
        SD_EXTCSD_MIN_PERF_R_4_26(pSd));
    TRACE_INFO(" .PWR_CL_26_360        : 0x%X\n\r",
        SD_EXTCSD_PWR_CL_26_360(pSd));
    TRACE_INFO(" .PWR_CL_52_360        : 0x%X\n\r",
        SD_EXTCSD_PWR_CL_52_360(pSd));
    TRACE_INFO(" .PWR_CL_26_195        : 0x%X\n\r",
        SD_EXTCSD_PWR_CL_26_195(pSd));
    TRACE_INFO(" .PWR_CL_52_195        : 0x%X\n\r",
        SD_EXTCSD_PWR_CL_52_195(pSd));
    TRACE_INFO(" .CARD_TYPE            : 0x%X\n\r",
        SD_EXTCSD_CARD_TYPE(pSd));
    TRACE_INFO(" .CSD_STRUCTURE        : 0x%X\n\r",
        SD_EXTCSD_CSD_STRUCTURE(pSd));
    TRACE_INFO(" .EXT_CSD_REV          : 0x%X\n\r",
        SD_EXTCSD_EXT_CSD_REV(pSd));
    TRACE_INFO(" .CMD_SET              : 0x%X\n\r",
        SD_EXTCSD_CMD_SET(pSd));
    TRACE_INFO(" .CMD_SET_REV          : 0x%X\n\r",
        SD_EXTCSD_CMD_SET_REV(pSd));
    TRACE_INFO(" .POWER_CLASS          : 0x%X\n\r",
        SD_EXTCSD_POWER_CLASS(pSd));
    TRACE_INFO(" .HS_TIMING            : 0x%X\n\r",
        SD_EXTCSD_HS_TIMING(pSd));
    TRACE_INFO(" .BUS_WIDTH            : 0x%X\n\r",
        SD_EXTCSD_BUS_WIDTH(pSd));
    TRACE_INFO(" .ERASED_MEM_CONT      : 0x%X\n\r",
        SD_EXTCSD_ERASED_MEM_CONT(pSd));
    TRACE_INFO(" .BOOT_CONFIG          : 0x%X\n\r",
        SD_EXTCSD_BOOT_CONFIG(pSd));
    TRACE_INFO(" .BOOT_BUS_WIDTH       : 0x%X\n\r",
        SD_EXTCSD_BOOT_BUS_WIDTH(pSd));
    TRACE_INFO(" .ERASE_GROUP_DEF      : 0x%X\n\r",
        SD_EXTCSD_ERASE_GROUP_DEF(pSd));
}

/**
 * Display the content of the SCR register
 * \param pSd  Pointer to SdCard instance.
 */
void SD_DisplayRegisterSCR(SdCard *pSd)
{
    if (pSd->cardType >= CARD_SD && pSd->cardType <= CARD_SDHC) {}
    else {
        TRACE_INFO("** SCR NOT Supported!\n\r");
        return;
    }
    TRACE_INFO("========== SCR ==========");
    TRACE_INFO_WP("\n\r");

    TRACE_INFO(" .SCR_STRUCTURE         :0x%X\n\r",
        SD_SCR_SCR_STRUCTURE(pSd));
    TRACE_INFO(" .SD_SPEC               :0x%X\n\r",
        SD_SCR_SD_SPEC(pSd));
    TRACE_INFO(" .DATA_STAT_AFTER_ERASE :0x%X\n\r",
        SD_SCR_DATA_STAT_AFTER_ERASE(pSd));
    TRACE_INFO(" .SD_SECURITY           :0x%X\n\r",
        SD_SCR_SD_SECURITY(pSd));
    TRACE_INFO(" .SD_BUS_WIDTHS         :0x%X\n\r",
        SD_SCR_SD_BUS_WIDTHS(pSd));
}

/**
 * Display the content of the SD Status
 * \param pSd  Pointer to SdCard instance.
 */
void SD_DisplaySdStatus(SdCard *pSd)
{
    if (   pSd->cardType >= CARD_SD
        && pSd->cardType <= CARD_SDHC
        && (pSd->optCmdBitMap & SD_ACMD13_SUPPORT) ) {}
    else {
        TRACE_INFO("** SD Status NOT Supported!\n\r");
        return;
    }
    TRACE_INFO("=========== STAT ============");
    TRACE_INFO_WP("\n\r");

    TRACE_INFO(" .DAT_BUS_WIDTH          :0x%X\n\r",
        SD_STAT_DAT_BUS_WIDTH(pSd));
    TRACE_INFO(" .SECURED_MODE           :0x%X\n\r",
        SD_STAT_SECURED_MODE(pSd));
    TRACE_INFO(" .SD_CARD_TYPE           :0x%X\n\r",
        SD_STAT_SD_CARD_TYPE(pSd));
    TRACE_INFO(" .SIZE_OF_PROTECTED_AREA :0x%X\n\r",
        SD_STAT_SIZE_OF_PROTECTED_AREA(pSd));
    TRACE_INFO(" .SPEED_CLASS            :0x%X\n\r",
        SD_STAT_SPEED_CLASS(pSd));
    TRACE_INFO(" .PERFORMANCE_MOVE       :0x%X\n\r",
        SD_STAT_PERFORMANCE_MOVE(pSd));
    TRACE_INFO(" .AU_SIZE                :0x%X\n\r",
        SD_STAT_AU_SIZE(pSd));
    TRACE_INFO(" .ERASE_SIZE             :0x%X\n\r",
        SD_STAT_ERASE_SIZE(pSd));
    TRACE_INFO(" .ERASE_TIMEOUT          :0x%X\n\r",
        SD_STAT_ERASE_TIMEOUT(pSd));
    TRACE_INFO(" .ERASE_OFFSET           :0x%X\n\r",
        SD_STAT_ERASE_OFFSET(pSd));
}
/**@}*/
