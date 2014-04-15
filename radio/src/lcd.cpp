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

#if defined(LUA)
bool lcd_locked = false;
#endif

void lcd_clear()
{
  memset(displayBuf, 0, sizeof(displayBuf));
}

uint8_t lcdLastPos;
uint8_t lcdNextPos;

void lcd_putcAtt(xcoord_t x, uint8_t y, const unsigned char c, LcdFlags flags)
{
  uint8_t *p = &displayBuf[ y / 8 * LCD_W + x ];

#if defined(CPUARM) && !defined(BOOT)
  const pm_uchar *q = (c < 0xC0) ? &font_5x7[(c-0x20)*5] : &font_5x7_extra[(c-0xC0)*5];
#else
  const pm_uchar *q = &font_5x7[(c-0x20)*5];
#endif

  lcdNextPos = x-1;
  p--;

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

#if !defined(BOOT)
  unsigned char c_remapped = 0;

#if defined(BOLD_SPECIFIC_FONT)
  if (flags & (DBLSIZE+BOLD)) {
#else
  if (flags & DBLSIZE) {
#endif
    // To save space only some DBLSIZE and BOLD chars are available 
    // c has to be remapped. All non existing chars mapped to 0 (space)
	  
    if (c>=',' && c<=':')
      c_remapped = c - ',' + 1;
    else if (c>='A' && c<='Z')
      c_remapped = c - 'A' + 16;
    else if (c>='a' && c<='z')
      c_remapped = c - 'a' + 42;
    else if (c=='_')
      c_remapped = 4;
#if defined(BOLD_SPECIFIC_FONT)
    else if (c!=' ')
      flags &= ~BOLD;
#endif
#if defined(CPUARM)
    if ((c>= 128) && (flags & DBLSIZE))
      c_remapped = c - 60;
#endif

#if defined(BOLD_SPECIFIC_FONT)
  }
  if (flags & DBLSIZE) {
#endif

    /* each letter consists of ten top bytes followed by
     * by ten bottom bytes (20 bytes per * char) */
    q = &font_10x14[((uint16_t)c_remapped)*20];
    for (int8_t i=0; i<=11; i++) {
      uint8_t b1=0, b2=0;
      if (!i) {
        if (!x || !inv) {
          lcdNextPos++;
          p++;
          continue;
        }
      }
      else if (i <= 10) {
        b1 = pgm_read_byte(q++); /*top byte*/
        b2 = pgm_read_byte(q++);
      }
      if ((b1 & b2) == 0xff) continue;
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
        lcdNextPos++;
      }   
    }   
  }
#endif

#if defined(CPUARM) && !defined(BOOT)
  else if (flags & MIDSIZE) {
    /* each letter consists of ten top bytes followed by
     * by ten bottom bytes (20 bytes per * char) */
    q = &font_8x10[((uint16_t)c-0x20)*16];
    for (int8_t i=0; i<=9; i++) {
      uint8_t b1=0, b2=0;
      if (!i) {
        if (!x || !inv) {
          lcdNextPos++;
          p++;
          continue;
        }
      }
      if (i <= 8) {
        b1 = pgm_read_byte(q++); /*top byte*/
        b2 = pgm_read_byte(q++);
      }
      if ((b1 == 0xff) && (b2 == 0x0f)) continue;
      if (inv) {
        b1 = ~b1;
        b2 = ~b2;
      }
      const uint8_t ym8 = (y & 0x07);
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
	lcdNextPos++;
      }
    }
  }
  else if (flags & SMLSIZE) {
    q = (c < 0xC0) ? &font_4x6[(c-0x20)*5] : &font_4x6_extra[(c-0xC0)*5];
    const uint8_t ym8 = (y & 0x07);
    for (int8_t i=0; i<=6; i++) {
      uint8_t b = 0;
      if (!i) {
        if (!x || !inv) {
          lcdNextPos++;
          p++;
          continue;
        }
      }
      else if (i <= 5) b = pgm_read_byte(q++);
      if (b == 0x7f) continue;
      if (inv) b = ~b & 0x7f;
      if (p<DISPLAY_END) {
        LCD_BYTE_FILTER(p, ~(0x7f << ym8), b << ym8);
        if (ym8) {
          uint8_t *r = p + LCD_W;
          if (r<DISPLAY_END)
            LCD_BYTE_FILTER(r, ~(0x7f >> (8-ym8)), b >> (8-ym8));
        }
      }
      p++;
      lcdNextPos++;
    }
  }
  else if (flags & TINSIZE) {
    q = &font_3x5[((uint16_t)c-0x2D)*3];
    const uint8_t ym8 = (y & 0x07);
    for (int8_t i=0; i<=4; i++) {
      uint8_t b = 0;
      if (!i) {
        if (!x || !inv) {
          lcdNextPos++;
          p++;
          continue;
        }
      }
      else if (i <= 3) b= pgm_read_byte(q++);
      if (inv) b = ~b & 0x3f;
      if (p<DISPLAY_END) {
        LCD_BYTE_FILTER(p, ~(0x3f << ym8), b << ym8);
        if (ym8) {
          uint8_t *r = p + LCD_W;
          if (r<DISPLAY_END)
            LCD_BYTE_FILTER(r, ~(0x3f >> (8-ym8)), b >> (8-ym8));
        }
      }
      p++;
      lcdNextPos++;
    }
  }
