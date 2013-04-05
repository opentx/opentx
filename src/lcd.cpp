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

#include "opentx.h"

uint8_t displayBuf[DISPLAY_BUF_SIZE];
#define DISPLAY_END (displayBuf+DISPLAY_PLAN_SIZE)
#define ASSERT_IN_DISPLAY(p) assert((p) >= displayBuf && (p) < DISPLAY_END)

void lcd_clear()
{
  memset(displayBuf, 0, sizeof(displayBuf));
}

void lcd_img(xcoord_t x, uint8_t y, const pm_uchar * img, uint8_t idx, LcdFlags att)
{
  const pm_uchar *q = img;
#if LCD_W >= 260
  xcoord_t w   = pgm_read_byte(q++);
  if (w == 255) w += pgm_read_byte(q++);
#else
  uint8_t w    = pgm_read_byte(q++);
#endif
  uint8_t hb   = (pgm_read_byte(q++)+7)/8;
  bool    inv  = (att & INVERS) ? true : (att & BLINK ? BLINK_ON_PHASE : false);
  q += idx*w*hb;
  for (uint8_t yb = 0; yb < hb; yb++) {
    uint8_t *p = &displayBuf[ (y / 8 + yb) * LCD_W + x ];
    for (xcoord_t i=0; i<w; i++){
      uint8_t b = pgm_read_byte(q++);
      ASSERT_IN_DISPLAY(p);
#if defined(PCBTARANIS)
      uint8_t val = inv ? ~b : b;
      if (!(att & GREY(1)))
        *p = val;
      if (!(att & GREY(2)))
        *(p+DISPLAY_PLAN_SIZE) = val;
      if (!(att & GREY(4)))
        *(p+2*DISPLAY_PLAN_SIZE) = val;
      if (!(att & GREY(8)))
        *(p+3*DISPLAY_PLAN_SIZE) = val;
      p++;
#else
      *p++ = inv ? ~b : b;
#endif
    }
  }
}

uint8_t lcdLastPos;

