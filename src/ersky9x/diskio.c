/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * open9x is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/*-----------------------------------------------------------------------*/
/* MMCv3/SDv1/SDv2 (in SPI mode) control module  (C)ChaN, 2010           */
/*-----------------------------------------------------------------------*/
/* Only rcvr_spi(), xmit_spi(), disk_timerproc() and some macros         */
/* are platform dependent.                                               */
/*-----------------------------------------------------------------------*/

#include "board.h"
#include "../FatFs/diskio.h"
#include "../FatFs/ff.h"
#include "../CoOS/kernel/CoOS.h"

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

// States for initialising card
#define SD_ST_EMPTY             0
#define SD_ST_INIT1             1
#define SD_ST_INIT2             2
#define SD_ST_IDLE              3
#define SD_ST_READY             4
#define SD_ST_IDENT             5
#define SD_ST_STBY              6
#define SD_ST_TRAN              7
#define SD_ST_DATA              8

uint32_t Card_ID[4] ;
uint32_t Card_SCR[2] ;
uint32_t Card_CSD[4] ;
uint32_t Card_state = SD_ST_EMPTY ;
uint32_t Sd_128_resp[4] ;
uint32_t Sd_rca ;
uint32_t Cmd_8_resp ;
uint32_t Cmd_A41_resp ;
uint8_t cardType;

/**
 * Configure the  MCI CLKDIV in the MCI_MR register. The max. for MCI clock is
 * MCK/2 and corresponds to CLKDIV = 0
 * \param mciSpeed  MCI clock speed in Hz, 0 will not change current speed.
 * \return The actual speed used, 0 for fail.
 */
uint32_t sdSetSpeed(uint32_t mciSpeed )
{
  uint32_t mciMr;
  uint32_t clkdiv;

  mciMr = HSMCI->HSMCI_MR & (~(uint32_t) HSMCI_MR_CLKDIV_Msk);
  /* Multimedia Card Interface clock (MCCK or MCI_CK) is Master Clock (MCK)
   * divided by (2*(CLKDIV+1))
   * mciSpeed = MCK / (2*(CLKDIV+1)) */
  if (mciSpeed > 0) {
    clkdiv = (Master_frequency / 2 / mciSpeed);
    /* Speed should not bigger than expired one */
    if (mciSpeed < Master_frequency / 2 / clkdiv) {
      clkdiv++;
    }

    if (clkdiv > 0) {
      clkdiv -= 1;
    }
  }
  else {
    clkdiv = 0;
  }

  /* Actual MCI speed */
  mciSpeed = Master_frequency / 2 / (clkdiv + 1);
  /* Modify MR */HSMCI->HSMCI_MR = mciMr | clkdiv;

  return mciSpeed;
}

#if 0
void SD_SetBlklen( uint32_t blklen )
{
      uint32_t mciMr;
      uint32_t clkdiv;

      mciMr = HSMCI->HSMCI_MR & (~(uint32_t)HSMCI_MR_BLKLEN);
      /* Modify MR */
      HSMCI->HSMCI_MR = mciMr | blklen << 16 ;
}
#endif

#if 0
/**
 * Reset MCI HW interface and disable it.
 * \param keepSettings Keep old register settings, including
 *                     _MR, _SDCR, _DTOR, _CSTOR, _DMA and _CFG.
 */
void SD_Reset( uint8_t keepSettings)
{
  Hsmci *pMciHw = HSMCI ;

  if (keepSettings)
  {
    uint32_t mr, sdcr, dtor, cstor;
    uint32_t cfg;
    mr    = pMciHw->HSMCI_MR;
    sdcr  = pMciHw->HSMCI_SDCR;
    dtor  = pMciHw->HSMCI_DTOR;
    cstor = pMciHw->HSMCI_CSTOR;
    cfg   = pMciHw->HSMCI_CFG;
    pMciHw->HSMCI_CR = HSMCI_CR_SWRST ;             // Reset
    pMciHw->HSMCI_CR = HSMCI_CR_MCIDIS;             // Disable
    pMciHw->HSMCI_MR    = mr;
    pMciHw->HSMCI_SDCR  = sdcr;
    pMciHw->HSMCI_DTOR  = dtor;
    pMciHw->HSMCI_CSTOR = cstor;
    pMciHw->HSMCI_CFG   = cfg;
  }
  else
  {
          pMciHw->HSMCI_CR = HSMCI_CR_SWRST ;             // Reset
          pMciHw->HSMCI_CR = HSMCI_CR_MCIDIS;           // Disable
  }
}
#endif

const char SD_NORESPONSE[] = "No response";

