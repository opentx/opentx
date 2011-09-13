#ifndef RTC_H
#define RTC_H

#include "integer.h"

typedef struct {
	WORD	year;	/* 2000..2099 */
	BYTE	month;	/* 1..12 */
	BYTE	mday;	/* 1.. 31 */
	BYTE	wday;	/* 1..7 */
	BYTE	hour;	/* 0..23 */
	BYTE	min;	/* 0..59 */
	BYTE	sec;	/* 0..59 */
} RTC;

int iic_write (BYTE, UINT, UINT, const void*);	/* Write to IIC device */
int iic_read (BYTE, UINT, UINT, void*);		/* Read from IIC device */

int rtc_init (void);				/* Initialize RTC */
int rtc_gettime (RTC*);				/* Get time */
int rtc_settime (const RTC*);			/* Set time */

#endif
