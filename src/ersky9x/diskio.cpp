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

#include "../open9x.h"
#include "diskio.h"

// SD Card routines

#define MCI_INITIAL_SPEED   400000

// States for initialising card
#define SD_ST_EMPTY             0
#define SD_ST_IDLE              1
#define SD_ST_READY             2
#define SD_ST_IDENT             3
#define SD_ST_STBY              4
#define SD_ST_TRAN              5
#define SD_ST_DATA              6


uint32_t Card_ID[4] ;
uint32_t Card_SCR[2] ;
uint32_t Card_CSD[4] ;
uint32_t Card_state = SD_ST_EMPTY ;
uint32_t Sd_128_resp[4] ;
uint32_t Sd_rca ;
//uint32_t Cmd_55_resp ;

/**
 * Initializes a MCI peripheral.
 */
void SD_Init()
{
  unsigned short clkDiv;
  Hsmci *pMciHw = HSMCI ;

  /* Enable the MCI peripheral */
  PMC->PMC_PCER0 |= 1 << ID_HSMCI ;             // Enable peripheral clock to HSMCI
  pMciHw->HSMCI_CR = HSMCI_CR_SWRST;  /* Reset the MCI */
  pMciHw->HSMCI_CR = HSMCI_CR_MCIDIS | HSMCI_CR_PWSDIS;  /* Disable the MCI */
  pMciHw->HSMCI_IDR = 0xFFFFFFFF;  /* Disable all the interrupts */
  pMciHw->HSMCI_DTOR = HSMCI_DTOR_DTOCYC | HSMCI_DTOR_DTOMUL ;  /* Set the Data Timeout Register */
  pMciHw->HSMCI_CSTOR = HSMCI_CSTOR_CSTOCYC | HSMCI_CSTOR_CSTOMUL ;  /* CSTOR ? */
  /* Set the Mode Register: 400KHz for MCK = 48MHz (CLKDIV = 58) */
  clkDiv = (Master_frequency / (MCI_INITIAL_SPEED * 2)) - 1;
  pMciHw->HSMCI_MR = clkDiv | (7 << 8) ;

  /* Set the SDCard Register 1-bit, slot A */
  pMciHw->HSMCI_SDCR = HSMCI_SDCR_SDCSEL_SLOTA | HSMCI_SDCR_SDCBUS_1 ;
  /* Enable the MCI and the Power Saving */
  pMciHw->HSMCI_CR = HSMCI_CR_MCIEN | HSMCI_CR_PWSEN ;
  /* Configure MCI */
  pMciHw->HSMCI_CFG = HSMCI_CFG_FIFOMODE | ((1 << 4) & HSMCI_CFG_FERRCTRL);
}


/**
 * Configure the  MCI SDCBUS in the MCI_SDCR register. Only two modes available
 *
 * \param busWidth  MCI bus width mode. 00: 1-bit, 10: 4-bit.
 */
uint32_t SD_SetBusWidth( uint32_t busWidth)
{
  uint32_t mciSdcr;

  if( (busWidth != HSMCI_SDCR_SDCBUS_1) && (busWidth != HSMCI_SDCR_SDCBUS_4) )
  {
        return (uint32_t)-1;
        }

  busWidth &= HSMCI_SDCR_SDCBUS ;

        mciSdcr = (HSMCI->HSMCI_SDCR & ~(uint32_t)(HSMCI_SDCR_SDCBUS));
        HSMCI->HSMCI_SDCR = mciSdcr | busWidth;

        return 0;
}

/**
 * Configure the MCI_CFG to enable the HS mode
 * \param hsEnable 1 to enable, 0 to disable HS mode.
 */
void SD_EnableHsMode( uint8_t hsEnable)
{
        uint32_t cfgr;

        cfgr = HSMCI->HSMCI_CFG;
        if (hsEnable)
        {
          cfgr |=  HSMCI_CFG_HSMODE;
        }
        else
        {
          cfgr &= ~(uint32_t)HSMCI_CFG_HSMODE;
        }

        HSMCI->HSMCI_CFG = cfgr;
}


