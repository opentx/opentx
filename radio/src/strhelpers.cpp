/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "opentx.h"

#if !defined(BOOT)
const pm_char s_charTab[] PROGMEM = "_-.,";

char hex2zchar(uint8_t hex)
{
  return (hex >= 10 ? hex-9 : 27+hex);
}

char idx2char(int8_t idx)
{
  if (idx == 0) return ' ';
  if (idx < 0) {
    if (idx > -27) return 'a' - idx - 1;
    idx = -idx;
  }
  if (idx < 27) return 'A' + idx - 1;
  if (idx < 37) return '0' + idx - 27;
  if (idx <= 40) return pgm_read_byte(s_charTab+idx-37);
#if LEN_SPECIAL_CHARS > 0
  if (idx <= (LEN_STD_CHARS + LEN_SPECIAL_CHARS)) return 'z' + 5 + idx - 40;
#endif
  return ' ';
}

#if defined(CPUARM) || defined(SIMU)
int8_t char2idx(char c)
{
  if (c == '_') return 37;
#if LEN_SPECIAL_CHARS > 0
  if (c < 0 && c+128 <= LEN_SPECIAL_CHARS) return 41 + (c+128);
#endif
  if (c >= 'a') return 'a' - c - 1;
  if (c >= 'A') return c - 'A' + 1;
  if (c >= '0') return c - '0' + 27;
  if (c == '-') return 38;
  if (c == '.') return 39;
  if (c == ',') return 40;
  return 0;
}

void str2zchar(char * dest, const char *src, int size)
{
  memset(dest, 0, size);
  for (int c=0; c<size && src[c]; c++) {
    dest[c] = char2idx(src[c]);
  }
}

int zchar2str(char * dest, const char *src, int size)
{
  for (int c=0; c<size; c++) {
    dest[c] = idx2char(src[c]);
  }
  do {
    dest[size--] = '\0';
  } while (size >= 0 && dest[size] == ' ');
  return size+1;
}
#endif

#if defined(CPUARM)
unsigned int effectiveLen(const char * str, unsigned int size)
{
  while (size > 0) {
    if (str[size-1] != ' ')
      return size;
    size--;
  }
  return 0;
}

bool zexist(const char * str, uint8_t size)
{
  for (int i=0; i<size; i++) {
    if (str[i] != 0)
      return true;
  }
  return false;
}

uint8_t zlen(const char * str, uint8_t size)
{
  while (size > 0) {
    if (str[size-1] != 0)
      return size;
    size--;
  }
  return 0;
}

char * strcat_zchar(char * dest, const char * name, uint8_t size, const char * defaultName, uint8_t defaultNameSize, uint8_t defaultIdx)
{
  int8_t len = 0;

  if (name) {
    memcpy(dest, name, size);
    dest[size] = '\0';

    int8_t i = size-1;

    while (i>=0) {
      if (!len && dest[i])
        len = i+1;
      if (len) {
        if (dest[i])
          dest[i] = idx2char(dest[i]);
        else
          dest[i] = '_';
      }
      i--;
    }
  }

  if (len == 0 && defaultName) {
    strcpy(dest, defaultName);
    dest[defaultNameSize] = (char)((defaultIdx / 10) + '0');
    dest[defaultNameSize + 1] = (char)((defaultIdx % 10) + '0');
    len = defaultNameSize + 2;
  }

  return &dest[len];
}
#endif
#endif

char * strAppendUnsigned(char * dest, uint32_t value, uint8_t digits, uint8_t radix)
{
  if (digits == 0) {
    unsigned int tmp = value;
    digits = 1;
    while (tmp >= 10) {
      ++digits;
      tmp /= radix;
    }
  }
  uint8_t idx = digits;
  while(idx > 0) {
    div_t qr = div(value, radix);
    dest[--idx] = (qr.rem >= 10 ? 'A'-10 : '0') + qr.rem;
    value = qr.quot;
  }
  dest[digits] = '\0';
  return &dest[digits];
}

char * strAppendSigned(char * dest, int32_t value, uint8_t digits, uint8_t radix)
{
  if (value < 0) {
    *dest++ = '-';
    value = -value;
  }
  return strAppendUnsigned(dest, (uint32_t)value, digits, radix);
}

#if defined(CPUARM) || defined(SDCARD)
char * strAppend(char * dest, const char * source, int len)
{
  while ((*dest++ = *source++)) {
    if (--len == 0) {
      *dest = '\0';
      return dest;
    }
  }
  return dest - 1;
}

char * strSetCursor(char * dest, int position)
{
  *dest++ = 0x1F;
  *dest++ = position;
  *dest = '\0';
  return dest;
}

char * strAppendFilename(char * dest, const char * filename, const int size)
{
  memset(dest, 0, size);
  for (int i=0; i<size; i++) {
    char c = *filename++;
    if (c == '\0' || c == '.')
      break;
    *dest++ = c;
  }
  return dest;
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
    qr = div(utm.tm_min, 10);
    str[15] = '0' + qr.rem;
    str[14] = '0' + qr.quot;
    qr = div(utm.tm_sec, 10);
    str[17] = '0' + qr.rem;
    str[16] = '0' + qr.quot;
    str[18] = '\0';
    return &str[18];
  }
  else {
    str[11] = '\0';
    return &str[11];
  }
}
#endif
#endif
