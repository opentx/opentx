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
 *  \page sdmmc_drv SD/MMC Driver Interface
 *
 *  \section Purpose
 *
 *  Implementation for sdcard sd mode physical layer driver. Supply a set of sdcard sd mode's
 *  interface.
 *
 *  \section Usage
 *  - General Card Support
 *    -# SD_Init(): Run the SDcard initialization sequence
 *    -# SD_GetCardType() : Return SD/MMC reported card type.
 *  - SD/MMC Memory Card Operations
 *    -# SD_ReadBlock() : Read one block of data
 *    -# SD_WriteBlock() : Write one block of data
 *    -# SD_ReadBlocks() : Read blocks of data
 *    -# SD_WriteBlocks() : Write blocks of data
 *    -# SD_Read() : Read blocks of data with multi-access command
 *                   (Optimized read, see \ref sdmmc_read_op).
 *    -# SD_Write() : Read blocks of data with multi-access command
 *                    (Optimized write, see \ref sdmmc_write_op).
 *    -# SD_GetNumberBlocks() : Return SD/MMC card reported number of blocks.
 *    -# SD_GetBlockSize() : Return SD/MMC card reported block size.
 *    -# SD_GetTotalSizeKB() : Return size of SD/MMC card in KBytes.
 *  - SDIO Card Operations
 *    -# SDIO_ReadDirect() : Read bytes from registers.
 *    -# SDIO_WriteDirect() : Write one byte to register.
 *    -# SDIO_ReadBytes() : Read 1 ~ 512 bytes from card.
 *    -# SDIO_WriteBytes() : Write 1 ~ 512 bytes to card.
 */

#ifndef SDMMC_H
#define SDMMC_H
/** \addtogroup sdmmc_lib Working with SD/MMC
    Please refer to \ref sdmmc_lib.
 * @{
 *  \addtogroup sdmmc_api SD/MMC Driver API
 *  \addtogroup sdmmc_hal SD/MMC Low Level Driver
 */
/**@}*/

/** \addtogroup sdmmc_api
 *  @{
 *    \addtogroup sdmmc_mem_api SD/MMC Memroy API
 *    @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
 *         Header
 *----------------------------------------------------------------------------*/

#include "sdio.h"
#include "sdmmc_cmd.h"

/*----------------------------------------------------------------------------
 *         Constants
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 *         Macros
 *----------------------------------------------------------------------------*/

/** CID register access macros (128 bits, 4 * 32 bits). */
#define SD_CID(pSd, bitfield, bits)   (  (pSd->cid[3-(bitfield)/32] >> ((bitfield)%32)) & ((1 << (bits)) - 1))
#define SD_CID_MID(pSd)               SD_CID(pSd, 120, 8)      ///< Manufacturer ID
#define SD_CID_BGA(pSd)               SD_CID(pSd, 112, 2)      ///< Card/BGA(eMMC)
#define SD_CID_CBX(pSd)               SD_CID(pSd, 112, 2)      ///< Card/BGA(eMMC)
#define SD_CID_OID_BYTE_1(pSd)        SD_CID(pSd, 112, 8)      ///< OEM/Application ID byte 1
#define SD_CID_OID_BYTE_0(pSd)        SD_CID(pSd, 104, 8)      ///< OEM/Application ID byte 0
#define SD_CID_PNM_BYTE_4(pSd)        SD_CID(pSd,  96, 8)      ///< Product revision byte 4
#define SD_CID_PNM_BYTE_3(pSd)        SD_CID(pSd,  88, 8)      ///< Product revision byte 3
#define SD_CID_PNM_BYTE_2(pSd)        SD_CID(pSd,  80, 8)      ///< Product revision byte 2
#define SD_CID_PNM_BYTE_1(pSd)        SD_CID(pSd,  72, 8)      ///< Product revision byte 1
#define SD_CID_PNM_BYTE_0(pSd)        SD_CID(pSd,  64, 8)      ///< Product revision byte 0
#define SD_CID_PRV_1(pSd)             SD_CID(pSd,  24, 8)      ///< Product serial number 1
#define SD_CID_PRV_2(pSd)             SD_CID(pSd,  32,24)      ///< Product serial number 2
#define SD_CID_MDT_YEAR(pSd)          (SD_CID(pSd, 12, 8))+2000///< Manufacturing date year
#define SD_CID_MDT_MONTH(pSd)         SD_CID(pSd,   8, 4)      ///< Manufacturing date month
#define SD_CID_CRC(pSd)               SD_CID(pSd,   1, 7)      ///< CRC7 checksum