/**
 * Configure the  MCI CLKDIV in the MCI_MR register. The max. for MCI clock is
 * MCK/2 and corresponds to CLKDIV = 0
 * \param mciSpeed  MCI clock speed in Hz, 0 will not change current speed.
 * \return The actual speed used, 0 for fail.
 */
uint32_t SD_SetSpeed( uint32_t mciSpeed )
{
        uint32_t mciMr;
        uint32_t clkdiv;

        mciMr = HSMCI->HSMCI_MR & (~(uint32_t)HSMCI_MR_CLKDIV);
        /* Multimedia Card Interface clock (MCCK or MCI_CK) is Master Clock (MCK)
         * divided by (2*(CLKDIV+1))
         * mciSpeed = MCK / (2*(CLKDIV+1)) */
        if (mciSpeed > 0)
        {
          clkdiv = (Master_frequency / 2 / mciSpeed);
          /* Speed should not bigger than expired one */
          if (mciSpeed < Master_frequency/2/clkdiv)
          {
              clkdiv++;
          }

          if ( clkdiv > 0 )
          {
              clkdiv -= 1;
          }
        }
        else
        {
                clkdiv = 0 ;
        }

        /* Actual MCI speed */
        mciSpeed = Master_frequency / 2 / (clkdiv + 1);
        /* Modify MR */
        HSMCI->HSMCI_MR = mciMr | clkdiv;

        return (mciSpeed);
}

//void SD_SetBlklen( uint32_t blklen )
//{
//      uint32_t mciMr;
//      uint32_t clkdiv;

//      mciMr = HSMCI->HSMCI_MR & (~(uint32_t)HSMCI_MR_BLKLEN);
//      /* Modify MR */
//      HSMCI->HSMCI_MR = mciMr | blklen << 16 ;
//}


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


void sd_cmd55()
{
        uint32_t i ;
  Hsmci *phsmci = HSMCI ;

        if ( CardIsConnected() )
        {
                phsmci->HSMCI_ARGR = Sd_rca ;
                phsmci->HSMCI_CMDR = 0x00001077 ;

                for ( i = 0 ; i < 30000 ; i += 1 )
                {
                        if ( phsmci->HSMCI_SR & HSMCI_SR_CMDRDY )
                        {
                                break ;
                        }
                }
//              Cmd_55_resp = phsmci->HSMCI_RSPR[0] ;
        }
}



uint32_t sd_acmd41()
{
        uint32_t i ;
        Hsmci *phsmci = HSMCI ;

        if ( CardIsConnected() )
        {
                sd_cmd55() ;
                phsmci->HSMCI_ARGR = 0X001F8000 ;
                phsmci->HSMCI_CMDR = 0x00001069 ;

                for ( i = 0 ; i < 30000 ; i += 1 )
                {
                        if ( phsmci->HSMCI_SR & HSMCI_SR_CMDRDY )
                        {
                                break ;
                        }
                }
                return phsmci->HSMCI_RSPR[0] ;
        }
        else
        {
                return 0 ;
        }
}

// Get Card ID
uint32_t sd_cmd2()
{
        uint32_t i ;
        Hsmci *phsmci = HSMCI ;

        if ( CardIsConnected() )
        {
                phsmci->HSMCI_ARGR = 0 ;
                phsmci->HSMCI_CMDR = 0x00001082 ;

                for ( i = 0 ; i < 30000 ; i += 1 )
                {
                        if ( phsmci->HSMCI_SR & HSMCI_SR_CMDRDY )
                        {
                                break ;
                        }
                }
                Sd_128_resp[0] = phsmci->HSMCI_RSPR[0] ;
                Sd_128_resp[1] = phsmci->HSMCI_RSPR[1] ;
                Sd_128_resp[2] = phsmci->HSMCI_RSPR[2] ;
                Sd_128_resp[3] = phsmci->HSMCI_RSPR[3] ;
                return 1 ;
        }
        else
        {
                return 0 ;
        }
}

