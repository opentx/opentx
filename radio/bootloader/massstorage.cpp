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

#include "radio.h"
#include "board.h"
//#include "debug.h"
//#include "Audio.h"

//#ifndef SIMU
//#include "CoOS.h"
//#endif

#ifndef SIMU

extern "C" {
#include "MSDDriver.h"
extern unsigned char MEDSdcard_Initialize(Media *media, unsigned char mciID);
}

/** Size of one block in bytes. */
#define BLOCK_SIZE          512

/** Size of the MSD IO buffer in bytes (6K, more the better). */
#define MSD_BUFFER_SIZE     (12*BLOCK_SIZE)

/** LUN read/write buffer. */
unsigned char msdBuffer[MSD_BUFFER_SIZE];

unsigned int msdReadTotal=0, msdWriteTotal=0;

static void ConfigureUsbClock(void)
{
    /* Enable PLLB for USB */
    PMC->CKGR_PLLBR = CKGR_PLLBR_DIVB(1)
                    | CKGR_PLLBR_MULB(7)
                    | CKGR_PLLBR_PLLBCOUNT;
    while((PMC->PMC_SR & PMC_SR_LOCKB) == 0); // TODO  && (timeout++ < CLOCK_TIMEOUT));
    /* USB Clock uses PLLB */
    PMC->PMC_USB = PMC_USB_USBDIV(1)    /* /2   */
                 | PMC_USB_USBS;        /* PLLB */
}

/** Maximum number of LUNs which can be defined. */
#define MAX_LUNS            2

/** Media index for different disks */
#define DRV_SDMMC           0    /** SD card */
#define DRV_EEPROM          1    /** EEPROM */

Media medias[MAX_LUNS];


/*----------------------------------------------------------------------------
 *        Local variables
 *----------------------------------------------------------------------------*/

#ifdef HID
    
uint8_t  hid_iBuffer[64] ;
uint8_t  hid_oBuffer[64] ;

static void hid_transfer()
{
    uint32_t dwCnt=0 ;
    uint32_t dwLen ;

    dwLen = HIDDTransferDriver_Read( hid_iBuffer, 64 ) ;
    if ( dwLen )
    {
//       printf( "Data In(%u):", (unsigned int)dwLen ) ;
//       _ShowBuffer( iBuffer, dwLen ) ;

//            bmLEDs = iBuffer[0] ;
//            update = 1 ;
    }

    dwLen = HIDDTransferDriver_ReadReport( iBuffer, 64 ) ;
    if ( dwLen )
    {
//        printf( "Report In(%u):", (unsigned int)dwLen ) ;
//        _ShowBuffer( iBuffer, dwLen ) ;

//        bmLEDs = iBuffer[0] ;
//        update = 1 ;
    }

    oBuffer[0] = 0x80 ;
    if ( PIO_Get( &pinsButtons[PUSHBUTTON_BP1]) == 0 )
    {
        oBuffer[0] |= 0x01 ;
    }
    if ( PIO_Get( &pinsButtons[PUSHBUTTON_BP2] ) == 0 )
    {
        oBuffer[0] |= 0x02 ;
    }

    sprintf( (char*)&oBuffer[5], ":%04x:%05u!", (unsigned int)dwCnt, (unsigned int)dwCnt ) ;
    oBuffer[1] = (uint8_t)(dwCnt) ;
    oBuffer[2] = (uint8_t)(dwCnt >> 8) ;
    oBuffer[3] = (uint8_t)(dwCnt >> 16) ;
    oBuffer[4] = (uint8_t)(dwCnt >> 24) ;
    if ( USBD_STATUS_SUCCESS == HIDDTransferDriver_Write( oBuffer, 64, 0, 0 ) )
    {
        dwCnt ++ ;
    }
}

#endif




/** Device LUNs. */
MSDLun luns[MAX_LUNS];

static void MSDCallbacks_Data( unsigned char flowDirection, unsigned int dataLength,
                               unsigned int fifoNullCount, unsigned int fifoFullCount )
{
#if 0
  if (flowDirection)
    msdReadTotal += dataLength;
  else
    msdWriteTotal += dataLength;
#endif
}

extern "C" unsigned char EEPROM_Initialize(Media *media, unsigned char mciID) ;
extern uint32_t sd_card_ready( void ) ;

void usbMassStorage()
{
  static bool initialized = false ;
  static bool active = false ;

  if ( PIOC->PIO_PDSR & 0x02000000 )
	{
    // TRACE_DEBUG("usbMassStorage\n\r");

    if (!initialized)
		{
      ConfigureUsbClock();
      /* Initialize LUN */
      MEDSdcard_Initialize(&(medias[DRV_SDMMC]), 0);
      EEPROM_Initialize(&(medias[DRV_EEPROM]), 0);
      LUN_Init(&(luns[DRV_SDMMC]), sd_card_ready() ? &(medias[DRV_SDMMC]) : 0 ,
          msdBuffer, MSD_BUFFER_SIZE,
          0, 0, 0, 0,
          MSDCallbacks_Data);
      
			LUN_Init(&(luns[DRV_EEPROM]), &(medias[DRV_EEPROM]),
          msdBuffer, MSD_BUFFER_SIZE,
          0, 0, 0, 0,
          MSDCallbacks_Data);
      
			/* BOT driver initialization */
      MSDDriver_Initialize( luns, 2 ) ;
      // VBus_Configure();
      USBD_Connect();
      initialized = true;
    }

  	if ( active == false )
		{
#ifndef BOOT			
			CoSchedLock() ;
			if ( Voice.VoiceLock == 0 )
			{
				Voice.VoiceLock = 1 ;
#endif
				active = true ;
#ifndef BOOT			
			}
  		CoSchedUnlock() ;
#endif
		}
  	if ( active )
		{
    	/* Mass storage state machine */
    	for (uint8_t i=0; i<50; i++)
			{
#ifdef HID
				hid_transfer() ;
#endif
				MSDDriver_StateMachine() ;
			}
		}
  }
  else
	{
  	if ( active )
		{
  		active = false ;
#ifndef BOOT			
			Voice.VoiceLock = 0 ;
#endif
		}
		
    msdReadTotal = 0;
    msdWriteTotal = 0;
  }
}

#endif
