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

#define LCD_BYTE_FILTER(p, keep, add) *(p) = (*(p) & (keep)) | (add)

#if !defined(CPUARM)
void lcd_putcAtt(coord_t x, uint8_t y, const unsigned char c, LcdFlags flags)
{
  lcdWaitDmaEnd();
  uint8_t *p = &displayBuf[ y / 8 * LCD_W + x ];
  const pm_uchar *q = &font_5x7[(c-0x20)*5];

  lcdNextPos = x-1;
  p--;

  bool inv = false;
  if (flags & BLINK) {
    if (BLINK_ON_PHASE) {
      if (flags & INVERS)
        inv = true;
      else {
        return;
      }
    }
  }
  else if (flags & INVERS) {
    inv = true;
  }

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

    uint8_t *lineEnd = &displayBuf[ y / 8 * LCD_W + LCD_W ];

    for (int8_t i=0; i<=6; i++) {
      uint8_t b = 0;
      if (i==0) {
        if (!x || !inv) {
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

      if (p<DISPLAY_END && p<lineEnd) {
        ASSERT_IN_DISPLAY(p);
        uint8_t mask = ~(0xff << ym8);
        LCD_BYTE_FILTER(p, mask, b << ym8);
        if (ym8) {
          uint8_t *r = p + LCD_W;
          if (r<DISPLAY_END)
            LCD_BYTE_FILTER(r, ~mask, b >> (8-ym8));
        }

        if (inv && (ym8 == 1)) *p |= 0x01;
      }
      p++;
      lcdNextPos++;
    }
  }
}
#endif

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

void lcd_plot(coord_t x, coord_t y, LcdFlags att)
{
  lcdWaitDmaEnd();
  uint8_t *p = &displayBuf[ y / 8 * LCD_W + x ];
  if (p<DISPLAY_END)
    lcd_mask(p, BITMASK(y%8), att);
}

void lcd_hlineStip(coord_t x, coord_t y, coord_t w, uint8_t pat, LcdFlags att)
{
  if (y >= LCD_H) return;
  if (x+w > LCD_W) { w = LCD_W - x; }

  lcdWaitDmaEnd();
  uint8_t *p  = &displayBuf[ y / 8 * LCD_W + x ];
  uint8_t msk = BITMASK(y%8);
  while (w--) {
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

#if defined(CPUM64)
void lcd_vlineStip(coord_t x, int8_t y, int8_t h, uint8_t pat)
{
  if (x >= LCD_W) return;
  if (h<0) { y+=h; h=-h; }
  if (y<0) { h+=y; y=0; }
  if (y+h > LCD_H) { h = LCD_H - y; }

  if (pat==DOTTED && !(y%2))
    pat = ~pat;

  lcdWaitDmaEnd();
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
void lcd_vlineStip(coord_t x, scoord_t y, scoord_t h, uint8_t pat, LcdFlags att)
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

  lcdWaitDmaEnd();
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

void lcd_invert_line(int8_t y)
{
  lcdWaitDmaEnd();
  uint8_t *p  = &displayBuf[y * LCD_W];
  for (coord_t x=0; x<LCD_W; x++) {
    ASSERT_IN_DISPLAY(p);
    *p++ ^= 0xff;
  }
}

#if !defined(BOOT)
void lcd_img(coord_t x, coord_t y, const pm_uchar * img, uint8_t idx, LcdFlags att)
{
  const pm_uchar *q = img;
#if LCD_W >= 260
  coord_t w   = pgm_read_byte(q++);
  if (w == 255) w += pgm_read_byte(q++);
#else
  uint8_t w    = pgm_read_byte(q++);
#endif
  uint8_t hb   = (pgm_read_byte(q++)+7)/8;
  bool    inv  = (att & INVERS) ? true : (att & BLINK ? BLINK_ON_PHASE : false);
  q += idx*w*hb;
  lcdWaitDmaEnd();
  for (uint8_t yb = 0; yb < hb; yb++) {
    uint8_t *p = &displayBuf[ (y / 8 + yb) * LCD_W + x ];
    for (coord_t i=0; i<w; i++){
      uint8_t b = pgm_read_byte(q);
      q++;
      ASSERT_IN_DISPLAY(p);
      *p++ = inv ? ~b : b;
    }
  }
}
#endif