/** CSD register access macros (128 bits, 4 * 32 bits). */
#define SD_CSD(pSd, bitfield, bits)   ((((pSd)->csd)[3-(bitfield)/32] >> ((bitfield)%32)) & ((1 << (bits)) - 1))
#define SD_CSD_STRUCTURE(pSd)          SD_CSD(pSd, 126, 2) ///< CSD structure 00b  Version 1.0 01b version 2.0 High Cap
#define SD_CSD_SPEC_VERS(pSd)          SD_CSD(pSd, 122, 4) ///< System Specification Version Number
#define SD_CSD_TAAC(pSd)               SD_CSD(pSd, 112, 8) ///< Data read-access-time-1
#define SD_CSD_NSAC(pSd)               SD_CSD(pSd, 104, 8) ///< Data read access-time-2 in CLK cycles
#define SD_CSD_TRAN_SPEED(pSd)         SD_CSD(pSd, 96,  8) ///< Max. data transfer rate
#define SD_CSD_CCC(pSd)                SD_CSD(pSd, 84, 12) ///< Card command class
#define SD_CSD_READ_BL_LEN(pSd)        SD_CSD(pSd, 80,  4) ///< Max. read data block length
#define SD_CSD_READ_BL_PARTIAL(pSd)    SD_CSD(pSd, 79,  1) ///< Bartial blocks for read allowed
#define SD_CSD_WRITE_BLK_MISALIGN(pSd) SD_CSD(pSd, 78,  1) ///< Write block misalignment
#define SD_CSD_READ_BLK_MISALIGN(pSd)  SD_CSD(pSd, 77,  1) ///< Read block misalignment
#define SD_CSD_DSR_IMP(pSd)            SD_CSD(pSd, 76,  1) ///< DSP implemented
#define SD_CSD_C_SIZE(pSd)             ((SD_CSD(pSd, 72,  2) << 10) + \
                                        (SD_CSD(pSd, 64,  8) << 2)  + \
                                        SD_CSD(pSd, 62,  2)) ///< Device size
#define SD_CSD_C_SIZE_HC(pSd)          ((SD_CSD(pSd, 64,  6) << 16) + \
                                        (SD_CSD(pSd, 56,  8) << 8)  + \
                                        SD_CSD(pSd, 48,  8)) ///< Device size v2.0 High Capacity
#define SD_CSD_VDD_R_CURR_MIN(pSd)     SD_CSD(pSd, 59,  3) ///< Max. read current VDD min
#define SD_CSD_VDD_R_CURR_MAX(pSd)     SD_CSD(pSd, 56,  3) ///< Max. read current VDD max
#define SD_CSD_VDD_W_CURR_MIN(pSd)     SD_CSD(pSd, 53,  3) ///< Max. write current VDD min
#define SD_CSD_VDD_W_CURR_MAX(pSd)     SD_CSD(pSd, 50,  3) ///< Max. write current VDD max
#define SD_CSD_C_SIZE_MULT(pSd)        SD_CSD(pSd, 47,  3) ///< Device size multiplier
#define SD_CSD_ERASE_BLK_EN(pSd)       SD_CSD(pSd, 46,  1) ///< Erase single block enable
#define MMC_CSD_ERASE_BLK_EN(pSd)      SD_CSD(pSd, 46,  1) ///< Erase single block enable
#define MMC_CSD_ERASE_GRP_SIZE(pSd)    SD_CSD(pSd, 42,  4) ///< Erase group size
#define SD_CSD_ERASE_GRP_MULT(pSd)     SD_CSD(pSd, 37,  4) ///< Erase group size multiplier
#define SD_CSD_SECTOR_SIZE(pSd)        ((SD_CSD(pSd, 40,  6) << 1) + SD_CSD(pSd, 39,  1)) ///< Erase sector size
#define SD_CSD_WP_GRP_SIZE(pSd)        SD_CSD(pSd, 32,  7) ///< Write protect group size
#define SD_CSD_WP_GRP_ENABLE(pSd)      SD_CSD(pSd, 31,  1) ///< write protect group enable
#define SD_CSD_R2W_FACTOR(pSd)         SD_CSD(pSd, 26,  3) ///< Write speed factor
#define SD_CSD_WRITE_BL_LEN(pSd)       ((SD_CSD(pSd, 24,  2) << 2) + SD_CSD(pSd, 22,  2)) ///< Max write block length
#define SD_CSD_WRITE_BL_PARTIAL(pSd)   SD_CSD(pSd, 21,  1) ///< Partial blocks for write allowed
#define SD_CSD_CONTENT_PROT_APP(pSd)   SD_CSD(pSd, 16,  1) ///< File format group
#define SD_CSD_FILE_FORMAT_GRP(pSd)    SD_CSD(pSd, 15,  1) ///< File format group
#define SD_CSD_COPY(pSd)               SD_CSD(pSd, 14,  1) ///< Copy flag (OTP)
#define SD_CSD_PERM_WRITE_PROTECT(pSd) SD_CSD(pSd, 13,  1) ///< Permanent write protect
#define SD_CSD_TMP_WRITE_PROTECT(pSd)  SD_CSD(pSd, 12,  1) ///< Temporary write protection
#define SD_CSD_FILE_FORMAT(pSd)        SD_CSD(pSd, 10,  2) ///< File format
#define SD_CSD_ECC(pSd)                SD_CSD(pSd,  8,  2) ///< CRC
#define SD_CSD_CRC(pSd)                SD_CSD(pSd,  1,  7) ///< CRC
#define SD_CSD_MULT(pSd)               (1 << (SD_CSD_C_SIZE_MULT(pSd) + 2))
#define SD_CSD_BLOCKNR(pSd)            ((SD_CSD_C_SIZE(pSd) + 1) * SD_CSD_MULT(pSd))
#define SD_CSD_BLOCKNR_HC(pSd)         ((SD_CSD_C_SIZE_HC(pSd) + 1) * 1024)
#define SD_CSD_BLOCK_LEN(pSd)          (1 << SD_CSD_READ_BL_LEN(pSd))
#define SD_CSD_TOTAL_SIZE(pSd)         (SD_CSD_BLOCKNR(pSd) * SD_CSD_BLOCK_LEN(pSd))
#define SD_CSD_TOTAL_SIZE_HC(pSd)      ((SD_CSD_C_SIZE_HC(pSd) + 1) * 512* 1024)
#define SD_TOTAL_SIZE(pSd)             ((pSd)->totalSize)
#define SD_TOTAL_BLOCK(pSd)            ((pSd)->blockNr)

