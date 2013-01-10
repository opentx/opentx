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

void rtc_settime(struct gtm * t)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	
	RTC_TimeStruct.RTC_Hours = t->tm_hour;
	RTC_TimeStruct.RTC_Minutes = t->tm_min;
	RTC_TimeStruct.RTC_Seconds = t->tm_sec;
	RTC_DateStruct.RTC_Year = t->tm_year - 100;
	RTC_DateStruct.RTC_Month = t->tm_mon;
	RTC_DateStruct.RTC_Date = t->tm_mday;
	
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
	RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);
}

void rtc_gettime(struct gtm * t)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	
	RTC_GetTime(RTC_Format_BIN, &RTC_TimeStruct);
	RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
	
	t->tm_hour = RTC_TimeStruct.RTC_Hours;
	t->tm_min  = RTC_TimeStruct.RTC_Minutes;
	t->tm_sec  = RTC_TimeStruct.RTC_Seconds;
	t->tm_year = RTC_DateStruct.RTC_Year + 100;
	t->tm_mon  = RTC_DateStruct.RTC_Month;
	t->tm_mday = RTC_DateStruct.RTC_Date;
}

void rtc_init()
{
	RTC_InitTypeDef RTC_InitStruct;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	PWR_BackupAccessCmd(ENABLE);
	
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	RCC_RTCCLKCmd(ENABLE);
	
	RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;
	RTC_InitStruct.RTC_AsynchPrediv = 128;
	RTC_InitStruct.RTC_SynchPrediv = 256;
	
	RTC_Init(&RTC_InitStruct);
}
