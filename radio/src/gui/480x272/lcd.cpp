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
  return spec[index+1] - spec[index];
}

int getCharWidth(uint8_t c, const uint16_t * spec)
{
  return getFontPatternWidth(spec, getMappedChar(c));
}

void lcdPutFontPattern(coord_t x, coord_t y, const uint8_t * font, const uint16_t * spec, int index, LcdFlags flags)
{
  coord_t offset = spec[index];
  coord_t width = spec[index+1] - offset;
  if (width > 0) lcdDrawBitmapPattern(x, y, font, flags, offset, width);
  lcdNextPos = x + width;
}

void lcdDrawChar(coord_t x, coord_t y, char c, LcdFlags flags)
{
  uint32_t fontindex = FONTINDEX(flags);
  const pm_uchar * font = fontsTable[fontindex];
  const uint16_t * fontspecs = fontspecsTable[fontindex];
  lcdPutFontPattern(x, y, font, fontspecs, getMappedChar(c), flags);
}

uint8_t getStringInfo(const char *s)
{
  uint8_t result = 0;
  for (int i=0; s[i]; i++) {
    result += s[i];
  }
  return result;
}

uint8_t getFontHeight(LcdFlags flags)
{
  static const uint8_t heightTable[16] = { 9, 13, 16, 24, 32, 64, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12 };
  return heightTable[FONTINDEX(flags)];
}