/** SCR register access macros (64 bits, 2 * 32 bits, 8 * 8 bits). */
#define SD_EXT_OFFSET_SD_SCR                0   // DW
#define SD_SCR(pSd, bitfield, bits) \
    ( ((char*)(pSd)->extData)[7 - ((bitfield)/8)] >> ((bitfield)%8) \
                                                  & ((1 << (bits)) - 1) \
    )
#define SD_SCR_SCR_STRUCTURE(pSd)           SD_SCR(pSd, 60, 4)
#define     SD_SCR_SCR_STRUCTURE_1_0        0
#define SD_SCR_SD_SPEC(pSd)                 SD_SCR(pSd, 56, 4)
#define     SD_SCR_SD_SPEC_1_0_01           0
#define     SD_SCR_SD_SPEC_1_10             1
#define     SD_SCR_SD_SPEC_2_00             2
#define SD_SCR_DATA_STAT_AFTER_ERASE(pSd)   SD_SCR(pSd, 55, 1)
#define SD_SCR_SD_SECURITY(pSd)             SD_SCR(pSd, 52, 3)
#define     SD_SCR_SD_SECURITY_NO           0
#define     SD_SCR_SD_SECURITY_NOTUSED      1
#define     SD_SCR_SD_SECURITY_1_01         2
#define     SD_SCR_SD_SECURITY_2_00         3
#define SD_SCR_SD_BUS_WIDTHS(pSd)           SD_SCR(pSd, 48, 4)
#define     SD_SCR_SD_BUS_WIDTH_1BITS       (1 << 0)
#define     SD_SCR_SD_BUS_WIDTH_4BITS       (1 << 2)

/** SD Status access macros (512 bits, 16 * 32 bits, 64 * 8 bits). */
#define SD_EXT_OFFSET_SD_STAT               2   // DW
#define SD_STAT(pSd, bitfield, bits) \
    ( ((char*)&(pSd)->extData[2])[63 - ((bitfield)/8)] >> ((bitfield)%8) \
                                                  & ((1 << (bits)) - 1) \
    )
/** Bus width, 00: default, 10:4-bit */
#define SD_STAT_DAT_BUS_WIDTH(pSd)           SD_STAT(pSd, 510, 2)
#define     SD_STAT_DATA_BUS_WIDTH_1BIT      0x0
#define     SD_STAT_DATA_BUS_WIDTH_4BIT      0x2
/// Secured Mode
#define SD_STAT_SECURED_MODE(pSd)            SD_STAT(pSd, 509, 1)
/// SD Memory Cards as defined in 1.01~2.00
#define SD_STAT_SD_CARD_TYPE(pSd)           (SD_STAT(pSd, 480, 8) \
                                           + (SD_STAT(pSd, 488, 8) << 8) )
/// STD: ThisSize*Multi*BlockLen, HC: Size in bytes
#define SD_STAT_SIZE_OF_PROTECTED_AREA(pSd) (SD_STAT(pSd, 448, 8) \
                                           + (SD_STAT(pSd, 456, 8) <<  8) \
                                           + (SD_STAT(pSd, 464, 8) << 16) \
                                           + (SD_STAT(pSd, 472, 8) << 24) )
/// Speed Class, value can be calculated by Pw/2
#define SD_STAT_SPEED_CLASS(pSd)             SD_STAT(pSd, 440, 8)
#define     SD_STAT_SPEED_CLASS_0            0
#define     SD_STAT_SPEED_CLASS_2            1  // >= 2MB/s
#define     SD_STAT_SPEED_CLASS_4            2  // >= 4MB/s
#define     SD_STAT_SPEED_CLASS_6            3  // >= 6MB/s
/// 8-bit, by 1MB/s step.
#define SD_STAT_PERFORMANCE_MOVE(pSd)        SD_STAT(pSd, 432, 8)
/// AU Size, in power of 2 from 16KB
#define SD_STAT_AU_SIZE(pSd)                 SD_STAT(pSd, 428, 4)
#define     SD_STAT_AU_SIZE_16K              1
#define     SD_STAT_AU_SIZE_32K              2
#define     SD_STAT_AU_SIZE_64K              3
#define     SD_STAT_AU_SIZE_128K             4
#define     SD_STAT_AU_SIZE_256K             5
#define     SD_STAT_AU_SIZE_512K             6
#define     SD_STAT_AU_SIZE_1M               7
#define     SD_STAT_AU_SIZE_2M               8
#define     SD_STAT_AU_SIZE_4M               9
/// 16-bit, number of AUs erased.
#define SD_STAT_ERASE_SIZE(pSd)             (SD_STAT(pSd, 408, 8) \
                                          + (SD_STAT(pSd, 416, 8) << 8) )
