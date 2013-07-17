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

#ifndef lcd_h
#define lcd_h

#if defined(PCBTARANIS)
  #define LCD_W         212
  #define LCD_H         64
  #define xcoord_t      uint16_t
  #define CENTER        "\015"
  #define CENTER_OFS    (7*FW-FW/2)
#else
  #define LCD_W         128
  #define LCD_H         64
  #define xcoord_t      uint8_t
  #define CENTER
  #define CENTER_OFS    0
#endif

#if defined(CPUARM)
  #define lcdint_t      int32_t
#else
  #define lcdint_t      int16_t
#endif

#if LCD_H > 64
  #define LCD_LINES     9
#else
  #define LCD_LINES     8
#endif

#define FW              6
#define FWNUM           5
#define FH              8

/* lcd common flags */
#define BLINK           0x01

/* lcd text flags */
#define INVERS          0x02
#define DBLSIZE         0x04
#ifdef BOLD_FONT
#define BOLD            0x40
#else
#define BOLD            0x00
#endif

/* lcd putc flags */
#define CONDENSED       0x08

/* lcd puts flags */
/* no 0x80 here because of "GV"1 which is aligned LEFT */
/* no 0x10 here because of "MODEL"01 which uses LEADING0 */
#define BSS             0x20
#define STRCONDENSED    0x40 /* means that THRm will be displayed as THR */
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
#if defined(PCBTARANIS)
  #define FILL_WHITE    0x10
#endif

/* telemetry flags */
#define NO_UNIT         0x40

#if defined(CPUARM)
  #define MIDSIZE       0x0100
  #define SMLSIZE       0x0200
  #define TINSIZE       0x0400
  #define STREXPANDED   0x0800
#else
  #define MIDSIZE       DBLSIZE
  #define SMLSIZE       0x00
  #define TINSIZE       0x00
  #define STREXPANDED   0x00
#endif

#if defined(PCBTARANIS)
#define GREY(x)         ((x) * 0x1000)
#define GREY_DEFAULT    GREY(11)
#define GREY_MASK(x)    ((x) & 0xF000)
#endif

#if defined(CPUARM)
  #define LcdFlags      uint32_t
#else
  #define LcdFlags      uint8_t
#endif

#define DISPLAY_PLAN_SIZE  (LCD_W*((LCD_H+7)/8))

#if defined(PCBTARANIS)
#define DISPLAY_BUF_SIZE   (4*DISPLAY_PLAN_SIZE)
#else
#define DISPLAY_BUF_SIZE   DISPLAY_PLAN_SIZE
#endif

extern uint8_t displayBuf[DISPLAY_BUF_SIZE];
extern uint8_t lcdLastPos;

#if defined(PCBSTD) && defined(VOICE)
  extern volatile uint8_t LcdLock ;
#endif

#if defined(PCBSKY9X)
  extern volatile uint8_t lcdLock ;
  extern volatile uint32_t lcdInputs ;
#endif

void lcd_putc(xcoord_t x, uint8_t y, const unsigned char c);
void lcd_putcAtt(xcoord_t x, uint8_t y, const unsigned char c, LcdFlags mode);
void lcd_putsAtt(xcoord_t x, uint8_t y, const pm_char * s, LcdFlags mode);
void lcd_putsiAtt(xcoord_t x, uint8_t y, const pm_char * s,uint8_t idx, LcdFlags mode);
void lcd_putsnAtt(xcoord_t x, uint8_t y, const pm_char * s,unsigned char len, LcdFlags mode);
void lcd_puts(xcoord_t x, uint8_t y, const pm_char * s);
void lcd_putsn(xcoord_t x, uint8_t y, const pm_char * s, unsigned char len);
void lcd_putsLeft(uint8_t y, const pm_char * s);
#define lcd_putsCenter(y, s) lcd_puts((LCD_W-sizeof(TR_##s)*FW+FW-2)/2, y, STR_##s)

void lcd_outhex4(xcoord_t x, uint8_t y, uint16_t val);
void lcd_outdezAtt(xcoord_t x, uint8_t y, lcdint_t val, LcdFlags mode=0);
void lcd_outdezNAtt(xcoord_t x, uint8_t y, lcdint_t val, LcdFlags mode=0, uint8_t len=0);
void lcd_outdez8(xcoord_t x, uint8_t y, int8_t val);

void putsStrIdx(xcoord_t x, uint8_t y, const pm_char *str, uint8_t idx, LcdFlags att=0);
void putsModelName(xcoord_t x, uint8_t y, char *name, uint8_t id, LcdFlags att);
void putsSwitches(xcoord_t x, uint8_t y, int8_t swtch, LcdFlags att=0);
void putsMixerSource(xcoord_t x, uint8_t y, uint8_t idx, LcdFlags att=0);
void putsFlightPhase(xcoord_t x, uint8_t y, int8_t idx, LcdFlags att=0);
void putsCurve(xcoord_t x, uint8_t y, int8_t idx, LcdFlags att=0);
void putsTmrMode(xcoord_t x, uint8_t y, int8_t mode, LcdFlags att);
void putsTrimMode(xcoord_t x, uint8_t y, uint8_t phase, uint8_t idx, LcdFlags att);
#if defined(ROTARY_ENCODERS)
  void putsRotaryEncoderMode(xcoord_t x, uint8_t y, uint8_t phase, uint8_t idx, LcdFlags att);