void lcd_putcAtt(xcoord_t x, uint8_t y, const unsigned char c, LcdFlags flags)
{
  uint8_t *p = &displayBuf[ y / 8 * LCD_W + x ];

#if defined(CPUARM)
  const pm_uchar *q = (c < 0xC0) ? &font_5x7[(c-0x20)*5+4] : &font_5x7_extra[(c-0xC0)*5+4];
#else
  const pm_uchar *q = &font_5x7[(c-0x20)*5+4];
#endif

  bool inv = false;
  if (flags & BLINK) {
    if (BLINK_ON_PHASE) {
      if (flags & INVERS)
        inv = true;
      else
        return;
    }
  }
  else if (flags & INVERS) {
    inv = true;
  }

  if (flags & DBLSIZE) {
    /* each letter consists of ten top bytes followed by
     * by ten bottom bytes (20 bytes per * char) */
    q = &font_10x14[((uint16_t)c-0x20)*20];
    for (int8_t i=11; i>=0; i--) {
      if (flags & CONDENSED && i<=1) break;
      uint8_t b1=0, b2=0;
      if (i>1) {
        b1 = pgm_read_byte(q++); /*top byte*/
        b2 = pgm_read_byte(q++); /*top byte*/
      }
      if (inv) {
        b1 = ~b1;
        b2 = ~b2;
      }   
      if(&p[LCD_W+1] < DISPLAY_END) {
        ASSERT_IN_DISPLAY(p);
        ASSERT_IN_DISPLAY(p+LCD_W);
        LCD_BYTE_FILTER(p, 0, b1);
        LCD_BYTE_FILTER(p+LCD_W, 0, b2);
        p++;
      }   
    }   
  }
#if defined(CPUARM)
  else if (flags & MIDSIZE) {
    /* each letter consists of ten top bytes followed by
     * by ten bottom bytes (20 bytes per * char) */
    q = &font_8x10[((uint16_t)c-0x20)*16];
    for (int8_t i=9; i>=0; i--) {
      uint8_t b1=0, b2=0;
      if (flags & CONDENSED && i<=1) break;
      if (i!=0 && i!=9) {
        b1 = pgm_read_byte(q++); /*top byte*/
        b2 = pgm_read_byte(q++); /*top byte*/
      }
      if (inv) {
        b1 = ~b1;
        b2 = ~b2;
      }
      uint8_t ym8 = (y & 0x07);
      if (&p[LCD_W+1] < DISPLAY_END) {
        LCD_BYTE_FILTER(p, ~(0xff << ym8), b1 << ym8);
        uint8_t *r = p + LCD_W;
        if (r<DISPLAY_END) {
          if (ym8)
            LCD_BYTE_FILTER(r, ~(0xff >> (8-ym8)), b1 >> (8-ym8));
          LCD_BYTE_FILTER(r, ~(0x0f << ym8), (b2&0x0f) << ym8);
          if (ym8) {
            r = r + LCD_W;
            if (r<DISPLAY_END)
              LCD_BYTE_FILTER(r, ~(0x0f >> (8-ym8)), (b2&0x0f) >> (8-ym8));
          }
        }
        p++;
      }
    }
  }
  else if (flags & SMLSIZE) {
    q = &font_4x6[((uint16_t)c-0x20)*5+4];
    uint8_t ym8 = (y & 0x07);
    p += 4;
    for (int8_t i=4; i>=0; i--) {
      uint8_t b = pgm_read_byte(q--);
      if (inv) b = ~b & 0x7f;
      if (p<DISPLAY_END) {
        LCD_BYTE_FILTER(p, ~(0x7f << ym8), b << ym8);
        if (ym8) {
          uint8_t *r = p + LCD_W;
          if (r<DISPLAY_END)
            LCD_BYTE_FILTER(r, ~(0x7f >> (8-ym8)), b >> (8-ym8));
        }
      }
      p--;
    }
  }
  else if (flags & TINSIZE) {
    q = &font_3x5[((uint16_t)c-0x2D)*3+2];
    uint8_t ym8 = (y & 0x07);
    p += 3;
    for (int8_t i=3; i>=0; i--) {
      uint8_t b = (i!=3 ? pgm_read_byte(q--) : 0);
      if (inv) b = ~b & 0x3f;
      if (p<DISPLAY_END) {
        LCD_BYTE_FILTER(p, ~(0x3f << ym8), b << ym8);
        if (ym8) {
          uint8_t *r = p + LCD_W;
          if (r<DISPLAY_END)
            LCD_BYTE_FILTER(r, ~(0x3f >> (8-ym8)), b >> (8-ym8));
        }
      }
      p--;
    }
  }
#endif
  else {
    uint8_t condense=0;

    if (flags & CONDENSED) {
      *p = inv ? ~0 : 0;
      condense=1;
    }

    uint8_t ym8 = (y & 0x07);
    p += 5;
    for (uint8_t i=6, b=0; i>0; i--, b=pgm_read_byte(q--)) {
      if (inv) b = ~b;

      if (condense && i==2) {
        /*condense the letter by skipping column 4 */
        continue;
      }

      if (p<DISPLAY_END) {
        LCD_BYTE_FILTER(p, ~(0xff << ym8), b << ym8);
        if (ym8) {
          uint8_t *r = p + LCD_W;
          if (r<DISPLAY_END)
            LCD_BYTE_FILTER(r, ~(0xff >> (8-ym8)), b >> (8-ym8));
        }
#if defined(BOLD_FONT)
        if (flags & BOLD) {
          ASSERT_IN_DISPLAY(p+1);
          if (inv)
            LCD_BYTE_FILTER(p+1, b << ym8, 0);
          else
            LCD_BYTE_FILTER(p+1, 0xff, b << ym8);
        }
#endif
      }
      p--;
    }
  }
}

void lcd_putc(xcoord_t x, uint8_t y, const unsigned char c)
{
  lcd_putcAtt(x, y, c, 0);
}

