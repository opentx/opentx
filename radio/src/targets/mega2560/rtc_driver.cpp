/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
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


/*--------------------------------------------------------------------------*/
/*  RTC controls                                                            */

#include "../opentx.h"
#include "../FatFs/integer.h"

typedef struct {
        WORD    year;   /* 2000..2099 */
        BYTE    month;  /* 1..12 */
        BYTE    mday;   /* 1.. 31 */
        BYTE    wday;   /* 1..7 */
        BYTE    hour;   /* 0..23 */
        BYTE    min;    /* 0..59 */
        BYTE    sec;    /* 0..59 */
} RTC;

void rtc_gettime(struct gtm * utm)
{
  RTC rtc = {0,0,0,0,0,0,0};

  // TODO g9x_rtc_gettime(&rtc);

  utm->tm_year = rtc.year - 1900;
  utm->tm_mon =  rtc.month - 1;
  utm->tm_mday = rtc.mday;
  utm->tm_hour = rtc.hour;
  utm->tm_min =  rtc.min;
  utm->tm_sec =  rtc.sec;
  utm->tm_wday = rtc.wday - 1;
}

void rtcSetTime(struct gtm * t)
{
  g_rtcTime = gmktime(t); // update local timestamp and get wday calculated
  g_ms100 = 0; // start of next second begins now

#if 0
  RTC rtc;
  rtc.year = t->tm_year + 1900;
  rtc.month = t->tm_mon + 1;
  rtc.mday = t->tm_mday;
  rtc.hour = t->tm_hour;
  rtc.min = t->tm_min;
  rtc.sec = t->tm_sec;
  rtc.wday = t->tm_wday + 1;
  g9x_rtcSetTime(&rtc);
#endif
}

void rtcInit (void)
{
  // TODO

  struct gtm utm;
  rtc_gettime(&utm);
  g_rtcTime = gmktime(&utm);
}

