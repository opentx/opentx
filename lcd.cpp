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

#include "open9x.h"

#define font_5x8_x20_x7f (font)
#define font_10x16_x20_x7f (font_dblsize)

uint8_t displayBuf[DISPLAY_W*DISPLAY_H/8];
#define DISPLAY_END (displayBuf+sizeof(displayBuf))

#ifdef SIMU
bool lcd_refresh = true;
uint8_t lcd_buf[DISPLAY_W*DISPLAY_H/8];
#endif

void lcd_clear()
{
  memset(displayBuf, 0, sizeof(displayBuf));
}

void lcd_img(uint8_t x, uint8_t y, const pm_uchar * img, uint8_t idx, uint8_t mode)
{
  const pm_uchar *q = img;
  uint8_t w    = pgm_read_byte(q++);
  uint8_t hb   = (pgm_read_byte(q++)+7)/8;
  bool    inv  = (mode & INVERS) ? true : (mode & BLINK ? BLINK_ON_PHASE : false);
  q += idx*w*hb;
  for (uint8_t yb = 0; yb < hb; yb++) {
    uint8_t *p = &displayBuf[ (y / 8 + yb) * DISPLAY_W + x ];
    for (uint8_t i=0; i<w; i++){
      uint8_t b = pgm_read_byte(q++);
      *p++ = inv ? ~b : b;
    }
  }
}

uint8_t lcd_lastPos;

void lcd_putcAtt(uint8_t x, uint8_t y, const unsigned char c, uint8_t mode)
{
  uint8_t *p = &displayBuf[ y / 8 * DISPLAY_W + x ];

  const pm_uchar *q = &font_5x8_x20_x7f[ (c-0x20)*5];

  bool inv = false;
  if (mode & BLINK) {
    if (BLINK_ON_PHASE) {
      if (mode & INVERS)
        inv = true;
      else
        return;
    }
  }
  else if (mode & INVERS) {
    inv = true;
  }

  if(mode & DBLSIZE)
  {
    /* each letter consists of ten top bytes followed by
     * by ten bottom bytes (20 bytes per * char) */
    q = &font_10x16_x20_x7f[((uint16_t)c-0x20)*20];
    for (int8_t i=11; i>=0; i--) {
      if (mode & CONDENSED && i<=1) break;
      uint8_t b1=0, b2=0;
      if (i>1) {
        b1 = pgm_read_byte(q++); /*top byte*/
        b2 = pgm_read_byte(q++); /*top byte*/
      }
      if(inv) {
        b1=~b1;
        b2=~b2;
      }   
      if(&p[DISPLAY_W+1] < DISPLAY_END) {
        p[0]=b1;
        p[DISPLAY_W] = b2;
        p++;
      }   
    }   
  }
  else {
    uint8_t condense=0;

    if (mode & CONDENSED) {
        *p++ = inv ? ~0 : 0;
        condense=1;
    }

    uint8_t ym8 = (y % 8);
    for (int8_t i=5; i>=0; i--) {
        uint8_t b = (i>0 ? pgm_read_byte(q++) : 0);
        if (inv) b = ~b;
        
        if (condense && i==4) {
            /*condense the letter by skipping column 4 */
            continue;
        }
        if (p<DISPLAY_END) {
          *p = (*p & (~(0xff << ym8))) + (b << ym8);
          if (ym8) { uint8_t *r = p + DISPLAY_W; if (r<DISPLAY_END) *r = (*r & (~(0xff >> (8-ym8)))) + (b >> (8-ym8)); }
          p++;
        }
    }
  }
}

void lcd_putc(uint8_t x, uint8_t y, const unsigned char c)
{
  lcd_putcAtt(x,y,c,0);
}

void lcd_putsiAtt(uint8_t x,uint8_t y,const pm_char * s,uint8_t idx, uint8_t flags)
{
  uint8_t length;
  length = pgm_read_byte(s++) ;
  lcd_putsnAtt(x,y,s+length*idx,length,flags);
}

void lcd_putsnAtt(uint8_t x,uint8_t y,const pm_char * s,uint8_t len,uint8_t mode)
{
  while(len!=0) {
    unsigned char c;
    switch (mode & (BSS+ZCHAR)) {
      case BSS:
        c = *s;
        break;
      case ZCHAR:
        c = idx2char(*s);
        break;
      default:
        c = pgm_read_byte(s);
        break;
    }
    if (!c || x>DISPLAY_W-6) break;
    if (c >= 0x20) {
      lcd_putcAtt(x,y,c,mode);
      x += FW;
      if (mode&DBLSIZE) x += FW-1;
    }
    else {
      x += (c*FW);
    }
    s++;
    len--;
  }
  lcd_lastPos = x;
}

