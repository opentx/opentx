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

#ifndef _LCD_H
#define _LCD_H

#define LCD_W         128
#define LCD_H         64
#define BOX_WIDTH     23
#define coord_t       uint8_t
#define scoord_t      int8_t
#define CENTER
#define CENTER_OFS    0
#define CONTRAST_MIN  10
#define CONTRAST_MAX  45

#if defined(CPUARM)
  #define lcdint_t      int32_t
  #define lcduint_t     uint32_t
#else
  #define lcdint_t      int16_t
  #define lcduint_t     uint16_t
#endif

#define FW              6
#define FWNUM           5
#define FH              8

#define LCD_LINES       (LCD_H/FH)
#define LCD_COLS        (LCD_W/FW)

/* lcd common flags */
#define BLINK           0x01

/* lcd text flags */
#define INVERS          0x02
#if defined(BOLD_FONT)
  #define BOLD          0x40
#else
  #define BOLD          0x00
#endif

/* lcd putc flags */
#define CONDENSED       0x08
#define FIXEDWIDTH      0x10
/* lcd puts flags */
/* no 0x80 here because of "GV"1 which is aligned LEFT */
/* no 0x10 here because of "MODEL"01 which uses LEADING0 */
#if defined(CPUARM)
  #define BSS           0x00
#else
  #define BSS           0x20
#endif
#define ZCHAR           0x80

/* lcd outdez flags */
#define UNSIGN          0x08
#define LEADING0        0x10
#define PREC1           0x20
#define PREC2           0x30
#define MODE(flags)     ((((int8_t)(flags) & 0x30) - 0x10) >> 4)
#define LEFT            0x80 /* align left */

/* line, rect, square flags */
#define FORCE           0x02
#define ERASE           0x04
#define ROUND           0x08

/* telemetry flags */
#define NO_UNIT         0x40

#if defined(CPUARM)
  #define FONTSIZE(x)   ((x) & 0x0700)
  #define TINSIZE       0x0100
  #define SMLSIZE       0x0200
  #define MIDSIZE       0x0300
  #define DBLSIZE       0x0400
  #define XXLSIZE       0x0500
  #define ERASEBG       0x8000
  #define VERTICAL      0x0800
#else
  #define DBLSIZE       0x04
  #define MIDSIZE       DBLSIZE
  #define SMLSIZE       0x00
  #define TINSIZE       0x00
  #define XXLSIZE       0x00
  #define ERASEBG       0x00
#endif

#if defined(CPUARM)
  #define TIMEBLINK     0x1000
  #define TIMEHOUR      0x2000
  #define STREXPANDED   0x4000
#else
  #define STREXPANDED   0x00
#endif

#if defined(CPUARM)
  #define LcdFlags             uint32_t
#else
  #define LcdFlags             uint8_t
#endif

#define display_t            uint8_t
#define DISPLAY_BUF_SIZE     (LCD_W*((LCD_H+7)/8))

extern display_t displayBuf[DISPLAY_BUF_SIZE];

#define lcdRefreshWait()

extern coord_t lcdLastPos;
extern coord_t lcdNextPos;

#define DISPLAY_BUFFER_SIZE     (sizeof(display_t)*DISPLAY_BUF_SIZE)
#define DISPLAY_END            (displayBuf + DISPLAY_BUF_SIZE)
#define ASSERT_IN_DISPLAY(p)   assert((p) >= displayBuf && (p) < DISPLAY_END)

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

void lcd_putc(coord_t x, coord_t y, const unsigned char c);
void lcd_putcAtt(coord_t x, coord_t y, const unsigned char c, LcdFlags mode);
void lcd_putsAtt(coord_t x, coord_t y, const pm_char * s, LcdFlags mode);
void lcd_putsiAtt(coord_t x, coord_t y, const pm_char * s,uint8_t idx, LcdFlags mode);
void lcd_putsnAtt(coord_t x, coord_t y, const pm_char * s,unsigned char len, LcdFlags mode);
void lcd_puts(coord_t x, coord_t y, const pm_char * s);
void lcd_putsn(coord_t x, coord_t y, const pm_char * s, unsigned char len);
void lcd_putsLeft(coord_t y, const pm_char * s);

#if defined(COLORLCD)
  void lcd_putsCenter(coord_t y, const pm_char * s, LcdFlags attr=0);
#else
  #define lcd_putsCenter(y, s) lcd_puts((LCD_W-sizeof(TR_##s)*FW+FW+1)/2, y, STR_##s)
#endif

#if defined(CPUARM)
  void lcd_outhex4(coord_t x, coord_t y, uint32_t val, LcdFlags mode=0);
#else
  void lcd_outhex4(coord_t x, coord_t y, uint16_t val);
#endif