const char * sdCommand(uint32_t cmd, uint32_t arg)
{
  uint32_t i;
  Hsmci *phsmci = HSMCI;

  if (CardIsConnected()) {
    phsmci->HSMCI_ARGR = arg;
    phsmci->HSMCI_CMDR = cmd;

    for (i = 0; i < 30000; i += 1) {
      if (phsmci->HSMCI_SR & HSMCI_SR_CMDRDY) {
        return 0;
      }
    }
    return SD_NORESPONSE;
  }
  else {
    return "No Sdcard";
  }
}

#define SDMMC_POWER_ON_INIT (0 | HSMCI_CMDR_TRCMD_NO_DATA \
                               | HSMCI_CMDR_SPCMD_INIT \
                               | HSMCI_CMDR_OPDCMD )

const char * sdPowerOn()
{
  return sdCommand(SDMMC_POWER_ON_INIT, 0);
}

#define SDMMC_GO_IDLE_STATE     (0 | HSMCI_CMDR_TRCMD_NO_DATA \
                                   | HSMCI_CMDR_SPCMD_STD )

const char * sdCmd0()
{
  return sdCommand(SDMMC_GO_IDLE_STATE, 0);
}

#if 0
#define SDIO_SEND_OP_COND           (5 | HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_TRCMD_NO_DATA \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_OPDCMD )

const char * sdCmd5(uint32_t *pIo)
{
  const char * result = sdCommand(SDIO_SEND_OP_COND, *pIo);
  if (result)
    return result;
  *pIo = HSMCI->HSMCI_RSPR[0];
  return 0;
}
#endif

#define SD_SEND_IF_COND         (8 | HSMCI_CMDR_TRCMD_NO_DATA \
                                   | HSMCI_CMDR_SPCMD_STD \
                                   | HSMCI_CMDR_RSPTYP_48_BIT \
                                   | HSMCI_CMDR_OPDCMD /* BSS difference */ \
                                   | HSMCI_CMDR_MAXLAT )

const char * sdCmd8(uint8_t supplyVoltage)
{
  return sdCommand(SD_SEND_IF_COND, (supplyVoltage << 8) | (0xAA));
}

#define SDMMC_APP_CMD               (55| HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_TRCMD_NO_DATA \
                                       | HSMCI_CMDR_MAXLAT)

const char * sdCmd55()
{
  return sdCommand(SDMMC_APP_CMD, Sd_rca);
}

#define SD_SD_SEND_OP_COND          (41| HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_TRCMD_NO_DATA \
                                       /*| HSMCI_CMDR_OPDCMD_OPENDRAIN */ )

const char * sdCmd41(uint32_t arg, uint32_t * status)
{
  const char * result = sdCommand(SD_SD_SEND_OP_COND, arg);
  if (result)
    return result;
  *status = HSMCI->HSMCI_RSPR[0];
  return 0;
}

#define OCR_VDD_27_28          ((uint32_t)(1 << 15))
#define OCR_VDD_28_29          ((uint32_t)(1 << 16))
#define OCR_VDD_29_30          ((uint32_t)(1 << 17))
#define OCR_VDD_30_31          ((uint32_t)(1 << 18))
#define OCR_VDD_31_32          ((uint32_t)(1 << 19))
#define OCR_VDD_32_33          ((uint32_t)(1 << 20))
#define OCR_VDD_33_34          ((uint32_t)(1 << 21))

#define SDMMC_HOST_VOLTAGE_RANGE     (OCR_VDD_27_28 +\
                                      OCR_VDD_28_29 +\
                                      OCR_VDD_29_30 +\
                                      OCR_VDD_30_31 +\
                                      OCR_VDD_31_32 +\
                                      OCR_VDD_32_33 +\
                                      OCR_VDD_32_33) /* not in SAM3S reference code */

#define OCR_SD_CCS             (1UL << 30)

#define OCR_POWER_UP_BUSY      (1UL << 31)

const char * sdMemInit(uint8_t hcs, uint32_t *pCCS)
{
  const char * result;
  uint32_t arg;
  uint32_t status;
  do {
    result = sdCmd55();
    if (result)
      return result;
    arg = SDMMC_HOST_VOLTAGE_RANGE;
    if (hcs) arg |= OCR_SD_CCS;
    result = sdCmd41(arg, &status);
    if (result)
      return result;
    *pCCS = (status & OCR_SD_CCS);
  } while ((status & OCR_POWER_UP_BUSY) != OCR_POWER_UP_BUSY);
  return 0;
}

// Get Card ID
const char * sdCmd2()
{
  const char * result = sdCommand(0x00001082, 0);
  if (result)
    return result;

  Hsmci *phsmci = HSMCI ;
  Card_ID[0] = phsmci->HSMCI_RSPR[0] ;
  Card_ID[1] = phsmci->HSMCI_RSPR[1] ;
  Card_ID[2] = phsmci->HSMCI_RSPR[2] ;
  Card_ID[3] = phsmci->HSMCI_RSPR[3] ;
  return 0;
}