void lcd_putsn(uint8_t x,uint8_t y,const pm_char * s,uint8_t len)
{
  lcd_putsnAtt(x, y, s, len, 0);
}

void lcd_putsAtt(uint8_t x,uint8_t y,const pm_char * s,uint8_t mode)
{
  lcd_putsnAtt(x, y, s, 255, mode);
}

void lcd_puts(uint8_t x,uint8_t y,const pm_char * s)
{
  lcd_putsAtt( x, y, s, 0);
}

void lcd_putsLeft(uint8_t y, const pm_char * s)
{
  lcd_putsAtt(0, y, s, 0);
}

void lcd_outhex4(uint8_t x,uint8_t y,uint16_t val)
{
  x+=FWNUM*4;
  for(int i=0; i<4; i++)
  {
    x-=FWNUM;
    char c = val & 0xf;
    c = c>9 ? c+'A'-10 : c+'0';
    lcd_putcAtt(x,y,c,c>='A'?CONDENSED:0);
    val>>=4;
  }
}
void lcd_outdez8(uint8_t x, uint8_t y, int8_t val)
{
  lcd_outdezAtt(x, y, val);
}

void lcd_outdezAtt(uint8_t x, uint8_t y, int16_t val, uint8_t flags)
{
  lcd_outdezNAtt(x, y, val, flags);
}

// TODO use doxygen style comments here

/*
USAGE:
  lcd_outdezNAtt(x-coord, y-coord, (un)signed-value{0..65535|0..+/-32768}, 
                  mode_flags, length)

  Available mode_flas: PREC{1..3}, UNSIGN (for programmer selected signed numbers
                  to allow for unsigned values up to the ful 65535 16-bit limt))

  LEADING0 means pad 0 to the left of sig. digits up to 'len' total characters
*/

void lcd_outdezNAtt(uint8_t x, uint8_t y, int16_t val, uint8_t flags, uint8_t len)
{
  uint8_t fw = FWNUM;
  int8_t mode = MODE(flags);
  bool dblsize = flags & DBLSIZE;

  bool neg = false;
  if (flags & UNSIGN) { flags -= UNSIGN; }
  else if (val < 0) { neg=true; val=-val; }

  uint8_t xn = 0;
  uint8_t ln = 2;

  if (mode != MODE(LEADING0)) {
    len = 1;
    uint16_t tmp = ((uint16_t)val) / 10;
    while (tmp) {
      len++;
      tmp /= 10;
    }
    if (len <= mode)
      len = mode + 1;
  }

  if (dblsize) {
    fw += FWNUM;
  }
  else {
    if (flags & LEFT) {
      if (mode > 0)
        x += 2;
    }
  }

  if (flags & LEFT) {
    x += len * fw;
    if (neg)
      x += (dblsize ? 7 : FWNUM);
  }

  lcd_lastPos = x;
  x -= fw + 1;

  for (uint8_t i=1; i<=len; i++) {
    char c = ((uint16_t)val % 10) + '0';
    uint8_t f = flags;
    if (dblsize) {
      if (c=='1' && i==len && xn>x+10) { x+=2; f|=CONDENSED; }
      if (val >= 1000) { x+=FWNUM; f&=~DBLSIZE; }
    }
    lcd_putcAtt(x, y, c, f);
    if (mode==i) {
      flags &= ~PREC2; // TODO not needed but removes 64bytes, could be improved for sure, check asm
      if (dblsize) {
        xn = x;
        if(c>='1' && c<='3') ln++;
        uint8_t tn = ((uint16_t)val/10) % 10;
        if (tn==2 || tn==4) {
          if (c=='4') { xn++; }
          else { xn--; ln++; }
        }
      }
      else {
        x -= 2;
        lcd_plot(x+1, y+6);
        if ((flags&INVERS) && ((~flags & BLINK) || BLINK_ON_PHASE))
          lcd_vline(x+1, y, 8);
      }
    }
    if (dblsize && val >= 1000 && val < 10000) x-=2;
    val = ((uint16_t)val) / 10;
    x-=fw;
  }

  if (xn) {
    y &= ~0x07;
    lcd_hline(xn, (y & ~0x07)+2*FH-3, ln);
    lcd_hline(xn, y+2*FH-2, ln);
  }

  if (neg) lcd_putcAtt(x, y, '-', flags);

#ifdef NAVIGATION_RE1
  if (flags & SURROUNDED) {
    xn = lcd_lastPos - x + 2;
    if (!neg) { x+=FW; xn-=FW; }
    lcd_rect(x-1, y-1, xn, 9, BLINK_ON_PHASE ? DOTTED : ~DOTTED);
  }
#endif
}

