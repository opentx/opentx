#include <stdlib.h>

char * strAppend(char * dest, const char * source)
{
  while ((*dest++ = *source++))
    ;
  return dest - 1;
}

#if defined(RTCLOCK)
#include "rtc.h"

char * strAppendDate(char * str, bool time)
{
  str[0] = '-';
  struct gtm utm;
  gettime(&utm);
  div_t qr = div(utm.tm_year+1900, 10);
  str[4] = '0' + qr.rem;
  qr = div(qr.quot, 10);
  str[3] = '0' + qr.rem;
  qr = div(qr.quot, 10);
  str[2] = '0' + qr.rem;
  str[1] = '0' + qr.quot;
  str[5] = '-';
  qr = div(utm.tm_mon+1, 10);
  str[7] = '0' + qr.rem;
  str[6] = '0' + qr.quot;
  str[8] = '-';
  qr = div(utm.tm_mday, 10);
  str[10] = '0' + qr.rem;
  str[9] = '0' + qr.quot;

  if (time) {
    str[11] = '-';
    div_t qr = div(utm.tm_hour, 10);
    str[13] = '0' + qr.rem;
    str[12] = '0' + qr.quot;
    str[14] = '-';
    qr = div(utm.tm_min, 10);
    str[16] = '0' + qr.rem;
    str[15] = '0' + qr.quot;
    str[17] = '-';
    qr = div(utm.tm_sec, 10);
    str[19] = '0' + qr.rem;
    str[18] = '0' + qr.quot;
    str[20] = '\0';
    return &str[20];
  }
  else {
    str[11] = '\0';
    return &str[11];
  }
}

#endif
