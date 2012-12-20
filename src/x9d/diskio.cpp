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
/* Only rcvr_spi(), xmit_spi(), sdPoll10ms() and some macros         */
/* are platform dependent.                                               */
/*-----------------------------------------------------------------------*/

#include <string.h>
#include "../FatFs/diskio.h"
#include "../FatFs/ff.h"
#include "../CoOS/kernel/CoOS.h"



uint32_t Card_ID[4] ;
uint32_t Card_SCR[2] ;
uint32_t Card_CSD[4] ;
int32_t Card_state; //  = SD_ST_STARTUP ;
volatile uint32_t Card_initialized = 0;
uint32_t Sd_rca ;
uint32_t Cmd_A41_resp ;
uint8_t  cardType;
uint32_t transSpeed;

/*-----------------------------------------------------------------------*/
/* Lock / unlock functions                                               */
/*-----------------------------------------------------------------------*/
int ff_cre_syncobj (BYTE vol, _SYNC_t *mutex)
{
  *mutex = CoCreateMutex();
  return 1;
}

int ff_req_grant (_SYNC_t mutex)
{
  CoEnterMutexSection(mutex);
  return 1;
}

void ff_rel_grant (_SYNC_t mutex)
{
  CoLeaveMutexSection(mutex);
}

int ff_del_syncobj (_SYNC_t mutex)
{
  return 1;
}

DSTATUS disk_initialize (
                         BYTE drv               /* Physical drive nmuber (0) */
                         )
{
  return RES_NOTRDY;
}

DSTATUS disk_status (
                                         BYTE drv               /* Physical drive number (0) */
                                         )
{
  return RES_NOTRDY;
}


DRESULT disk_read (
                                   BYTE drv,                    /* Physical drive nmuber (0) */
                                   BYTE *buff,                  /* Pointer to the data buffer to store read data */
                                   DWORD sector,                /* Start sector number (LBA) */
                                   BYTE count                   /* Sector count (1..255) */
                                   )
{
  return RES_NOTRDY;
}

DRESULT disk_write (
                                        BYTE drv,                       /* Physical drive nmuber (0) */
                                        const BYTE *buff,       /* Pointer to the data to be written */
                                        DWORD sector,           /* Start sector number (LBA) */
                                        BYTE count                      /* Sector count (1..255) */
                                        )
{
  return RES_NOTRDY;
}

DRESULT disk_ioctl (
                        BYTE drv,               /* Physical drive nmuber (0) */
                        BYTE ctrl,              /* Control code */
                        void *buff              /* Buffer to send/receive control data */
                        )
{
  return RES_NOTRDY;
}

void sdInit()
{
}

void sdMountPoll()
{
}

uint32_t sd_card_mounted( void )
{
  return 0; // CardIsConnected() && Card_state == SD_ST_MOUNTED;
}