void lcd_mask(uint8_t *p, uint8_t mask, uint8_t att)
{
  assert(p >= displayBuf && p < DISPLAY_END);

  if (att & BLACK)
    *p |= mask;
  else if (att & WHITE)
    *p &= ~mask;
  else
    *p ^= mask;
}

void lcd_plot(uint8_t x,uint8_t y, uint8_t att)
{
  uint8_t *p   = &displayBuf[ y / 8 * DISPLAY_W + x ];
  if (p<DISPLAY_END)
    lcd_mask(p, BITMASK(y%8), att);
}

void lcd_hlineStip(int8_t x, uint8_t y, uint8_t w, uint8_t pat, uint8_t att)
{
  if (y >= DISPLAY_H) return;
  if (x<0) { w+=x; x=0; }
  if (x+w > DISPLAY_W) { w = DISPLAY_W - x; }

  uint8_t *p  = &displayBuf[ y / 8 * DISPLAY_W + x ];
  uint8_t msk = BITMASK(y%8);
  while(w) {
    if(pat&1) {
      lcd_mask(p, msk, att);
      pat = (pat >> 1) | 0x80;
    }
    else {
      pat = pat >> 1;
    }
    w--;
    p++;
  }
}

void lcd_hline(uint8_t x, uint8_t y, uint8_t w, uint8_t att)
{
  lcd_hlineStip(x, y, w, 0xff, att);
}

void lcd_vlineStip(uint8_t x, int8_t y, int8_t h, uint8_t pat)
{
  if (x >= DISPLAY_W) return;
  if (h<0) { y+=h; h=-h; }
  if (y<0) { h+=y; y=0; }
  if (y+h > DISPLAY_H) { h = DISPLAY_H - y; }

  if (pat==DOTTED && !(y%2))
    pat = ~pat;

  uint8_t *p  = &displayBuf[ y / 8 * DISPLAY_W + x ];
  y = y % 8;
  if (y) {
    assert(p >= displayBuf && p < DISPLAY_END);
    *p ^= ~(BITMASK(y)-1) & pat;
    p += DISPLAY_W;
    h -= 8-y;
  }
  while (h>0) {
    assert(p >= displayBuf && p < DISPLAY_END);
    *p ^= pat;
    p += DISPLAY_W;
    h -= 8;
  }
  h = (h+8) % 8; // TODO optim
  if (h) {
    p -= DISPLAY_W;
    assert(p >= displayBuf && p < DISPLAY_END);
    *p ^= ~(BITMASK(h)-1) & pat;
  }
}

void lcd_vline(uint8_t x, int8_t y, int8_t h)
{
  lcd_vlineStip(x, y, h, 0xff);
}

void lcd_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t pat, uint8_t att)
{
  if (!((att & BLINK) && BLINK_ON_PHASE)) {
    lcd_vlineStip(x, y, h, pat);
    lcd_vlineStip(x+w-1, y, h, pat);
    if (~att & ROUND) { x+=1; w-=2; }
    lcd_hlineStip(x, y+h-1, w, pat);
    lcd_hlineStip(x, y, w, pat);
  }
}

void lcd_filled_rect(uint8_t x, int8_t y, uint8_t w, uint8_t h, uint8_t pat, uint8_t att)
{
  for (int8_t i=y; i<y+h; i++) {
    if (i>=0 && i<64) lcd_hlineStip(x, i, w, pat, att);
    if (pat != 0xff) pat = ~pat;
  }
}

void putsTime(uint8_t x,uint8_t y,int16_t tme,uint8_t att,uint8_t att2)
{
  if (att & LEFT) x+=3*FW;

  if (tme<0) {
    lcd_putcAtt(x - ((att & DBLSIZE) ? FW+1 : FWNUM), y, '-', att);
    tme = -tme;
  }

  lcd_outdezNAtt(x, y, tme/60, att|LEADING0|LEFT, 2);
  lcd_putcAtt(lcd_lastPos-((att & DBLSIZE) ? 1 : 0), y, ':', att&att2);
  lcd_outdezNAtt(lcd_lastPos+FW, y, tme%60, att2|LEADING0|LEFT, 2);
}

// TODO to be optimized with putsTelemetryValue
void putsVolts(uint8_t x, uint8_t y, uint16_t volts, uint8_t att)
{
  lcd_outdezAtt(x, y, (int16_t)volts, (~NO_UNIT) & (att | ((att&PREC2)==PREC2 ? 0 : PREC1)));
  if (~att & NO_UNIT) lcd_putcAtt(lcd_lastPos, y, 'v', att);
}

void putsVBat(uint8_t x, uint8_t y, uint8_t att)
{
  putsVolts(x, y, g_vbat100mV, att);
}

