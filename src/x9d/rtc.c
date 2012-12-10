/*--------------------------------------------------------------------------  */
/*  RTC controls for STM32                                                    */
/*  Copyright (c) 2009, Martin Thomas 4/2009, 3BSD-license                    */
/*  partly based on code from STMircoelectronics, Peter Dannegger, "LaLaDumm" */
/*--------------------------------------------------------------------------  */

#include <stdint.h>
#include "stm32f10x.h"
#include "rtc.h"

#define FIRSTYEAR   2000		// start year
#define FIRSTDAY    6			// 0 = Sunday

static const uint8_t DaysInMonth[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/*******************************************************************************
* Function Name  : isDST
* Description    : checks if given time is in Daylight Saving time-span.
* Input          : time-struct, must be fully populated including weekday
* Output         : none
* Return         : false: no DST ("winter"), true: in DST ("summer")
*  DST according to German standard
*  Based on code from Peter Dannegger found in the microcontroller.net forum.
*******************************************************************************/
static bool isDST( const RTC_t *t )
{
	uint8_t wday, month;		// locals for faster access

	month = t->month;

	if( month < 3 || month > 10 ) {		// month 1, 2, 11, 12
		return false;					// -> Winter
	}

	wday  = t->wday;

	if( t->mday - wday >= 25 && (wday || t->hour >= 2) ) { // after last Sunday 2:00
		if( month == 10 ) {				// October -> Winter
			return false;
		}
	} else {							// before last Sunday 2:00
		if( month == 3 ) {				// March -> Winter
			return false;
		}
	}

	return true;
}

/*******************************************************************************
* Function Name  : adjustDST
* Description    : adjusts time to DST if needed
* Input          : non DST time-struct, must be fully populated including weekday
* Output         : time-stuct gets modified
* Return         : false: no DST ("winter"), true: in DST ("summer")
*  DST according to German standard
*  Based on code from Peter Dannegger found in the mikrocontroller.net forum.
*******************************************************************************/
static bool adjustDST( RTC_t *t )
{
	uint8_t hour, day, wday, month;			// locals for faster access

	hour  = t->hour;
	day   = t->mday;
	wday  = t->wday;
	month = t->month;

	if ( isDST(t) ) {
		t->dst = 1;
		hour++;								// add one hour
		if( hour == 24 ){					// next day
			hour = 0;
			wday++;							// next weekday
			if( wday == 7 ) {
				wday = 0;
			}
			if( day == DaysInMonth[month-1] ) {		// next month
				day = 0;
				month++;
			}
			day++;
		}
		t->month = month;
		t->hour  = hour;
		t->mday  = day;
		t->wday  = wday;
		return true;
	} else {
		t->dst = 0;
		return false;
	}
}

/*******************************************************************************
* Function Name  : counter_to_struct
* Description    : populates time-struct based on counter-value
* Input          : - counter-value (unit seconds, 0 -> 1.1.2000 00:00:00),
*                  - Pointer to time-struct
* Output         : time-struct gets populated, DST not taken into account here
* Return         : none
*  Based on code from Peter Dannegger found in the mikrocontroller.net forum.
*******************************************************************************/
static void counter_to_struct( uint32_t sec, RTC_t *t )
{
	uint16_t day;
	uint8_t year;
	uint16_t dayofyear;
	uint8_t leap400;
	uint8_t month;

	t->sec = sec % 60;
	sec /= 60;
	t->min = sec % 60;
	sec /= 60;
	t->hour = sec % 24;
	day = (uint16_t)(sec / 24);

	t->wday = (day + FIRSTDAY) % 7;		// weekday

	year = FIRSTYEAR % 100;				// 0..99
	leap400 = 4 - ((FIRSTYEAR - 1) / 100 & 3);	// 4, 3, 2, 1

	for(;;) {
		dayofyear = 365;
		if( (year & 3) == 0 ) {
			dayofyear = 366;					// leap year
			if( year == 0 || year == 100 || year == 200 ) {	// 100 year exception
				if( --leap400 ) {					// 400 year exception
					dayofyear = 365;
				}
			}
		}
		if( day < dayofyear ) {
			break;
		}
		day -= dayofyear;
		year++;					// 00..136 / 99..235
	}
	t->year = year + FIRSTYEAR / 100 * 100;	// + century

	if( dayofyear & 1 && day > 58 ) { 	// no leap year and after 28.2.
		day++;					// skip 29.2.
	}

	for( month = 1; day >= DaysInMonth[month-1]; month++ ) {
		day -= DaysInMonth[month-1];
	}

	t->month = month;				// 1..12
	t->mday = day + 1;				// 1..31
}

/*******************************************************************************
* Function Name  : struct_to_counter
* Description    : calculates second-counter from populated time-struct
* Input          : Pointer to time-struct
* Output         : none
* Return         : counter-value (unit seconds, 0 -> 1.1.2000 00:00:00),
*  Based on code from "LalaDumm" found in the mikrocontroller.net forum.
*******************************************************************************/
static uint32_t struct_to_counter( const RTC_t *t )
{
	uint16_t i;
	uint32_t result = 0;
	uint16_t idx, year;

	year = t->year;

	/* Calculate days of years before */
	result = (uint32_t)year * 365;
	if (t->year >= 1) {
		result += (year + 3) / 4;
		result -= (year - 1) / 100;
		result += (year - 1) / 400;
	}

	/* Start with 2000 a.d. */
	result -= 730485UL;

	/* Make month an array index */
	idx = t->month - 1;

	/* Loop thru each month, adding the days */
	for (i = 0; i < idx; i++) {
		result += DaysInMonth[i];
	}

	/* Leap year? adjust February */
	if (year%400 == 0 || (year%4 == 0 && year%100 !=0)) {
		;
	} else {
		if (t->month > 1) {
			result--;
		}
	}

	/* Add remaining days */
	result += t->mday;

	/* Convert to seconds, add all the other stuff */
	result = (result-1) * 86400L + (uint32_t)t->hour * 3600 +
		(uint32_t)t->min * 60 + t->sec;

	return result;
}

/*******************************************************************************
* Function Name  : rtc_gettime
* Description    : populates structure from HW-RTC, takes DST into account
* Input          : None
* Output         : time-struct gets modified
* Return         : always true/not used
*******************************************************************************/
bool rtc_gettime (RTC_t *rtc)
{
	uint32_t t;

	while ( ( t = RTC_GetCounter() ) != RTC_GetCounter() ) { ; }
	counter_to_struct( t, rtc ); // get non DST time
	adjustDST( rtc );

	return true;
}

/*******************************************************************************
* Function Name  : my_RTC_SetCounter
* Description    : sets the hardware-counter
* Input          : new counter-value
* Output         : None
* Return         : None
*******************************************************************************/
static void my_RTC_SetCounter(uint32_t cnt)
{
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
	/* Change the current time */
	RTC_SetCounter(cnt);
	/* Wait until last write operation on RTC registers has finished */
	RTC_WaitForLastTask();
}

/*******************************************************************************
* Function Name  : rtc_settime
* Description    : sets HW-RTC with values from time-struct, takes DST into
*                  account, HW-RTC always running in non-DST time
* Input          : None
* Output         : None
* Return         : not used
*******************************************************************************/
bool rtc_settime (const RTC_t *rtc)
{
	uint32_t cnt;
	RTC_t ts;

	cnt = struct_to_counter( rtc ); // non-DST counter-value
	counter_to_struct( cnt, &ts );  // normalize struct (for weekday)
	if ( isDST( &ts ) ) {
		cnt -= 60*60; // Subtract one hour
	}
	PWR_BackupAccessCmd(ENABLE);
	my_RTC_SetCounter( cnt );
	PWR_BackupAccessCmd(DISABLE);

	return true;
}

/*******************************************************************************
* Function Name  : rtc_init
* Description    : initializes HW RTC,
*                  sets default time-stamp if RTC has not been initialized before
* Input          : None
* Output         : None
* Return         : not used
*  see STM RTC example in the StdPeriph-Library package
*******************************************************************************/
int rtc_init(void)   //not use back_vcc
{

}



