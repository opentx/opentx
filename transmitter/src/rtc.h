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

#ifndef RTC_H
#define RTC_H

#include <inttypes.h>

typedef long int gtime_t;

struct gtm
{
  int8_t tm_sec;                   /* Seconds.     [0-60] (1 leap second) */
  int8_t tm_min;                   /* Minutes.     [0-59] */
  int8_t tm_hour;                  /* Hours.       [0-23] */
  int8_t tm_mday;                  /* Day.         [1-31] */
  int8_t tm_mon;                   /* Month.       [0-11] */
  int8_t tm_year;                  /* Year - 1900. Limited to the year 2115. Oh no! :P */
  int8_t tm_wday;                  /* Day of week. [0-6] */
  int16_t tm_yday;                 /* Day of year. [0-365] Needed internally for calculations */
};

extern gtime_t g_rtcTime;
extern uint8_t g_ms100; // global to allow time set function to reset to zero

void rtcInit();

#ifdef __cplusplus
extern "C" {
#endif
void rtcSetTime(struct gtm * tm);
void gettime(struct gtm * tm);
gtime_t gmktime (struct gtm *tm);
#ifdef __cplusplus
}
#endif

#endif