#define SD_STAT_ERASE_TIMEOUT(pSd)           SD_STAT(pSd, 402, 6)
#define SD_STAT_ERASE_OFFSET(pSd)            SD_STAT(pSd, 400, 2)

// SD Switch Status access macros (512 bits, 16 * 32 bits, 64 * 8 bits).
#define SD_SW_STAT(p, bitfield, bits) \
    ( ((char*)(p))[63 - ((bitfield)/8)] >> ((bitfield)%8) \
                                                  & ((1 << (bits)) - 1) \
    )
#define SD_SW_STAT_MAX_CURRENT_CONSUMPTION(p)   ( SD_SW_STAT(p, 496, 8) \
                                               + (SD_SW_STAT(p, 504, 8) << 8) )
#define SD_SW_STAT_FUN_GRP6_INFO(p)             ( SD_SW_STAT(p, 480, 8) \
                                               + (SD_SW_STAT(p, 488, 8) << 8) )
#define SD_SW_STAT_FUN_GRP5_INFO(p)             ( SD_SW_STAT(p, 464, 8) \
                                               + (SD_SW_STAT(p, 472, 8) << 8) )
#define SD_SW_STAT_FUN_GRP4_INFO(p)             ( SD_SW_STAT(p, 448, 8) \
                                               + (SD_SW_STAT(p, 456, 8) << 8) )
#define SD_SW_STAT_FUN_GRP3_INFO(p)             ( SD_SW_STAT(p, 432, 8) \
                                               + (SD_SW_STAT(p, 440, 8) << 8) )
#define SD_SW_STAT_FUN_GRP2_INFO(p)             ( SD_SW_STAT(p, 416, 8) \
                                               + (SD_SW_STAT(p, 424, 8) << 8) )
#define SD_SW_STAT_FUN_GRP1_INFO(p)             ( SD_SW_STAT(p, 400, 8) \
                                               + (SD_SW_STAT(p, 408, 8) << 8) )
#define SD_SW_STAT_FUN_GRP6_RC(p)                SD_SW_STAT(p, 396, 4)
#define SD_SW_STAT_FUN_GRP5_RC(p)                SD_SW_STAT(p, 392, 4)
#define SD_SW_STAT_FUN_GRP4_RC(p)                SD_SW_STAT(p, 388, 4)
#define SD_SW_STAT_FUN_GRP3_RC(p)                SD_SW_STAT(p, 384, 4)
#define SD_SW_STAT_FUN_GRP2_RC(p)                SD_SW_STAT(p, 380, 4)
#define SD_SW_STAT_FUN_GRP1_RC(p)                SD_SW_STAT(p, 376, 4)
#define     SD_SW_STAT_FUN_GRP_RC_ERROR         0xF
#define SD_SW_STAT_DATA_STRUCT_VER(p)            SD_SW_STAT(p, 368, 8)
#define SD_SW_STAT_FUN_GRP6_BUSY(p)             ( SD_SW_STAT(p, 352, 8) \
                                               + (SD_SW_STAT(p, 360, 8) << 8) )
#define SD_SW_STAT_FUN_GRP5_BUSY(p)             ( SD_SW_STAT(p, 336, 8) \
                                               + (SD_SW_STAT(p, 344, 8) << 8) )
#define SD_SW_STAT_FUN_GRP4_BUSY(p)             ( SD_SW_STAT(p, 320, 8) \
                                               + (SD_SW_STAT(p, 328, 8) << 8) )
#define SD_SW_STAT_FUN_GRP3_BUSY(p)             ( SD_SW_STAT(p, 304, 8) \
                                               + (SD_SW_STAT(p, 312, 8) << 8) )
#define SD_SW_STAT_FUN_GRP2_BUSY(p)             ( SD_SW_STAT(p, 288, 8) \
                                               + (SD_SW_STAT(p, 296, 8) << 8) )
#define SD_SW_STAT_FUN_GRP1_BUSY(p)             ( SD_SW_STAT(p, 272, 8) \
                                               + (SD_SW_STAT(p, 280, 8) << 8) )
#define SD_SW_STAT_FUN_GRP_FUN_BUSY(funNdx)     (1 << (funNdx))

