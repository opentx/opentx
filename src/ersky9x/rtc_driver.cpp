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

struct t_i2cTime
{
  uint8_t setCode ;
  uint8_t Time[7] ;
} I2CTime ;

void rtc_settime(struct gtm * t)
{
  g_rtcTime = gmktime(t); // update local timestamp and get wday calculated
  g_ms100 = 0; // start of next second begins now

  I2CTime.setCode = 0x74 ;    // Tiny SET TIME CODE command
  I2CTime.Time[0] = t->tm_sec ;
  I2CTime.Time[1] = t->tm_min ;
  I2CTime.Time[2] = t->tm_hour ;
  I2CTime.Time[3] = t->tm_mday ;
  I2CTime.Time[4] = t->tm_mon+1 ;
  I2CTime.Time[5] = (uint8_t) (t->tm_year+1900) ;
  I2CTime.Time[6] = (t->tm_year+1900) >> 8 ;
  write_coprocessor( (uint8_t *) &I2CTime, 8 ) ;
}

void rtc_init()
{
  read_coprocessor();
}
