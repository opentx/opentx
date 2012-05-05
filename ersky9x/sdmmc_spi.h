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
/// \page "sdmmc_spi"
///
/// !Purpose
/// 
/// Implementation for sdcard spi mode physical layer driver. Supply a set of sdcard spi mode's
/// interface.
///
/// !Usage
///
/// -# SD_Init: Run the SDcard initialization sequence
/// -# SD_SPI_Init : Run the SDcard SPI Mode initialization sequence
/// -# SD_Stop: Stop the SDcard by sending Cmd12
/// -# SD_ReadBlock : Read blocks of data
/// -# SD_WriteBlock : Write blocks of data
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "sdcard spi mode initialization and identification"
///
/// !Purpose
/// 
/// sdcard spi mode initialization and identification sequence
///
/// !Description
/// - Host sends Cmd0 to do card reset, card is in "idle state".
/// - Host sends Cmd8 and checks the response of the card, only sdcard supports physical
///    layer version 2.00 will response correctly to Cmd8, and this command is mandatory to be
///    sent before ACmd41 for sdcard which support physical layer version 2.00, to enable new
///    functions or to validate a dual-voltage card.
///       - refer to "If Cmd8 response ok" branch for the initialize of sdcard 2.0.
///       - refer to "If Cmd8 response fail" branch for the initialize of sdcard 1.x, mmc card, sdcard2.0
///          with invalid voltage.
/// - If Cmd8 response ok
///       - Host sends Cmd58 to read OCR register to validate the voltage.
///          - If the response to Cmd58 is fail, initialize ends, the card is put into inactive state.
///          - If the response to Cmd58 is ok, continue to the next step.
///       - Host sends ACmd41* with argument "HCS" equal to "1". 
///       - If the response to ACmd41 failed, it means the card does not match the voltage 
///          desired by the host, the card will be put into inactive state, initialize ends.
///       - If the response with "CCS" equal to "1", the card is a version 2.0 high capacity sdcard, 
///          refer to "Card Initialize" for the succeeding initialize sequence.
///       - If the response with "CCS" equal to "0", the card is a version 2.0 standard capacity sdcard.
///          refer to "Card Initialize" for the succeeding initialize sequence.
/// - If Cmd8 response fail
///       - Host sends Cmd58 to read OCR register to validate the voltage.
///          - If the response to Cmd58 is fail, initialize ends, the card is put into inactive state.
///          - If the response to Cmd58 is ok, continue to the next step.
///       - Host sends ACmd41* argument "HCS" equal to "0".
///       - If the response to ACmd41 ok, the card is a version 1.x sdcard, refer to "Card Initialize" for 
///          the succeeding initialize sequence.
///       - If the response to ACmd41 fails
///          - Host sends Cmd0 to reset card.
///          - Host sends Cmd1 to card.
///          - If card has response to Cmd1, the card is a MMC card, refer to "Card Initialize" for the 
///             succeeding initialize sequence.
///          - If card has no response to Cmd1, the card is either an unknown card or a card does 
///             not match host's voltage, the initialize ends.
/// - Card Initialize
///       - At this stage, the initialization and identification process is over, the following steps are done 
///          for the sdcard's succeeding operation.
///       - Host sends Cmd59 to turn sdcard's CRC option off.
///       - Host sends Cmd9 to get the Card Specific Data (CSD).
///
///     \note Send Cmd55 before send ACmd41. \endnote
///     \note sdcard include ver 1.x sdcard, ver2.0 standard capacity sdcard, ver2.0 high capacity sdcard \endnote
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "sdcard spi mode write"
///
/// !Purpose
/// 
/// sdcard spi mode write process
///
/// !Description
/// - Make sure sdcard is under "transfer state", if the sdcard is under other state, host will send
///    Cmd12 or use stop transmission token to stop multiple block write, and to transit sdcard to 
///    "stand-by state".
/// - Host sends Cmd13 to check sdcard's status, to make sure sdcard is "ready-for-data".
/// - Host sends Cmd25 to do multiple blocks write, the address here is different between high capacity
///    sdcard and normal sdcard, the address of SDHC is equal to the block number, while normal sdcard's
///    address is equal to  block number times 512.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "sdcard spi mode read"
///
/// !Purpose
/// 
/// sdcard spi mode read process
///
/// !Description
/// - Make sure sdcard is under "transfer state", if the sdcard is under other state, host will send
///    Cmd12 or use stop transmission token to stop multiple block read and to transit sdcard to 
///    "stand-by state".
/// - Host sends Cmd13 to check sdcard's status, to make sure sdcard is "ready-for-data".
/// - Host sends Cmd18 to do multiple blocks read, the address here is different between high capacity
///    sdcard and normal sdcard, the address of SDHC is equal to the block number, while normal sdcard's
///    address is equal to  block number times 512.
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/// \page "sdhc"
///
/// !Purpose
/// 
/// highlight of sdhc
///
/// !Sdhc initialization and identification
///
///   - Refer to page "sdcard spi mode initialization and identification" for the initialization and identification
///       sequence of a sdhc.
/// 
/// !Functional difference between sdhc and standard capacity sdcard
///
/// - Command argument is different:
///   - Sdhc uses block address format in memory access commands*, block length is fixed to 512 bytes.
///   - Standard capacity sdcard uses byte address format in memory access commands, block length
///       is defined in Cmd16.
/// - Partial access and misalign access are disabled in sdhc as the block address is used.
/// - Sdhc does not support write-protected commands (Cmd28, Cmd29, Cmd30).
///
/// \note Memory access commands means block read commands (CMD17, CMD18), block write commands 
///   (CMD24, CMD25), and block erase commands (CMD32, CMD33).
///
//------------------------------------------------------------------------------