// Get new RCA
uint32_t sd_cmd3()
{
        uint32_t i ;
        Hsmci *phsmci = HSMCI ;

        if ( CardIsConnected() )
        {
                phsmci->HSMCI_ARGR = 0 ;
                phsmci->HSMCI_CMDR = 0x00001043 ;

                for ( i = 0 ; i < 30000 ; i += 1 )
                {
                        if ( phsmci->HSMCI_SR & HSMCI_SR_CMDRDY )
                        {
                                break ;
                        }
                }
                return phsmci->HSMCI_RSPR[0] ;
        }
        else
        {
                return 0 ;
        }
}

// Get CSD
uint32_t sd_cmd9()
{
        uint32_t i ;
        Hsmci *phsmci = HSMCI ;

        if ( CardIsConnected() )
        {
                phsmci->HSMCI_ARGR = Sd_rca ;
                phsmci->HSMCI_CMDR = 0x00001089 ;

                for ( i = 0 ; i < 30000 ; i += 1 )
                {
                        if ( phsmci->HSMCI_SR & HSMCI_SR_CMDRDY )
                        {
                                break ;
                        }
                }
                Sd_128_resp[0] = phsmci->HSMCI_RSPR[0] ;
                Sd_128_resp[1] = phsmci->HSMCI_RSPR[1] ;
                Sd_128_resp[2] = phsmci->HSMCI_RSPR[2] ;
                Sd_128_resp[3] = phsmci->HSMCI_RSPR[3] ;
                return 1 ;
        }
        else
        {
                return 0 ;
        }
}

// Select Card
uint32_t sd_cmd7()
{
        uint32_t i ;
        Hsmci *phsmci = HSMCI ;

        if ( CardIsConnected() )
        {
                phsmci->HSMCI_ARGR = Sd_rca ;
                phsmci->HSMCI_CMDR = 0x00001047 ;

                for ( i = 0 ; i < 30000 ; i += 1 )
                {
                        if ( phsmci->HSMCI_SR & HSMCI_SR_CMDRDY )
                        {
                                break ;
                        }
                }
                return 1 ;
        }
        else
        {
                return 0 ;
        }
}

#define HSMCI_CMDR_SPCMD_STD            ( 0 << 8 )
#define HSMCI_CMDR_RSPTYP_48_BIT        ( 1 << 6 )
#define HSMCI_CMDR_TRCMD_START_DATA     ( 1 << 16 )
#define HSMCI_CMDR_TRDIR_READ           ( 1 << 18 )
#define HSMCI_CMDR_TRTYP_SINGLE         ( 0 << 19 )

#define SD_SEND_SCR  (51 | HSMCI_CMDR_SPCMD_STD | HSMCI_CMDR_RSPTYP_48_BIT \
                         | HSMCI_CMDR_TRCMD_START_DATA | HSMCI_CMDR_TRDIR_READ \
                         | HSMCI_CMDR_TRTYP_SINGLE | HSMCI_CMDR_MAXLAT)

#define SD_READ_SINGLE_BLOCK     (17 | HSMCI_CMDR_SPCMD_STD | HSMCI_CMDR_RSPTYP_48_BIT \
                                     | HSMCI_CMDR_TRCMD_START_DATA | HSMCI_CMDR_TRDIR_READ \
                                     | HSMCI_CMDR_TRTYP_SINGLE | HSMCI_CMDR_MAXLAT)

#define SD_WRITE_SINGLE_BLOCK     (17 | HSMCI_CMDR_SPCMD_STD | HSMCI_CMDR_RSPTYP_48_BIT \
                                      | HSMCI_CMDR_TRCMD_START_DATA | HSMCI_CMDR_TRDIR_READ \
                                      | HSMCI_CMDR_TRTYP_SINGLE | HSMCI_CMDR_MAXLAT)