void putsStrIdx(uint8_t x, uint8_t y, const pm_char *str, uint8_t idx, uint8_t att)
{
  lcd_putsAtt(x, y, str, att & ~BSS); // TODO use something else than BSS for LEADING0
  lcd_outdezNAtt(lcd_lastPos, y, idx, att|LEFT, 2);
  if (att&TWO_DOTS) lcd_putc(lcd_lastPos, y, ':');
}

void putsChnRaw(uint8_t x, uint8_t y, uint8_t idx, uint8_t att)
{
  if (idx==0)
    lcd_putsiAtt(x, y, STR_MMMINV, 0, att);
  else if (idx<=NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS+2+3)
    lcd_putsiAtt(x, y, STR_VSRCRAW, idx-1, att);
  else if (idx<=NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS+2+3+NUM_PPM)
    putsStrIdx(x, y, STR_PPM, idx - (NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS+2+3), att);
  else if (idx<=NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS+2+3+NUM_PPM+NUM_CHNOUT)
    putsStrIdx(x, y, STR_CH, idx - (NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS+2+3+NUM_PPM), att);
  else
    lcd_putsiAtt(x, y, STR_VTELEMCHNS, idx-(NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS+2+3+NUM_PPM+NUM_CHNOUT), att);
}

void putsChn(uint8_t x, uint8_t y, uint8_t idx, uint8_t att)
{
  if (idx > 0 && idx <= NUM_CHNOUT)
    putsChnRaw(x, y, idx+(NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS+2+3+NUM_PPM), att);
}

void putsMixerSource(uint8_t x, uint8_t y, uint8_t idx, uint8_t att)
{
  if (idx<=NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS+2)
    putsChnRaw(x, y, idx, att);
  else if (idx<=NUM_STICKS+NUM_POTS+NUM_ROTARY_ENCODERS+2+MAX_SWITCH)
    putsSwitches(x, y, idx-NUM_STICKS-NUM_POTS-NUM_ROTARY_ENCODERS-2, att);
  else
    putsChnRaw(x, y, idx-MAX_SWITCH, att);
}

void putsChnLetter(uint8_t x, uint8_t y, uint8_t idx, uint8_t attr)
{
  lcd_putsiAtt(x, y, STR_RETA123, idx-1, attr);
}

void putsModelName(uint8_t x, uint8_t y, char *name, uint8_t id, uint8_t att)
{
  uint8_t len = sizeof(g_model.name);
  while (len>0 && !name[len-1]) --len;
  if (len==0) {
    putsStrIdx(x, y, STR_MODEL, id+1, att|LEADING0);
  }
  else {
    lcd_putsnAtt(x, y, name, sizeof(g_model.name), ZCHAR|att);
  }
}

void putsSwitches(uint8_t x, uint8_t y, int8_t idx, uint8_t att)
{
  if (idx == 0)
    return lcd_putsiAtt(x, y, STR_MMMINV, 0, att);
  if (~att & SWONLY) {
    if (idx == SWITCH_ON)
      return lcd_putsiAtt(x, y, STR_OFFON, 1, att);
    if (idx == SWITCH_OFF)
      return lcd_putsiAtt(x, y, STR_OFFON, 0, att);
  }
  if (idx<0) {
    lcd_vlineStip(x-2, y, 8, 0x5E/*'!'*/);
    idx = -idx;
  }
  if (idx > MAX_SWITCH) {
    idx -= ((att & SWONLY) ? MAX_SWITCH : MAX_SWITCH+1);
    if (~att & SWCONDENSED) lcd_putcAtt(x+3*FW, y, 'm', att);
  }
  lcd_putsiAtt(x, y, STR_VSWITCHES, idx-1, att);
}

void putsFlightPhase(uint8_t x, uint8_t y, int8_t idx, uint8_t att)
{
  if (idx==0) { lcd_putsiAtt(x, y, STR_MMMINV, 0, att); return; }
  if (idx < 0) { lcd_vlineStip(x-2, y, 8, 0x5E/*'!'*/); idx = -idx; }
  putsStrIdx(x, y, STR_FP, idx-1, att);
}

void putsCurve(uint8_t x, uint8_t y, int8_t idx, uint8_t att)
{
  if (idx < 0) {
    lcd_putcAtt(x-1*FW, y, '!', att);
    idx = -idx + 6;
  }
  if (idx < CURVE_BASE)
    lcd_putsiAtt(x, y, STR_VCURVEFUNC, idx, att);
  else
    putsStrIdx(x, y, PSTR("c"), idx-CURVE_BASE+1, att);
}

void putsTmrMode(uint8_t x, uint8_t y, int8_t mode, uint8_t att)
{
  if (mode < 0) {
    mode = TMR_VAROFS - mode - 1;
    lcd_putcAtt(x-1*FW, y, '!', att);
  }
  else if (mode < TMR_VAROFS) {
    lcd_putsiAtt(x, y, STR_VTMRMODES, mode, att);
    return;
  }

  putsSwitches(x, y, mode-(TMR_VAROFS-1), att|SWONLY);
}