#endif
  else {
    const uint8_t ym8 = (y & 0x07);
#if defined(BOLD_FONT)
  #if defined(BOLD_SPECIFIC_FONT)
    if (flags & BOLD) {
      q = &font_5x7_B[(c_remapped)*5];
    }
  #else
    uint8_t bb = 0;
    if (inv) bb = 0xff;
  #endif
#endif
    for (int8_t i=0; i<=6; i++) {
      uint8_t b = 0;
      if (i==0) {
        if ( !x || !inv ) {
          lcdNextPos++;
          p++;
          continue;
        }
      }
      else if (i <= 5) {
        b = pgm_read_byte(q++);
      }
      if (b == 0xff) {
        if (flags & FIXEDWIDTH)
          b = 0;
        else
          continue;
      }
      if (inv) b = ~b;
      if ((flags & CONDENSED) && i==2) {
        /*condense the letter by skipping column 3 */
        continue;
      }

#if defined(BOLD_FONT) && !defined(BOLD_SPECIFIC_FONT)
      if (flags & BOLD) {
        uint8_t a;
        if (inv)
          a = b & bb; 
        else
          a = b | bb;
        bb = b;
        b = a;
      }
#endif

      if (p<DISPLAY_END) {
        ASSERT_IN_DISPLAY(p);
        uint8_t mask = ~(0xff << ym8);
        LCD_BYTE_FILTER(p, mask, b << ym8);
        if (ym8) {
          uint8_t *r = p + LCD_W;
          if (r<DISPLAY_END)
            LCD_BYTE_FILTER(r, ~mask, b >> (8-ym8));
        }

#if defined(PCBTARANIS)
        if (inv) {
          if (ym8) lcd_mask(p, 0x01 << (ym8-1), FORCE);
          else if (y) {
            ASSERT_IN_DISPLAY(p - LCD_W);
            lcd_mask(p - LCD_W, 0x80, FORCE);
          }  
        }
#else
        if (inv && (ym8 == 1)) *p |= 0x01;
#endif
      }
      p++;
      lcdNextPos++;
    }
  }
}

void lcd_putc(xcoord_t x, uint8_t y, const unsigned char c)
{
  lcd_putcAtt(x, y, c, 0);
}