// Get new RCA
const char * sdCmd3()
{
  const char * result = sdCommand(0x00001043, 0);
  if (result)
    return result;
  Sd_rca = HSMCI->HSMCI_RSPR[0];
  return 0;
}

// Get CSD
const char * sdCmd9()
{
  const char * result = sdCommand(0x00001089, Sd_rca);
  if (result)
    return result;

  Hsmci *phsmci = HSMCI ;
  Card_CSD[0] = phsmci->HSMCI_RSPR[0] ;
  Card_CSD[1] = phsmci->HSMCI_RSPR[1] ;
  Card_CSD[2] = phsmci->HSMCI_RSPR[2] ;
  Card_CSD[3] = phsmci->HSMCI_RSPR[3] ;
  return 0;
}

/** Cmd7 MCI, ac, R1/R1b */
#define SDMMC_SELECT_CARD       (7 | HSMCI_CMDR_TRCMD_NO_DATA \
                                   | HSMCI_CMDR_SPCMD_STD \
                                   | HSMCI_CMDR_RSPTYP_R1B \
                                   | HSMCI_CMDR_MAXLAT /* TODO not the same */ )

const char * sdCmd7()
{
  return sdCommand(SDMMC_SELECT_CARD, Sd_rca);
}

#define SD_SEND_SCR  (51 | HSMCI_CMDR_SPCMD_STD | HSMCI_CMDR_RSPTYP_48_BIT \
                         | HSMCI_CMDR_TRCMD_START_DATA | HSMCI_CMDR_TRDIR_READ \
                         | HSMCI_CMDR_TRTYP_SINGLE | HSMCI_CMDR_MAXLAT)

#define SDMMC_SET_BLOCKLEN       (16 | HSMCI_CMDR_TRCMD_NO_DATA \
                                     | HSMCI_CMDR_SPCMD_STD \
                                     | HSMCI_CMDR_RSPTYP_48_BIT \
                                     | HSMCI_CMDR_MAXLAT_64 )

#define SD_READ_SINGLE_BLOCK     (17 | HSMCI_CMDR_SPCMD_STD | HSMCI_CMDR_RSPTYP_48_BIT \
                                     | HSMCI_CMDR_TRCMD_START_DATA | HSMCI_CMDR_TRDIR_READ \
                                     | HSMCI_CMDR_TRTYP_SINGLE | HSMCI_CMDR_MAXLAT)

#define SD_WRITE_SINGLE_BLOCK    (24 | HSMCI_CMDR_SPCMD_STD \
                                     | HSMCI_CMDR_RSPTYP_48_BIT \
                                     | HSMCI_CMDR_TRCMD_START_DATA \
                                     | HSMCI_CMDR_TRDIR_WRITE \
                                     | HSMCI_CMDR_TRTYP_SINGLE \
                                     | HSMCI_CMDR_MAXLAT)

// Get SCR
const char * sdAcmd51()
{
  const char * result;
  result = sdCmd55();
  if (result)
    return result;

  Hsmci *phsmci = HSMCI ;
  // Block size = 64/ 8, nblocks = 1
  phsmci->HSMCI_BLKR = ( ( 64 / 8 ) << 16 ) | 1 ;
  phsmci->HSMCI_ARGR = 0 ;
  phsmci->HSMCI_CMDR = SD_SEND_SCR ;

  uint8_t scrLen = 0;
  uint32_t i;
  for ( i = 0 ; i < 50000 ; i += 1 ) {
    if ( phsmci->HSMCI_SR & HSMCI_SR_RXRDY )
      Card_SCR[scrLen++] = __REV(phsmci->HSMCI_RDR) ;
    if ( ( phsmci->HSMCI_SR & ( HSMCI_SR_CMDRDY | HSMCI_SR_XFRDONE ) ) == ( HSMCI_SR_CMDRDY | HSMCI_SR_XFRDONE ) )
      return 0;
  }
  return "No response";
}

#define SD_SET_BUS_WIDTH            (6 | HSMCI_CMDR_SPCMD_STD | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_MAXLAT)

void sdEnableHsMode(uint8_t enable)
{
  Hsmci *phsmci = HSMCI;
  if (enable)
    phsmci->HSMCI_CFG |= HSMCI_CFG_HSMODE;
  else
    phsmci->HSMCI_CFG &= ~HSMCI_CFG_HSMODE;
}

void sdSetBusWidth(uint32_t busWidth)
{
  Hsmci *phsmci = HSMCI;
  phsmci->HSMCI_SDCR = (HSMCI->HSMCI_SDCR & ~HSMCI_SDCR_SDCBUS_Msk) | busWidth;
}

