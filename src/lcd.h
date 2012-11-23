/*
 * Authors (alphabetical order)
 * - Bertrand Songis <bsongis@gmail.com>
 * - Bryan J. Rentoul (Gruvin) <gruvin@gmail.com>
 * - Cameron Weeks <th9xer@gmail.com>
 * - Erez Raviv
 * - Jean-Pierre Parisy
 * - Karl Szmutny <shadow@privy.de>
 * - Michael Blandford
 * - Michal Hlavinka
 * - Pat Mackenzie
 * - Philip Moss
 * - Rob Thomson
 * - Romolo Manfredini <romolo.manfredini@gmail.com>
 * - Thomas Husterer
 *
 * open9x is based on code named
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

#if defined(LCD212)
#define DISPLAY_W 212
#define DISPLAY_H  64
#define xcoord_t uint16_t
#define CENTER      "\015"
#define CENTER_OFS  (7*FW-FW/2)
#else
#define DISPLAY_W 128
#define DISPLAY_H  64
#define xcoord_t uint8_t
#define CENTER
#define CENTER_OFS 0
#endif

#define FW          6
#define FWNUM       5
#define FH          8

/* lcd common flags */
#define BLINK         0x01

/* lcd text flags */
#define INVERS        0x02
#define DBLSIZE       0x04
#ifdef BOLD_FONT
#define BOLD          0x40
#else
#define BOLD          0x00
#endif

/* lcd putc flags */
#define CONDENSED     0x08

/* lcd puts flags */
/* no 0x80 here because of "GV"1 which is aligned LEFT */
/* no 0x10 here because of "MODEL"01 which uses LEADING0 */
#define BSS           0x20
#define ZCHAR         0x80

/* lcd outdez flags */
#define UNSIGN        0x08
#define LEADING0      0x10
#define PREC1         0x20
#define PREC2         0x30
#define MODE(flags)   ((((int8_t)(flags) & 0x30) - 0x10) >> 4)
#define LEFT          0x80 /* align left */

/* line, rect, square flags */
#define BLACK         0x02
#define WHITE         0x04
#define ROUND         0x08

/* switches flags */
#define SWONOFF       0x10 /* means inlude ON OFF in switches */
#define SWCONDENSED   0x20 /* means that THRm will be displayed as THR */

/* telemetry flags */
#define NO_UNIT       0x40

#if defined(PCBSKY9X)
#define LcdFlags uint32_t
#elif defined(PCBGRUVIN9X)
#define LcdFlags uint16_t
#else
#define LcdFlags uint8_t
#endif

extern uint8_t displayBuf[DISPLAY_W*DISPLAY_H/8];
extern uint8_t lcdLastPos;

#if defined(PCBSTD) && defined(VOICE)
extern volatile uint8_t LcdLock ;
#endif

#if defined(PCBSKY9X)
extern uint8_t lcdLock ;
extern uint32_t lcdInputs ;
#endif

extern void lcd_putc(xcoord_t x,uint8_t y,const unsigned char c);
extern void lcd_putcAtt(xcoord_t x,uint8_t y,const unsigned char c,uint8_t mode);

extern void lcd_putsAtt(xcoord_t x,uint8_t y,const pm_char * s,uint8_t mode);
extern void lcd_putsiAtt(xcoord_t x,uint8_t y,const pm_char * s,uint8_t idx, uint8_t mode);
extern void lcd_putsnAtt(xcoord_t x,uint8_t y,const pm_char * s,unsigned char len,uint8_t mode);
extern void lcd_puts(xcoord_t x,uint8_t y,const pm_char * s);
extern void lcd_putsLeft(uint8_t y, const pm_char * s);
extern void lcd_putsn(xcoord_t x,uint8_t y,const pm_char * s,unsigned char len);

extern void lcd_outhex4(xcoord_t x,uint8_t y,uint16_t val);

extern void lcd_outdezAtt(xcoord_t x, uint8_t y, int16_t val, LcdFlags mode=0);
extern void lcd_outdezNAtt(xcoord_t x, uint8_t y, int16_t val, LcdFlags mode=0, uint8_t len=0);
extern void lcd_outdez8(xcoord_t x, uint8_t y, int8_t val);