void putsTrimMode(uint8_t x, uint8_t y, uint8_t phase, uint8_t idx, uint8_t att)
{
  int16_t v = getRawTrimValue(phase, idx);

  if (v > TRIM_EXTENDED_MAX) {
    uint8_t p = v - TRIM_EXTENDED_MAX - 1;
    if (p >= phase) p++;
    lcd_putcAtt(x, y, '0'+p, att);
  }
  else {
    putsChnLetter(x, y, idx+1, att);
  }
}

#if defined(PCBV4)
void putsRotaryEncoderMode(uint8_t x, uint8_t y, uint8_t phase, uint8_t idx, uint8_t att)
{
  int16_t v = phaseaddress(phase)->rotaryEncoders[idx];

  if (v > ROTARY_ENCODER_MAX) {
    uint8_t p = v - ROTARY_ENCODER_MAX - 1;
    if (p >= phase) p++;
    lcd_putcAtt(x, y, '0'+p, att);
  }
  else {
    lcd_putcAtt(x, y, 'A'+idx, att);
  }
}
#endif

#ifdef PCBARM

// LCD i/o pins
// LCD_RES     PC27
// LCD_CS1     PC26
// LCD_E       PC12
// LCD_RnW     PC13
// LCD_A0      PC15
// LCD_D0      PC0
// LCD_D1      PC7
// LCD_D2      PC6
// LCD_D3      PC5
// LCD_D4      PC4
// LCD_D5      PC3
// LCD_D6      PC2
// LCD_D7      PC1

#define LCD_DATA        0x000000FFL
#ifdef REVB
#define LCD_A0    0x00000080L
#else
#define LCD_A0    0x00008000L
#endif
#define LCD_RnW   0x00002000L
#define LCD_E     0x00001000L
#define LCD_CS1   0x04000000L
#define LCD_RES   0x08000000L

// Lookup table for prototype board
#ifndef REVB
const uint8_t Lcd_lookup[] =
{
0x00,0x01,0x80,0x81,0x40,0x41,0xC0,0xC1,0x20,0x21,0xA0,0xA1,0x60,0x61,0xE0,0xE1,
0x10,0x11,0x90,0x91,0x50,0x51,0xD0,0xD1,0x30,0x31,0xB0,0xB1,0x70,0x71,0xF0,0xF1,
0x08,0x09,0x88,0x89,0x48,0x49,0xC8,0xC9,0x28,0x29,0xA8,0xA9,0x68,0x69,0xE8,0xE9,
0x18,0x19,0x98,0x99,0x58,0x59,0xD8,0xD9,0x38,0x39,0xB8,0xB9,0x78,0x79,0xF8,0xF9,
0x04,0x05,0x84,0x85,0x44,0x45,0xC4,0xC5,0x24,0x25,0xA4,0xA5,0x64,0x65,0xE4,0xE5,
0x14,0x15,0x94,0x95,0x54,0x55,0xD4,0xD5,0x34,0x35,0xB4,0xB5,0x74,0x75,0xF4,0xF5,
0x0C,0x0D,0x8C,0x8D,0x4C,0x4D,0xCC,0xCD,0x2C,0x2D,0xAC,0xAD,0x6C,0x6D,0xEC,0xED,
0x1C,0x1D,0x9C,0x9D,0x5C,0x5D,0xDC,0xDD,0x3C,0x3D,0xBC,0xBD,0x7C,0x7D,0xFC,0xFD,
0x02,0x03,0x82,0x83,0x42,0x43,0xC2,0xC3,0x22,0x23,0xA2,0xA3,0x62,0x63,0xE2,0xE3,
0x12,0x13,0x92,0x93,0x52,0x53,0xD2,0xD3,0x32,0x33,0xB2,0xB3,0x72,0x73,0xF2,0xF3,
0x0A,0x0B,0x8A,0x8B,0x4A,0x4B,0xCA,0xCB,0x2A,0x2B,0xAA,0xAB,0x6A,0x6B,0xEA,0xEB,
0x1A,0x1B,0x9A,0x9B,0x5A,0x5B,0xDA,0xDB,0x3A,0x3B,0xBA,0xBB,0x7A,0x7B,0xFA,0xFB,
0x06,0x07,0x86,0x87,0x46,0x47,0xC6,0xC7,0x26,0x27,0xA6,0xA7,0x66,0x67,0xE6,0xE7,
0x16,0x17,0x96,0x97,0x56,0x57,0xD6,0xD7,0x36,0x37,0xB6,0xB7,0x76,0x77,0xF6,0xF7,
0x0E,0x0F,0x8E,0x8F,0x4E,0x4F,0xCE,0xCF,0x2E,0x2F,0xAE,0xAF,0x6E,0x6F,0xEE,0xEF,
0x1E,0x1F,0x9E,0x9F,0x5E,0x5F,0xDE,0xDF,0x3E,0x3F,0xBE,0xBF,0x7E,0x7F,0xFE,0xFF
} ;
#endif