#endif

#define putsChn(x, y, idx, att) putsMixerSource(x, y, MIXSRC_CH1+idx-1, att)
void putsChnLetter(xcoord_t x, uint8_t y, uint8_t idx, LcdFlags attr);

void putsVolts(xcoord_t x, uint8_t y, uint16_t volts, LcdFlags att);
void putsVBat(xcoord_t x, uint8_t y, LcdFlags att);
void putsTelemetryChannel(xcoord_t x, uint8_t y, uint8_t channel, lcdint_t val, uint8_t att=0);

#if defined(CPUARM)
  #define putstime_t int32_t
#else
  #define putstime_t int16_t
#endif

void putsTime(xcoord_t x, uint8_t y, putstime_t tme, LcdFlags att, LcdFlags att2);

#define SOLID  0xff
#define DOTTED 0x55

void lcd_plot(xcoord_t x, uint8_t y, LcdFlags att=0);
void lcd_hline(xcoord_t x, uint8_t y, xcoord_t w, LcdFlags att=0);
void lcd_hlineStip(xcoord_t x, uint8_t y, xcoord_t w, uint8_t pat, LcdFlags att=0);
void lcd_vline(xcoord_t x, int8_t y, int8_t h);
#if defined(CPUM64)
  void lcd_vlineStip(xcoord_t x, int8_t y, int8_t h, uint8_t pat);
#else
  void lcd_vlineStip(xcoord_t x, int8_t y, int8_t h, uint8_t pat, LcdFlags att=0);
#endif

void lcd_rect(xcoord_t x, uint8_t y, xcoord_t w, uint8_t h, uint8_t pat=SOLID, LcdFlags att=0);
void lcd_filled_rect(xcoord_t x, int8_t y, xcoord_t w, uint8_t h, uint8_t pat=SOLID, LcdFlags att=0);
void lcd_invert_line(int8_t y);
#define lcd_status_line() lcd_invert_line(LCD_LINES-1)
inline void lcd_square(xcoord_t x, uint8_t y, xcoord_t w, LcdFlags att=0) { lcd_rect(x, y, w, w, SOLID, att); }

void lcdDrawTelemetryTopBar();

#define DO_CROSS(xx,yy,ww)          \
    lcd_vline(xx,yy-ww/2,ww);  \
    lcd_hline(xx-ww/2,yy,ww);

#define V_BAR(xx,yy,ll)       \
    lcd_vline(xx-1,yy-ll,ll); \
    lcd_vline(xx  ,yy-ll,ll); \
    lcd_vline(xx+1,yy-ll,ll);

#define LCD_2DOTS(x, y, att)     \
    lcd_putcAtt(x, y, ' ', att); \
    lcd_vline(x+4, y+3, 2);      \
    lcd_vline(x+5, y+3, 2);      \
    lcd_vline(x+4, y+8, 2);      \
    lcd_vline(x+5, y+8, 2);

void lcd_img(xcoord_t x, uint8_t y, const pm_uchar * img, uint8_t idx, LcdFlags att=0);
void lcd_bmp(xcoord_t x, uint8_t y, const pm_uchar * img, uint8_t offset=0, uint8_t width=0);
#define LCD_ICON(x, y, icon) lcd_bmp(x, y, icons, icon)
void lcdSetRefVolt(unsigned char val);
void lcdInit();
void lcd_clear();
void lcdSetContrast();

void lcdRefresh();

#if defined(PCBTARANIS)
  const pm_char * bmpLoad(uint8_t *dest, const char *filename, const xcoord_t width, const uint8_t height);
#endif

#define BLINK_ON_PHASE (g_blinkTmr10ms & (1<<6))

#ifdef SIMU
  extern bool lcd_refresh;
  extern uint8_t lcd_buf[DISPLAY_BUF_SIZE];
#endif

#define LCD_BYTE_FILTER_PLAN(p, keep, add) *(p) = (*(p) & (keep)) | (add)

#if defined(PCBTARANIS)
#define LCD_BYTE_FILTER(p, keep, add) \
  do { \
    if (!(flags & GREY(1))) \
      LCD_BYTE_FILTER_PLAN(p, keep, add); \
    if (!(flags & GREY(2))) \
      LCD_BYTE_FILTER_PLAN((p+DISPLAY_PLAN_SIZE), keep, add); \
    if (!(flags & GREY(4))) \
      LCD_BYTE_FILTER_PLAN((p+2*DISPLAY_PLAN_SIZE), keep, add); \
    if (!(flags & GREY(8))) \
      LCD_BYTE_FILTER_PLAN((p+3*DISPLAY_PLAN_SIZE), keep, add); \
  } while (0)
#else
#define LCD_BYTE_FILTER(p, keep, add) LCD_BYTE_FILTER_PLAN(p, keep, add)
#endif

#endif
/*eof*/
