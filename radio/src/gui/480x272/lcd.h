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
#include "opentx_types.h"

#if LCD_W >= 480
  #define LCD_COLS                     40
#else
  #define LCD_COLS                     30
#endif

#define CENTER

#define lcdint_t                       int32_t

#define BSS                            0x00

/* lcd common flags */
#define BLINK                          0x01

/* drawText flags */
#define INVERS                         0x02
#define LEFT                           0x00 /* align left */
#define CENTERED                       0x04 /* align center */
#define RIGHT                          0x08 /* align right */
#define SHADOWED                       0x80 /* black copy at +1 +1 */

/* drawNumber flags */
#define LEADING0                       0x10
#define PREC1                          0x20
#define PREC2                          0x30
#define MODE(flags)                    ((((int8_t)(flags) & 0x30) - 0x10) >> 4)

#define ZCHAR                          0x10

/* rect, square flags */
#define ROUND                          0x04

/* telemetry flags */
#define NO_UNIT                        0x40

enum FontSizeIndex {
  STDSIZE_INDEX,
  TINSIZE_INDEX,
  SMLSIZE_INDEX,
  MIDSIZE_INDEX,
  DBLSIZE_INDEX,
  XXLSIZE_INDEX,
  SPARE6_INDEX,
  SPARE7_INDEX,
  STDSIZE_BOLD_INDEX,
  SPARE9_INDEX,
  SPAREa_INDEX,
  SPAREb_INDEX,
  SPAREc_INDEX,
  SPAREd_INDEX,
  SPAREe_INDEX,
  SPAREf_INDEX,
};

#define STDSIZE                        (STDSIZE_INDEX << 8)
#define TINSIZE                        (TINSIZE_INDEX << 8)
#define SMLSIZE                        (SMLSIZE_INDEX << 8)
#define MIDSIZE                        (MIDSIZE_INDEX << 8)
#define DBLSIZE                        (DBLSIZE_INDEX << 8)
#define XXLSIZE                        (XXLSIZE_INDEX << 8)
#define BOLD                           (STDSIZE_BOLD_INDEX << 8)
#define FONTSIZE_MASK                  0x0f00

#if !defined(BOOT)
#define FONTSIZE(flags)                ((flags) & FONTSIZE_MASK)
#define FONTINDEX(flags)               (FONTSIZE(flags) >> 8)
#else
#define FONTSIZE(flags)                STDSIZE
#define FONTINDEX(flags)               STDSIZE_INDEX
#endif

#define TIMEBLINK                      0x1000
#define TIMEHOUR                       0x2000
#define EXPANDED                       0x2000
#define VERTICAL                       0x4000
#define NO_FONTCACHE                   0x8000

#include "colors.h"

#define DISPLAY_PIXELS_COUNT           (LCD_W*LCD_H)
#define DISPLAY_BUFFER_SIZE            (sizeof(display_t)*DISPLAY_PIXELS_COUNT)

#if defined(SIMU)
extern display_t displayBuf[DISPLAY_BUFFER_SIZE];
#else
#define displayBuf                     lcd->getData()
#endif

#define DISPLAY_END                    (displayBuf + DISPLAY_BUFFER_SIZE)
#define ASSERT_IN_DISPLAY(p)           assert((p) >= displayBuf && (p) < DISPLAY_END)

extern coord_t lcdNextPos;

void lcdDrawChar(coord_t x, coord_t y, char c, LcdFlags flags=0);

void lcdDrawTextAtIndex(coord_t x, coord_t y, const pm_char * s, uint8_t idx, LcdFlags flags=0);

inline void lcdClear()
{
  lcd->clear();
}

inline void lcdDrawText(coord_t x, coord_t y, const char * s, LcdFlags attr=0)
{
  lcd->drawText(x, y, s, attr);
}

inline void lcdDrawSizedText(coord_t x, coord_t y, const pm_char * s, uint8_t len, LcdFlags attr=0)
{
  lcd->drawSizedText(x, y, s, len, attr);
}

void lcdDrawHexNumber(coord_t x, coord_t y, uint32_t val, LcdFlags mode=0);
void lcdDrawNumber(coord_t x, coord_t y, int32_t val, LcdFlags flags=0, uint8_t len=0, const char * prefix=NULL, const char * suffix=NULL);