void lcd_putsnAtt(xcoord_t x, uint8_t y, const pm_char * s, uint8_t len, LcdFlags mode)
{
  xcoord_t orig_x = x;
  bool setx = false;
  while(len--) {
    unsigned char c;
    switch (mode & (BSS+ZCHAR)) {
      case BSS:
        c = *s;
        break;
#if !defined(BOOT)
      case ZCHAR:
        c = idx2char(*s);
        break;
#endif
      default:
        c = pgm_read_byte(s);
        break;
    }

    if (setx) {
      x = c;
      setx = false;
    }
    else if (!c || x>LCD_W-6) break;
    else if (c >= 0x20) {
      lcd_putcAtt(x, y, c, mode);
      x = lcdNextPos;
    }
    else if (setx) {
      x = c;
      setx = false;
    }
    else if (c == 0x1F) {  //X-coord prefix
      setx = true;
    }
    else if (c == 0x1E) {  //NEWLINE
      x = orig_x;
      y += FH;
#if defined(CPUARM)      
      if (mode & DBLSIZE) y += FH;
      else if (mode & MIDSIZE) y += 4;
      else if (mode & SMLSIZE) y--;
#endif
      if (y >= LCD_H) break;
    }
    else {
      x += (c*FW/2); //EXTENDED SPACE
    }
    s++;
  }
  lcdLastPos = x;
  lcdNextPos = x;
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

#if !defined(BOOT)
void lcd_putsiAtt(xcoord_t x, uint8_t y,const pm_char * s,uint8_t idx, LcdFlags flags)
{
  uint8_t length;
  length = pgm_read_byte(s++);
  lcd_putsnAtt(x,y,s+length*idx,length,flags & ~(BSS|ZCHAR));
}

void lcd_outhex4(xcoord_t x, uint8_t y, uint16_t val)
{
  x+=FWNUM*4+1;
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
  flags &= ~LEADING0;
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
    fw += FWNUM-3;
  }
  else if (tinsize) {
    fw -= 1;
  }
  else {
    if (flags & LEFT) {
      if (mode > 0)
        x += 2;
    }
#if defined(BOLD_FONT) && !defined(CPUM64) || defined(EXTSTD)
    if (flags & BOLD) fw += 1;
#endif
  }

  if (flags & LEFT) {
    x += len * fw;
    if (neg)
      x += ((dblsize|midsize) ? 7 : FWNUM);
  }

  lcdLastPos = x;
  x -= fw;
  if (dblsize) x++;

  for (uint8_t i=1; i<=len; i++) {
    div_t qr = div((uint16_t)val, 10);
    char c = qr.rem + '0';
    LcdFlags f = flags;
#if !defined(PCBTARANIS)
    if (dblsize) {
      if (c=='1' && i==len && xn>x+10) { x+=1; }
      if ((uint16_t)val >= 1000) { x+=FWNUM; f&=~DBLSIZE; }
    }
#endif
    lcd_putcAtt(x, y, c, f);
    if (mode==i) {
      flags &= ~PREC2; // TODO not needed but removes 20bytes, could be improved for sure, check asm
      if (dblsize) {
        xn = x - 2;
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
        x -= 2;
        lcd_putcAtt(x, y, '.', f);
      }
    }
#if !defined(PCBTARANIS)
    if (dblsize && (uint16_t)val >= 1000 && (uint16_t)val < 10000) x-=2;
#endif
    val = qr.quot;
    x-=fw;
#if defined(BOLD_FONT) && !defined(CPUM64) || defined(EXTSTD)
    if (i==len && (flags & BOLD)) x += 1;
#endif
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
#endif

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
  while(w--) {
    if(pat&1) {
      lcd_mask(p, msk, att);
      pat = (pat >> 1) | 0x80;
    }
    else {
      pat = pat >> 1;
    }
    p++;
  }
}

void lcd_hline(xcoord_t x, uint8_t y, xcoord_t w, LcdFlags att)
{
  lcd_hlineStip(x, y, w, 0xff, att);
}

#if defined(CPUARM)
void lcd_line(int x1, int y1, int x2, int y2, LcdFlags att)
{
  int i,dx,dy,sdx,sdy,dxabs,dyabs,x,y,px,py;

  dx=x2-x1;      /* the horizontal distance of the line */
  dy=y2-y1;      /* the vertical distance of the line */
  dxabs=abs(dx);
  dyabs=abs(dy);
  sdx=sgn(dx);
  sdy=sgn(dy);
  x=dyabs>>1;
  y=dxabs>>1;
  px=x1;
  py=y1;

  // VGA[(py<<8)+(py<<6)+px]=color;

  if (dxabs>=dyabs) /* the line is more horizontal than vertical */
  {
    for(i=0;i<dxabs;i++)
    {
      y+=dyabs;
      if (y>=dxabs)
      {
        y-=dxabs;
        py+=sdy;
      }
      px+=sdx;
      lcd_plot(px,py,att);
    }
  }
  else /* the line is more vertical than horizontal */
  {
    for(i=0;i<dyabs;i++)
    {
      x+=dxabs;
      if (x>=dyabs)
      {
        x-=dyabs;
        px+=sdx;
      }
      py+=sdy;
      lcd_plot(px,py,att);
    }
  }
}
#endif

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
#if defined(CPUARM)
  // should never happen on 9X
  if (y >= LCD_H) return;
#endif

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

void lcd_rect(xcoord_t x, uint8_t y, xcoord_t w, uint8_t h, uint8_t pat, LcdFlags att)
{
  lcd_vlineStip(x, y, h, pat);
  lcd_vlineStip(x+w-1, y, h, pat);
  if (~att & ROUND) { x+=1; w-=2; }
  lcd_hlineStip(x, y+h-1, w, pat);
  lcd_hlineStip(x, y, w, pat);
}

#if !defined(BOOT)
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

#if defined(PCBTARANIS)
void lcdDrawTelemetryTopBar()
{
  putsModelName(0, 0, g_model.header.name, g_eeGeneral.currModel, 0);
  uint8_t att = (g_vbat100mV < g_eeGeneral.vBatWarn ? BLINK : 0);
  putsVBat(16*FW+1,0,att);
  if (g_model.timers[0].mode) {
    att = (timersStates[0].val<0 ? BLINK : 0);
    putsTime(22*FW+5*FWNUM+3, 0, timersStates[0].val, att, att);
    lcd_putsiAtt(18*FW+2, 1, STR_VTELEMCHNS, TELEM_TM1, SMLSIZE);
  }
  if (g_model.timers[1].mode) {
    att = (timersStates[1].val<0 ? BLINK : 0);
    putsTime(31*FW+5*FWNUM+3, 0, timersStates[1].val, att, att);
    lcd_putsiAtt(27*FW+2, 1, STR_VTELEMCHNS, TELEM_TM2, SMLSIZE);
  }
  lcd_invert_line(0);
}
#else
void lcdDrawTelemetryTopBar()
{
  putsModelName(0, 0, g_model.header.name, g_eeGeneral.currModel, 0);
  uint8_t att = (g_vbat100mV < g_eeGeneral.vBatWarn ? BLINK : 0);
  putsVBat(14*FW,0,att);
  if (g_model.timers[0].mode) {
    att = (timersStates[0].val<0 ? BLINK : 0);
    putsTime(17*FW+5*FWNUM+1, 0, timersStates[0].val, att, att);
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
    lcd_putcAtt(x - ((att & DBLSIZE) ? FW+2 : ((att & MIDSIZE) ? FW+0 : FWNUM)), y, '-', att);
    tme = -tme;
  }

  qr = div(tme, 60);

#if defined(PCBTARANIS)
#define separator ':'
  if (att & MIDSIZE) {
    div_t qr2 = div(qr.quot, 60);
    lcd_outdezNAtt(x, y, qr2.quot, att|LEADING0|LEFT, 2);
    lcd_putcAtt(lcdLastPos, y, separator, att&att2);
    qr.quot = qr2.rem;
    x += 2*8+1;
  }
#elif defined(CPUARM)
  char separator = ':';
  if (tme >= 3600 && (~att & DBLSIZE)) {
    qr = div(qr.quot, 60);
    separator = CHR_HOUR;
  }
#else
#define separator ':'
#endif
  lcd_outdezNAtt(x, y, qr.quot, att|LEADING0|LEFT, 2);
  lcd_putcAtt(lcdLastPos, y, separator, att&att2);
  lcd_outdezNAtt(lcdNextPos, y, qr.rem, att2|LEADING0|LEFT, 2);
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
  lcd_putsAtt(x, y, str, att & ~LEADING0);
  lcd_outdezNAtt(lcdNextPos, y, idx, att|LEFT, 2);
}

void putsMixerSource(xcoord_t x, uint8_t y, uint8_t idx, LcdFlags att)
{
#if defined(PCBTARANIS)
  if (idx == 0) {
    lcd_putsiAtt(x, y, STR_VSRCRAW, 0, att);
  }
  else if (idx <= MIXSRC_LAST_INPUT) {
    lcd_putcAtt(x+2, y+1, CHR_INPUT, TINSIZE);
    lcd_filled_rect(x, y, 7, 7);
    if (ZEXIST(g_model.inputNames[idx-MIXSRC_FIRST_INPUT]))
      lcd_putsnAtt(x+8, y, g_model.inputNames[idx-MIXSRC_FIRST_INPUT], 4, ZCHAR|att);
    else
      lcd_outdezNAtt(x+8, y, idx, att|LEADING0|LEFT, 2);
  }
#endif

#if defined(PCBTARANIS)
  else if (idx <= MIXSRC_LAST_LUA) {
    div_t qr = div(idx-MIXSRC_FIRST_LUA, MAX_SCRIPT_OUTPUTS);
#if defined(LUA_MODEL_SCRIPTS)
    if (qr.quot < MAX_SCRIPTS && qr.rem < scriptInternalData[qr.quot].outputsCount) {
      lcd_putcAtt(x+2, y+1, '1'+qr.quot, TINSIZE);
      lcd_filled_rect(x, y, 7, 7);
      lcd_putsnAtt(x+8, y, scriptInternalData[qr.quot].outputs[qr.rem].name, att & STREXPANDED ? 9 : 4, att);
    }
    else
#endif
    {
      putsStrIdx(x, y, "LUA", qr.quot+1, att);
      lcd_putcAtt(lcdLastPos, y, 'a'+qr.rem, att);
    }
  }
#endif

#if defined(PCBTARANIS)
  else if (idx < MIXSRC_SW1)
    lcd_putsiAtt(x, y, STR_VSRCRAW, idx-MIXSRC_Rud+1, att);
  else if (idx <= MIXSRC_LAST_LOGICAL_SWITCH)
    putsSwitches(x, y, SWSRC_SW1+idx-MIXSRC_SW1, att);
#else
  if (idx < MIXSRC_THR)
    lcd_putsiAtt(x, y, STR_VSRCRAW, idx, att);
  else if (idx < MIXSRC_FIRST_TRAINER)
    putsSwitches(x, y, idx-MIXSRC_THR+1+3*(1/*+EXTRA_3POS*/), att);
#endif
  else if (idx < MIXSRC_CH1)
    putsStrIdx(x, y, STR_PPM_TRAINER, idx-MIXSRC_FIRST_TRAINER+1, att);
  else if (idx <= MIXSRC_LAST_CH) {
    putsStrIdx(x, y, STR_CH, idx-MIXSRC_CH1+1, att);
#if defined(PCBTARANIS)
    if (ZEXIST(g_model.limitData[idx-MIXSRC_CH1].name) && (att & STREXPANDED)) {
      lcd_putcAtt(lcdLastPos, y, ' ', att);
      lcd_putsnAtt(lcdLastPos+3, y, g_model.limitData[idx-MIXSRC_CH1].name, LEN_CHANNEL_NAME, ZCHAR|att);
    }
#endif
  }
#if defined(GVARS) || !defined(PCBSTD)
  else if (idx <= MIXSRC_LAST_GVAR)
    putsStrIdx(x, y, STR_GV, idx-MIXSRC_GVAR1+1, att);
#endif
  else
    lcd_putsiAtt(x, y, STR_VTELEMCHNS, idx-MIXSRC_FIRST_TELEM+1, att);
}

void putsChnLetter(xcoord_t x, uint8_t y, uint8_t idx, LcdFlags att)
{
  lcd_putsiAtt(x, y, STR_RETA123, idx-1, att);
}

void putsModelName(xcoord_t x, uint8_t y, char *name, uint8_t id, LcdFlags att)
{
  uint8_t len = sizeof(g_model.header.name);
  while (len>0 && !name[len-1]) --len;
  if (len==0) {
    putsStrIdx(x, y, STR_MODEL, id+1, att|LEADING0);
  }
  else {
    lcd_putsnAtt(x, y, name, sizeof(g_model.header.name), ZCHAR|att);
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
    lcd_putcAtt(x-2, y, '!', att);
    idx = -idx;
  }
  lcd_putsiAtt(x, y, STR_VSWITCHES, idx-1, att);
}

#if defined(FLIGHT_MODES)
void putsFlightPhase(xcoord_t x, uint8_t y, int8_t idx, LcdFlags att)
{
  if (idx==0) { lcd_putsiAtt(x, y, STR_MMMINV, 0, att); return; }
  if (idx < 0) { lcd_putcAtt(x-2, y, '!', att); idx = -idx; }
  if (att & CONDENSED)
    lcd_outdezNAtt(x+FW*1, y, idx-1, (att & ~CONDENSED), 1);
  else
    putsStrIdx(x, y, STR_FP, idx-1, att);
}
#endif

#if defined(PCBTARANIS)
void putsCurveRef(xcoord_t x, uint8_t y, CurveRef &curve, LcdFlags att)
{
  if (curve.value != 0) {
    switch (curve.type) {
      case CURVE_REF_DIFF:
        lcd_putcAtt(x, y, 'D', att);
        GVAR_MENU_ITEM(x+FW, y, curve.value, -100, 100, LEFT|att, 0, 0);
        break;

      case CURVE_REF_EXPO:
        lcd_putcAtt(x, y, 'E', att);
        GVAR_MENU_ITEM(x+FW, y, curve.value, -100, 100, LEFT|att, 0, 0);
        break;

      case CURVE_REF_FUNC:
        lcd_putsiAtt(x, y, STR_VCURVEFUNC, curve.value, att);
        break;

      case CURVE_REF_CUSTOM:
        putsCurve(x, y, curve.value, att);
        break;
    }
  }
}

void putsCurve(xcoord_t x, uint8_t y, int8_t idx, LcdFlags att)
{
  if (idx == 0) {
    return lcd_putsiAtt(x, y, STR_MMMINV, 0, att);
  }

  if (idx < 0) {
    lcd_putcAtt(x-3, y, '!', att);
    idx = -idx;
  }

  putsStrIdx(x, y, STR_CV, idx, att);
}
#else
void putsCurve(xcoord_t x, uint8_t y, int8_t idx, LcdFlags att)
{
  if (idx < 0) {
    lcd_putcAtt(x-1*FW, y, '!', att);
    idx = -idx+CURVE_BASE-1;
  }
  if (idx < CURVE_BASE)
    lcd_putsiAtt(x, y, STR_VCURVEFUNC, idx, att);
  else
    putsStrIdx(x, y, STR_CV, idx-CURVE_BASE+1, att);
}
#endif

void putsTimerMode(xcoord_t x, uint8_t y, int8_t mode, LcdFlags att)
{
  if (mode >= 0) {
    if (mode < TMR_VAROFS)
      return lcd_putsiAtt(x, y, STR_VTMRMODES, mode, att);
    else
      mode -= (TMR_VAROFS-1);
  }
  putsSwitches(x, y, mode, att);
}

#if defined(PCBTARANIS)
void putsTrimMode(xcoord_t x, uint8_t y, uint8_t phase, uint8_t idx, LcdFlags att)
{
  trim_t v = getRawTrimValue(phase, idx);
  unsigned int mode = v.mode;
  unsigned int p = mode >> 1;

  if (mode == TRIM_MODE_NONE) {
    lcd_putsAtt(x, y, "--", att);
  }
  else {
    if (mode % 2 == 0)
      lcd_putcAtt(x, y, ':', att|FIXEDWIDTH);
    else
      lcd_putcAtt(x, y, '+', att|FIXEDWIDTH);
    lcd_putcAtt(lcdNextPos, y, '0'+p, att);
  }
}
#else
void putsTrimMode(xcoord_t x, uint8_t y, uint8_t phase, uint8_t idx, LcdFlags att)
{
  trim_t v = getRawTrimValue(phase, idx);

  if (v > TRIM_EXTENDED_MAX) {
    uint8_t p = v - TRIM_EXTENDED_MAX - 1;
    if (p >= phase) p++;
    lcd_putcAtt(x, y, '0'+p, att);
  }
  else {
    putsChnLetter(x, y, idx+1, att);
  }
}
#endif

#if ROTARY_ENCODERS > 0
void putsRotaryEncoderMode(xcoord_t x, uint8_t y, uint8_t phase, uint8_t idx, LcdFlags att)
{
#if ROTARY_ENCODERS > 2
  int16_t v;
  if(idx < (NUM_ROTARY_ENCODERS - NUM_ROTARY_ENCODERS_EXTRA))
    v = phaseAddress(phase)->rotaryEncoders[idx];
  else
    v = g_model.rotaryEncodersExtra[phase][idx - (NUM_ROTARY_ENCODERS - NUM_ROTARY_ENCODERS_EXTRA)];
#else
  int16_t v = phaseAddress(phase)->rotaryEncoders[idx];
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

#if defined(FRSKY) || defined(CPUARM)
void putsTelemetryValue(xcoord_t x, uint8_t y, lcdint_t val, uint8_t unit, uint8_t att)
{
  convertUnit(val, unit);
  lcd_outdezAtt(x, y, val, att & (~NO_UNIT));
  if (!(att & NO_UNIT) && unit != UNIT_RAW) {
    lcd_putsiAtt(lcdLastPos/*+1*/, y, STR_VTELEMUNIT, unit, 0);
  }
}

const pm_uint8_t bchunit_ar[] PROGMEM = {
  UNIT_DIST,    // Alt
  UNIT_RAW,     // Rpm
  UNIT_PERCENT, // Fuel
  UNIT_TEMPERATURE, // T1
  UNIT_TEMPERATURE, // T2
  UNIT_KTS,     // Speed
  UNIT_DIST,    // Dist
  UNIT_DIST,    // GPS Alt
};

void putsTelemetryChannel(xcoord_t x, uint8_t y, uint8_t channel, lcdint_t val, uint8_t att)
{
  switch (channel) {
    case TELEM_TM1-1:
    case TELEM_TM2-1:
      att &= ~NO_UNIT;
      putsTime(x, y, val, att, att);
      break;
#if defined(FRSKY)
    case TELEM_MIN_A1-1:
    case TELEM_MIN_A2-1:
      channel -= TELEM_MIN_A1-TELEM_A1;
      // no break
    case TELEM_A1-1:
    case TELEM_A2-1:
      channel -= TELEM_A1-1;
      // A1 and A2
    {
      lcdint_t converted_value = applyChannelRatio(channel, val);
      if (g_model.frsky.channels[channel].type >= UNIT_RAW) {
        converted_value = div10_and_round(converted_value);
      }
      else {
#if !defined(PCBTARANIS)
        if (abs(converted_value) < 1000) {
          att |= PREC2;
        }
        else {
          converted_value = div10_and_round(converted_value);
          att |= PREC1;
        }
#else
        att |= PREC2;
#endif
      }
      putsTelemetryValue(x, y, converted_value, g_model.frsky.channels[channel].type, att);
      break;
    }
#endif

    case TELEM_CELL-1:
    case TELEM_MIN_CELL-1:
      putsTelemetryValue(x, y, val, UNIT_VOLTS, att|PREC2);
      break;

    case TELEM_TX_VOLTAGE-1:
    case TELEM_VFAS-1:
    case TELEM_CELLS_SUM-1:
    case TELEM_MIN_CELLS_SUM-1:
    case TELEM_MIN_VFAS-1:
      putsTelemetryValue(x, y, val, UNIT_VOLTS, att|PREC1);
      break;

    case TELEM_CURRENT-1:
    case TELEM_MAX_CURRENT-1:
      putsTelemetryValue(x, y, val, UNIT_AMPS, att|PREC1);
      break;

    case TELEM_CONSUMPTION-1:
      putsTelemetryValue(x, y, val, UNIT_MAH, att);
      break;

    case TELEM_POWER-1:
    case TELEM_MAX_POWER-1:
      putsTelemetryValue(x, y, val, UNIT_WATTS, att);
      break;

    case TELEM_ACCx-1:
    case TELEM_ACCy-1:
    case TELEM_ACCz-1:
      putsTelemetryValue(x, y, val, UNIT_RAW, att|PREC2);
      break;

    case TELEM_VSPD-1:
      putsTelemetryValue(x, y, div10_and_round(val), UNIT_RAW, att|PREC1);
      break;

    case TELEM_ASPD-1:
      putsTelemetryValue(x, y, val, UNIT_KTS, att);
      break;

#if defined(CPUARM)
    case TELEM_SWR-1:
#endif
    case TELEM_RSSI_TX-1:
    case TELEM_RSSI_RX-1:
      putsTelemetryValue(x, y, val, UNIT_RAW, att);
      break;

#if defined(FRSKY_SPORT)
    case TELEM_ALT-1:
      putsTelemetryValue(x, y, div10_and_round(val), UNIT_DIST, att|PREC1);
      break;
#elif defined(WS_HOW_HIGH)
    case TELEM_ALT-1:
    case TELEM_MIN_ALT-1:
    case TELEM_MAX_ALT-1:
      if (IS_IMPERIAL_ENABLE() && IS_USR_PROTO_WS_HOW_HIGH()) {
        putsTelemetryValue(x, y, val, UNIT_FEET, att);
        break;
      }
      // no break
#endif

    default:
    {
      uint8_t unit = 1;
      if (channel >= TELEM_MAX_T1-1 && channel <= TELEM_MAX_DIST-1)
        channel -= TELEM_MAX_T1 - TELEM_T1;
      if (channel <= TELEM_GPSALT-1)
        unit = channel + 1 - TELEM_ALT;
      if (channel >= TELEM_MIN_ALT-1 && channel <= TELEM_MAX_ALT-1)
        unit = 0;
      if (channel == TELEM_HDG-1)
        unit = 3;
      putsTelemetryValue(x, y, val, pgm_read_byte(bchunit_ar+unit), att);
      break;
    }
  }
}
#else // defined(FRSKY)
void putsTelemetryChannel(xcoord_t x, uint8_t y, uint8_t channel, lcdint_t val, uint8_t att)
{
  switch (channel) {
    case TELEM_TM1-1:
    case TELEM_TM2-1:
      att &= ~NO_UNIT;
      putsTime(x, y, val, att, att);
      break;

    case TELEM_TX_VOLTAGE-1:
      lcd_outdezAtt(x, y, val, (att|PREC1) & (~NO_UNIT));
      if (!(att & NO_UNIT))
        lcd_putc(lcdLastPos/*+1*/, y, 'v');
      break;
  }
}
#endif

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
      uint8_t b = pgm_read_byte(q);
      q++;
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

void lcdSetContrast()
{
  lcdSetRefVolt(g_eeGeneral.contrast);
}

#endif // !defined(BOOT)
