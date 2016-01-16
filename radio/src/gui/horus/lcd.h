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

#define LCD_W           480
#define LCD_H           272

#define BOX_WIDTH       81
#define coord_t         int
#define scoord_t        int
#define CENTER
#define CENTER_OFS      0
#define CONTRAST_MIN    0
#define CONTRAST_MAX    45

#define lcdint_t        int32_t

#define LCD_COLS        30

#define BSS             0x00
#define FIXEDWIDTH      0x00
#define BOLD            0x00

/* lcd common flags */
#define BLINK           0x01

/* lcd text flags */
#define INVERS          0x02
#define LEFT            0x08 /* align left */

/* lcd puts flags */
#define ZCHAR           0x04

/* lcd outdez flags */
#define LEADING0        0x10
#define PREC1           0x20
#define PREC2           0x30
#define MODE(flags)     ((((int8_t)(flags) & 0x30) - 0x10) >> 4)

/* rect, square flags */
#define ROUND           0x02

/* telemetry flags */
#define NO_UNIT         0x40

#define FONTSIZE(x)     ((x) & 0x0700)
#define TINSIZE         0x0100
#define SMLSIZE         0x0200
#define MIDSIZE         0x0300
#define DBLSIZE         0x0400
#define XXLSIZE         0x0500
#define ERASEBG         0x8000
#define VERTICAL        0x0800

#define TIMEBLINK       0x1000
#define TIMEHOUR        0x2000
#define STREXPANDED     0x4000

// remove windows default definitions
#undef OPAQUE
#undef RGB

#define TO5BITS(x)      ((x) >> 3)
#define TO6BITS(x)      ((x) >> 2)
#define RGB(r, g, b)   ((TO5BITS(r) << 11) + (TO6BITS(g) << 5) + (TO5BITS(b) << 0))
#define WHITE          RGB(0xF0, 0xF0, 0xF0)
#define BLACK          RGB(0, 0, 0)
#define YELLOW         RGB(0xF0, 0xD0, 0x10)
#define BLUE           RGB(0x30, 0xA0, 0xE0)
#define DARKGREY       RGB(0x40, 0x40, 0x40)
#define RED            RGB(229, 32, 30)
#define DARKRED        RGB(160, 0, 6)

#define LcdFlags        uint32_t

enum LcdColorIndex
{
  TEXT_COLOR_INDEX,
  TEXT_BGCOLOR_INDEX,
  TEXT_INVERTED_COLOR_INDEX,
  TEXT_INVERTED_BGCOLOR_INDEX,
  LINE_COLOR_INDEX,
  SCROLLBOX_COLOR_INDEX,
  MENU_TITLE_BGCOLOR_INDEX,
  MENU_TITLE_COLOR_INDEX,
  MENU_TITLE_DISABLE_COLOR_INDEX,
  HEADER_COLOR_INDEX,
  ALARM_COLOR_INDEX,
  WARNING_COLOR_INDEX,
  TEXT_DISABLE_COLOR_INDEX,
  CURVE_AXIS_COLOR_INDEX,
  CURVE_COLOR_INDEX,
  CURVE_CURSOR_COLOR_INDEX,
  HEADER_BGCOLOR_INDEX,
  TITLE_BGCOLOR_INDEX,
  LCD_COLOR_COUNT
};

extern uint16_t lcdColorTable[LCD_COLOR_COUNT];
void lcdColorsInit();

#define OPACITY_MAX                    0x0F
#define OPACITY(x)                     ((x)<<24)