void lcdSendCtl(uint8_t val)
{
  register Pio *pioptr ;
//      register uint32_t x ;

#ifdef REVB
  pioptr = PIOC ;
  pioptr->PIO_CODR = LCD_CS1 ;            // Select LCD
  PIOA->PIO_CODR = LCD_A0 ;
  pioptr->PIO_CODR = LCD_RnW ;            // Write
  pioptr->PIO_ODSR = val ;
#else
  pioptr = PIOC ;
  pioptr->PIO_CODR = LCD_CS1 ;            // Select LCD
  pioptr->PIO_CODR = LCD_A0 ;                     // Control
  pioptr->PIO_CODR = LCD_RnW ;            // Write
  pioptr->PIO_ODSR = Lcd_lookup[val] ;
#endif

  pioptr->PIO_SODR = LCD_E ;                      // Start E pulse
  // Need a delay here (250nS)
  TC0->TC_CHANNEL[0].TC_CCR = 5 ; // Enable clock and trigger it (may only need trigger)
  while ( TC0->TC_CHANNEL[0].TC_CV < 3 )          // Value depends on MCK/2 (used 6MHz)
  {
    // Wait
  }
  pioptr->PIO_CODR = LCD_E ;                      // End E pulse
#ifdef REVB
  PIOA->PIO_SODR = LCD_A0 ;                               // Data
#else
  pioptr->PIO_SODR = LCD_A0 ;                     // Data
#endif
  pioptr->PIO_SODR = LCD_CS1 ;            // Deselect LCD
}

void lcd_init()
{
  register Pio *pioptr ;
  // /home/thus/txt/datasheets/lcd/KS0713.pdf
  // ~/txt/flieger/ST7565RV17.pdf  from http://www.glyn.de/content.asp?wdid=132&sid=

#ifdef REVB
  pioptr = PIOA ;
  pioptr->PIO_PER = LCD_A0 ;              // Enable bit 7 (LCD-A0)
  pioptr->PIO_CODR = LCD_A0 ;
  pioptr->PIO_OER = LCD_A0 ;              // Set bit 7 output
  pioptr = PIOC ;
  pioptr->PIO_PER = 0x0C0030FFL ;         // Enable bits 27,26,13,12,7-0
  pioptr->PIO_CODR = LCD_E | LCD_RnW ;
  pioptr->PIO_SODR = LCD_RES | LCD_CS1 ;
  pioptr->PIO_OER = 0x0C0030FFL ;         // Set bits 27,26,13,12,7-0 output
  pioptr->PIO_OWER = 0x000000FFL ;                // Allow write to ls 8 bits in ODSR
#else
  pioptr = PIOC ;
  pioptr->PIO_PER = 0x0C00B0FFL ;         // Enable bits 27,26,15,13,12,7-0
  pioptr->PIO_CODR = LCD_E | LCD_RnW | LCD_A0 ;
  pioptr->PIO_SODR = LCD_RES | LCD_CS1 ;
  pioptr->PIO_OER = 0x0C00B0FFL ;         // Set bits 27,26,15,13,12,7-0 output
  pioptr->PIO_OWER = 0x000000FFL ;                // Allow write to ls 8 bits in ODSR
#endif

  pioptr->PIO_CODR = LCD_RES ;            // Reset LCD
  TC0->TC_CHANNEL[0].TC_CCR = 5 ; // Enable clock and trigger it (may only need trigger)
  while ( TC0->TC_CHANNEL[0].TC_CV < 12 )         // 2 uS, Value depends on MCK/2 (used 6MHz)
  {
    // Wait
  }
  pioptr->PIO_SODR = LCD_RES ;            // Remove LCD reset
  TC0->TC_CHANNEL[0].TC_CCR = 5 ; // Enable clock and trigger it (may only need trigger)
  while ( TC0->TC_CHANNEL[0].TC_CV < 9000 )               // 1500 uS, Value depends on MCK/2 (used 6MHz)
  {
    // Wait
  }
  lcdSendCtl(0xe2); //Initialize the internal functions
  lcdSendCtl(0xae); //DON = 0: display OFF
  lcdSendCtl(0xa1); //ADC = 1: reverse direction(SEG132->SEG1)
  lcdSendCtl(0xA6); //REV = 0: non-reverse display
  lcdSendCtl(0xA4); //EON = 0: normal display. non-entire
  lcdSendCtl(0xA2); // Select LCD bias=0
  lcdSendCtl(0xC0); //SHL = 0: normal direction (COM1->COM64)
  lcdSendCtl(0x2F); //Control power circuit operation VC=VR=VF=1
  lcdSendCtl(0x25); //Select int resistance ratio R2 R1 R0 =5
  lcdSendCtl(0x81); //Set reference voltage Mode
  lcdSendCtl(0x22); // 24 SV5 SV4 SV3 SV2 SV1 SV0 = 0x18
  lcdSendCtl(0xAF); //DON = 1: display ON
 // g_eeGeneral.contrast = 0x22;

#ifdef REVB
  pioptr->PIO_ODR = 0x0000003AL ;         // Set bits 1, 3, 4, 5 input
  pioptr->PIO_PUER = 0x0000003AL ;                // Set bits 1, 3, 4, 5 with pullups
  pioptr->PIO_ODSR = 0 ;                                                  // Drive D0 low
#else
  pioptr->PIO_ODR = 0x0000003CL ;         // Set bits 2, 3, 4, 5 input
  pioptr->PIO_PUER = 0x0000003CL ;                // Set bits 2, 3, 4, 5 with pullups
  pioptr->PIO_ODSR = 0 ;                                                  // Drive D0 low
#endif
}

