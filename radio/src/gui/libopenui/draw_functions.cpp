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

void drawTextAtIndex(BitmapBuffer * dc, coord_t x, coord_t y, const char * s, uint8_t idx, LcdFlags flags)
{
  char length = *s++;
  dc->drawSizedText(x, y, s+length*idx, length, flags & ~ZCHAR);
}

void drawNumber(BitmapBuffer * dc, coord_t x, coord_t y, int32_t val, LcdFlags flags, uint8_t len, const char * prefix, const char * suffix)
{
  char str[48+1]; // max=16 for the prefix, 16 chars for the number, 16 chars for the suffix
  char *s = str+32;
  *s = '\0';
  int idx = 0;
  int mode = MODE(flags);
  bool neg = false;
  if (val < 0) {
    val = -val;
    neg = true;
  }
  do {
    *--s = '0' + (val % 10);
    ++idx;
    val /= 10;
    if (mode!=0 && idx==mode) {
      mode = 0;
      *--s = '.';
      if (val==0)
        *--s = '0';
    }
  } while (val!=0 || mode>0 || (mode==MODE(LEADING0) && idx<len));
  if (neg) *--s = '-';

  // TODO needs check on all string lengths ...
  if (prefix) {
    int len = strlen(prefix);
    if (len <= 16) {
      s -= len;
      strncpy(s, prefix, len);
    }
  }
  if (suffix) {
    strncpy(&str[32], suffix, 16);
  }
  flags &= ~LEADING0;
  dc->drawText(x, y, s, flags);
}

void drawSolidRect(BitmapBuffer * dc, coord_t x, coord_t y, coord_t w, coord_t h, uint8_t thickness, LcdFlags att)
{
  dc->drawSolidFilledRect(x, y, thickness, h, att);
  dc->drawSolidFilledRect(x+w-thickness, y, thickness, h, att);
  dc->drawSolidFilledRect(x, y, w, thickness, att);
  dc->drawSolidFilledRect(x, y+h-thickness, w, thickness, att);
}

void drawSource(BitmapBuffer * dc, coord_t x, coord_t y, mixsrc_t idx, LcdFlags flags)
{
  char s[16];
  getSourceString(s, idx);
  dc->drawText(x, y, s, flags);
}

void drawSwitch(BitmapBuffer * dc, coord_t x, coord_t y, int32_t idx, LcdFlags flags)
{
  char s[8];
  getSwitchString(s, idx);
  dc->drawText(x, y, s, flags);
}
