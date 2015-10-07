/*
 * Authors (alphabetical order)
 * - Andre Bernet <bernet.andre@gmail.com>
 * - Andreas Weitl
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Gabriel Birkus
 * - Jean-Pierre Parisy
 * - Karl Szmutny
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * opentx is based on code named
 * gruvin9x by Bryan J. Rentoul: http://code.google.com/p/gruvin9x/,
 * er9x by Erez Raviv: http://code.google.com/p/er9x/,
 * and the original (and ongoing) project by
 * Thomas Husterer, th9x: http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef _LCD_H_
#define _LCD_H_

#define LCD_W           480
#define LCD_H           272

#define BOX_WIDTH       61
#define coord_t         int
#define scoord_t        int
#define CENTER
#define CENTER_OFS      0
#define CONTRAST_MIN    0
#define CONTRAST_MAX    45

#define lcdint_t        int32_t
#define lcduint_t       uint32_t

#define LCD_LINES       17 // TODO change this constant which has no sense here, the header uses more than one line
#define LCD_COLS        30

#define BSS             0x00
#define ERASE           0x00
#define FORCE           0x00
#define FIXEDWIDTH      0x00
#define FILLWHITE      0x00
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
  HEADER_COLOR_INDEX,
  ALARM_COLOR_INDEX,
  WARNING_COLOR_INDEX,
  TEXT_DISABLE_COLOR_INDEX,
  CURVE_AXIS_COLOR_INDEX,
  HEADER_BGCOLOR_INDEX,
  TITLE_BGCOLOR_INDEX,
  LCD_COLOR_COUNT
};

extern uint16_t lcdColorTable[LCD_COLOR_COUNT];
void lcdColorsInit();

#define OPACITY_MAX 0x0F

#define COLOR(index)            ((index) << 16)
#define TEXT_COLOR              COLOR(TEXT_COLOR_INDEX)
#define TEXT_BGCOLOR            COLOR(TEXT_BGCOLOR_INDEX)
#define TEXT_INVERTED_COLOR     COLOR(TEXT_INVERTED_COLOR_INDEX)
#define TEXT_INVERTED_BGCOLOR   COLOR(TEXT_INVERTED_BGCOLOR_INDEX)
#define LINE_COLOR              COLOR(LINE_COLOR_INDEX)
#define SCROLLBOX_COLOR         COLOR(SCROLLBOX_COLOR_INDEX)
#define HEADER_SEPARATOR_COLOR  COLOR(HEADER_SEPARATOR_COLOR_INDEX)
#define MENU_TITLE_BGCOLOR      COLOR(MENU_TITLE_BGCOLOR_INDEX)
#define MENU_TITLE_COLOR        COLOR(MENU_TITLE_COLOR_INDEX)
#define HEADER_COLOR            COLOR(HEADER_COLOR_INDEX)
#define ALARM_COLOR             COLOR(ALARM_COLOR_INDEX)
#define WARNING_COLOR           COLOR(WARNING_COLOR_INDEX)
#define TEXT_DISABLE_COLOR      COLOR(TEXT_DISABLE_COLOR_INDEX)
#define CURVE_AXIS_COLOR        COLOR(CURVE_AXIS_COLOR_INDEX)
#define TITLE_BGCOLOR           COLOR(TITLE_BGCOLOR_INDEX)
#define HEADER_BGCOLOR          COLOR(HEADER_BGCOLOR_INDEX)

#define COLOR_IDX(att) uint8_t((att) >> 16)

#define COLOR_SPLIT(color, r, g, b) \
  uint16_t r = ((color) & 0xF800) >> 11; \
  uint16_t g = ((color) & 0x07E0) >> 5; \
  uint16_t b = ((color) & 0x001F)

#define COLOR_JOIN(r, g, b) \
  (((r) << 11) + ((g) << 5) + (b))

#define display_t            uint16_t
#define DISPLAY_BUF_SIZE     (LCD_W*LCD_H)

#if defined(SIMU)
extern display_t displayBuf[DISPLAY_BUF_SIZE];
#else
extern uint32_t CurrentFrameBuffer;
#define displayBuf         ((uint16_t *)CurrentFrameBuffer)
#endif

#define lcdRefreshWait()

#define DISPLAY_BUFFER_SIZE     (sizeof(display_t)*DISPLAY_BUF_SIZE)
#define DISPLAY_END            (displayBuf + DISPLAY_BUF_SIZE)
#define ASSERT_IN_DISPLAY(p)   assert((p) >= displayBuf && (p) < DISPLAY_END)

void lcdStartScreen();
void lcd_putc(coord_t x, coord_t y, const unsigned char c);
void lcd_putcAtt(coord_t x, coord_t y, const unsigned char c, LcdFlags attr=TEXT_COLOR);
void lcd_putsAtt(coord_t x, coord_t y, const pm_char * s, LcdFlags attr=TEXT_COLOR);
void lcd_putsiAtt(coord_t x, coord_t y, const pm_char * s, uint8_t idx, LcdFlags attr=TEXT_COLOR);
void lcd_putsnAtt(coord_t x, coord_t y, const pm_char * s, unsigned char len, LcdFlags attr=TEXT_COLOR);
void lcd_puts(coord_t x, coord_t y, const pm_char * s);
void lcd_putsn(coord_t x, coord_t y, const pm_char * s, unsigned char len);
void lcd_putsLeft(coord_t y, const pm_char * s);
void lcd_putsCenter(coord_t y, const pm_char * s, LcdFlags attr=0);
void lcd_outhex4(coord_t x, coord_t y, uint32_t val, LcdFlags mode=0);
void lcd_outdezNAtt(coord_t x, coord_t y, lcdint_t val, LcdFlags flags, int len, const char *suffix=NULL, const char *prefix=NULL);
void lcd_outdezAtt(coord_t x, coord_t y, lcdint_t val, LcdFlags mode=0, const char *suffix=NULL, const char *prefix=NULL);
void lcd_outdez8(coord_t x, coord_t y, int8_t val);

void putsStrIdx(coord_t x, coord_t y, const pm_char *str, int idx, LcdFlags att=0, const char *prefix="");
void putsModelName(coord_t x, coord_t y, char *name, uint8_t id, LcdFlags att);
void putsSwitches(coord_t x, coord_t y, int8_t swtch, LcdFlags att=0);
void putsMixerSource(coord_t x, coord_t y, uint8_t idx, LcdFlags att=0);
void putsFlightMode(coord_t x, coord_t y, int8_t idx, LcdFlags att=0);
void putsCurveRef(coord_t x, coord_t y, CurveRef &curve, LcdFlags att=0);
void putsCurve(coord_t x, coord_t y, int8_t idx, LcdFlags att=0);
void putsTimerMode(coord_t x, coord_t y, int8_t mode, LcdFlags att=0);
void putsTrimMode(coord_t x, coord_t y, uint8_t phase, uint8_t idx, LcdFlags att);

#define putsChn(x, y, idx, att) putsMixerSource(x, y, MIXSRC_CH1+idx-1, att)
void putsChnLetter(coord_t x, coord_t y, uint8_t idx, LcdFlags attr);

void putsVolts(coord_t x, coord_t y, uint16_t volts, LcdFlags att);
void putsVBat(coord_t x, coord_t y, LcdFlags att);

void putsChannelValue(coord_t x, coord_t y, source_t channel, lcdint_t val, LcdFlags att=0);
void putsChannel(coord_t x, coord_t y, source_t channel, LcdFlags att=0);
void putsTelemetryChannelValue(coord_t x, coord_t y, uint8_t channel, lcdint_t val, LcdFlags att=0);

#define putstime_t int32_t

void putsRtcTime(coord_t x, coord_t y, LcdFlags att=0);
void putsTimer(coord_t x, coord_t y, putstime_t tme, LcdFlags att=0);

#define SOLID  0xff
#define DOTTED 0x55

void lcdDrawTransparentPixel(coord_t x, coord_t y, uint8_t opacity, uint16_t color);
void lcdDrawTransparentPixel(display_t * p, uint8_t opacity, uint16_t color);
void lcdDrawPoint(coord_t x, coord_t y, LcdFlags att=0);
void lcd_mask(uint8_t *p, uint8_t mask, LcdFlags att=0);
void lcd_hline(coord_t x, coord_t y, coord_t w, LcdFlags att=0);
void lcd_hlineStip(coord_t x, coord_t y, coord_t w, uint8_t pat, LcdFlags att=0);
void lcd_vline(coord_t x, scoord_t y, scoord_t h);
void lcd_vlineStip(coord_t x, scoord_t y, scoord_t h, uint8_t pat, LcdFlags att=0);
void lcd_line(coord_t x1, coord_t y1, coord_t x2, coord_t y2, uint8_t pat=SOLID, LcdFlags att=0);

#if !defined(SIMU)
inline void lcdDrawFilledRect(coord_t x, scoord_t y, coord_t w, coord_t h, LcdFlags att)
{
  lcdFillRectangle(x, y, w, h, lcdColorTable[COLOR_IDX(att)]);
}
#else
void lcdDrawFilledRect(coord_t x, scoord_t y, coord_t w, coord_t h, LcdFlags att);
#endif

inline void lcdDrawHorizontalLine(coord_t x, scoord_t y, coord_t w, LcdFlags att)
{
  lcdDrawFilledRect(x, y, w, 1, att);
}

inline void lcdDrawVerticalLine(coord_t x, scoord_t y, coord_t h, LcdFlags att)
{
  lcdDrawFilledRect(x, y, 1, h, att);
}

inline void lcdDrawRect(coord_t x, scoord_t y, coord_t w, coord_t h, LcdFlags att)
{
  lcdDrawVerticalLine(x, y, h, att);
  lcdDrawVerticalLine(x+w-1, y, h, att);
  lcdDrawHorizontalLine(x, y, w, att);
  lcdDrawHorizontalLine(x, y+h-1, w, att);
}

void lcdDrawFilledRectWithAttributes(coord_t x, scoord_t y, coord_t w, coord_t h, uint8_t pat, LcdFlags att);
void drawBlackOverlay();
void lcd_rect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t pat=SOLID, LcdFlags att=0);

inline void lcd_square(coord_t x, coord_t y, coord_t w, LcdFlags att=0) { lcd_rect(x, y, w, w, SOLID, att); }

void lcdDrawTelemetryTopBar();

#define V_BAR(xx, yy, ll)    \
  lcd_vline(xx-1,yy-ll,ll);  \
  lcd_vline(xx  ,yy-ll,ll);  \
  lcd_vline(xx+1,yy-ll,ll);

void lcd_img(coord_t x, coord_t y, const pm_uchar * img, uint8_t idx, LcdFlags att=0);

int getTextWidth(const pm_char *s, int len=0, LcdFlags flags=0);
void lcdDrawBitmap(coord_t x, coord_t y, const uint16_t * img, coord_t offset=0, coord_t width=0);
void lcdDrawBitmapPattern(coord_t x, coord_t y, const uint8_t * img, LcdFlags flags=0, coord_t offset=0, coord_t width=0);

#define lcdSetRefVolt(...)
void lcdSetContrast();
void lcdInit();
#define lcdOff(...)

void lcdRefresh();
void lcdSendByte(uint8_t data);
void lcdSend(const uint8_t * data, uint32_t size);
void lcdSendString(const char * s);
uint32_t lcdReceive(uint8_t * data, uint32_t len);
void lcdClearRxBuffer();

const pm_char * bmpLoad(uint8_t *dest, const char *filename, const unsigned int width, const unsigned int height);

#if defined(BOOT)
  #define BLINK_ON_PHASE (0)
#else
  #define BLINK_ON_PHASE (g_blinkTmr10ms & (1<<6))
#endif

#ifdef SIMU
  extern bool lcd_refresh;
  extern display_t lcd_buf[DISPLAY_BUF_SIZE];
#endif

char *strAppend(char * dest, const char * source, int len=0);
char *strSetCursor(char *dest, int position);
char *strAppendDate(char * str, bool time=false);
char *strAppendFilename(char * dest, const char * filename, const int size);

#define BITMAP_IDX_SPLASH              0x00
#define BITMAP_IDX_ALERT               0x01
#define BITMAP_IDX_WARNING             0x02
#define BITMAP_IDX_MESSAGE             0x03
#define BITMAP_IDX_USB                 0x04
#define BITMAP_IDX_STICKS_FIRST        0x05 // 4 bitmaps
#define BITMAP_IDX_BATTERY_FIRST       0x09 // 5 bitmaps
#define BITMAP_IDX_RSSI_FIRST          0x0E // 6 bitmaps
#define BITMAP_IDX_CHECKBOX_FIRST      0x14 // 4 bitmaps
#define BITMAP_IDX_MODEL_FIRST         0x80

#endif // _LCD_H_
