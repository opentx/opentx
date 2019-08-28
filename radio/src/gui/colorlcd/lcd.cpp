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

#include <math.h>
#include <stdio.h>
#include "opentx.h"
#include "strhelpers.h"

#if defined(SIMU)
display_t displayBuf[DISPLAY_BUFFER_SIZE];
#endif

uint16_t lcdColorTable[LCD_COLOR_COUNT];

coord_t lcdNextPos;

uint8_t getMappedChar(uint8_t c)
{
  uint8_t result;
  if (c == 0)
    result = 0;
#if defined(TRANSLATIONS_FR)
  else if (c >= 0x80 && c <= 0x84) {
    result = 115 + c - 0x80;
  }
#elif defined(TRANSLATIONS_DE)
  else if (c >= 0x80 && c <= 0x86) {
    result = 120 + c - 0x80;
  }
#elif defined(TRANSLATIONS_CZ)
  else if (c >= 0x80 && c <= 0x80+29) {
    result = 127 + c - 0x80;
  }
#elif defined(TRANSLATIONS_ES)
  else if (c >= 0x80 && c <= 0x81) {
    result = 157 + c - 0x80;
  }
#elif defined(TRANSLATIONS_FI)
  else if (c >= 0x80 && c <= 0x85) {
    result = 159 + c - 0x80;
  }
#elif defined(TRANSLATIONS_IT)
  else if (c >= 0x80 && c <= 0x81) {
    result = 165 + c - 0x80;
  }
#elif defined(TRANSLATIONS_PL)
  else if (c >= 0x80 && c <= 0x80+17) {
    result = 167 + c - 0x80;
  }
#elif defined(TRANSLATIONS_PT)
  else if (c >= 0x80 && c <= 0x80+21) {
    result = 185 + c - 0x80;
  }
#elif defined(TRANSLATIONS_SE)
  else if (c >= 0x80 && c <= 0x85) {
    result = 207 + c - 0x80;
  }
#endif
  else if (c < 0xC0)
    result = c - 0x20;
  else
    result = c - 0xC0 + 96;
  // TRACE("getMappedChar '%c' (%d) = %d", c, c, result);
  return result;
}

int getFontPatternWidth(const uint16_t * spec, uint8_t index)
{
  return spec[index + 2] - spec[index + 1];
}

int getCharWidth(uint8_t c, const uint16_t * spec)
{
  return getFontPatternWidth(spec, getMappedChar(c));
}

void lcdPutFontPattern(coord_t x, coord_t y, const uint8_t * font, const uint16_t * spec, int index, LcdFlags flags)
{
  coord_t offset = spec[index + 1];
  coord_t width = spec[index + 2] - offset;
  if (width > 0)
    lcdDrawBitmapPattern(x, y, font, flags, offset, width);
  lcdNextPos = x + width;
}

void lcdDrawChar(coord_t x, coord_t y, char c, LcdFlags flags)
{
  uint32_t fontindex = FONTINDEX(flags);
  const unsigned char * font = fontsTable[fontindex];
  const uint16_t * fontspecs = fontspecsTable[fontindex];
  lcdPutFontPattern(x, y, font, fontspecs, getMappedChar(c), flags);
}

uint8_t getFontHeight(LcdFlags flags)
{
  uint32_t fontindex = FONTINDEX(flags);
  return fontspecsTable[fontindex][0];
}

int getTextWidth(const char * s, int len, LcdFlags flags)
{
  const uint16_t * specs = fontspecsTable[FONTINDEX(flags)];

  int result = 0;
  for (int i=0; len==0 || i<len; ++i) {

#if !defined(BOOT)
    char c = (flags & ZCHAR) ? zchar2char(*s) : *s;
#else
    char c = *s;
#endif
    if (c == '\0')
      break;
    result += getCharWidth(c, specs);
    ++s;
  }
  return result;
}

void lcdDrawTextAtIndex(coord_t x, coord_t y, const char * s, uint8_t idx, LcdFlags flags)
{
  uint8_t length;
  length = *(s++);
  lcdDrawSizedText(x, y, s+length*idx, length, flags & ~ZCHAR);
}

void lcdDrawNumber(coord_t x, coord_t y, int32_t val, LcdFlags flags, uint8_t len, const char * prefix, const char * suffix)
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
  lcdDrawText(x, y, s, flags);
}

void lcdSetContrast()
{
  lcdSetRefVolt(g_eeGeneral.contrast);
}

#if defined(SIMU)
BitmapBuffer _lcd(BMP_RGB565, LCD_W, LCD_H, displayBuf);
BitmapBuffer * lcd = &_lcd;

void DMAFillRect(uint16_t * dest, uint16_t destw, uint16_t desth, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
#if defined(PCBX10) && !defined(SIMU)
  x = destw - (x + w);
  y = desth - (y + h);
#endif

  for (int i=0; i<h; i++) {
    for (int j=0; j<w; j++) {
      dest[(y+i)*destw+x+j] = color;
    }
  }
}

void DMACopyBitmap(uint16_t * dest, uint16_t destw, uint16_t desth, uint16_t x, uint16_t y, const uint16_t * src, uint16_t srcw, uint16_t srch, uint16_t srcx, uint16_t srcy, uint16_t w, uint16_t h)
{
#if defined(PCBX10) && !defined(SIMU)
  x = destw - (x + w);
  y = desth - (y + h);
  srcx = srcw - (srcx + w);
  srcy = srch - (srcy + h);
#endif

  for (int i=0; i<h; i++) {
    memcpy(dest+(y+i)*destw+x, src+(srcy+i)*srcw+srcx, 2*w);
  }
}

void DMACopyAlphaBitmap(uint16_t * dest, uint16_t destw, uint16_t desth, uint16_t x, uint16_t y, const uint16_t * src, uint16_t srcw, uint16_t srch, uint16_t srcx, uint16_t srcy, uint16_t w, uint16_t h)
{
#if defined(PCBX10) && !defined(SIMU)
  x = destw - (x + w);
  y = desth - (y + h);
  srcx = srcw - (srcx + w);
  srcy = srch - (srcy + h);
#endif

  for (coord_t line=0; line<h; line++) {
    uint16_t * p = dest + (y+line)*destw + x;
    const uint16_t * q = src + (srcy+line)*srcw + srcx;
    for (coord_t col=0; col<w; col++) {
      uint8_t alpha = *q >> 12;
      uint8_t red = ((((*q >> 8) & 0x0f) << 1) * alpha + (*p >> 11) * (0x0f-alpha)) / 0x0f;
      uint8_t green = ((((*q >> 4) & 0x0f) << 2) * alpha + ((*p >> 5) & 0x3f) * (0x0f-alpha)) / 0x0f;
      uint8_t blue = ((((*q >> 0) & 0x0f) << 1) * alpha + ((*p >> 0) & 0x1f) * (0x0f-alpha)) / 0x0f;
      *p = (red << 11) + (green << 5) + (blue << 0);
      p++; q++;
    }
  }
}

void DMABitmapConvert(uint16_t * dest, const uint8_t * src, uint16_t w, uint16_t h, uint32_t format)
{
  if (format == DMA2D_ARGB4444) {
    for (int row = 0; row < h; ++row) {
      for (int col = 0; col < w; ++col) {
        *dest = ARGB(src[0], src[1], src[2], src[3]);
        ++dest;
        src += 4;
      }
    }
  }
  else {
    for (int row = 0; row < h; ++row) {
      for(int col = 0; col < w; ++col) {
        *dest = RGB(src[1], src[2], src[3]);
        ++dest;
        src += 4;
      }
    }
  }
}
#endif