// EXT_CSD register definition.
#define SD_EXTCSD_S_CMD_SET_INDEX              504   // Below belongs to Properties Segment
#define SD_EXTCSD_BOOT_INFO_INDEX              228
#define SD_EXTCSD_BOOT_SIZE_MULTI_INDEX        226
#define SD_EXTCSD_ACC_SIZE_INDEX               225
#define SD_EXTCSD_HC_ERASE_GRP_SIZE_INDEX      224
#define SD_EXTCSD_ERASE_TIMEOUT_MULT_INDEX     223
#define SD_EXTCSD_REL_WR_SEC_C_INDEX           222
#define SD_EXTCSD_HC_WP_GRP_SIZE_INDEX         221
#define SD_EXTCSD_S_C_VCC_INDEX                220
#define SD_EXTCSD_S_C_VCCQ_INDEX               219
#define SD_EXTCSD_S_A_TIMEOUT_INDEX            217
#define SD_EXTCSD_SEC_COUNT_INDEX              212
#define SD_EXTCSD_MIN_PERF_W_8_52_INDEX        210
#define SD_EXTCSD_MIN_PERF_R_8_52_INDEX        209
#define SD_EXTCSD_MIN_PERF_W_8_26_4_52_INDEX   208
#define SD_EXTCSD_MIN_PERF_R_8_26_4_52_INDEX   207
#define SD_EXTCSD_MIN_PERF_W_4_26_INDEX        206
#define SD_EXTCSD_MIN_PERF_R_4_26_INDEX        205
#define SD_EXTCSD_PWR_CL_26_360_INDEX          203
#define SD_EXTCSD_PWR_CL_52_360_INDEX          202
#define SD_EXTCSD_PWR_CL_26_195_INDEX          201
#define SD_EXTCSD_PWR_CL_52_195_INDEX          200
#define SD_EXTCSD_CARD_TYPE_INDEX              196
#define SD_EXTCSD_CSD_STRUCTURE_INDEX          194
#define SD_EXTCSD_EXT_CSD_REV_INDEX            192

#define SD_EXTCSD_CMD_SET_INDEX                191  //Below belongs to Mode Segment
#define SD_EXTCSD_CMD_SET_REV_INDEX            189
#define SD_EXTCSD_POWER_CLASS_INDEX            187
#define SD_EXTCSD_HS_TIMING_INDEX              185
#define SD_EXTCSD_BUS_WIDTH_INDEX              183
#define SD_EXTCSD_ERASED_MEM_CONT_INDEX        181
#define SD_EXTCSD_BOOT_CONFIG_INDEX            179
#define SD_EXTCSD_BOOT_BUS_WIDTH_INDEX         177
#define SD_EXTCSD_ERASE_GROUP_DEF_INDEX        175

// Ext_CSD register access marco
#define MMC_EXTCSD_U8(pSd, bytefield) \
    ( ((uint8_t*)((pSd)->extData))[(bytefield)] )
#define MMC_EXTCSD_U16(pSd, bytefield) \
    ( (((uint8_t*)((pSd)->extData))[(bytefield)    ] << 0) + \
      (((uint8_t*)((pSd)->extData))[(bytefield) + 1] << 8) )
#define MMC_EXTCSD_U32(pSd, bytefield) \
    ( (((uint8_t*)((pSd)->extData))[(bytefield)     ] <<  0) + \
      (((uint8_t*)((pSd)->extData))[(bytefield) +  1] <<  8) + \
      (((uint8_t*)((pSd)->extData))[(bytefield) +  2] << 16) + \
      (((uint8_t*)((pSd)->extData))[(bytefield) + 24] << 24) )
#define MMC_EXTCSD(pSd) ((uint8_t*)((pSd)->extData))
#define SD_EXTCSD_S_CMD_SET(pSd)               (MMC_EXTCSD(pSd)[SD_EXTCSD_S_CMD_SET_INDEX]) // Supported Command Sets
#define SD_EXTCSD_BOOT_INFO(pSd)               (MMC_EXTCSD(pSd)[SD_EXTCSD_BOOT_INFO_INDEX]) // Boot information
#define SD_EXTCSD_BOOT_SIZE_MULTI(pSd)         (MMC_EXTCSD(pSd)[SD_EXTCSD_BOOT_SIZE_MULTI_INDEX]) // Boot partition size
#define SD_EXTCSD_ACC_SIZE(pSd)                (MMC_EXTCSD(pSd)[SD_EXTCSD_ACC_SIZE_INDEX]) // Access size
#define SD_EXTCSD_HC_ERASE_GRP_SIZE(pSd)       (MMC_EXTCSD(pSd)[SD_EXTCSD_HC_ERASE_GRP_SIZE_INDEX]) // High-capacity erase unit size
#define SD_EXTCSD_ERASE_TIMEOUT_MULT(pSd)      (MMC_EXTCSD(pSd)[SD_EXTCSD_ERASE_TIMEOUT_MULT_INDEX]) // High-capacity erase timeout
#define SD_EXTCSD_REL_WR_SEC_C(pSd)            (MMC_EXTCSD(pSd)[SD_EXTCSD_REL_WR_SEC_C_INDEX]) // Reliable write sector count
#define SD_EXTCSD_HC_WP_GRP_SIZE(pSd)          (MMC_EXTCSD(pSd)[SD_EXTCSD_HC_WP_GRP_SIZE_INDEX]) // High-capacity write protect group size
#define SD_EXTCSD_S_C_VCC(pSd)                 (MMC_EXTCSD(pSd)[SD_EXTCSD_S_C_VCC_INDEX]) // Sleep current(VCC)
#define SD_EXTCSD_S_C_VCCQ(pSd)                (MMC_EXTCSD(pSd)[SD_EXTCSD_S_C_VCCQ_INDEX]) // Sleep current(VCCQ)
#define SD_EXTCSD_S_A_TIMEOUT(pSd)             (MMC_EXTCSD(pSd)[SD_EXTCSD_S_A_TIMEOUT_INDEX]) // Sleep/awake timeout
#define SD_EXTCSD_SEC_COUNT(pSd)               ((MMC_EXTCSD(pSd)[SD_EXTCSD_SEC_COUNT_INDEX]) + \
                                                (MMC_EXTCSD(pSd)[SD_EXTCSD_SEC_COUNT_INDEX+1] << 8 ) + \
                                                (MMC_EXTCSD(pSd)[SD_EXTCSD_SEC_COUNT_INDEX+2] << 16 ) + \
                                                (MMC_EXTCSD(pSd)[SD_EXTCSD_SEC_COUNT_INDEX+3] << 24 )) //Sector Count
