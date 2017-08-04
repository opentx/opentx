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

#include <inttypes.h>

#define BOX_WIDTH                      23
#define coord_t                        uint8_t
#define scoord_t                       int8_t
#define CENTER
#define CENTER_OFS                     0

#if defined(CPUARM)
  typedef int32_t lcdint_t;
  typedef uint32_t lcduint_t;
#else
  typedef int16_t lcdint_t;
  typedef uint16_t lcduint_t;
#endif

#define FW                             6
#define FWNUM                          5
#define FH                             8

#define LCD_LINES                      (LCD_H/FH)
#define LCD_COLS                       (LCD_W/FW)

#define BITMAP_BUFFER_SIZE(w, h)       (2 + (w) * (((h)+7)/8))

/* lcd common flags */
#define BLINK                          0x01

/* lcd text flags */
#define INVERS                         0x02
#if defined(BOLD_FONT)
  #define BOLD                         0x40
#else
  #define BOLD                         0x00
#endif

/* lcd putc flags */
#define CONDENSED                      0x08
#define FIXEDWIDTH                     0x10
/* lcd puts flags */
/* no 0x80 here because of "GV"1 which is aligned LEFT */
/* no 0x10 here because of "MODEL"01 which uses LEADING0 */
#if defined(CPUARM)
  #define BSS                          0x00
#else
  #define BSS                          0x20
#endif
#define ZCHAR                          0x80

/* lcd outdez flags */
#define UNSIGN                         0x08
#define LEADING0                       0x10
#define PREC1                          0x20
#define PREC2                          0x30
#define MODE(flags)                    ((((int8_t)(flags) & 0x30) - 0x10) >> 4)
#if defined(CPUARM)
#define LEFT                           0x00 /* fake */
#define RIGHT                          0x04 /* align right */
#define IS_LEFT_ALIGNED(att)           !((att) & RIGHT)
#else
#define LEFT                           0x80 /* align left */
#define RIGHT                          0x00 /* fake */
#define IS_LEFT_ALIGNED(att)           ((att) & LEFT)
#endif
#define IS_RIGHT_ALIGNED(att)          (!IS_LEFT_ALIGNED(att))

/* line, rect, square flags */
#define FORCE                          0x02
#define ERASE                          0x04
#define ROUND                          0x08

/* telemetry flags */
#define NO_UNIT                        0x40

#if defined(CPUARM)
  #define FONTSIZE_MASK                0x0700
  #define FONTSIZE(x)                  ((x) & FONTSIZE_MASK)
  #define TINSIZE                      0x0100
  #define SMLSIZE                      0x0200
  #define MIDSIZE                      0x0300
  #define DBLSIZE                      0x0400
  #define XXLSIZE                      0x0500
  #define ERASEBG                      0x8000
  #define VERTICAL                     0x0800
#else
  #define DBLSIZE                      0x04
  #define MIDSIZE                      DBLSIZE
  #define SMLSIZE                      0x00
  #define TINSIZE                      0x00
  #define XXLSIZE                      0x00
  #define ERASEBG                      0x00
#endif

#if defined(CPUARM)
  #define TIMEBLINK                    0x1000
  #define TIMEHOUR                     0x2000
  #define STREXPANDED                  0x4000
#else
  #define STREXPANDED                  0x00
#endif

#if defined(CPUARM)
  typedef uint32_t LcdFlags;
#else
  typedef uint8_t LcdFlags;
#endif

#define display_t                      uint8_t
#define DISPLAY_BUFFER_SIZE            (LCD_W*((LCD_H+7)/8))

extern display_t displayBuf[DISPLAY_BUFFER_SIZE];

extern coord_t lcdLastRightPos;
extern coord_t lcdLastLeftPos;
extern coord_t lcdNextPos;

#define DISPLAY_END                    (displayBuf + DISPLAY_BUFFER_SIZE)
#define ASSERT_IN_DISPLAY(p)           assert((p) >= displayBuf && (p) < DISPLAY_END)