#define COLOR(index)                   ((index) << 16)
#define TEXT_COLOR                     COLOR(TEXT_COLOR_INDEX)
#define TEXT_BGCOLOR                   COLOR(TEXT_BGCOLOR_INDEX)
#define TEXT_INVERTED_COLOR            COLOR(TEXT_INVERTED_COLOR_INDEX)
#define TEXT_INVERTED_BGCOLOR          COLOR(TEXT_INVERTED_BGCOLOR_INDEX)
#define LINE_COLOR                     COLOR(LINE_COLOR_INDEX)
#define SCROLLBOX_COLOR                COLOR(SCROLLBOX_COLOR_INDEX)
#define HEADER_SEPARATOR_COLOR         COLOR(HEADER_SEPARATOR_COLOR_INDEX)
#define MENU_TITLE_BGCOLOR             COLOR(MENU_TITLE_BGCOLOR_INDEX)
#define MENU_TITLE_COLOR               COLOR(MENU_TITLE_COLOR_INDEX)
#define MENU_TITLE_DISABLE_COLOR       COLOR(MENU_TITLE_DISABLE_COLOR_INDEX)
#define HEADER_COLOR                   COLOR(HEADER_COLOR_INDEX)
#define ALARM_COLOR                    COLOR(ALARM_COLOR_INDEX)
#define WARNING_COLOR                  COLOR(WARNING_COLOR_INDEX)
#define TEXT_DISABLE_COLOR             COLOR(TEXT_DISABLE_COLOR_INDEX)
#define CURVE_AXIS_COLOR               COLOR(CURVE_AXIS_COLOR_INDEX)
#define CURVE_COLOR                    COLOR(CURVE_COLOR_INDEX)
#define CURVE_CURSOR_COLOR             COLOR(CURVE_CURSOR_COLOR_INDEX)
#define TITLE_BGCOLOR                  COLOR(TITLE_BGCOLOR_INDEX)
#define HEADER_BGCOLOR                 COLOR(HEADER_BGCOLOR_INDEX)

#define COLOR_IDX(att)                 uint8_t((att) >> 16)

#define COLOR_SPLIT(color, r, g, b) \
  uint16_t r = ((color) & 0xF800) >> 11; \
  uint16_t g = ((color) & 0x07E0) >> 5; \
  uint16_t b = ((color) & 0x001F)

#define COLOR_JOIN(r, g, b) \
  (((r) << 11) + ((g) << 5) + (b))

#define display_t                      uint16_t
#define DISPLAY_PIXELS_COUNT           (LCD_W*LCD_H)
#define DISPLAY_BUFFER_SIZE            (sizeof(display_t)*DISPLAY_PIXELS_COUNT)

#if defined(SIMU)
extern display_t displayBuf[DISPLAY_BUFFER_SIZE];
#else
extern uint32_t CurrentFrameBuffer;
#define displayBuf                     ((uint16_t *)CurrentFrameBuffer)
#endif

#define lcdRefreshWait()

#define DISPLAY_END                    (displayBuf + DISPLAY_BUFFER_SIZE)
#define ASSERT_IN_DISPLAY(p)           assert((p) >= displayBuf && (p) < DISPLAY_END)

extern coord_t lcdNextPos;

void lcdDrawChar(coord_t x, coord_t y, const unsigned char c, LcdFlags attr=TEXT_COLOR);
void lcdDrawText(coord_t x, coord_t y, const pm_char * s, LcdFlags attr=TEXT_COLOR);
void lcdDrawTextAtIndex(coord_t x, coord_t y, const pm_char * s, uint8_t idx, LcdFlags attr=TEXT_COLOR);
void lcdDrawSizedText(coord_t x, coord_t y, const pm_char * s, uint8_t len, LcdFlags attr=TEXT_COLOR);
void lcdDrawSizedText(coord_t x, coord_t y, const pm_char * s, unsigned char len);
void lcd_putsCenter(coord_t y, const pm_char * s, LcdFlags attr=0);
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

void putsVolts(coord_t x, coord_t y, uint16_t volts, LcdFlags att);
void putsVBat(coord_t x, coord_t y, LcdFlags att);

void putsChannelValue(coord_t x, coord_t y, source_t channel, int32_t val, LcdFlags att=0);
void putsChannel(coord_t x, coord_t y, source_t channel, LcdFlags att=0);
void putsTelemetryChannelValue(coord_t x, coord_t y, uint8_t channel, int32_t val, LcdFlags att=0);

#define putstime_t int32_t

void getTimerString(char * str, putstime_t tme, LcdFlags att=0);
void putsRtcTime(coord_t x, coord_t y, LcdFlags att=0);
void putsTimer(coord_t x, coord_t y, putstime_t tme, LcdFlags att=0);

#define SOLID   0xff
#define DOTTED  0x55
#define STASHED 0x70

#define PIXEL_PTR(x, y) &displayBuf[(y)*LCD_W + (x)]

inline void lcdDrawPixel(display_t * p, display_t value)
{
  *p = value;
}

inline void lcdDrawPixel(coord_t x, coord_t y, display_t value)
{
  display_t * p = PIXEL_PTR(x, y);
  lcdDrawPixel(p, value);
}

