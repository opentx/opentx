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

#ifndef _LCD_H_
#define _LCD_H_

#include "bitmapbuffer.h"

#define LCD_W           480
#define LCD_H           272

#define CENTER

#define lcdint_t        int32_t

#define LCD_COLS        30

#define BSS             0x00
#define BOLD            0x00

/* lcd common flags */
#define BLINK           0x01

/* drawText flags */
#define INVERS          0x02
#define LEFT            0x00
#define CENTERED        0x04 /* align left */
#define RIGHT           0x08 /* align left */

/* drawNumber flags */
#define LEADING0        0x10
#define PREC1           0x20
#define PREC2           0x30
#define MODE(flags)     ((((int8_t)(flags) & 0x30) - 0x10) >> 4)

#define ZCHAR           0x10

/* rect, square flags */
#define ROUND           0x04

/* telemetry flags */
#define NO_UNIT         0x40

#define FONTSIZE(x)     ((x) & 0x0700)
#define TINSIZE         0x0100
#define SMLSIZE         0x0200
#define MIDSIZE         0x0300
#define DBLSIZE         0x0400
#define XXLSIZE         0x0500
#define VERTICAL        0x0800

#define TIMEBLINK       0x1000
#define TIMEHOUR        0x2000
#define STREXPANDED     0x4000

#include "colors.h"
#define DISPLAY_PIXELS_COUNT           (LCD_W*LCD_H)
#define DISPLAY_BUFFER_SIZE            (sizeof(display_t)*DISPLAY_PIXELS_COUNT)

#if defined(SIMU)
extern display_t displayBuf[DISPLAY_BUFFER_SIZE];
#else
#define displayBuf                     lcd->getData()
#endif

#define lcdRefreshWait()

#define DISPLAY_END                    (displayBuf + DISPLAY_BUFFER_SIZE)
#define ASSERT_IN_DISPLAY(p)           assert((p) >= displayBuf && (p) < DISPLAY_END)

extern coord_t lcdNextPos;

void lcdDrawChar(coord_t x, coord_t y, const unsigned char c, LcdFlags attr=0);
void lcdDrawText(coord_t x, coord_t y, const pm_char * s, LcdFlags attr=0);
void lcdDrawTextAtIndex(coord_t x, coord_t y, const pm_char * s, uint8_t idx, LcdFlags attr=0);

inline void lcdClear()
{
  lcd->clear();
}

inline void lcdDrawSizedText(coord_t x, coord_t y, const pm_char * s, uint8_t len, LcdFlags attr=0)
{
  lcd->drawSizedText(x, y, s, len, attr);
}

void lcdDrawHexNumber(coord_t x, coord_t y, uint32_t val, LcdFlags mode=0);
void lcdDrawNumber(coord_t x, coord_t y, int32_t val, LcdFlags flags=0, uint8_t len=0, const char * prefix=NULL, const char * suffix=NULL);

void drawStringWithIndex(coord_t x, coord_t y, const pm_char *str, int idx, LcdFlags att=0, const char *prefix="");
void putsModelName(coord_t x, coord_t y, char *name, uint8_t id, LcdFlags att);
void putsStickName(coord_t x, coord_t y, uint8_t idx, LcdFlags att=0);
void putsSwitches(coord_t x, coord_t y, swsrc_t swtch, LcdFlags flags=0);
void putsMixerSource(coord_t x, coord_t y, uint8_t idx, LcdFlags att=0);
void putsFlightMode(coord_t x, coord_t y, int8_t idx, LcdFlags att=0);
void putsCurveRef(coord_t x, coord_t y, CurveRef &curve, LcdFlags att=0);
void drawCurveName(coord_t x, coord_t y, int8_t idx, LcdFlags att=0);
void putsTimerMode(coord_t x, coord_t y, int8_t mode, LcdFlags att=0);
void putsTrimMode(coord_t x, coord_t y, uint8_t phase, uint8_t idx, LcdFlags att);

#define putsChn(x, y, idx, att) putsMixerSource(x, y, MIXSRC_CH1+idx-1, att)
void putsChnLetter(coord_t x, coord_t y, uint8_t idx, LcdFlags attr);

void putsChannelValue(coord_t x, coord_t y, source_t channel, int32_t val, LcdFlags att=0);
void putsChannel(coord_t x, coord_t y, source_t channel, LcdFlags att=0);
void putsTelemetryChannelValue(coord_t x, coord_t y, uint8_t channel, int32_t val, LcdFlags att=0);

#define putstime_t int32_t

void getTimerString(char * str, putstime_t tme, LcdFlags att=0);
void putsRtcTime(coord_t x, coord_t y, LcdFlags att=0);
void putsTimer(coord_t x, coord_t y, putstime_t tme, LcdFlags att=0);