#define SD_EXTCSD_MIN_PERF_W_8_52(pSd)         (MMC_EXTCSD(pSd)[SD_EXTCSD_MIN_PERF_W_8_52_INDEX]) // Minimum Write Performance for 8bit at 52MHz
#define SD_EXTCSD_MIN_PERF_R_8_52(pSd)         (MMC_EXTCSD(pSd)[SD_EXTCSD_MIN_PERF_R_8_52_INDEX]) // Minimum Read Performance for 8bit at 52MHz
#define SD_EXTCSD_MIN_PERF_W_8_26_4_52(pSd)    (MMC_EXTCSD(pSd)[SD_EXTCSD_MIN_PERF_W_8_26_4_52_INDEX]) // Minimum Write Performance for 8bit at 26MHz, for 4bit at 52MHz
#define SD_EXTCSD_MIN_PERF_R_8_26_4_52(pSd)    (MMC_EXTCSD(pSd)[SD_EXTCSD_MIN_PERF_R_8_26_4_52_INDEX]) // Minimum Read Performance for 8bit at 26MHz, for 4bit at 52MHz
#define SD_EXTCSD_MIN_PERF_W_4_26(pSd)         (MMC_EXTCSD(pSd)[SD_EXTCSD_MIN_PERF_W_4_26_INDEX]) // Minimum Write Performance for 4bit at 26MHz
#define SD_EXTCSD_MIN_PERF_R_4_26(pSd)         (MMC_EXTCSD(pSd)[SD_EXTCSD_MIN_PERF_R_4_26_INDEX]) // Minimum Read Performance for 4bit at 26MHz
#define SD_EXTCSD_PWR_CL_26_360(pSd)           (MMC_EXTCSD(pSd)[SD_EXTCSD_PWR_CL_26_360_INDEX]) // Power class for 26MHz at 3.6v
#define SD_EXTCSD_PWR_CL_52_360(pSd)           (MMC_EXTCSD(pSd)[SD_EXTCSD_PWR_CL_52_360_INDEX]) // Power class for 52MHz at 3.6v
#define SD_EXTCSD_PWR_CL_26_195(pSd)           (MMC_EXTCSD(pSd)[SD_EXTCSD_PWR_CL_26_195_INDEX]) // Power class for 26MHz at 1.95v
#define SD_EXTCSD_PWR_CL_52_195(pSd)           (MMC_EXTCSD(pSd)[SD_EXTCSD_PWR_CL_52_195_INDEX]) // Power class for 52MHz at 1.95v
#define SD_EXTCSD_CARD_TYPE(pSd)               (MMC_EXTCSD(pSd)[SD_EXTCSD_CARD_TYPE_INDEX]) // Card type
#define SD_EXTCSD_CSD_STRUCTURE(pSd)           (MMC_EXTCSD(pSd)[SD_EXTCSD_CSD_STRUCTURE_INDEX]) // CSD structure version
#define SD_EXTCSD_EXT_CSD_REV(pSd)             (MMC_EXTCSD(pSd)[SD_EXTCSD_EXT_CSD_REV_INDEX]) // Extended CSD structure version
#define SD_EXTCSD_CMD_SET(pSd)                 (MMC_EXTCSD(pSd)[SD_EXTCSD_CMD_SET_INDEX]) // Command set
#define SD_EXTCSD_CMD_SET_REV(pSd)             (MMC_EXTCSD(pSd)[SD_EXTCSD_CMD_SET_REV_INDEX]) // Command set revision
#define SD_EXTCSD_POWER_CLASS(pSd)             (MMC_EXTCSD(pSd)[SD_EXTCSD_POWER_CLASS_INDEX]) // Power class
#define SD_EXTCSD_HS_TIMING(pSd)               (MMC_EXTCSD(pSd)[SD_EXTCSD_HS_TIMING_INDEX]) // High-speed interface timing
#define SD_EXTCSD_BUS_WIDTH(pSd)               (MMC_EXTCSD(pSd)[SD_EXTCSD_BUS_WIDTH_INDEX]) // Bus width mode
#define SD_EXTCSD_ERASED_MEM_CONT(pSd)         (MMC_EXTCSD(pSd)[SD_EXTCSD_ERASED_MEM_CONT_INDEX]) // Erased memory content
#define SD_EXTCSD_BOOT_CONFIG(pSd)             (MMC_EXTCSD(pSd)[SD_EXTCSD_BOOT_CONFIG_INDEX]) // Boot configuration
#define SD_EXTCSD_BOOT_BUS_WIDTH(pSd)          (MMC_EXTCSD(pSd)[SD_EXTCSD_BOOT_BUS_WIDTH_INDEX]) // Boot bus width
#define SD_EXTCSD_ERASE_GROUP_DEF(pSd)         (MMC_EXTCSD(pSd)[SD_EXTCSD_ERASE_GROUP_DEF_INDEX]) // High-density erase group definition