void lcd_putsiAtt(xcoord_t x, uint8_t y,const pm_char * s,uint8_t idx, LcdFlags flags)
{
  uint8_t length;
  length = pgm_read_byte(s++);
  lcd_putsnAtt(x,y,s+length*idx,length,flags & ~(BSS|ZCHAR));
}

void lcd_putsnAtt(xcoord_t x, uint8_t y, const pm_char * s, uint8_t len, LcdFlags mode)
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
    if (!c || x>LCD_W-6) break;
    if (c >= 0x20) {
#if defined(CPUARM)
      if ((mode & MIDSIZE) && ((c>='a'&&c<='z')||(c>='0'&&c<='9'))) {
        lcd_putcAtt(x, y, c, mode);
        x-=1;
      }
      else
#endif
      lcd_putcAtt(x, y, c, mode);
      x += FW;
      if (c == '|') x -= 4;
      if (mode&DBLSIZE) x += FW-1;
      else if (mode&MIDSIZE) x += FW-3;
      else if (mode&SMLSIZE) x -= 1;
    }
    else {
      x += (c*FW/2);
    }
    s++;
    len--;
  }
  lcdLastPos = x;
#if defined(CPUARM)
  if (mode&MIDSIZE)
    lcdLastPos += 1;
#endif
}

void lcd_putsn(xcoord_t x, uint8_t y, const pm_char * s, uint8_t len)
{
  lcd_putsnAtt(x, y, s, len, 0);
}

void lcd_putsAtt(xcoord_t x, uint8_t y, const pm_char * s, LcdFlags flags)
{
  lcd_putsnAtt(x, y, s, 255, flags);
}

void lcd_puts(xcoord_t x, uint8_t y, const pm_char * s)
{
  lcd_putsAtt(x, y, s, 0);
}

void lcd_putsLeft(uint8_t y, const pm_char * s)
{
  lcd_puts(0, y, s);
}

void lcd_outhex4(xcoord_t x, uint8_t y, uint16_t val)
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

void lcd_outdez8(xcoord_t x, uint8_t y, int8_t val)
{
  lcd_outdezAtt(x, y, val);
}

void lcd_outdezAtt(xcoord_t x, uint8_t y, lcdint_t val, LcdFlags flags)
{
  lcd_outdezNAtt(x, y, val, flags);
}