// Get SCR
uint32_t sd_acmd51( uint32_t *presult )
{
        uint32_t i ;
        uint32_t j = 0 ;
        Hsmci *phsmci = HSMCI ;

        if ( CardIsConnected() )
        {
                sd_cmd55() ;
                // Block size = 64/ 8, nblocks = 1
                phsmci->HSMCI_BLKR = ( ( 64 / 8 ) << 16 ) | 1 ;
                phsmci->HSMCI_ARGR = 0 ;
                phsmci->HSMCI_CMDR = SD_SEND_SCR ;

                for ( i = 0 ; i < 50000 ; i += 1 )
                {
                        if ( phsmci->HSMCI_SR & HSMCI_SR_RXRDY )
                        {
                                *presult++ = __REV(phsmci->HSMCI_RDR) ;
                                j += 0x10000000 ;
                        }
                        if ( ( phsmci->HSMCI_SR & ( HSMCI_SR_CMDRDY | HSMCI_SR_XFRDONE ) ) == ( HSMCI_SR_CMDRDY | HSMCI_SR_XFRDONE ) )
                        {
                                break ;
                        }

                }
                if ( i >= 50000 )
                {
                        *presult = phsmci->HSMCI_SR ;
                }
                return i | j ; //phsmci->HSMCI_RSPR[0] ;
        }
        else
        {
                return 0 ;
        }
}

#define SD_SET_BUS_WIDTH            (6 | HSMCI_CMDR_SPCMD_STD | HSMCI_CMDR_RSPTYP_48_BIT \
                                       | HSMCI_CMDR_MAXLAT)

// Set bus width to 4 bits, set speed to 9 MHz
uint32_t sd_acmd6()
{
        uint32_t i ;
  Hsmci *phsmci = HSMCI ;

        if ( CardIsConnected() )
        {
                sd_cmd55() ;
                phsmci->HSMCI_ARGR = 2 ;                // Bus width 4 bits
                phsmci->HSMCI_CMDR = SD_SET_BUS_WIDTH ;

                for ( i = 0 ; i < 300000 ; i += 1 )
                {
                        if ( phsmci->HSMCI_SR & HSMCI_SR_CMDRDY )
                        {
                                break ;
                        }
                }
                if ( i >= 300000 )
                {
                        return 0 ;
                }
                SD_SetBusWidth( HSMCI_SDCR_SDCBUS_4 ) ;
                SD_SetSpeed( 9000000 ) ;
                return i ; //phsmci->HSMCI_RSPR[0] ;
        }
        else
        {
                return 0 ;
        }
}

