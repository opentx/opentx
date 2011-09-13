#ifndef g9x_time_h
#define g9x_time_h

#include <inttypes.h>

#define CHAR_BIT 8
#define INT_MAX 32767
#define INT_MIN -32767
#define LONG_MAX 0x7FFFFFFFL
#define LONG_MIN ((long) 0x80000000L)
#define UINT_MAX 0xFFFFU/0xFFFFFFFFUL
#define ULONG_MAX 0xFFFFFFFFUL

typedef long int time_t;

struct tm
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

extern const unsigned short int __mon_yday[2][13];

extern time_t mktime(struct tm *tp);
extern time_t filltm(time_t *t, struct tm *tp);

#endif