int getTextWidth(const char * s, int len, LcdFlags flags)
{
  const uint16_t * specs = fontspecsTable[FONTINDEX(flags)];

  int result = 0;
  for (int i=0; len==0 || i<len; ++i) {

#if !defined(BOOT)
    char c = (flags & ZCHAR) ? idx2char(*s) : *s;
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

void lcdDrawTextAtIndex(coord_t x, coord_t y, const pm_char * s, uint8_t idx, LcdFlags flags)
{
  uint8_t length;
  length = pgm_read_byte(s++);
  lcdDrawSizedText(x, y, s+length*idx, length, flags & ~ZCHAR);
}

void lcdDrawHexNumber(coord_t x, coord_t y, uint32_t val, LcdFlags flags)
{
  char s[5];
  for (int i=0; i<4; i++) {
    char c = val & 0xf;
    s[3-i] = c>9 ? c+'A'-10 : c+'0';
    val >>= 4;
  }
  s[4] = '\0';
  lcdDrawText(x, y, s, flags);
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

void lcdDrawLine(coord_t x1, coord_t y1, coord_t x2, coord_t y2, uint8_t pat, LcdFlags att)
{
  int dx = x2-x1;      /* the horizontal distance of the line */
  int dy = y2-y1;      /* the vertical distance of the line */
  int dxabs = abs(dx);
  int dyabs = abs(dy);
  int sdx = sgn(dx);
  int sdy = sgn(dy);
  int x = dyabs>>1;
  int y = dxabs>>1;
  int px = x1;
  int py = y1;

  if (dxabs >= dyabs) {
    /* the line is more horizontal than vertical */
    for (int i=0; i<=dxabs; i++) {
      if ((1<<(px%8)) & pat) {
        lcdDrawPoint(px, py, att);
      }
      y += dyabs;
      if (y>=dxabs) {
        y -= dxabs;
        py += sdy;
      }
      px += sdx;
    }
  }
  else {
    /* the line is more vertical than horizontal */
    for (int i=0; i<=dyabs; i++) {
      if ((1<<(py%8)) & pat) {
        lcdDrawPoint(px, py, att);
      }
      x += dxabs;
      if (x >= dyabs) {
        x -= dyabs;
        px += sdx;
      }
      py += sdy;
    }
  }
}

#if !defined(BOOT)
void drawRtcTime(coord_t x, coord_t y, LcdFlags flags)
{
  drawTimer(x, y, getValue(MIXSRC_TX_TIME), flags);
}

void drawTimer(coord_t x, coord_t y, putstime_t tme, LcdFlags flags)
{
  char str[LEN_TIMER_STRING];
  getTimerString(str, tme, (flags & TIMEHOUR) != 0);
  lcdDrawText(x, y, str, flags);
}

void putsStickName(coord_t x, coord_t y, uint8_t idx, LcdFlags att)
{
  uint8_t length = STR_VSRCRAW[0];
  lcdDrawSizedText(x, y, STR_VSRCRAW+2+length*(idx+1), length-1, att);
}

void drawSource(coord_t x, coord_t y, mixsrc_t idx, LcdFlags flags)
{
  char s[16];
  getSourceString(s, idx);
  lcdDrawText(x, y, s, flags);
}

void putsChnLetter(coord_t x, coord_t y, uint8_t idx, LcdFlags att)
{
  lcdDrawTextAtIndex(x, y, STR_RETA123, idx-1, att);
}

void putsModelName(coord_t x, coord_t y, char * name, uint8_t id, LcdFlags att)
{
  uint8_t len = sizeof(g_model.header.name);
  while (len>0 && !name[len-1]) --len;
  if (len==0) {
    drawStringWithIndex(x, y, STR_MODEL, id+1, att|LEADING0);
  }
  else {
    lcdDrawSizedText(x, y, name, sizeof(g_model.header.name), ZCHAR|att);
  }
}

void drawSwitch(coord_t x, coord_t y, swsrc_t idx, LcdFlags flags)
{
  char s[8];
  getSwitchString(s, idx);
  lcdDrawText(x, y, s, flags);
}

void drawCurveName(coord_t x, coord_t y, int8_t idx, LcdFlags flags)
{
  char s[8];
  getCurveString(s, idx);
  lcdDrawText(x, y, s, flags);
}

void drawTimerMode(coord_t x, coord_t y, int32_t mode, LcdFlags att)
{
  if (mode >= 0) {
    if (mode < TMRMODE_COUNT) {
      lcdDrawTextAtIndex(x, y, STR_VTMRMODES, mode, att);
      return;
    }
    else {
      mode -= (TMRMODE_COUNT-1);
    }
  }
  drawSwitch(x, y, mode, att);
}

void drawTrimMode(coord_t x, coord_t y, uint8_t phase, uint8_t idx, LcdFlags att)
{
  trim_t v = getRawTrimValue(phase, idx);
  unsigned int mode = v.mode;
  unsigned int p = mode >> 1;

  if (mode == TRIM_MODE_NONE) {
    lcdDrawText(x, y, "--", att);
  }
  else {
    char s[2];
    s[0] = (mode % 2 == 0) ? ':' : '+';
    s[1] = '0'+p;
    lcdDrawSizedText(x, y, s, 2, att);
  }
}


void drawDate(coord_t x, coord_t y, TelemetryItem & telemetryItem, LcdFlags att)
{
  // TODO
  if (att & DBLSIZE) {
    x -= 42;
    att &= ~FONTSIZE_MASK;
    lcdDrawNumber(x, y, telemetryItem.datetime.day, att|LEADING0|LEFT, 2);
    lcdDrawChar(lcdNextPos-1, y, '-', att);
    lcdDrawNumber(lcdNextPos-1, y, telemetryItem.datetime.month, att|LEFT, 2);
    lcdDrawChar(lcdNextPos-1, y, '-', att);
    lcdDrawNumber(lcdNextPos-1, y, telemetryItem.datetime.year-2000, att|LEFT);
    y += FH;
    lcdDrawNumber(x, y, telemetryItem.datetime.hour, att|LEADING0|LEFT, 2);
    lcdDrawChar(lcdNextPos, y, ':', att);
    lcdDrawNumber(lcdNextPos, y, telemetryItem.datetime.min, att|LEADING0|LEFT, 2);
    lcdDrawChar(lcdNextPos, y, ':', att);
    lcdDrawNumber(lcdNextPos, y, telemetryItem.datetime.sec, att|LEADING0|LEFT, 2);
  }
  else {
    lcdDrawNumber(x, y, telemetryItem.datetime.day, att|LEADING0|LEFT, 2);
    lcdDrawChar(lcdNextPos-1, y, '-', att);
    lcdDrawNumber(lcdNextPos, y, telemetryItem.datetime.month, att|LEFT, 2);
    lcdDrawChar(lcdNextPos-1, y, '-', att);
    lcdDrawNumber(lcdNextPos, y, telemetryItem.datetime.year-2000, att|LEFT);
    lcdDrawNumber(lcdNextPos+11, y, telemetryItem.datetime.hour, att|LEADING0|LEFT, 2);
    lcdDrawChar(lcdNextPos, y, ':', att);
    lcdDrawNumber(lcdNextPos, y, telemetryItem.datetime.min, att|LEADING0|LEFT, 2);
    lcdDrawChar(lcdNextPos, y, ':', att);
    lcdDrawNumber(lcdNextPos, y, telemetryItem.datetime.sec, att|LEADING0|LEFT, 2);
  }
}

void drawGPSCoord(coord_t x, coord_t y, int32_t value, const char * direction, LcdFlags flags, bool seconds=true)
{
  char s[32];
  uint32_t absvalue = abs(value);
  char * tmp = strAppendUnsigned(s, absvalue / 1000000);
  *tmp++ = '@';
  absvalue = absvalue % 1000000;
  absvalue *= 60;
  if (g_eeGeneral.gpsFormat == 0 || !seconds) {
    tmp = strAppendUnsigned(tmp, absvalue / 1000000, 2);
    *tmp++ = '\'';
    if (seconds) {
      absvalue %= 1000000;
      absvalue *= 60;
      absvalue /= 100000;
      tmp = strAppendUnsigned(tmp, absvalue / 10);
      *tmp++ = '.';
      tmp = strAppendUnsigned(tmp, absvalue % 10);
      *tmp++ = '"';
    }
  }
  else {
    tmp = strAppendUnsigned(tmp, absvalue / 1000000, 2);
    *tmp++ = '.';
    absvalue /= 1000;
    tmp = strAppendUnsigned(tmp, absvalue, 3);
  }
  *tmp++ = direction[value>=0 ? 0 : 1];
  *tmp = '\0';
  lcdDrawText(x, y, s, flags);
}

void drawGPSPosition(coord_t x, coord_t y, int32_t longitude, int32_t latitude, LcdFlags flags)
{
  if (flags & EXPANDED) {
    drawGPSCoord(x, y, latitude, "NS", flags, true);
    drawGPSCoord(x, y + FH, longitude, "EW", flags, true);
  }
  else {
    drawGPSCoord(x, y, latitude, "NS", flags, false);
    drawGPSCoord(lcdNextPos+5, y, longitude, "EW", flags, false);
  }
}

void drawGPSSensorValue(coord_t x, coord_t y, TelemetryItem & telemetryItem, LcdFlags flags)
{
  drawGPSPosition(x, y, telemetryItem.gps.longitude, telemetryItem.gps.latitude, flags);
}
#endif

void lcdSetContrast()
{
  lcdSetRefVolt(g_eeGeneral.contrast);
}

void lcdDrawPoint(coord_t x, coord_t y, LcdFlags att)
{
  display_t * p = PIXEL_PTR(x, y);
  display_t color = lcdColorTable[COLOR_IDX(att)];
  if (p < DISPLAY_END) {
    *p = color;
  }
}

void lcdDrawBlackOverlay()
{
  lcdDrawFilledRect(0, 0, LCD_W, LCD_H, SOLID, OVERLAY_COLOR | OPACITY(8));
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