#if defined(PCBSTD) && defined(VOICE)
  extern volatile uint8_t LcdLock ;
#endif

#if defined(PCBSKY9X)
  extern volatile uint8_t lcdLock ;
  extern volatile uint32_t lcdInputs ;
#endif

#if defined(BOOT)
// TODO quick & dirty :(
typedef const unsigned char pm_uchar;
typedef const char pm_char;
#endif

void lcdDrawChar(coord_t x, coord_t y, const unsigned char c);
void lcdDrawChar(coord_t x, coord_t y, const unsigned char c, LcdFlags flags);
void lcdDrawText(coord_t x, coord_t y, const pm_char * s, LcdFlags flags);
void lcdDrawTextAtIndex(coord_t x, coord_t y, const pm_char * s, uint8_t idx, LcdFlags flags);
void lcdDrawSizedText(coord_t x, coord_t y, const pm_char * s, unsigned char len, LcdFlags flags);
void lcdDrawText(coord_t x, coord_t y, const pm_char * s);
void lcdDrawSizedText(coord_t x, coord_t y, const pm_char * s, unsigned char len);
void lcdDrawTextAlignedLeft(coord_t y, const pm_char * s);

#define lcdDrawTextAlignedCenter(y, s) lcdDrawText((LCD_W-sizeof(TR_##s)*FW+FW+1)/2, y, STR_##s)

#if defined(CPUARM)
  void lcdDrawHexNumber(coord_t x, coord_t y, uint32_t val, LcdFlags mode=0);
#else
  void lcdDrawHexNumber(coord_t x, coord_t y, uint16_t val);
#endif

void lcdDrawNumber(coord_t x, coord_t y, lcdint_t val, LcdFlags mode, uint8_t len);
void lcdDrawNumber(coord_t x, coord_t y, lcdint_t val, LcdFlags mode=0);
void lcdDraw8bitsNumber(coord_t x, coord_t y, int8_t val);

void drawStringWithIndex(coord_t x, coord_t y, const pm_char * str, uint8_t idx, LcdFlags att=0);
void putsModelName(coord_t x, coord_t y, char * name, uint8_t id, LcdFlags att);
#if !defined(BOOT) // TODO not here ...
void drawSwitch(coord_t x, coord_t y, swsrc_t swtch, LcdFlags att=0);
void drawSource(coord_t x, coord_t y, mixsrc_t idx, LcdFlags att=0);
#endif
void drawCurveName(coord_t x, coord_t y, int8_t idx, LcdFlags att=0);
void drawTimerMode(coord_t x, coord_t y, int8_t mode, LcdFlags att=0);

void drawTrimMode(coord_t x, coord_t y, uint8_t phase, uint8_t idx, LcdFlags att);
#if defined(CPUARM)
void drawShortTrimMode(coord_t x, coord_t y, uint8_t mode, uint8_t idx, LcdFlags att);
#else
#define drawShortTrimMode drawTrimMode
#endif

#if defined(ROTARY_ENCODERS)
  void putsRotaryEncoderMode(coord_t x, coord_t y, uint8_t phase, uint8_t idx, LcdFlags att);
#endif

#define putsChn(x, y, idx, att) drawSource(x, y, MIXSRC_CH1+idx-1, att)
void putsChnLetter(coord_t x, coord_t y, uint8_t idx, LcdFlags attr);

void putsVolts(coord_t x, coord_t y, uint16_t volts, LcdFlags att);
void putsVBat(coord_t x, coord_t y, LcdFlags att);

#if !defined(CPUARM)
void drawTelemetryValue(coord_t x, coord_t y, uint8_t channel, lcdint_t val, LcdFlags att=0);
#endif

#if defined(CPUARM)
  #define putstime_t int32_t
  #define FlightModesType uint16_t
#else
  #define putstime_t int16_t
  #define FlightModesType uint8_t
#endif