void lcd_outdezNAtt(xcoord_t x, uint8_t y, lcdint_t val, LcdFlags flags, uint8_t len)
{
  uint8_t fw = FWNUM;
  int8_t mode = MODE(flags);
  bool dblsize = flags & DBLSIZE;
#if defined(CPUARM)
  bool midsize = flags & MIDSIZE;
  bool tinsize = flags & TINSIZE;
#else
#define midsize 0
#define tinsize 0
#endif

  bool neg = false;
  if (flags & UNSIGN) { flags -= UNSIGN; }
  else if (val < 0) { neg=true; val=-val; }

  xcoord_t xn = 0;
  uint8_t ln = 2;

  if (mode != MODE(LEADING0)) {
    len = 1;
#if defined(CPUARM)
    uint32_t tmp = ((uint32_t)val) / 10;
#else
    uint16_t tmp = ((uint16_t)val) / 10;
#endif
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
  else if (midsize) {
    flags |= CONDENSED;
    fw += FWNUM-3;
  }
  else if (tinsize) {
    fw -= 1;
    x += 1;
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
      x += ((dblsize|midsize) ? 7 : FWNUM);
  }

  lcdLastPos = x;
  x -= fw + 1;

  for (uint8_t i=1; i<=len; i++) {
    div_t qr = div((uint16_t)val, 10);
    char c = qr.rem + '0';
    LcdFlags f = flags;
#if !defined(PCBTARANIS)
    if (dblsize) {
      if (c=='1' && i==len && xn>x+10) { x+=2; f|=CONDENSED; }
      if ((uint16_t)val >= 1000) { x+=FWNUM; f&=~DBLSIZE; }
    }
#endif
    lcd_putcAtt(x, y, c, f);
    if (mode==i) {
      flags &= ~PREC2; // TODO not needed but removes 20bytes, could be improved for sure, check asm
      if (dblsize) {
        xn = x;
        if (c>='1' && c<='3') ln++;
        uint8_t tn = (qr.quot) % 10;
        if (tn==2 || tn==4) {
          if (c=='4') { xn++; }
          else { xn--; ln++; }
        }
      }
      else if (midsize) {
        x -= 3;
        xn = x+1;
      }
      else if (tinsize) {
        x--;
        lcd_plot(x-1, y+4);
        if ((flags&INVERS) && ((~flags & BLINK) || BLINK_ON_PHASE)) {
          lcd_vline(x-1, y, 6);
          lcd_vline(x, y, 6);
        }
        x--;
      }
      else {
        x--;
        lcd_plot(x, y+6);
        if ((flags&INVERS) && ((~flags & BLINK) || BLINK_ON_PHASE))
          lcd_vline(x, y, 8);
        x--;
      }
    }
#if !defined(PCBTARANIS)
    if (dblsize && (uint16_t)val >= 1000 && (uint16_t)val < 10000) x-=2;
#endif
    val = qr.quot;
    x-=fw;
  }

  if (xn) {
    if (midsize) {
      if ((flags&INVERS) && ((~flags & BLINK) || BLINK_ON_PHASE)) {
        lcd_vline(xn, y, 12);
        lcd_vline(xn+1, y, 12);
      }
      lcd_hline(xn, y+9, 2);
      lcd_hline(xn, y+10, 2);
    }
    else {
      y &= ~0x07;
      lcd_hline(xn, (y & ~0x07)+2*FH-3, ln);
      lcd_hline(xn, y+2*FH-2, ln);
    }
  }

  if (neg) lcd_putcAtt(x, y, '-', flags);
}

#if defined(PCBTARANIS)
void lcd_mask(uint8_t *p, uint8_t mask, LcdFlags att)
{
  ASSERT_IN_DISPLAY(p);

  if ((att&FILL_WHITE) && ((*p&mask) || (*(p+DISPLAY_PLAN_SIZE)&mask) || (*(p+2*DISPLAY_PLAN_SIZE)&mask) || (*(p+3*DISPLAY_PLAN_SIZE)&mask))) {
    return;
  }

  if (!(att & GREY(1))) {
    if (att & FORCE)
      *p |= mask;
    else if (att & ERASE)
      *p &= ~mask;
    else
      *p ^= mask;
  }

  p += DISPLAY_PLAN_SIZE;
  if (!(att & GREY(2))) {
    if (att & FORCE)
      *p |= mask;
    else if (att & ERASE)
      *p &= ~mask;
    else
      *p ^= mask;
  }

  p += DISPLAY_PLAN_SIZE;
  if (!(att & GREY(4))) {
    if (att & FORCE)
      *p |= mask;
    else if (att & ERASE)
      *p &= ~mask;
    else
      *p ^= mask;
  }

  p += DISPLAY_PLAN_SIZE;
  if (!(att & GREY(8))) {
    if (att & FORCE)
      *p |= mask;
    else if (att & ERASE)
      *p &= ~mask;
    else
      *p ^= mask;
  }
}
#else
void lcd_mask(uint8_t *p, uint8_t mask, LcdFlags att)
{
  ASSERT_IN_DISPLAY(p);

  if (att & FORCE)
    *p |= mask;
  else if (att & ERASE)
    *p &= ~mask;
  else
    *p ^= mask;
}
#endif

void lcd_plot(xcoord_t x, uint8_t y, LcdFlags att)
{
  uint8_t *p = &displayBuf[ y / 8 * LCD_W + x ];
  if (p<DISPLAY_END)
    lcd_mask(p, BITMASK(y%8), att);
}

void lcd_hlineStip(xcoord_t x, uint8_t y, xcoord_t w, uint8_t pat, LcdFlags att)
{
  if (y >= LCD_H) return;
  if (x+w > LCD_W) { w = LCD_W - x; }

  uint8_t *p  = &displayBuf[ y / 8 * LCD_W + x ];
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

void lcd_hline(xcoord_t x, uint8_t y, xcoord_t w, LcdFlags att)
{
  lcd_hlineStip(x, y, w, 0xff, att);
}

#if defined(CPUM64)
void lcd_vlineStip(xcoord_t x, int8_t y, int8_t h, uint8_t pat)
{
  if (x >= LCD_W) return;
  if (h<0) { y+=h; h=-h; }
  if (y<0) { h+=y; y=0; }
  if (y+h > LCD_H) { h = LCD_H - y; }

  if (pat==DOTTED && !(y%2))
    pat = ~pat;

  uint8_t *p  = &displayBuf[ y / 8 * LCD_W + x ];
  y = (y & 0x07);
  if (y) {
    ASSERT_IN_DISPLAY(p);
    *p ^= ~(BITMASK(y)-1) & pat;
    p += LCD_W;
    h -= 8-y;
  }
  while (h>0) {
    ASSERT_IN_DISPLAY(p);
    *p ^= pat;
    p += LCD_W;
    h -= 8;
  }
  if (h < 0) h += 8;
  if (h) {
    p -= LCD_W;
    ASSERT_IN_DISPLAY(p);
    *p ^= ~(BITMASK(h)-1) & pat;
  }
}
#else
// allows the att parameter...
void lcd_vlineStip(xcoord_t x, int8_t y, int8_t h, uint8_t pat, LcdFlags att)
{
  if (x >= LCD_W) return;
  if (h<0) { y+=h; h=-h; }
  if (y<0) { h+=y; y=0; }
  if (y+h > LCD_H) { h = LCD_H - y; }

  if (pat==DOTTED && !(y%2))
    pat = ~pat;

  uint8_t *p  = &displayBuf[ y / 8 * LCD_W + x ];
  y = (y & 0x07);
  if (y) {
    ASSERT_IN_DISPLAY(p);
    uint8_t msk = ~(BITMASK(y)-1);
    h -= 8-y;
    if (h < 0)
      msk -= ~(BITMASK(8+h)-1);
    lcd_mask(p, msk & pat, att);
    p += LCD_W;
  }
  while (h>=8) {
    ASSERT_IN_DISPLAY(p);
    lcd_mask(p, pat, att);
    p += LCD_W;
    h -= 8;
  }
  if (h>0) {
    ASSERT_IN_DISPLAY(p);
    lcd_mask(p, (BITMASK(h)-1) & pat, att);
  }
}
#endif

void lcd_vline(xcoord_t x, int8_t y, int8_t h)
{
  lcd_vlineStip(x, y, h, 0xff);
}

void lcd_rect(xcoord_t x, uint8_t y, xcoord_t w, uint8_t h, uint8_t pat, LcdFlags att)
{
  lcd_vlineStip(x, y, h, pat);
  lcd_vlineStip(x+w-1, y, h, pat);
  if (~att & ROUND) { x+=1; w-=2; }
  lcd_hlineStip(x, y+h-1, w, pat);
  lcd_hlineStip(x, y, w, pat);
}

void lcd_filled_rect(xcoord_t x, int8_t y, xcoord_t w, uint8_t h, uint8_t pat, LcdFlags att)
{
#if defined(CPUM64)
  for (int8_t i=y; i<y+h; i++) {
    lcd_hlineStip(x, i, w, pat, att);
    pat = (pat >> 1) + ((pat & 1) << 7);
  }
#else
  for (int8_t i=y; i<y+h; i++) {
    if ((att&ROUND) && (i==y || i==y+h-1))
      lcd_hlineStip(x+1, i, w-2, pat, att);
    else
      lcd_hlineStip(x, i, w, pat, att);
    pat = (pat >> 1) + ((pat & 1) << 7);
  }
#endif
}

void lcd_invert_line(int8_t y)
{
  uint8_t *p  = &displayBuf[y * LCD_W];
  for (xcoord_t x=0; x<LCD_W; x++) {
    ASSERT_IN_DISPLAY(p);
#if defined(PCBTARANIS)
    *(p+3*DISPLAY_PLAN_SIZE) ^= 0xff;
    *(p+2*DISPLAY_PLAN_SIZE) ^= 0xff;
    *(p+DISPLAY_PLAN_SIZE)   ^= 0xff;
#endif
    *p++ ^= 0xff;
  }
}

#if !defined(PCBTARANIS) // TODO test inversion
void lcdDrawTelemetryTopBar()
{
  putsModelName(0, 0, g_model.name, g_eeGeneral.currModel, 0);
  uint8_t att = (g_vbat100mV < g_eeGeneral.vBatWarn ? BLINK : 0);
  putsVBat(14*FW,0,att);
  if (g_model.timers[0].mode) {
    att = (s_timerState[0]==TMR_BEEPING ? BLINK : 0);
    putsTime(17*FW+5*FWNUM+1, 0, s_timerVal[0], att, att);
  }
  lcd_invert_line(0);
}
#else
void lcdDrawTelemetryTopBar()
{
  putsModelName(0, 0, g_model.name, g_eeGeneral.currModel, 0);
  uint8_t att = (g_vbat100mV < g_eeGeneral.vBatWarn ? BLINK : 0);
  putsVBat(16*FW+2,0,att);
  if (g_model.timers[0].mode) {
    att = (s_timerState[0]==TMR_BEEPING ? BLINK : 0);
    putsTime(22*FW+5*FWNUM+1, 0, s_timerVal[0], att, att);
    lcd_putsiAtt(18*FW+2, 1, STR_VTELEMCHNS, TELEM_TM1, SMLSIZE);
  }
  if (g_model.timers[1].mode) {
    att = (s_timerState[1]==TMR_BEEPING ? BLINK : 0);
    putsTime(31*FW+5*FWNUM+1, 0, s_timerVal[1], att, att);
    lcd_putsiAtt(27*FW+2, 1, STR_VTELEMCHNS, TELEM_TM2, SMLSIZE);
  }
  lcd_invert_line(0);
}
#endif

void putsTime(xcoord_t x, uint8_t y, putstime_t tme, LcdFlags att, LcdFlags att2)
{
  div_t qr;

  if (!(att & LEFT)) {
    if (att & DBLSIZE)
      x -= 5*(2*FWNUM)-4;
    else if (att & MIDSIZE)
      x -= 5*8-4;
    else
      x -= 5*FWNUM+1;
  }

  if (tme < 0) {
    lcd_putcAtt(x - ((att & DBLSIZE) ? FW+3 : ((att & MIDSIZE) ? FW+1 : FWNUM)), y, '-', att);
    tme = -tme;
  }

  qr = div(tme, 60);

#if defined(PCBTARANIS)
  if (att & MIDSIZE) {
    div_t qr2 = div(qr.quot, 60);
    LCD_2DOTS(x+2*8-6, y, att);
    lcd_outdezNAtt(x, y, qr2.quot, att|LEADING0|LEFT, 2);
    qr.quot = qr2.rem;
    x += 2*8+1;
  }
#define separator ':'
#elif defined(CPUARM)
  char separator = ':';
  if (tme >= 3600 && (~att & DBLSIZE)) {
    qr = div(qr.quot, 60);
    separator = CHR_HOUR;
  }
#else
#define separator ':'
#endif

  uint8_t x2, x3;
  if (att & DBLSIZE) {
    x2 = x+2*(FW+FWNUM)-3;
    x3 = x+2*(FW+FWNUM)+FW-2;
  }
  else if (att & MIDSIZE) {
    x2 = x+2*8-6;
    x3 = x+2*8+1;
  }
  else {
    x2 = x+2*FWNUM-1;
    x3 = x+2*FWNUM-1+FW;
  }

#if defined(CPUARM)
  if (att & MIDSIZE) {
    LCD_2DOTS(x2, y, att);
  }
  else
#endif
  lcd_putcAtt(x2, y, separator, att&att2);
  lcd_outdezNAtt(x, y, qr.quot, att|LEADING0|LEFT, 2);
  lcd_outdezNAtt(x3, y, qr.rem, att2|LEADING0|LEFT, 2);
}

// TODO to be optimized with putsTelemetryValue
void putsVolts(xcoord_t x, uint8_t y, uint16_t volts, LcdFlags att)
{
  lcd_outdezAtt(x, y, (int16_t)volts, (~NO_UNIT) & (att | ((att&PREC2)==PREC2 ? 0 : PREC1)));
  if (~att & NO_UNIT) lcd_putcAtt(lcdLastPos, y, 'v', att);
}

void putsVBat(xcoord_t x, uint8_t y, LcdFlags att)
{
  putsVolts(x, y, g_vbat100mV, att);
}

void putsStrIdx(xcoord_t x, uint8_t y, const pm_char *str, uint8_t idx, LcdFlags att)
{
  lcd_putsAtt(x, y, str, att);
  if (att & SMLSIZE)
    lcd_outdezNAtt(lcdLastPos+1, y, idx, att|LEFT, 2);
  else
    lcd_outdezNAtt(lcdLastPos, y, idx, att|LEFT, 2);
  lcd_putsAtt(x, y, str, att);
}

void putsMixerSource(xcoord_t x, uint8_t y, uint8_t idx, LcdFlags att)
{
#if defined(PCBTARANIS)
  if (idx < MIXSRC_SW1)
    lcd_putsiAtt(x, y, STR_VSRCRAW, idx, att);
  else if (idx < MIXSRC_PPM1)
    putsSwitches(x, y, SWSRC_SW1+idx-MIXSRC_SW1, att);
#else
  if (idx < MIXSRC_THR)
    lcd_putsiAtt(x, y, STR_VSRCRAW, idx, att);
  else if (idx < MIXSRC_PPM1)
    putsSwitches(x, y, idx-MIXSRC_THR+1+3*(1/*+EXTRA_3POS*/), att);
#endif
  else if (idx < MIXSRC_CH1)
    putsStrIdx(x, y, STR_PPM, idx-MIXSRC_PPM1+1, att);
  else if (idx <= MIXSRC_LAST_CH)
    putsStrIdx(x, y, STR_CH, idx-MIXSRC_CH1+1, att);
#if defined(GVARS) || !defined(CPUM64)
  else if (idx <= MIXSRC_LAST_GVAR)
    putsStrIdx(x, y, STR_GV, idx-MIXSRC_GVAR1+1, att);
#endif
  else
    lcd_putsiAtt(x, y, STR_VTELEMCHNS, idx-MIXSRC_FIRST_TELEM+1, att);
}

void putsChnLetter(xcoord_t x, uint8_t y, uint8_t idx, LcdFlags attr)
{
  lcd_putsiAtt(x, y, STR_RETA123, idx-1, attr);
}

void putsModelName(xcoord_t x, uint8_t y, char *name, uint8_t id, LcdFlags att)
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

void putsSwitches(xcoord_t x, uint8_t y, int8_t idx, LcdFlags att)
{
  if (idx == SWSRC_NONE)
    return lcd_putsiAtt(x, y, STR_MMMINV, 0, att);
  if (idx == SWSRC_ON)
    return lcd_putsiAtt(x, y, STR_OFFON, 1, att);
  if (idx == SWSRC_OFF)
    return lcd_putsiAtt(x, y, STR_OFFON, 0, att);
  if (idx < 0) {
    lcd_vlineStip(x-2, y, 8, 0x5E/*'!'*/);
    idx = -idx;
  }

#if ROTARY_ENCODERS > 0
  else if (idx >= SWSRC_FIRST_ROTENC_SWITCH) {
    idx -= SWSRC_FIRST_ROTENC_SWITCH;
    char suffix = (idx & 1) ? CHR_LONG : CHR_SHORT;
    lcd_putcAtt(x+3*FW, y, suffix, att);
    return lcd_putsiAtt(x, y, STR_VRENCODERS, idx/2, att);
  }
#endif

#if !defined(CPUM64)
  else if (idx >= SWSRC_TRAINER_SHORT) {
    idx -= SWSRC_TRAINER_SHORT;
    lcd_putcAtt(x+3*FW, y, (idx & 1) ? CHR_LONG : CHR_SHORT, att);
#if ROTARY_ENCODERS > 0
    if (idx >= 2) {
      idx -= 2;
      return lcd_putsiAtt(x, y, STR_VRENCODERS, idx/2, att);
    }
    else
#endif
    {
      return lcd_putsiAtt(x, y, STR_VSWITCHES, SWSRC_TRAINER-1, att);
    }
  }
#endif

  if (idx > SWSRC_ON) {
    idx -= SWSRC_ON;
    char suffix = CHR_TOGGLE;
    if (idx != SWSRC_ON && (~att & SWCONDENSED)) lcd_putcAtt(x+3*FW, y, suffix, att);
  }
  lcd_putsiAtt(x, y, STR_VSWITCHES, idx-1, att);
}

#ifdef FLIGHT_PHASES
void putsFlightPhase(xcoord_t x, uint8_t y, int8_t idx, LcdFlags att)
{
  if (idx==0) { lcd_putsiAtt(x, y, STR_MMMINV, 0, att); return; }
  if (idx < 0) { lcd_vlineStip(x-2, y, 8, 0x5E/*'!'*/); idx = -idx; }
  if (att & CONDENSED)
    lcd_outdezNAtt(x+FW*1, y, idx-1, (att & ~CONDENSED), 1);
  else
    putsStrIdx(x, y, STR_FP, idx-1, att);
}
#endif

void putsCurve(xcoord_t x, uint8_t y, int8_t idx, LcdFlags att)
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

void putsTmrMode(xcoord_t x, uint8_t y, int8_t mode, LcdFlags att)
{
  if (mode >= TMR_VAROFS+MAX_PSWITCH+NUM_CSW)
    mode++;

  if (mode < 0) {
    mode = TMR_VAROFS - mode - 1;
    lcd_putcAtt(x-1*FW, y, '!', att);
  }
  else if (mode < TMR_VAROFS) {
    lcd_putsiAtt(x, y, STR_VTMRMODES, mode, att);
    return;
  }

  putsSwitches(x, y, mode-(TMR_VAROFS-1), att);
}

void putsTrimMode(xcoord_t x, uint8_t y, uint8_t phase, uint8_t idx, LcdFlags att)
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

#if ROTARY_ENCODERS > 0
void putsRotaryEncoderMode(xcoord_t x, uint8_t y, uint8_t phase, uint8_t idx, LcdFlags att)
{
#if ROTARY_ENCODERS > 2
  int16_t v;
  if(idx < (NUM_ROTARY_ENCODERS - NUM_ROTARY_ENCODERS_EXTRA))
    v = phaseaddress(phase)->rotaryEncoders[idx];
  else
    v = g_model.rotaryEncodersExtra[phase][idx - (NUM_ROTARY_ENCODERS - NUM_ROTARY_ENCODERS_EXTRA)];
#else
  int16_t v = phaseaddress(phase)->rotaryEncoders[idx];
#endif

  if (v > ROTARY_ENCODER_MAX) {
    uint8_t p = v - ROTARY_ENCODER_MAX - 1;
    if (p >= phase) p++;
    lcd_putcAtt(x, y, '0'+p, att);
  }
  else {
    lcd_putcAtt(x, y, 'a'+idx, att);
  }
}
#endif

void lcdSetContrast()
{
  lcdSetRefVolt(g_eeGeneral.contrast);
}
