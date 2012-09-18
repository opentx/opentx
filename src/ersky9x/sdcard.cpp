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

#include "../open9x.h"

volatile int8_t sdState = SD_STATE_STARTUP;

#if !defined(SIMU)

extern "C" {

/* Include USB definitions */
#include <usb/device/massstorage/MSDDriver.h>

/**  MCI driver instance. */
static Mcid mciDrv;

/**  SDCard driver instance. */
static SdCard sdDrv;

/**
 *  MCI0 interrupt handler. Forwards the event to the MCI driver handler.
 */
void MCI_IRQHandler(void)
{
  Sdmmc_Handler(&mciDrv);
}

void MEDSdcard_Initialize(Media * /*media*/, SdCard * /*sdDrv*/, uint8_t /*protected*/);

} // extern "C"

#define MCI_INITIAL_SPEED   400000

FATFS g_FATFS_Obj;

void sdInit()
{
  configure_pins( 0xFC000000, PIN_PERIPHERAL | PIN_INPUT | PIN_PER_C | PIN_PORTA | PIN_NO_PULLUP ) ;
  configure_pins( PIO_PB7, PIN_INPUT | PIN_PORTB | PIN_NO_PULLUP | PIN_NO_PULLDOWN ) ;

  unsigned short clkDiv;
  Hsmci *pMciHw = HSMCI ;

  /* Enable the MCI peripheral */
  PMC->PMC_PCER0 |= 1 << ID_HSMCI ;             // Enable peripheral clock to HSMCI
  pMciHw->HSMCI_CR = HSMCI_CR_SWRST;  /* Reset the MCI */
  pMciHw->HSMCI_CR = HSMCI_CR_MCIDIS | HSMCI_CR_PWSDIS;  /* Disable the MCI */
  pMciHw->HSMCI_IDR = 0xFFFFFFFF;  /* Disable all the interrupts */
  pMciHw->HSMCI_DTOR = HSMCI_DTOR_DTOCYC_Msk | HSMCI_DTOR_DTOMUL_Msk ;  /* Set the Data Timeout Register */
  pMciHw->HSMCI_CSTOR = HSMCI_CSTOR_CSTOCYC_Msk | HSMCI_CSTOR_CSTOMUL_Msk ;  /* CSTOR ? */
  /* Set the Mode Register: 400KHz for MCK = 48MHz (CLKDIV = 58) */
  clkDiv = (Master_frequency / (MCI_INITIAL_SPEED * 2)) - 1;
  pMciHw->HSMCI_MR = clkDiv | (7 << 8) ;

  /* Set the SDCard Register 1-bit, slot A */
  pMciHw->HSMCI_SDCR = HSMCI_SDCR_SDCSEL_SLOTA | HSMCI_SDCR_SDCBUS_1 ;
  /* Enable the MCI and the Power Saving */
  pMciHw->HSMCI_CR = HSMCI_CR_MCIEN | HSMCI_CR_PWSEN ; // TODO HSMCI_CR_PWSEN ?
  /* Configure MCI */
  pMciHw->HSMCI_CFG = HSMCI_CFG_FIFOMODE | ((1 << 4) & HSMCI_CFG_FERRCTRL);

  if (CardIsConnected()) {
    sdLoad();
  }
}

void sdLoad()
{
  /* Initialize the MCI driver */
  MCI_Init(&mciDrv, HSMCI, ID_HSMCI, BOARD_MCK);

  /* Enable the interrupts */
  NVIC_EnableIRQ( HSMCI_IRQn ) ;

  /* Initialize the SD card driver */
  SD_Init(&sdDrv, &mciDrv);

  MEDSdcard_Initialize(&(medias[DRV_SDMMC]), &sdDrv, 0);

  sdState = SD_STATE_CONNECTED;
}

/** Size of one block in bytes. */
#define BLOCK_SIZE          512

/** Size of the MSD IO buffer in bytes (6K, more the better). */
#define MSD_BUFFER_SIZE     (12*BLOCK_SIZE)

/** LUN read/write buffer. */
unsigned char msdBuffer[MSD_BUFFER_SIZE];

static void ConfigureUsbClock(void)
{
    /* Enable PLLB for USB */
    PMC->CKGR_PLLBR = CKGR_PLLBR_DIVB(1)
                    | CKGR_PLLBR_MULB(7)
                    | CKGR_PLLBR_PLLBCOUNT_Msk;
    while((PMC->PMC_SR & PMC_SR_LOCKB) == 0); // TODO  && (timeout++ < CLOCK_TIMEOUT));
    /* USB Clock uses PLLB */
    PMC->PMC_USB = PMC_USB_USBDIV(1)    /* /2   */
                 | PMC_USB_USBS;        /* PLLB */
}

/*----------------------------------------------------------------------------
 *        Local variables
 *----------------------------------------------------------------------------*/

/** Device LUNs. */
MSDLun luns[MAX_MEDS];

char *bytes2Str(char *str, uint32_t bytes)
{
  if (bytes < 1024*100)
    sprintf(str, "%lu.%luK", bytes / 1024, (bytes % 1024) / 103);
  else if (bytes < 1024*1024)
    sprintf(str, "%luK", bytes / 1024);
  else if (bytes < 1024*1024*100)
    sprintf(str, "%lu.%luM", bytes / (1024*1024), (bytes % (1024*1024)) / 104858);
  else
    sprintf(str, "%luM", bytes / (1024*1024));
  return str;
}

unsigned int msdReadTotal=0, msdWriteTotal=0;

static void MSDCallbacks_Data( unsigned char flowDirection, unsigned int dataLength,
                               unsigned int fifoNullCount, unsigned int fifoFullCount )
{
  if (flowDirection)
    msdReadTotal += dataLength;
  else
    msdWriteTotal += dataLength;

  char rdStr[10];
  char wrStr[10];

  sprintf(statusLineMsg, "USB Rd:%s Wr:%s", bytes2Str(rdStr, msdReadTotal), bytes2Str(wrStr, msdWriteTotal));
  showStatusLine();
}

void usbMassStorage()
{
  static bool initialized = false;

  if (usbPlugged()) {
    if (!initialized) {
      initialized = true;

      ConfigureUsbClock();

      /* Initialize LUN */
      LUN_Init(&(luns[DRV_SDMMC]), &(medias[DRV_SDMMC]),
            msdBuffer, MSD_BUFFER_SIZE,
            0, 0, 0, 0,
            MSDCallbacks_Data);

      /* BOT driver initialization */
      MSDDriver_Initialize(luns, 1);

      // VBus_Configure();
      USBD_Connect();
    }

    /* Mass storage state machine */
    for (uint8_t i=0; i<50; i++) {
      MSDDriver_StateMachine();
    }
  }
  else {
    msdReadTotal = 0;
    msdWriteTotal = 0;
  }
}

bool sdIsHC()
{
  return (sdDrv.cardType == CARD_SDHC);
}

#endif