void drawRtcTime(coord_t x, coord_t y, LcdFlags att);
void drawTimer(coord_t x, coord_t y, putstime_t tme, LcdFlags att, LcdFlags att2);
inline void drawTimer(coord_t x, coord_t y, putstime_t tme, LcdFlags att)
{
  drawTimer(x, y, tme, att, att);
}

#define SOLID                          0xff
#define DOTTED                         0x55

void lcdDrawPoint(coord_t x, coord_t y, LcdFlags att=0);
void lcdMaskPoint(uint8_t *p, uint8_t mask, LcdFlags att=0);
void lcdDrawSolidHorizontalLine(coord_t x, coord_t y, coord_t w, LcdFlags att=0);
void lcdDrawHorizontalLine(coord_t x, coord_t y, coord_t w, uint8_t pat, LcdFlags att=0);
#if defined(CPUM64)
  void lcdDrawSolidVerticalLine(coord_t x, scoord_t y, scoord_t h);
  void lcdDrawVerticalLine(coord_t x, scoord_t y, int8_t h, uint8_t pat);
#else
  void lcdDrawSolidVerticalLine(coord_t x, scoord_t y, scoord_t h, LcdFlags att=0);
  void lcdDrawVerticalLine(coord_t x, scoord_t y, scoord_t h, uint8_t pat, LcdFlags att=0);
#endif

#if defined(CPUARM)
  void lcdDrawLine(coord_t x1, coord_t y1, coord_t x2, coord_t y2, uint8_t pat=SOLID, LcdFlags att=0);
#endif

void lcdDrawFilledRect(coord_t x, scoord_t y, coord_t w, coord_t h, uint8_t pat=SOLID, LcdFlags att=0);
inline void lcdDrawSolidFilledRect(coord_t x, scoord_t y, coord_t w, coord_t h, LcdFlags att=0)
{
  lcdDrawFilledRect(x, y, w, h, SOLID, att);
}
void lcdDrawRect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t pat=SOLID, LcdFlags att=0);

void lcdInvertLine(int8_t line);
#define lcdInvertLastLine() lcdInvertLine(LCD_LINES-1)
inline void lcdDrawSquare(coord_t x, coord_t y, coord_t w, LcdFlags att=0) { lcdDrawRect(x, y, w, w, SOLID, att); }

void drawTelemetryTopBar();

#define V_BAR(xx, yy, ll)    \
  lcdDrawSolidVerticalLine(xx-1,yy-ll,ll);  \
  lcdDrawSolidVerticalLine(xx  ,yy-ll,ll);  \
  lcdDrawSolidVerticalLine(xx+1,yy-ll,ll)

#if defined(PCBMEGA2560) && !defined(SIMU)
void lcd_imgfar(coord_t x, coord_t y, const uint_farptr_t img, uint8_t idx, LcdFlags att); // progmem "far"
#endif

void lcdClear(void);
void lcdDraw1bitBitmap(coord_t x, coord_t y, const pm_uchar * img, uint8_t idx, LcdFlags att=0);
inline void lcdDrawBitmap(coord_t x, coord_t y, const uint8_t * bitmap)
{
  lcdDraw1bitBitmap(x, y, bitmap, 0);
}

uint8_t * lcdLoadBitmap(uint8_t * dest, const char * filename, uint8_t width, uint8_t height);

#if defined(BOOT)
  #define BLINK_ON_PHASE               (0)
#else
  #define BLINK_ON_PHASE               (g_blinkTmr10ms & (1<<6))
#endif

inline display_t getPixel(uint8_t x, uint8_t y)
{
  if (x>=LCD_W || y>=LCD_H) {
    return 0;
  }

  display_t pixel = displayBuf[(y / 8) * LCD_W + x];
  display_t mask = 1 << (y & 7);
  return ((pixel & mask) ? 0xf : 0);
}

const char * writeScreenshot();

void drawShutdownAnimation(uint32_t index, const char * message);

#if defined(CPUARM)
uint8_t getTextWidth(const char * s, uint8_t len=0, LcdFlags flags=0);
#endif

#endif // _LCD_H_