#ifndef SDCARD_H
#define SDCARD_H

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

//------------------------------------------------------------------------------
//         Macros
//------------------------------------------------------------------------------

// CID register access macros.
#define SD_CID(pSd, bitfield, bits)   (  (pSd->cid[3-(bitfield)/32] >> ((bitfield)%32)) & ((1 << (bits)) - 1))
#define SD_CID_MID(pSd)               SD_CID(pSd, 120, 8)      ///< Manufacturer ID  
#define SD_CID_BGA(pSd)               SD_CID(pSd, 112, 2)      ///< Card/BGA(eMMC)
#define SD_CID_CBS(pSd)               SD_CID(pSd, 112, 2)      ///< Card/BGA(eMMC)
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

// CSD register access macros.
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
#define SD_CSD_VDD_R_CURR_MIN(pSd)     SD_CSD(pSd, 59,  3) ///< Max. read current @VDD min
#define SD_CSD_VDD_R_CURR_MAX(pSd)     SD_CSD(pSd, 56,  3) ///< Max. read current @VDD max
#define SD_CSD_VDD_W_CURR_MIN(pSd)     SD_CSD(pSd, 53,  3) ///< Max. write current @VDD min
#define SD_CSD_VDD_W_CURR_MAX(pSd)     SD_CSD(pSd, 50,  3) ///< Max. write current @VDD max
#define SD_CSD_C_SIZE_MULT(pSd)        SD_CSD(pSd, 47,  3) ///< Device size multiplier
#define SD_CSD_ERASE_BLK_EN(pSd)       SD_CSD(pSd, 46,  1) ///< Erase single block enable
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
#define SD_CSD_FILE_FORMAT(pSd)        SD_CSD(pSd, 11,  2) ///< File format
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

// SCR register access macros.
#define SD_SCR_BUS_WIDTHS(pScr)        ((pScr[1] >> 16) & 0xF) ///< Describes all the DAT bus that are supported by this card
#define SD_SCR_BUS_WIDTH_4BITS         (1 << 1) ///< 4bit Bus Width is supported
#define SD_SCR_BUS_WIDTH_1BIT          (1 << 0) ///< 1bit Bus Width is supported

//------------------------------------------------------------------------------
//         Types
//------------------------------------------------------------------------------

/// SD end-of-transfer callback function.
typedef void (*SdCallback)(unsigned char status, void *pCommand);

//------------------------------------------------------------------------------
/// SD Transfer Request prepared by the application upper layer. This structure
/// is sent to the SD_SendCommand function to start the transfer. At the end of
/// the transfer, the callback is invoked by the interrupt handler.
//------------------------------------------------------------------------------
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
    unsigned char conTrans;
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

//------------------------------------------------------------------------------
/// SD driver structure. Holds the internal state of the SD driver and
/// prevents parallel access to a SPI peripheral.
//------------------------------------------------------------------------------
typedef struct {

    /// Pointer to a SPI peripheral.
    //AT91S_MCI *pSdHw;
    /// SPI peripheral identifier.
    unsigned char spiId;
    /// MCI HW mode
    unsigned char mciMode;
    /// Pointer to currently executing command.
    SdCmd *pCommand;
    /// Mutex.
    volatile char semaphore;

} SdDriver;

//------------------------------------------------------------------------------
/// Sdcard driver structure. It holds the current command being processed and
/// the SD card address.
//------------------------------------------------------------------------------
typedef struct _SdCard {

    /// Pointer to the underlying MCI driver.
    SdDriver *pSdDriver;
    /// Current MCI command being processed.
    SdCmd command;
    /// SD card current address.
    unsigned short cardAddress;
    /// Card-specific data.
    unsigned int csd[4];
    /// Previous access block number.
    unsigned int preBlock;
    /// State after sd command complete
    unsigned char state;
    /// Card type
    unsigned char cardType;
    /// Card total size
    unsigned int totalSize;
    /// Card block number
    unsigned int blockNr;
    /// Card access mode
    unsigned char mode;

} SdCard;

//------------------------------------------------------------------------------
//         Global functions
//------------------------------------------------------------------------------

extern unsigned char SD_Init(SdCard *pSd,
                             SdDriver *pSdDriver);

extern unsigned char SD_ReadBlock(
    SdCard *pSd,
    unsigned int address,
    unsigned short nbBlocks,
    unsigned char *pData);

extern unsigned char SD_WriteBlock(
    SdCard *pSd,
    unsigned int address,
    unsigned short nbBlocks,
    const unsigned char *pData);

extern unsigned char SD_Stop(SdCard *pSd, SdDriver *pSdDriver);

#endif //#ifndef SDCARD_H