void lcdDrawAlphaPixel(display_t * p, uint8_t opacity, uint16_t color);
void lcdDrawPoint(coord_t x, coord_t y, LcdFlags att=0);
void lcdDrawHorizontalLine(coord_t x, coord_t y, coord_t w, uint8_t pat, LcdFlags att=0);
void lcdDrawVerticalLine(coord_t x, scoord_t y, scoord_t h, uint8_t pat, LcdFlags att=0);
void lcdDrawLine(coord_t x1, coord_t y1, coord_t x2, coord_t y2, uint8_t pat=SOLID, LcdFlags att=0);

inline void lcdDrawAlphaPixel(coord_t x, coord_t y, uint8_t opacity, uint16_t color)
{
  display_t * p = PIXEL_PTR(x, y);
  lcdDrawAlphaPixel(p, opacity, color);
}

#if !defined(SIMU)
inline void lcdDrawSolidFilledRect(coord_t x, scoord_t y, coord_t w, coord_t h, LcdFlags att)
{
  lcdDrawSolidFilledRectDMA(x, y, w, h, lcdColorTable[COLOR_IDX(att)]);
}
#else
void lcdDrawSolidFilledRect(coord_t x, scoord_t y, coord_t w, coord_t h, LcdFlags att);
#endif

inline void lcdClear()
{
  lcdDrawSolidFilledRect(0, 0, LCD_W, LCD_H, 0);
}

inline void lcdDrawSolidHorizontalLine(coord_t x, scoord_t y, coord_t w, LcdFlags att)
{
  lcdDrawSolidFilledRect(x, y, w, 1, att);
}

inline void lcdDrawSolidVerticalLine(coord_t x, scoord_t y, coord_t h, LcdFlags att)
{
  if (h<0) { y+=h; h=-h; }
  lcdDrawSolidFilledRect(x, y, 1, h, att);
}

inline void lcdDrawSolidRect(coord_t x, scoord_t y, coord_t w, coord_t h, LcdFlags att)
{
  lcdDrawSolidVerticalLine(x, y, h, att);
  lcdDrawSolidVerticalLine(x+w-1, y, h, att);
  lcdDrawSolidHorizontalLine(x, y, w, att);
  lcdDrawSolidHorizontalLine(x, y+h-1, w, att);
}

void lcdDrawFilledRect(coord_t x, scoord_t y, coord_t w, coord_t h, uint8_t pat, LcdFlags att);
void lcdDrawBlackOverlay();
void lcdDrawRect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t pat=SOLID, LcdFlags att=0);
void lcdDrawCircle(int x0, int y0, int radius);
void lcdDrawPie(int x0, int y0, int radius, int angle1=0, int angle2=360);
void lcdDrawBitmapPie(int x0, int y0, const uint16_t * img, int startAngle, int endAngle);
void lcdDrawBitmapPatternPie(coord_t x0, coord_t y0, const uint8_t * img, LcdFlags flags=0, int startAngle=0, int endAngle=360);

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

inline int getBitmapScaledSize(int size, int scale)
{
  if (scale >= -1 && scale <= 1)
    return size;
  else if (scale < 0)
    return -(size / scale);
  else
    return size * scale;
}

int getBitmapScale(const uint8_t * bmp, int dstWidth, int dstHeight);
int getTextWidth(const pm_char *s, int len=0, LcdFlags flags=0);
void lcdDrawBitmap(coord_t x, coord_t y, const uint8_t * img, coord_t offset=0, coord_t height=0, int scale=0);
void lcdDrawBitmapPattern(coord_t x, coord_t y, const uint8_t * img, LcdFlags flags=0, coord_t offset=0, coord_t width=0);
void lcdDrawAlphaBitmap(coord_t x, coord_t y, const uint8_t * bmp);

#define lcdSetRefVolt(...)
void lcdSetContrast();
#define lcdOff(...)

const char * bmpLoad(uint8_t * dest, const char * filename, uint16_t width, uint16_t height);
const char * imgLoad(uint8_t * dest, const char * filename, uint16_t width, uint16_t height);

void drawSleepBitmap();
void drawShutdownBitmap(uint32_t index);

#if defined(BOOT)
  #define BLINK_ON_PHASE               (0)
#else
  #define BLINK_ON_PHASE               (g_blinkTmr10ms & (1<<6))
#endif

#endif // _LCD_H_