#if !defined(BOOT)

#define putstime_t int32_t

void drawRtcTime(coord_t x, coord_t y, LcdFlags att=0);
void drawTimer(coord_t x, coord_t y, putstime_t tme, LcdFlags att=0);

void putsModelName(coord_t x, coord_t y, char *name, uint8_t id, LcdFlags att);
void putsStickName(coord_t x, coord_t y, uint8_t idx, LcdFlags att=0);
void drawSwitch(coord_t x, coord_t y, swsrc_t swtch, LcdFlags flags=0);
void drawSource(coord_t x, coord_t y, mixsrc_t idx, LcdFlags att=0);
void drawCurveName(coord_t x, coord_t y, int8_t idx, LcdFlags att=0);
void drawTimerMode(coord_t x, coord_t y, int32_t mode, LcdFlags att=0);
void drawTrimMode(coord_t x, coord_t y, uint8_t phase, uint8_t idx, LcdFlags att);

#define putsChn(x, y, idx, att) drawSource(x, y, MIXSRC_CH1+idx-1, att)
void putsChnLetter(coord_t x, coord_t y, uint8_t idx, LcdFlags attr);

#endif // !BOOT

#define SOLID   0xff
#define DOTTED  0x55
#define STASHED 0x33

#if defined(PCBX10) && !defined(SIMU)
  #define PIXEL_PTR(x, y) &displayBuf[(LCD_H*LCD_W-1) - (y)*LCD_W - (x)]
#else
  #define PIXEL_PTR(x, y) &displayBuf[(y)*LCD_W + (x)]
#endif

void lcdDrawAlphaPixel(display_t * p, uint8_t opacity, uint16_t color);
void lcdDrawPoint(coord_t x, coord_t y, LcdFlags att=0);

inline void lcdDrawSolidHorizontalLine(coord_t x, coord_t y, coord_t w, LcdFlags att)
{
  lcd->drawSolidHorizontalLine(x, y, w, att);
}

inline void lcdDrawSolidVerticalLine(coord_t x, coord_t y, coord_t h, LcdFlags att)
{
  lcd->drawSolidVerticalLine(x, y, h, att);
}

inline void lcdDrawSolidFilledRect(coord_t x, coord_t y, coord_t w, coord_t h, LcdFlags flags)
{
  lcd->drawSolidFilledRect(x, y, w, h, flags);
}

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

inline void lcdSetColor(uint16_t color)
{
  lcdColorTable[CUSTOM_COLOR_INDEX] = color;
}

inline void lcdDrawSolidRect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t thickness=1, LcdFlags att=0)
{
  lcd->drawSolidFilledRect(x, y, thickness, h, att);
  lcd->drawSolidFilledRect(x+w-thickness, y, thickness, h, att);
  lcd->drawSolidFilledRect(x, y, w, thickness, att);
  lcd->drawSolidFilledRect(x, y+h-thickness, w, thickness, att);
}

inline void lcdDrawFilledRect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t pat, LcdFlags att=0)
{
  lcd->drawFilledRect(x, y, w, h, pat, att);
}

void lcdDrawBlackOverlay();
inline void lcdDrawRect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t thickness=1, uint8_t pat=SOLID, LcdFlags att=0)
{
  lcd->drawRect(x, y, w, h, thickness, pat, att);
}

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

uint8_t getMappedChar(uint8_t c);
uint8_t getFontHeight(LcdFlags flags);
int getTextWidth(const char * s, int len=0, LcdFlags flags=0);

inline void lcdDrawBitmapPattern(coord_t x, coord_t y, const uint8_t * img, LcdFlags flags=0, coord_t offset=0, coord_t width=0)
{
  lcd->drawBitmapPattern(x, y, img, flags, offset, width);
}

#if defined(BOOT)
  #define BLINK_ON_PHASE               (0)
#else
  #define BLINK_ON_PHASE               (g_blinkTmr10ms & (1<<6))
  #define SLOW_BLINK_ON_PHASE          (g_blinkTmr10ms & (1<<7))
#endif

#endif // _LCD_H_