// Set bus width to 4 bits, set speed to 9 MHz
const char * sdAcmd6()
{
  const char * result;
  result = sdCmd55() ;
  if (result)
    return result;

  result = sdCommand(SD_SET_BUS_WIDTH, 2);
  if (result)
    return result;

  sdSetBusWidth( HSMCI_SDCR_SDCBUS_4 ) ;
  sdSetSpeed(9000000);
  return 0;
}

#if 0
/** SDIO CMD52, R5 */
#define SDIO_IO_RW_DIRECT           (52| HSMCI_CMDR_SPCMD_STD \
                                       | HSMCI_CMDR_TRCMD_NO_DATA \
                                       | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_MAXLAT)

static const char * sdCmd52(uint8_t wrFlag,
                            uint8_t funcNb,
                            uint8_t rdAfterWr,
                            uint32_t addr,
                            uint32_t *status)
{
  typedef struct
  {
      uint32_t data:8, /**< [ 7: 0] data for writing */
      stuff0:1,        /**< [    8] reserved */
      regAddress:17,   /**< [25: 9] register address */
      stuff1:1,        /**< [   26] reserved */
      rawFlag:1,       /**< [   27] Read after Write flag */
      functionNum:3,   /**< [30:28] Number of the function */
      rwFlag:1;        /**< [   31] Direction, 1:write, 0:read. */
  } SdioCmd52Arg;

  SdioCmd52Arg pArg52;
  pArg52.rwFlag = wrFlag;
  pArg52.functionNum = funcNb;
  pArg52.rawFlag = rdAfterWr;
  pArg52.regAddress = addr;

  const char * result = sdCommand(SDIO_IO_RW_DIRECT, *(uint32_t*)&pArg52);
  if (result)
    return result;
  *status = HSMCI->HSMCI_RSPR[0];
  return 0;
}
#endif

const char * sdSwReset(uint32_t retry)
{
    uint32_t i;
    const char * result = 0;

    for (i = 0; i < retry; i++) {
      result = sdCmd0(0);
        if (result != SD_NORESPONSE)
          break;
    }
    return result;
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

#define SDIO_CIA            0       /**< SDIO Function 0 (CIA) */
#define SDIO_IOA_REG        0x06    /**< I/O Abort */

/** SDIO state (in R5) */
#define SDIO_R5_ERROR        (1UL << 11)/**< General error */
#define SDIO_R5_FUNC_NUM     (1UL << 10)/**< Invalid function number */
#define SDIO_R5_OUT_OF_RANGE (1UL << 9) /**< Argument out of range */

/** Status bits mask for SDIO R5 */
#define STATUS_SDIO_R5  (0/*SDIO_R5_STATE*/ \
                         | SDIO_R5_ERROR \
                         | SDIO_R5_FUNC_NUM \
                         | SDIO_R5_OUT_OF_RANGE)


#define OCR_SDIO_MP            (1UL << 27)
#define OCR_SDIO_NF            (7UL << 28)

const char * sdIdentify()
{
    uint8_t mem = 0, io = 0, f8 = 0, mp = 1;
    uint32_t status, ccs=0;
    const char * result;

    /* Reset HC to default HS and BusMode */
    sdEnableHsMode(0);
    sdSetBusWidth(HSMCI_SDCR_SDCBUS_1);

#if 0
    /* Reset SDIO: CMD52, write 1 to RES bit in CCCR (bit 3 of register 6) */
    result = sdCmd52(1, SDIO_CIA, 0, SDIO_IOA_REG, &status);
    if (result)
      return result;
    if (status & STATUS_SDIO_R5)
      return "Identify.52 error";
#endif

    /* Reset MEM: CMD0 */
    result = sdSwReset(1);
    if (result)
      return result;

    /* CMD8 is newly added in the Physical Layer Specification Version 2.00 to
     * support multiple voltage ranges and used to check whether the card
     * supports supplied voltage. The version 2.00 host shall issue CMD8 and
     * verify voltage before card initialization.
     * The host that does not support CMD8 shall supply high voltage range... */
    result = sdCmd8(1);
    if (result == 0) f8 = 1;
    else if (result == SD_NORESPONSE) CoTickDelay(1); /* 2ms delay after "no response" */
    else return "Identify.8 error";

#if 0
    /* CMD5 is newly added for SDIO initialize & power on */
    status = 0;
    result = sdCmd5(&status);
    if (!result && (status & OCR_SDIO_NF) > 0) {
      unsigned int cmd5Retries = 10000;
      do {
        status &= SDMMC_HOST_VOLTAGE_RANGE;
        result = sdCmd5(&status);
        if (status & OCR_POWER_UP_BUSY) break;
      } while(!result && cmd5Retries --);
      if (result)
        return "Identify.5 error";
      io = 1;
      /* IO only ?*/
      mp = ((status & OCR_SDIO_MP) > 0);
    }
#endif

    /* Has memory: SD/MMC/COMBO */
    /*if (mp) */
    {
      /* Try SD memory initialize */
      result = sdMemInit(f8, &ccs);
      if (result) {
        //unsigned int cmd1Retries = 10000;
        /* Try MMC initialize */
        result = sdSwReset(10);
        if (result)
          return "Reset error";
        /* TODO ccs = 1;
        do { result = sdCmd1(&ccs); } while(result && cmd1Retries -- > 0);
        if (error) {
            TRACE_ERROR("SdMmcIdentify.Cmd1: %u\n\r", error);
            return SDMMC_ERROR;
        }
        else if (ccs) cardType = CARD_MMCHD;
        else          cardType = CARD_MMC;
        */
        return "TODO error";
        /* MMC card identification OK */
        return 0;
      }
      // mem = 1;
    }
    /* SD(IO) + MEM ? */
    /* if (!mem) {
      if (io) cardType = CARD_SDIO;
      else
        return "Unknown SD";
    } */
    /* SD(HC) combo */
    /*else if (io)
      cardType = ccs ? CARD_SDHCCOMBO : CARD_SDCOMBO;*/
    /* SD(HC) */
    //else
      cardType = ccs ? CARD_SDHC : CARD_SD;

    return 0;
}