void lcdSetRefVolt(uint8_t val)
{
#ifndef SIMU
  register Pio *pioptr ;
  pioptr = PIOC ;

  pioptr->PIO_OER = 0x0C00B0FFL ;         // Set bits 27,26,15,13,12,7-0 output

  lcdSendCtl(0x81);
  if ( val == 0 )
  {
    val = 0x22 ;
  }
  lcdSendCtl(val);

#ifdef REVB
  pioptr->PIO_ODR = 0x0000003AL ;         // Set bits 1, 3, 4, 5 input
  pioptr->PIO_PUER = 0x0000003AL ;                // Set bits 1, 3, 4, 5 with pullups
  pioptr->PIO_ODSR = 0 ;                                                  // Drive D0 low
#else
  pioptr->PIO_ODR = 0x0000003CL ;         // Set bits 2, 3, 4, 5 input
  pioptr->PIO_PUER = 0x0000003CL ;                // Set bits 2, 3, 4, 5 with pullups
  pioptr->PIO_ODSR = 0 ;                                                  // Drive D0 low
#endif
#endif
}

#ifndef SIMU
void refreshDisplay()
{
  register Pio *pioptr;
  register uint8_t *p = displayBuf;
  register uint32_t y;
  register uint32_t x;
  register uint32_t z;
  register uint32_t ebit;
#ifdef REVB
#else
  register uint8_t *lookup;
  lookup = (uint8_t *) Lcd_lookup;
#endif
  ebit = LCD_E;

#ifdef REVB
  pioptr = PIOA;
  pioptr->PIO_PER = 0x00000080; // Enable bit 7 (LCD-A0)
  pioptr->PIO_OER = 0x00000080;// Set bit 7 output
#endif

  pioptr = PIOC;
#ifdef REVB
  pioptr->PIO_OER = 0x0C0030FFL; // Set bits 27,26,15,13,12,7-0 output
#else
  pioptr->PIO_OER = 0x0C00B0FFL; // Set bits 27,26,15,13,12,7-0 output
#endif
  for (y = 0; y < 8; y++) {
    lcdSendCtl(/*TODO g_eeGeneral.optrexDisplay ? 0 : */0x04);
    lcdSendCtl(0x10); //column addr 0
    lcdSendCtl(y | 0xB0); //page addr y

    pioptr->PIO_CODR = LCD_CS1; // Select LCD
    PIOA->PIO_SODR = LCD_A0; // Data
    pioptr->PIO_CODR = LCD_RnW; // Write

#ifdef REVB
    x = *p;
#else
    x = lookup[*p];
#endif
    for (z = 0; z < 128; z += 1) {

// The following 7 lines replaces by a lookup table
//                      x = __RBIT( *p++ ) ;
//                      x >>= 23 ;
//                      if ( x & 0x00000100 )
//                      {
//                              x |= 1 ;
//                      }
//                      pioptr->PIO_ODSR = x ;

      pioptr->PIO_ODSR = x;
      pioptr->PIO_SODR = ebit; // Start E pulse
      // Need a delay here (250nS)
      p += 1;
#ifdef REVB
      x = *p;
#else
      x = lookup[*p];
#endif
//                      TC0->TC_CHANNEL[0].TC_CCR = 5 ; // Enable clock and trigger it (may only need trigger)
//                      while ( TC0->TC_CHANNEL[0].TC_CV < 3 )          // Value depends on MCK/2 (used 6MHz)
//                      {
//                              // Wait
//                      }
      pioptr->PIO_CODR = ebit; // End E pulse
    }
    pioptr->PIO_SODR = LCD_CS1; // Deselect LCD
  }

  pioptr->PIO_ODSR = 0xFF ;                                       // Drive lines high
#ifdef REVB
  pioptr->PIO_PUER = 0x0000003AL ;        // Set bits 1, 3, 4, 5 with pullups
  pioptr->PIO_ODR = 0x0000003AL ;         // Set bits 1, 3, 4, 5 input
#else
  pioptr->PIO_PUER = 0x0000003CL ;        // Set bits 2, 3, 4, 5 with pullups
  pioptr->PIO_ODR = 0x0000003CL ;         // Set bits 2, 3, 4, 5 input
#endif
  pioptr->PIO_ODSR = 0xFE ;                                       // Drive D0 low
}
#endif