// EXTCSD total size and block number
#define SD_EXTCSD_TOTAL_SIZE(pSd)              (SD_EXTCSD_SEC_COUNT(pSd)*512)
#define SD_EXTCSD_BLOCKNR(pSd)                 (SD_EXTCSD_SEC_COUNT(pSd))

// Bus width Byte
#define SD_EXTCSD_BUS_WIDTH_1BIT               (0x0UL)
#define SD_EXTCSD_BUS_WIDTH_4BIT               (0x1UL)
#define SD_EXTCSD_BUS_WIDTH_8BIT               (0x2UL)

// High speed mode
#define SD_EXTCSD_HS_TIMING_ENABLE             (0x1UL)
#define SD_EXTCSD_HS_TIMING_DISABLE            (0x0UL)

// Boot config
#define SD_EXTCSD_BOOT_PARTITION_ACCESS        (0x7UL) // boot partition access
#define SD_EXTCSD_BOOT_PART_NO_ACCESS          (0x0UL)
#define SD_EXTCSD_BOOT_PART_RW_PART1           (0x1UL)
#define SD_EXTCSD_BOOT_PART_RW_PART2           (0x2UL)
#define SD_EXTCSD_BOOT_PARTITION_ENABLE        (0x7UL << 3) // boot partition enable
#define SD_EXTCSD_BOOT_PART_DISABLE            (0x0UL << 3)
#define SD_EXTCSD_BOOT_PART_ENABLE_PART1       (0x1UL << 3)
#define SD_EXTCSD_BOOT_PART_ENABLE_PART2       (0x2UL << 3)
#define SD_EXTCSD_BOOT_PART_ENABLE_USER        (0x7UL << 3)
#define SD_EXTCSD_BOOT_PARTITION_ACK           (0x1UL << 7) // boot acknowledge
#define SD_EXTCSD_BOOT_PART_NOACK              (0x0UL << 7)
#define SD_EXTCSD_BOOT_PART_ACK                (0x1UL << 7)

// Boot bus width
#define SD_EXTCSD_BOOT_BUS_WIDTH_BIT           (0x3UL) // boot bus width
#define SD_EXTCSD_BOOT_BUS_1BIT                (0x0UL)
#define SD_EXTCSD_BOOT_BUS_4BIT                (0x1UL)
#define SD_EXTCSD_BOOT_BUS_8BIT                (0x2UL)
#define SD_EXTCSD_RESET_BOOT_BUS_WIDTH_BIT     (0x1UL << 2) // boot bus width
#define SD_EXTCSD_RESET_BOOT_BUS               (0x0UL << 2)
#define SD_EXTCSD_RETAIN_BOOT_BUS              (0x1UL << 2)

// Mode Switch Arguments for CMD6
#define MMC_CMD6_ARG_ACCESS
#define MMC_CMD6_ARG_INDEX
#define MMC_CMD6_ARG_VALUE
#define MMC_CMD6_ARG_CMDSET