#if 0
const char * sdEnum()
{
    uint8_t mem , io;
    const char * result;
    uint32_t ioSpeed = 0, memSpeed = 0;
    uint8_t hsExec = 0, bwExec = 0;

    /* - has Memory/IO/High-Capacity - */
    mem = ((cardType & CARD_TYPE_bmSDMMC) > 0);
    io  = ((cardType & CARD_TYPE_bmSDIO)  > 0);

    /* For MEMORY cards:
     * The host then issues the command ALL_SEND_CID (CMD2) to the card to get
     * its unique card identification (CID) number.
     * Card that is unidentified (i.e. which is in Ready State) sends its CID
     * number as the response (on the CMD line). */
    if (mem) {
      result = sdCmd2();
      if (result)
        return result;
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
    result = sdCmd3();
    if (result)
      return result;

    /* For MEMORY cards:
     * SEND_CSD (CMD9) to obtain the Card Specific Data (CSD register),
     * e.g. block length, card storage capacity, etc... */
    if (mem) {
      result = sdCmd9();
      if (result)
        return result;
    }

    /* Now select the card, to TRAN state */
    error = MmcSelectCard(pSd, pSd->cardAddress, 0);
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
#endif

void sdInit()
{
  const char * result;
  uint8_t i;

  /* Clear CID, CSD, EXT_CSD data */
  for (i = 0; i < 4; i++) Card_ID[i] = 0;
  for (i = 0; i < 4; i++) Card_CSD[i] = 0;
  // TODO for (i = 0; i < 512/4; i++) pSd->extData[i] = 0;

  /* Set low speed for device identification (LS device max speed) */
  sdSetSpeed(400000);

  /* Initialization delay: The maximum of 1 msec, 74 clock cycles and supply
   * ramp up time. Supply ramp up time provides the time that the power is
   * built up to the operating level (the bus master supply voltage) and the
   * time to wait until the SD card can accept the first command.
   */

  /* Power On Init Special Command */
  result = sdPowerOn();
  if (result)
    return;

  /* After power-on or CMD0, all cards?
   * CMD lines are in input mode, waiting for start bit of the next command.
   * The cards are initialized with a default relative card address
   * (RCA=0x0000) and with a default driver stage register setting
   * (lowest speed, highest driving current capability).
   */
  result = sdIdentify();
  if (result) {
    return;
  }

#if 0
  result = sdEnum();
  if (result) {
    return;
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
#endif
}


extern FATFS g_FATFS_Obj;
void sd_poll_10mS()
{
  if (!CardIsConnected()) {
    Card_state = SD_ST_EMPTY;
    Sd_rca = 0;
  }

  switch (Card_state) {
    case SD_ST_EMPTY:
      if (CardIsConnected()) {
        Card_state = SD_ST_INIT1;
      }
      break;

    case SD_ST_INIT1:
      sdCmd0();
      Card_state = SD_ST_INIT2;
      break;

    case SD_ST_INIT2:
      sdCmd8(1);
      Card_state = SD_ST_IDLE;
      break;

    case SD_ST_IDLE:
      sdMemInit(1, &Cmd_A41_resp);
      Card_state = SD_ST_READY;
      break;

    case SD_ST_READY:
      if (!sdCmd2()) Card_state = SD_ST_IDENT;
      break;

    case SD_ST_IDENT:
      if (!sdCmd3()) Card_state = SD_ST_STBY;
      break;

    case SD_ST_STBY:
      sdCmd9();
      sdCmd7(); // Select Card
      Card_state = SD_ST_TRAN;
      break;

    case SD_ST_TRAN:
      sdAcmd51();
      Card_state = SD_ST_DATA;
      sdAcmd6(); // Set bus width to 4 bits, and speed to 9 MHz
      // Should check the card can do this ****
      f_mount(0, &g_FATFS_Obj);
      break;
  }
}

// Checks for card ready for read/write
// returns 1 for YES, 0 for NO
uint32_t sd_card_ready( void )
{
        if ( CardIsConnected() )
        {
                if ( Card_state == SD_ST_DATA )
                {
                        return 1 ;
                }
        }
        return 0 ;
}

uint32_t sd_cmd16()
{
#if 0
  Hsmci *phsmci = HSMCI;

  if (CardIsConnected()) {
    phsmci->HSMCI_BLKR = ( ( 512 ) << 16 ) | 1 ;
    phsmci->HSMCI_ARGR = 512;
    phsmci->HSMCI_CMDR = SDMMC_SET_BLOCKLEN;

    while(1) {
      if (phsmci->HSMCI_SR & HSMCI_SR_CMDRDY) {
        break;
      }
    }
    return phsmci->HSMCI_RSPR[0];
  }
  else {
    return 0;
  }
#else
  return 0;
#endif
}

extern OS_MutexID sdMutex;

uint32_t sd_read_block(uint32_t block_no, uint32_t *data)
{
  uint32_t result = 0;
  Hsmci *phsmci = HSMCI;

  CoEnterMutexSection(sdMutex);

  if (Card_state == SD_ST_DATA) {
    if (CardIsConnected()) {
      sd_cmd16();
      // Block size = 512, nblocks = 1
      phsmci->HSMCI_BLKR = ((512) << 16) | 1;
      phsmci->HSMCI_MR   = (phsmci->HSMCI_MR & (~(HSMCI_MR_BLKLEN_Msk|HSMCI_MR_FBYTE))) | (HSMCI_MR_PDCMODE|HSMCI_MR_WRPROOF|HSMCI_MR_RDPROOF) | (512 << 16);
      phsmci->HSMCI_ARGR = (Cmd_A41_resp & OCR_SD_CCS ? block_no : (block_no << 9));
      phsmci->HSMCI_RPR  = (uint32_t)data;
      phsmci->HSMCI_RCR  = 512 / 4;
      phsmci->HSMCI_PTCR = HSMCI_PTCR_RXTEN;
      phsmci->HSMCI_CMDR = SD_READ_SINGLE_BLOCK;
        
      uint8_t retry = 100;
      while (retry-- > 0) {
        CoTickDelay(1); // 2ms
        if (phsmci->HSMCI_SR & HSMCI_SR_ENDRX) {
          result = 1;
          break;
        }
      }

    }
  }

  phsmci->HSMCI_MR &= ~(uint32_t)HSMCI_MR_PDCMODE;

  CoLeaveMutexSection(sdMutex);
  return result;
}

uint32_t sd_write_block( uint32_t block_no, uint32_t *data )
{
  uint32_t result = 0;
  Hsmci *phsmci = HSMCI;

  CoEnterMutexSection(sdMutex);

  if (Card_state == SD_ST_DATA) {
    if (CardIsConnected()) {
      sd_cmd16();
      // Block size = 512, nblocks = 1
      phsmci->HSMCI_BLKR = ((512) << 16) | 1;
      phsmci->HSMCI_MR   = (phsmci->HSMCI_MR & (~(HSMCI_MR_BLKLEN_Msk|HSMCI_MR_FBYTE))) | (HSMCI_MR_PDCMODE|HSMCI_MR_WRPROOF|HSMCI_MR_RDPROOF) | (512 << 16);
      phsmci->HSMCI_ARGR = (Cmd_A41_resp & OCR_SD_CCS ? block_no : (block_no << 9));
      phsmci->HSMCI_TPR  = (uint32_t)data;
      phsmci->HSMCI_TCR  = 512 / 4;
      phsmci->HSMCI_CMDR = SD_WRITE_SINGLE_BLOCK;
      phsmci->HSMCI_PTCR = HSMCI_PTCR_TXTEN;
      
      uint8_t retry = 100;
      while (retry-- > 0) {
        CoTickDelay(1); // 2ms
        if (phsmci->HSMCI_SR & HSMCI_SR_NOTBUSY) {
          result = 1;
          break;
        }
      }

    }
  }

  phsmci->HSMCI_MR &= ~(uint32_t)HSMCI_MR_PDCMODE;

  CoLeaveMutexSection(sdMutex);

  return result;
}

/*
 Notes on SD card:

1) CMD8 fails and CMD58 fails: must be MMC, thus initialize using CMD1
2) CMD8 fails and CMD58 passes: must be Ver1.x Standard Capacity SD Memory Card
3) CMD8 passes and CMD58 passes (CCS = 0): must be Ver2.00 or later Standard Capacity SD Memory Card
4) CMD8 passes and CMD58 passes (CCS = 1): must be Ver2.00 or later High Capacity SD Memory Card
5) CMD8 passes and CMD58 passes but indicates non compatible voltage range: unusable card

On card present
1. Send CMD 55 (resp 48bit)
                followed by ACMD41 (resp48bit)
                until bit 31 of response is '1'

2. Send CMD 2 (resp 136bit) - card ID

3. Send CMD 3 (resp 48 bit) - new RCA returned

4. Send CMD 9 (resp 136) - CSD

5. Send CMD 7 (resp 48) -

6. Send CMD 55 (resp 48bit)
                followed by ACMD51 (resp48bit) - SCR stored as 2 32 bit values

Now decide what the card can do!

7. Set block length

8. Set bus width

9. Read block 0

*/

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
                         BYTE drv               /* Physical drive nmuber (0) */
                         )
{
  if (drv) return STA_NOINIT;             /* Supports only single drive */
  if ( sd_card_ready() == 0 ) return RES_NOTRDY;
  return RES_OK;

#if 0
  BYTE n, cmd, ty, ocr[4];


        if (drv) return STA_NOINIT;             /* Supports only single drive */

        if (Stat & STA_NODISK) return Stat;     /* No card in the socket */

        power_on();                             /* Force socket power on */
        FCLK_SLOW();
        for (n = 10; n; n--) rcvr_spi();        /* 80 dummy clocks */

        ty = 0;
        if (send_cmd(CMD0, 0) == 1) {           /* Enter Idle state */
                Timer1 = 100;                   /* Initialization timeout of 1000 msec */
                if (send_cmd(CMD8, 0x1AA) == 1) {       /* SDv2? */
                        for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();  /* Get trailing return value of R7 resp */
                        if (ocr[2] == 0x01 && ocr[3] == 0xAA) {       /* The card can work at vdd range of 2.7-3.6V */
                                while (Timer1 && send_cmd(ACMD41, 1UL << 30));  /* Wait for leaving idle state (ACMD41 with HCS bit) */
                                if (Timer1 && send_cmd(CMD58, 0) == 0) {  /* Check CCS bit in the OCR */
                                        for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();
                                        ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;      /* SDv2 */
                                }
                        }
                } else {                                                        /* SDv1 or MMCv3 */
                        if (send_cmd(ACMD41, 0) <= 1)   {
                                ty = CT_SD1; cmd = ACMD41;      /* SDv1 */
                        } else {
                                ty = CT_MMC; cmd = CMD1;        /* MMCv3 */
                        }
                        while (Timer1 && send_cmd(cmd, 0));     /* Wait for leaving idle state */
                        if (!Timer1 || send_cmd(CMD16, 512) != 0)       /* Set R/W block length to 512 */
                                ty = 0;
                }
        }
        CardType = ty;
        deselect();

        if (ty) {                       /* Initialization succeded */
                Stat &= ~STA_NOINIT;    /* Clear STA_NOINIT */
                FCLK_FAST();
        } else {                        /* Initialization failed */
                power_off();
        }

        return Stat;
#endif
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
                                         BYTE drv               /* Physical drive number (0) */
                                         )
{
        if (drv) return STA_NOINIT;             /* Supports only single drive */
        if ( sd_card_ready() == 0 ) return RES_NOTRDY;
        return RES_OK;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
                                   BYTE drv,                    /* Physical drive nmuber (0) */
                                   BYTE *buff,                  /* Pointer to the data buffer to store read data */
                                   DWORD sector,                /* Start sector number (LBA) */
                                   BYTE count                   /* Sector count (1..255) */
                                   )
{
        uint32_t result ;
        if (drv || !count) return RES_PARERR;

        if ( sd_card_ready() == 0 ) return RES_NOTRDY;

        do {
          result = sd_read_block( sector, ( uint32_t *)buff ) ;
          if (result) {
            sector += 1 ;
            buff += 512 ;
            count -= 1 ;
          }
          else {
            count = 1 ;             // Flag error
            break ;
          }
        } while ( count ) ;

        return count ? RES_ERROR : RES_OK;
}


/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
                                        BYTE drv,                       /* Physical drive nmuber (0) */
                                        const BYTE *buff,       /* Pointer to the data to be written */
                                        DWORD sector,           /* Start sector number (LBA) */
                                        BYTE count                      /* Sector count (1..255) */
                                        )
{
        uint32_t result ;

        if (drv || !count) return RES_PARERR;

        if ( sd_card_ready() == 0 ) return RES_NOTRDY;

        // TODO if (Stat & STA_PROTECT) return RES_WRPRT;

        do {
          result = sd_write_block( sector, ( uint32_t *)buff ) ;
          if (result) {
            sector += 1 ;
            buff += 512 ;
            count -= 1 ;
          }
          else {
            count = 1 ;             // Flag error
            break ;
          }
        } while ( count ) ;

        return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
                        BYTE drv,               /* Physical drive nmuber (0) */
                        BYTE ctrl,              /* Control code */
                        void *buff              /* Buffer to send/receive control data */
                        )
{
        DRESULT res;
        BYTE *csd = (BYTE*)&Card_CSD[0];
        BYTE n; // csd[16], *ptr = buff;
        WORD csize;

        if (drv) return RES_PARERR;

        res = RES_ERROR;

        if (ctrl == CTRL_POWER) {
#if 0
                switch (ptr[0]) {
                        case 0:         /* Sub control code (POWER_OFF) */
                                power_off();            /* Power off */
                                res = RES_OK;
                                break;
                        case 1:         /* Sub control code (POWER_GET) */
                                ptr[1] = (BYTE)power_status();
                                res = RES_OK;
                                break;
                        default :
                                res = RES_PARERR;
                }
#endif
        }
        else {
                switch (ctrl) {
                        case CTRL_SYNC :                /* Make sure that no pending write process. Do not remove this or written sector might not left updated. */
                               /* BSS if (select()) {
                                        deselect(); */
                                        res = RES_OK;
                               // }
                                break;

                        case GET_SECTOR_COUNT : /* Get number of sectors on the disk (DWORD) */
                                // if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
                                        if ((csd[0] >> 6) == 1) {       /* SDC ver 2.00 */
                                                csize = csd[9] + ((WORD)csd[8] << 8) + 1;
                                                *(DWORD*)buff = (DWORD)csize << 10;
                                        } else {                                        /* SDC ver 1.XX or MMC*/
                                                n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
                                                csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
                                                *(DWORD*)buff = (DWORD)csize << (n - 9);
                                        }
                                        res = RES_OK;
                                // }
                                break;

                        case GET_SECTOR_SIZE :  /* Get R/W sector size (WORD) */
                                *(WORD*)buff = 512;
                                res = RES_OK;
                                break;

                        case GET_BLOCK_SIZE :   /* Get erase block size in unit of sector (DWORD) */
                          // only used in f_mkfs
#if 0
                                if (CardType & CT_SD2) {        /* SDv2? */
                                        if (send_cmd(ACMD13, 0) == 0) { /* Read SD status */
                                                rcvr_spi();
                                                if (rcvr_datablock(csd, 16)) {                          /* Read partial block */
                                                        for (n = 64 - 16; n; n--) rcvr_spi();   /* Purge trailing data */
                                                        *(DWORD*)buff = 16UL << (csd[10] >> 4);
                                                        res = RES_OK;
                                                }
                                        }
                                } else {                                        /* SDv1 or MMCv3 */
                                        if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {      /* Read CSD */
                                                if (CardType & CT_SD1) {        /* SDv1 */
                                                        *(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
                                                } else {                                        /* MMCv3 */
                                                        *(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
                                                }
                                                res = RES_OK;
                                        }
                                }
#endif
                                break;

#if 0
                        case MMC_GET_TYPE :             /* Get card type flags (1 byte) */
                                *ptr = CardType;
                                res = RES_OK;
                                break;

                        case MMC_GET_CSD :              /* Receive CSD as a data block (16 bytes) */
                                if (send_cmd(CMD9, 0) == 0              /* READ_CSD */
                                        && rcvr_datablock(ptr, 16))
                                        res = RES_OK;
                                break;

                        case MMC_GET_CID :              /* Receive CID as a data block (16 bytes) */
                                if (send_cmd(CMD10, 0) == 0             /* READ_CID */
                                        && rcvr_datablock(ptr, 16))
                                        res = RES_OK;
                                break;

                        case MMC_GET_OCR :              /* Receive OCR as an R3 resp (4 bytes) */
                                if (send_cmd(CMD58, 0) == 0) {  /* READ_OCR */
                                        for (n = 4; n; n--) *ptr++ = rcvr_spi();
                                        res = RES_OK;
                                }
                                break;

                        case MMC_GET_SDSTAT :   /* Receive SD statsu as a data block (64 bytes) */
                                if (send_cmd(ACMD13, 0) == 0) { /* SD_STATUS */
                                        rcvr_spi();
                                        if (rcvr_datablock(ptr, 64))
                                                res = RES_OK;
                                }
                                break;
#endif
                        default:
                                res = RES_PARERR;
                                break;
                }

                // BSS deselect();
        }

        return res;
}