void lcd_outdezNAtt(coord_t x, coord_t y, lcdint_t val, LcdFlags mode=0, uint8_t len=0);
void lcd_outdezAtt(coord_t x, coord_t y, lcdint_t val, LcdFlags mode=0);
void lcd_outdez8(coord_t x, coord_t y, int8_t val);

void putsStrIdx(coord_t x, coord_t y, const pm_char *str, uint8_t idx, LcdFlags att=0);
void putsModelName(coord_t x, coord_t y, char *name, uint8_t id, LcdFlags att);
void putsSwitches(coord_t x, coord_t y, int8_t swtch, LcdFlags att=0);
void putsMixerSource(coord_t x, coord_t y, uint8_t idx, LcdFlags att=0);
void putsFlightMode(coord_t x, coord_t y, int8_t idx, LcdFlags att=0);
void putsCurve(coord_t x, coord_t y, int8_t idx, LcdFlags att=0);
void putsTimerMode(coord_t x, coord_t y, int8_t mode, LcdFlags att=0);
void putsTrimMode(coord_t x, coord_t y, uint8_t phase, uint8_t idx, LcdFlags att);
#if defined(ROTARY_ENCODERS)
  void putsRotaryEncoderMode(coord_t x, coord_t y, uint8_t phase, uint8_t idx, LcdFlags att);
#endif

#define putsChn(x, y, idx, att) putsMixerSource(x, y, MIXSRC_CH1+idx-1, att)
void putsChnLetter(coord_t x, coord_t y, uint8_t idx, LcdFlags attr);

void putsVolts(coord_t x, coord_t y, uint16_t volts, LcdFlags att);
void putsVBat(coord_t x, coord_t y, LcdFlags att);

#if !defined(BOOT)
void putsChannelValue(coord_t x, coord_t y, source_t channel, lcdint_t val, LcdFlags att=0);
void putsChannel(coord_t x, coord_t y, source_t channel, LcdFlags att=0);
void putsTelemetryChannelValue(coord_t x, coord_t y, uint8_t channel, lcdint_t val, LcdFlags att=0);
#endif

#if defined(CPUARM)
  #define putstime_t int32_t
#else
  #define putstime_t int16_t
#endif

void putsRtcTime(coord_t x, coord_t y, LcdFlags att);
void putsTimer(coord_t x, coord_t y, putstime_t tme, LcdFlags att, LcdFlags att2);

#define SOLID  0xff
#define DOTTED 0x55

void lcd_plot(coord_t x, coord_t y, LcdFlags att=0);
void lcd_mask(uint8_t *p, uint8_t mask, LcdFlags att=0);
void lcd_hline(coord_t x, coord_t y, coord_t w, LcdFlags att=0);
void lcd_hlineStip(coord_t x, coord_t y, coord_t w, uint8_t pat, LcdFlags att=0);
void lcd_vline(coord_t x, scoord_t y, scoord_t h);
#if defined(CPUM64)
  void lcd_vlineStip(coord_t x, scoord_t y, int8_t h, uint8_t pat);
#else
  void lcd_vlineStip(coord_t x, scoord_t y, scoord_t h, uint8_t pat, LcdFlags att=0);
#endif

#if defined(CPUARM)
  void lcd_line(coord_t x1, coord_t y1, coord_t x2, coord_t y2, uint8_t pat=SOLID, LcdFlags att=0);
#endif

void drawFilledRect(coord_t x, scoord_t y, coord_t w, coord_t h, uint8_t pat=SOLID, LcdFlags att=0);
void lcd_rect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t pat=SOLID, LcdFlags att=0);

void lcd_invert_line(int8_t line);
#define lcd_status_line() lcd_invert_line(LCD_LINES-1)
inline void lcd_square(coord_t x, coord_t y, coord_t w, LcdFlags att=0) { lcd_rect(x, y, w, w, SOLID, att); }

void lcdDrawTelemetryTopBar();

#define V_BAR(xx, yy, ll)    \
  lcd_vline(xx-1,yy-ll,ll);  \
  lcd_vline(xx  ,yy-ll,ll);  \
  lcd_vline(xx+1,yy-ll,ll)

void lcd_img(coord_t x, coord_t y, const pm_uchar * img, uint8_t idx, LcdFlags att=0);

void lcdSetRefVolt(unsigned char val);
void lcd_clear();
void lcdSetContrast();
void lcdInit();
#define lcdOff()

void lcdRefresh();

#if defined(BOOT)
  #define BLINK_ON_PHASE (0)
#else
  #define BLINK_ON_PHASE (g_blinkTmr10ms & (1<<6))
#endif

#if defined(SIMU)
  extern bool lcd_refresh;
  extern display_t lcd_buf[DISPLAY_BUF_SIZE];
#endif

char *strAppend(char * dest, const char * source, int len=0);
char *strSetCursor(char *dest, int position);
char *strAppendDate(char * str, bool time=false);
char *strAppendFilename(char * dest, const char * filename, const int size);

#endif