/* Response Status Code Bits */
#define SDMMC_SC_OUT_OF_RANGE       (1UL << 31) /**< address out of the allowed range */
#define SDMMC_SC_ADDR_ERROR         (1UL << 30) /**< address misaligned to the card physical blocks */
#define SDMMC_SC_BLOCK_LEN_ERROR    (1UL << 29) /**< argument of SET_BLOCKLEN or defined block length illegal */
#define SDMMC_SC_ERASE_SEQ_ERROR    (1UL << 28) /**< error in the sequence of erase commands */
#define SDMMC_SC_ERASE_PARAM        (1UL << 27) /**< invalid selection of erase groups */
#define SDMMC_SC_WP_VIOLATION       (1UL << 26) /**< program a write protected block */
#define SDMMC_SC_CARD_IS_LOCKED     (1UL << 25) /**< card is locked by host */
#define SDMMC_SC_LOCK_UNLOCK_FAILED (1UL << 24) /**< sequence or password error in lock/unlock command */
#define SDMMC_SC_COM_CRC_ERROR      (1UL << 23) /**< Command CRC error */
#define SDMMC_SC_ILLEGAL_COMMAND    (1UL << 22) /**< Not legal for the card state */
#define SDMMC_SC_ECC_FAILED         (1UL << 21) /**< Card internal ECC failed */
#define SDMMC_SC_CC_ERROR           (1UL << 20) /**< Internal card controller error */
#define SDMMC_SC_ERROR              (1UL << 19) /**< General error */
#define SDMMC_SC_UNDERRUN           (1UL << 18) /**< Could not sustain data transfer in stream read mode */
#define SDMMC_SC_OVERRUN            (1UL << 17) /**< Could not sustain data programming in stream write mode */
#define SDMMC_SC_CIDCSD_OVERWRITE   (1UL << 16) /**< CID, RO section of CSD can not be overwritten */
#define SDMMC_SC_WP_ERASE_SKIP      (1UL << 15) /**< Only partial address erased due to write protection */
#define SDMMC_SC_ECC_DISABLED       (1UL << 14) /**< Command executed without internal ECC */
#define SDMMC_SC_ERASE_RESET        (1UL << 13) /**< An erase sequence was cleared before executing */
#define SDMMC_SC_CURRENT_STATE      (0xFUL << 9)/**< Card state */
#define SDMMC_SC_IDLE           0UL
#define SDMMC_SC_STATE_IDLE     (0UL << 9)
#define SDMMC_SC_READY          1UL
#define SDMMC_SC_STATE_READY    (1UL << 9)
#define SDMMC_SC_IDENT          2UL
#define SDMMC_SC_STATE_IDENT    (2UL << 9)
#define SDMMC_SC_STBY           3UL
#define SDMMC_SC_STATE_STBY     (3UL << 9)
#define SDMMC_SC_TRAN           4UL
#define SDMMC_SC_STATE_TRAN     (4UL << 9)
#define SDMMC_SC_DATA           5UL
#define SDMMC_SC_STATE_DATA     (5UL << 9)
#define SDMMC_SC_RCV            6UL
#define SDMMC_SC_STATE_RCV      (6UL << 9)
#define SDMMC_SC_PRG            7UL
#define SDMMC_SC_STATE_PRG      (7UL << 9)
#define SDMMC_SC_DIS            8UL
#define SDMMC_SC_STATE_DIS      (8UL << 9)
#define SDMMC_SC_BTST           9
#define SDMMC_SC_STATE_BTST     (9UL << 9)
#define SDMMC_SC_SLP            10
#define SDMMC_SC_STATE_SLP      (10UL << 9)
#define SDMMC_SC_READY_FOR_DATA     (1UL << 8)  /**< buffer empty signalling */
#define SDMMC_SC_SWITCH_ERROR       (1UL << 7)  /**< did not switch to expected mode */
#define SDMMC_SC_APP_CMD            (1UL << 5)  /**< Expect ACMD or command is interpreted as ACMD */
#define SDMMC_SC_AKE_SEQ_ERROR      (1UL << 3)  /**< Error in the sequence of the authentication process */

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------

extern uint8_t SD_Init(SdCard *pSd,
                       void   *pSdDriver);

extern uint8_t SD_GetCardType(SdCard * pSd);

extern uint32_t SD_GetNumberBlocks(SdCard * pSd);

extern uint32_t SD_GetBlockSize(SdCard * pSd);

extern uint32_t SD_GetTotalSizeKB(SdCard * pSd);

extern uint8_t SD_Read(SdCard        *pSd,
                       uint32_t      address,
                       void          *pData,
                       uint16_t      length,
                       SdmmcCallback pCallback,
                       void          *pArgs);

extern uint8_t SD_Write(SdCard        *pSd,
                        uint32_t      address,
                        void          *pData,
                        uint16_t      length,
                        SdmmcCallback pCallback,
                        void          *pArgs);

extern uint8_t SD_ReadBlock(
    SdCard *pSd,
    uint32_t address,
    uint8_t *pData);

extern uint8_t SD_WriteBlock(
    SdCard *pSd,
    uint32_t address,
    uint8_t *pData);

extern uint8_t SD_ReadBlocks(
    SdCard *pSd,
    uint32_t address,
    uint16_t nbBlocks,
    uint8_t *pData);

extern uint8_t SD_WriteBlocks(
    SdCard *pSd,
    uint32_t address,
    uint16_t nbBlocks,
    uint8_t *pData);

extern uint8_t SDIO_ReadDirect(
    SdCard * pSd,
    uint8_t functionNum,
    uint32_t address,
    uint8_t * pData,
    uint32_t size);

extern uint8_t SDIO_WriteDirect(
    SdCard * pSd,
    uint8_t functionNum,
    uint32_t address,
    uint8_t dataByte);

extern uint8_t SDIO_ReadBytes(
    SdCard * pSd,
    uint8_t functionNum,
    uint32_t address,
    uint8_t isFixedAddress,
    uint8_t * pData,
    uint16_t size,
    SdmmcCallback fCallback,
    void * pArg);

extern uint8_t SDIO_WriteBytes(
    SdCard * pSd,
    uint8_t functionNum,
    uint32_t address,
    uint8_t isFixedAddress,
    uint8_t * pData,
    uint16_t size,
    SdmmcCallback fCallback,
    void * pArg);

extern void SDIO_DisplayCardInformation(SdCard *pSd);

extern void SD_DisplayRegisterCID(SdCard *pSd);

extern void SD_DisplayRegisterCSD(SdCard *pSd);

extern void SD_DisplayRegisterECSD(SdCard * pSd);

extern void SD_DisplayRegisterSCR(SdCard * pSd);

extern void SD_DisplaySdStatus(SdCard * pSd);

#ifdef __cplusplus
}
#endif

/**   @}*/
/** @}*/
#endif //#ifndef SDMMC_H