// This routine is called every 10 mS, and checks for card
// insertion and removal.
// When a card is inserted, it initialises it ready for read/write access.
void sd_poll_10mS()
{
        uint32_t i ;

        if ( !CardIsConnected() )
        {
                Card_state = SD_ST_EMPTY ;
                Sd_rca = 0 ;

        }

        switch ( Card_state )
        {
                case SD_ST_EMPTY :
                        if ( CardIsConnected() )
                        {
                                Card_state = SD_ST_IDLE ;
                        }
                break ;

                case SD_ST_IDLE :
                        i = sd_acmd41() ;
                        if ( i & 0x80000000 )
                        {
                                Card_state = SD_ST_READY ;
                        }
                break ;

                case SD_ST_READY :
                        i = sd_cmd2() ;
                        if ( i )
                        {
                                Card_ID[0] = Sd_128_resp[0] ;
                                Card_ID[1] = Sd_128_resp[1] ;
                                Card_ID[2] = Sd_128_resp[2] ;
                                Card_ID[3] = Sd_128_resp[3] ;
                                Card_state = SD_ST_IDENT ;
                        }
                break ;

                case SD_ST_IDENT :
                        i = sd_cmd3() ;         // Get new RCA
                        Sd_rca = i ;
                        Card_state = SD_ST_STBY ;
                break ;

                case SD_ST_STBY :
                        i = sd_cmd9() ;
                        Card_CSD[0] = Sd_128_resp[0] ;
                        Card_CSD[1] = Sd_128_resp[1] ;
                        Card_CSD[2] = Sd_128_resp[2] ;
                        Card_CSD[3] = Sd_128_resp[3] ;
                        i = sd_cmd7() ;         // Select Card
//                      txmit( '+' ) ;
//                      p8hex( i ) ;
//                      crlf() ;
                        Card_state = SD_ST_TRAN ;
                break ;

                case SD_ST_TRAN :
                        i = sd_acmd51( Sd_128_resp ) ;
                        Card_SCR[0] = Sd_128_resp[0] ;
                        Card_SCR[1] = Sd_128_resp[1] ;
                        Card_state = SD_ST_DATA ;
                        i = sd_acmd6() ;                // Set bus width to 4 bits, and speed to 9 MHz
//                      txmit( '-' ) ;
//                      p8hex( i ) ;
//                      crlf() ;
                        // Should check the card can do this ****
                break ;
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

uint32_t sd_read_block( uint32_t block_no, uint32_t *data )
{
        if ( Card_state == SD_ST_DATA )
        {
                uint32_t i ;
                uint32_t j = 0 ;
                Hsmci *phsmci = HSMCI ;

                if ( CardIsConnected() )
                {
                  // Block size = 512, nblocks = 1
                  phsmci->HSMCI_BLKR = ( ( 512 ) << 16 ) | 1 ;
                  phsmci->HSMCI_ARGR = block_no << 9 ;
                  phsmci->HSMCI_CMDR = SD_READ_SINGLE_BLOCK ;

                  for ( i = 0 ; i < 50000 ; i += 1 )
                  {
                    if ( phsmci->HSMCI_SR & HSMCI_SR_RXRDY )
                    {
                      *data++ = phsmci->HSMCI_RDR ;
                      j += 1 ;
                    }
                    if ( ( phsmci->HSMCI_SR & ( HSMCI_SR_CMDRDY | HSMCI_SR_XFRDONE ) ) == ( HSMCI_SR_CMDRDY | HSMCI_SR_XFRDONE ) )
                    {
                      break ;
                    }
                    if ( j >= 128 )
                    {
                      break ;
                    }
                  }
                  return i | (j << 16) ; //phsmci->HSMCI_RSPR[0] ;
                }
                else
                {
                  return 0 ;
                }
        }
        else
        {
                return 0 ;
        }
}

uint32_t sd_write_block( uint32_t block_no, uint32_t *data )
{
        if ( Card_state == SD_ST_DATA )
        {
                uint32_t i ;
                uint32_t j = 0 ;
                Hsmci *phsmci = HSMCI ;

                if ( CardIsConnected() )
                {
                  // Block size = 512, nblocks = 1
                  phsmci->HSMCI_BLKR = ( ( 512 ) << 16 ) | 1 ;
                  phsmci->HSMCI_ARGR = block_no << 9 ;
                  phsmci->HSMCI_CMDR = SD_WRITE_SINGLE_BLOCK ;
#if 0
                  for ( i = 0 ; i < 50000 ; i += 1 )
                  {
                    if ( phsmci->HSMCI_SR & HSMCI_SR_RXRDY )
                    {
                      *data++ = phsmci->HSMCI_RDR ;
                      j += 1 ;
                    }
                    if ( ( phsmci->HSMCI_SR & ( HSMCI_SR_CMDRDY | HSMCI_SR_XFRDONE ) ) == ( HSMCI_SR_CMDRDY | HSMCI_SR_XFRDONE ) )
                    {
                      break ;
                    }
                    if ( j >= 128 )
                    {
                      break ;
                    }
                  }
#endif
                  return i | (j << 16) ; //phsmci->HSMCI_RSPR[0] ;
                }
                else
                {
                  return 0 ;
                }
        }
        else
        {
                return 0 ;
        }
}

/*
 Notes on SD card:

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


#include "ff.h"
#include "diskio.h"

/* FAT sub-type boundaries */
/* Note that the FAT spec by Microsoft says 4085 but Windows works with 4087! */
#define MIN_FAT16       4086    /* Minimum number of clusters for FAT16 */
#define MIN_FAT32       65526   /* Minimum number of clusters for FAT32 */


/* FatFs refers the members in the FAT structures as byte array instead of
/ structure member because there are incompatibility of the packing option
/ between compilers. */

#define BS_jmpBoot                      0
#define BS_OEMName                      3
#define BPB_BytsPerSec          11
#define BPB_SecPerClus          13
#define BPB_RsvdSecCnt          14
#define BPB_NumFATs                     16
#define BPB_RootEntCnt          17
#define BPB_TotSec16            19
#define BPB_Media                       21
#define BPB_FATSz16                     22
#define BPB_SecPerTrk           24
#define BPB_NumHeads            26
#define BPB_HiddSec                     28
#define BPB_TotSec32            32
#define BS_DrvNum                       36
#define BS_BootSig                      38
#define BS_VolID                        39
#define BS_VolLab                       43
#define BS_FilSysType           54
#define BPB_FATSz32                     36
#define BPB_ExtFlags            40
#define BPB_FSVer                       42
#define BPB_RootClus            44
#define BPB_FSInfo                      48
#define BPB_BkBootSec           50
#define BS_DrvNum32                     64
#define BS_BootSig32            66
#define BS_VolID32                      67
#define BS_VolLab32                     71
#define BS_FilSysType32         82
#define FSI_LeadSig                     0
#define FSI_StrucSig            484
#define FSI_Free_Count          488
#define FSI_Nxt_Free            492
#define MBR_Table                       446
#define BS_55AA                         510

#define DIR_Name                        0
#define DIR_Attr                        11
#define DIR_NTres                       12
#define DIR_CrtTime                     14
#define DIR_CrtDate                     16
#define DIR_FstClusHI           20
#define DIR_WrtTime                     22
#define DIR_WrtDate                     24
#define DIR_FstClusLO           26
#define DIR_FileSize            28
#define LDIR_Ord                        0
#define LDIR_Attr                       11
#define LDIR_Type                       12
#define LDIR_Chksum                     13
#define LDIR_FstClusLO          26


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
                         BYTE drv               /* Physical drive nmuber (0) */
                         )
{
        // TODO BSS BYTE n, cmd, ty, ocr[4];


        if (drv) return STA_NOINIT;             /* Supports only single drive */

#if 0
        // TODO BSS
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
        return 0;
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
                                         BYTE drv               /* Physical drive number (0) */
                                         )
{
        if (drv) return STA_NOINIT;             /* Supports only single drive */
        return 0; // TODO BSS return Stat;
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
                        BYTE *buff              /* Buffer to send/receive control data */
                        )
{
        DRESULT res;
        // TODO BSS BYTE n, csd[16], *ptr = buff;
        // TODO BSS WORD csize;

        if (drv) return RES_PARERR;

        res = RES_ERROR;
#if 0
        if (ctrl == CTRL_POWER) {
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
        }
        else {
                if (Stat & STA_NOINIT) return RES_NOTRDY;

                switch (ctrl) {
                        case CTRL_SYNC :                /* Make sure that no pending write process. Do not remove this or written sector might not left updated. */
                                if (select()) {
                                        deselect();
                                        res = RES_OK;
                                }
                                break;

                        case GET_SECTOR_COUNT : /* Get number of sectors on the disk (DWORD) */
                                if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
                                        if ((csd[0] >> 6) == 1) {       /* SDC ver 2.00 */
                                                csize = csd[9] + ((WORD)csd[8] << 8) + 1;
                                                *(DWORD*)buff = (DWORD)csize << 10;
                                        } else {                                        /* SDC ver 1.XX or MMC*/
                                                n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
                                                csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
                                                *(DWORD*)buff = (DWORD)csize << (n - 9);
                                        }
                                        res = RES_OK;
                                }
                                break;

                        case GET_SECTOR_SIZE :  /* Get R/W sector size (WORD) */
                                *(WORD*)buff = 512;
                                res = RES_OK;
                                break;

                        case GET_BLOCK_SIZE :   /* Get erase block size in unit of sector (DWORD) */
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
                                break;

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

                        default:
                                res = RES_PARERR;
                }

                deselect();
        }
#endif
        return res;
}