extern void putsStrIdx(xcoord_t x, uint8_t y, const pm_char *str, uint8_t idx, uint8_t att=0);

extern void putsModelName(xcoord_t x, uint8_t y, char *name, uint8_t id, uint8_t att);
extern void putsSwitches(xcoord_t x, uint8_t y, int8_t swtch, uint8_t att=0);
extern void putsMixerSource(xcoord_t x, uint8_t y, uint8_t idx, uint8_t att=0);
extern void putsFlightPhase(xcoord_t x, uint8_t y, int8_t idx, uint8_t att=0);
extern void putsCurve(xcoord_t x, uint8_t y, int8_t idx, uint8_t att=0);
extern void putsTmrMode(xcoord_t x, uint8_t y, int8_t mode, uint8_t att);
extern void putsTrimMode(xcoord_t x, uint8_t y, uint8_t phase, uint8_t idx, uint8_t att);
#if defined(ROTARY_ENCODERS)
void putsRotaryEncoderMode(xcoord_t x, uint8_t y, uint8_t phase, uint8_t idx, uint8_t att);
#endif

extern void putsChnRaw(xcoord_t x,uint8_t y,uint8_t idx1,uint8_t att);
#define putsChn(x, y, idx, att) putsChnRaw(x, y, (idx)+(NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS+NUM_STICKS+1+NUM_SW_SRCRAW+NUM_CYC+NUM_PPM), att)
extern void putsChnLetter(xcoord_t x, uint8_t y, uint8_t idx, uint8_t attr);

extern void putsVolts(xcoord_t x, uint8_t y, uint16_t volts, uint8_t att);
extern void putsVBat(xcoord_t x, uint8_t y, uint8_t att);

#if defined(PCBSKY9X)
#define putstime_t int32_t
#else
#define putstime_t int16_t
#endif

extern void putsTime(xcoord_t x, uint8_t y, putstime_t tme, uint8_t att, uint8_t att2);

#define SOLID  0xff
#define DOTTED 0x55

extern void lcd_plot(xcoord_t x, uint8_t y, uint8_t att=0);
extern void lcd_hline(xcoord_t x, uint8_t y, xcoord_t w, uint8_t att=0);
extern void lcd_hlineStip(xcoord_t x, uint8_t y, xcoord_t w, uint8_t pat, uint8_t att=0);
extern void lcd_vline(xcoord_t x, int8_t y, int8_t h);
#if defined(PCBSTD)
extern void lcd_vlineStip(xcoord_t x, int8_t y, int8_t h, uint8_t pat);
#else
extern void lcd_vlineStip(xcoord_t x, int8_t y, int8_t h, uint8_t pat, uint8_t att=0);
#endif

extern void lcd_rect(xcoord_t x, uint8_t y, xcoord_t w, uint8_t h, uint8_t pat=SOLID, uint8_t att=0);
extern void lcd_filled_rect(xcoord_t x, int8_t y, xcoord_t w, uint8_t h, uint8_t pat=SOLID, uint8_t att=0);
extern void lcd_invert_line(int8_t y);
#define lcd_status_line() lcd_invert_line(7)
inline void lcd_square(xcoord_t x, uint8_t y, xcoord_t w, uint8_t att=0) { lcd_rect(x, y, w, w, SOLID, att); }

#define DO_CROSS(xx,yy,ww)          \
    lcd_vline(xx,yy-ww/2,ww);  \
    lcd_hline(xx-ww/2,yy,ww);

// TODO optimization here!!!
#define V_BAR(xx,yy,ll)       \
    lcd_vline(xx-1,yy-ll,ll); \
    lcd_vline(xx  ,yy-ll,ll); \
    lcd_vline(xx+1,yy-ll,ll);

extern void lcd_img(xcoord_t x, uint8_t y, const pm_uchar * img, uint8_t idx, uint8_t mode);
extern void lcdSetRefVolt(unsigned char val);
extern void lcd_init();
extern void lcd_clear();
extern void lcdSetContrast();

extern void refreshDisplay();

#define BLINK_ON_PHASE (g_blinkTmr10ms & (1<<6))

#ifdef SIMU
extern bool lcd_refresh;
extern uint8_t lcd_buf[DISPLAY_W*DISPLAY_H/8];
#endif

#endif
/*eof*/
