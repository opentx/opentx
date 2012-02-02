/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 *
 * gruvin9x is based on code named er9x by
 * Author - Erez Raviv <erezraviv@gmail.com>, which is in turn
 * was based on the original (and ongoing) project by Thomas Husterer,
 * th9x -- http://code.google.com/p/th9x/
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

#include "open9x.h"

#define DISPLAY_W 128
#define DISPLAY_H  64
#define FW          6
#define FWNUM       5
#define FH          8

/* lcd common flags */
#define BLINK         0x01

/* lcd text flags */
#define INVERS        0x02
#define DBLSIZE       0x04

/* lcd putc flags */
#define CONDENSED     0x08

/* lcd puts flags */
#define BSS           0x10
#define ZCHAR         0x20

/* lcd outdez flags */
#define UNSIGN        0x08
#define LEADING0      0x10
#define SPARE1        0x20
#define SPARE2        0x30
#define SPARE3        0x40
#define PREC1         0x50
#define PREC2         0x60
#define PREC3         0x70
#define MODE(flags)   (-4 + ((int8_t)(flags & 0x70) >> 4))
#define LEFT          0x80 /* align left */

/* line, rect, square flags */
#define BLACK         0x02
#define WHITE         0x04

/* other flags */
#define NO_UNIT       UNSIGN
#define SHRT_TM_MODE  LEFT

extern uint8_t displayBuf[DISPLAY_W*DISPLAY_H/8];
extern uint8_t lcd_lastPos;

extern void lcd_putc(unsigned char x,unsigned char y,const char c);
extern void lcd_putcAtt(unsigned char x,unsigned char y,const char c,uint8_t mode);

extern void lcd_putsAtt(unsigned char x,unsigned char y,const pm_char * s,uint8_t mode);
extern void lcd_putsnAtt(unsigned char x,unsigned char y,const pm_char * s,unsigned char len,uint8_t mode);
extern void lcd_puts_P(unsigned char x,unsigned char y,const pm_char * s);
extern void lcd_putsn_P(unsigned char x,unsigned char y,const pm_char * s,unsigned char len);

extern void lcd_outhex4(unsigned char x,unsigned char y,uint16_t val);

extern void lcd_outdezAtt(uint8_t x, uint8_t y, int16_t val, uint8_t mode=0);
extern void lcd_outdezNAtt(uint8_t x, uint8_t y, int16_t val, uint8_t mode=0, uint8_t len=0);
extern void lcd_outdez8(uint8_t x, uint8_t y, int8_t val);

extern void putsStrIdx(uint8_t x, uint8_t y, const pm_char *str, uint8_t idx, uint8_t att=0);

extern void putsModelName(uint8_t x, uint8_t y, char *name, uint8_t id, uint8_t att);
extern void putsSwitches(uint8_t x, uint8_t y, int8_t swtch, uint8_t att=0);
extern void putsFlightPhase(uint8_t x, uint8_t y, int8_t idx, uint8_t att=0);
extern void putsCurve(uint8_t x, uint8_t y, uint8_t idx, uint8_t att=0);
extern void putsTmrMode(uint8_t x, uint8_t y, int8_t mode, uint8_t att);

extern void putsChnRaw(uint8_t x,uint8_t y,uint8_t idx1,uint8_t att);
extern void putsChn(uint8_t x,uint8_t y,uint8_t idx1,uint8_t att);
extern void putsChnLetter(uint8_t x, uint8_t y, uint8_t idx, uint8_t attr);

extern void putsVolts(uint8_t x, uint8_t y, uint16_t volts, uint8_t att);
extern void putsVBat(uint8_t x, uint8_t y, uint8_t att);
extern void putsTime(uint8_t x,uint8_t y, int16_t tme, uint8_t att, uint8_t att2);

#ifdef FRSKY
// TODO move this into frsky.h
void putsTelemetryChannel(uint8_t x, uint8_t y, uint8_t channel, uint8_t val, uint8_t att);
void putsTelemetryValue(uint8_t x, uint8_t y, int16_t val, uint8_t unit, uint8_t att);
#endif

extern void lcd_plot(unsigned char x, unsigned char y, uint8_t att=0);
extern void lcd_hline(unsigned char x, unsigned char y, uint8_t w, uint8_t att=0);
extern void lcd_hlineStip(int8_t x, uint8_t y, uint8_t w, uint8_t pat, uint8_t att=0);
extern void lcd_vline(uint8_t x, int8_t y, int8_t h);
extern void lcd_vlineStip(uint8_t x, int8_t y, int8_t h, uint8_t pat);

extern void lcd_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t pat=0xff, uint8_t att=0);
extern void lcd_filled_rect(uint8_t x, int8_t y, uint8_t w, uint8_t h, uint8_t att=0);
inline void lcd_square(uint8_t x, uint8_t y, uint8_t w, uint8_t att=0) { lcd_rect(x, y, w, w, 0xff, att); }

#define DO_CROSS(xx,yy,ww)          \
    lcd_vline(xx,yy-ww/2,ww);  \
    lcd_hline(xx-ww/2,yy,ww);

#define V_BAR(xx,yy,ll)       \
    lcd_vline(xx-1,yy-ll,ll); \
    lcd_vline(xx  ,yy-ll,ll); \
    lcd_vline(xx+1,yy-ll,ll);

extern void lcd_img(uint8_t x, uint8_t y, const pm_uchar * img, uint8_t idx, uint8_t mode);
extern void lcdSetRefVolt(unsigned char val);
extern void lcd_init();
extern void lcd_clear();

extern void refreshDisplay();

#define BLINK_ON_PHASE (g_blinkTmr10ms & (1<<6))
#define BLINK_SYNC      g_blinkTmr10ms = (3<<5)

#ifdef SIMU
extern bool lcd_refresh;
extern uint8_t lcd_buf[DISPLAY_W*DISPLAY_H/8];
#endif

#endif
/*eof*/