#define SOLID   0xff
#define DOTTED  0x55
#define STASHED 0x33

#define PIXEL_PTR(x, y) &displayBuf[(y)*LCD_W + (x)]

void lcdDrawAlphaPixel(display_t * p, uint8_t opacity, uint16_t color);
void lcdDrawPoint(coord_t x, coord_t y, LcdFlags att=0);
inline void lcdDrawHorizontalLine(coord_t x, coord_t y, coord_t w, uint8_t pat, LcdFlags att=0)
{
  lcd->drawHorizontalLine(x, y, w, pat, att);
}

inline void lcdDrawVerticalLine(coord_t x, coord_t y, coord_t h, uint8_t pat, LcdFlags att=0)
{
  lcd->drawVerticalLine(x, y, h, pat, att);
}

void lcdDrawLine(coord_t x1, coord_t y1, coord_t x2, coord_t y2, uint8_t pat=SOLID, LcdFlags att=0);

inline void lcdDrawAlphaPixel(coord_t x, coord_t y, uint8_t opacity, uint16_t color)
{
  display_t * p = PIXEL_PTR(x, y);
  lcdDrawAlphaPixel(p, opacity, color);
}

#if !defined(SIMU)
inline void lcdDrawSolidFilledRect(coord_t x, coord_t y, coord_t w, coord_t h, LcdFlags flags)
{
  DMAFillRect(lcd->getData(), LCD_W, x, y, w, h, lcdColorTable[COLOR_IDX(flags)]);
}
#else
void lcdDrawSolidFilledRect(coord_t x, coord_t y, coord_t w, coord_t h, LcdFlags flags);
#endif

inline void lcdSetColor(uint16_t color)
{
  lcdColorTable[CUSTOM_COLOR_INDEX] = color;
}

inline void lcdDrawSolidHorizontalLine(coord_t x, coord_t y, coord_t w, LcdFlags att)
{
  lcdDrawSolidFilledRect(x, y, w, 1, att);
}

inline void lcdDrawSolidVerticalLine(coord_t x, coord_t y, coord_t h, LcdFlags att)
{
  if (h<0) { y+=h; h=-h; }
  lcdDrawSolidFilledRect(x, y, 1, h, att);
}

inline void lcdDrawSolidRect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t thickness=1, LcdFlags att=0)
{
  lcdDrawSolidFilledRect(x, y, thickness, h, att);
  lcdDrawSolidFilledRect(x+w-thickness, y, thickness, h, att);
  lcdDrawSolidFilledRect(x, y, w, thickness, att);
  lcdDrawSolidFilledRect(x, y+h-thickness, w, thickness, att);
}

inline void lcdDrawFilledRect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t pat, LcdFlags att=0)
{
  lcd->drawFilledRect(x, y, w, h, pat, att);
}

void lcdDrawBlackOverlay();
void lcdDrawRect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t thickness=1, uint8_t pat=SOLID, LcdFlags att=0);
void lcdDrawCircle(int x0, int y0, int radius);
void lcdDrawPie(int x0, int y0, int radius, int angle1=0, int angle2=360);
void lcdDrawBitmapPie(int x0, int y0, const uint16_t * img, int startAngle, int endAngle);

inline void lcdDrawBitmapPatternPie(coord_t x0, coord_t y0, const uint8_t * bmp, LcdFlags flags=0, int startAngle=0, int endAngle=360)
{
  lcd->drawBitmapPatternPie(x0, y0, bmp, flags, startAngle, endAngle);
}

inline void lcdDrawSquare(coord_t x, coord_t y, coord_t w, LcdFlags att=0)
{
  lcdDrawSolidRect(x, y, w, w, att);
}

inline int getBitmapWidth(const uint8_t * bmp)
{
  return *((const uint16_t *)bmp);
}

inline int getBitmapHeight(const uint8_t * bmp)
{
  return *(((const uint16_t *)bmp)+1);
}

char getMappedChar(unsigned char c);
int getFontHeight(LcdFlags flags);
int getTextWidth(const pm_char *s, int len=0, LcdFlags flags=0);

inline void lcdDrawBitmapPattern(coord_t x, coord_t y, const uint8_t * img, LcdFlags flags=0, coord_t offset=0, coord_t width=0)
{
  lcd->drawBitmapPattern(x, y, img, flags, offset, width);
}

#define lcdSetRefVolt(...)
void lcdSetContrast();
#define lcdOff(...)

#if defined(BOOT)
  #define BLINK_ON_PHASE               (0)
#else
  #define BLINK_ON_PHASE               (g_blinkTmr10ms & (1<<6))
#endif

#endif // _LCD_H_