#else

#define delay_1us() _delay_us(1)
void delay_1_5us(int ms)
{
  for(int i=0; i<ms; i++) delay_1us();
}

void lcdSendCtl(uint8_t val)
{
  PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_CS1);
#ifdef LCD_MULTIPLEX
  DDRA = 0xFF; // set LCD_DAT pins to output
#endif
  PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_A0);
  PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_RnW);
  PORTA_LCD_DAT = val;
  PORTC_LCD_CTRL |=  (1<<OUT_C_LCD_E);
  PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_E);
  PORTC_LCD_CTRL |=  (1<<OUT_C_LCD_A0);
#ifdef LCD_MULTIPLEX
  DDRA = 0x00; // set LCD_DAT pins to input
#endif
  PORTC_LCD_CTRL |=  (1<<OUT_C_LCD_CS1);
}

inline void lcd_init()
{
  // /home/thus/txt/datasheets/lcd/KS0713.pdf
  // ~/txt/flieger/ST7565RV17.pdf  from http://www.glyn.de/content.asp?wdid=132&sid=

  PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_RES);  //LCD_RES
  delay_1us();
  delay_1us();//    f520  call  0xf4ce  delay_1us() ; 0x0xf4ce
  PORTC_LCD_CTRL |= (1<<OUT_C_LCD_RES); //  f524  sbi 0x15, 2 IOADR-PORTC_LCD_CTRL; 21           1
  delay_1_5us(1500);
  lcdSendCtl(0xe2); //Initialize the internal functions
  lcdSendCtl(0xae); //DON = 0: display OFF
  lcdSendCtl(0xa1); //ADC = 1: reverse direction(SEG132->SEG1)
  lcdSendCtl(0xA6); //REV = 0: non-reverse display
  lcdSendCtl(0xA4); //EON = 0: normal display. non-entire
  lcdSendCtl(0xA2); // Select LCD bias=0
  lcdSendCtl(0xC0); //SHL = 0: normal direction (COM1->COM64)
  lcdSendCtl(0x2F); //Control power circuit operation VC=VR=VF=1
  lcdSendCtl(0x25); //Select int resistance ratio R2 R1 R0 =5
  lcdSendCtl(0x81); //Set reference voltage Mode
  lcdSendCtl(0x22); // 24 SV5 SV4 SV3 SV2 SV1 SV0 = 0x18
  lcdSendCtl(0xAF); //DON = 1: display ON
  g_eeGeneral.contrast = 0x22;
}

void lcdSetRefVolt(uint8_t val)
{
  lcdSendCtl(0x81);
  lcdSendCtl(val);
}

#ifndef SIMU
void refreshDisplay()
{
  uint8_t *p=displayBuf;
  for(uint8_t y=0; y < 8; y++) {
    lcdSendCtl(0x04);
    lcdSendCtl(0x10); //column addr 0
    lcdSendCtl( y | 0xB0); //page addr y
    PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_CS1);
#ifdef LCD_MULTIPLEX
    DDRA = 0xFF; // set LCD_DAT pins to output
#endif
    PORTC_LCD_CTRL |=  (1<<OUT_C_LCD_A0);
    PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_RnW);
    for(uint8_t x=128; x>0; --x) {
      PORTA_LCD_DAT = *p++;
      PORTC_LCD_CTRL |= (1<<OUT_C_LCD_E);
      PORTC_LCD_CTRL &= ~(1<<OUT_C_LCD_E);
    }
    PORTC_LCD_CTRL |=  (1<<OUT_C_LCD_A0);
    PORTC_LCD_CTRL |=  (1<<OUT_C_LCD_CS1);
  }
}
#endif

#endif

#ifdef SIMU
void refreshDisplay()
{
  memcpy(lcd_buf, displayBuf, sizeof(displayBuf));
  lcd_refresh = true;
}
#endif

