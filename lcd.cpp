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

#define font_5x8_x20_x7f (font+2)
#define font_10x16_x20_x7f (font_dblsize+2)

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
  uint8_t *p    = &displayBuf[ y / 8 * DISPLAY_W + x ];

  const pm_uchar    *q = &font_5x8_x20_x7f[ + (c-0x20)*5];

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
    q = &font_10x16_x20_x7f[((uint16_t)c-0x20)*10 + (((uint16_t)c-0x20)/16)*160];
    for(char i=5; i>=0; i--) {
      if (mode & CONDENSED && i==0) break;
      uint8_t b1=0, b2=0, b3=0, b4=0;
      if (i>0) {
        b1 = pgm_read_byte(q); /*top byte*/
        b3 = pgm_read_byte((uint16_t)160+q); /*bottom byte*/
        b2 = pgm_read_byte(++q); /*top byte*/
        b4 = pgm_read_byte((uint16_t)160+q); /*bottom byte*/
      }
      if(inv) {
        b1=~b1;
        b2=~b2;
        b3=~b3;
        b4=~b4;
      }   
      if(&p[DISPLAY_W+1] < DISPLAY_END) {
        p[0]=b1;
        p[1]=b2;
        p[DISPLAY_W] = b3; 
        p[DISPLAY_W+1] = b4; 
        p+=2;
      }   
      q++;
    }   
  }
  else {
    uint8_t condense=0;

    if (mode & CONDENSED) {
        *p++ = inv ? ~0 : 0;
        condense=1;
    }

    uint8_t ym8 = (y % 8);
    for (char i=5; i>=0; i--) {
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

void lcd_putc(uint8_t x,uint8_t y,const char c)
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

  if (flags & DBLSIZE) {
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
      x += (flags & DBLSIZE ? 7 : FWNUM);
  }

  lcd_lastPos = x;
  x -= fw + 1;

  for (uint8_t i=1; i<=len; i++) {
    char c = ((uint16_t)val % 10) + '0';
    uint8_t f = flags;
    if (flags & DBLSIZE) {
      if (c=='1' && i==len) { x+=2; f|=CONDENSED; }
      if (val >= 1000) { x+=FWNUM; f&=~DBLSIZE; }
    }
    lcd_putcAtt(x, y, c, f);
    if (mode==i) {
      flags &= ~PREC2; // TODO not needed but removes 64bytes, could be improved for sure, check asm
      if (flags & DBLSIZE) {
        xn = x;
        if(c=='2' || c=='3' || c=='1') ln++;
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
    if ((flags & DBLSIZE) && val >= 1000 && val < 10000) x-=2;
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
  lcd_outdezAtt(x, y, (int16_t)volts, att | ((att&PREC2)==PREC2 ? 0 : PREC1));
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
  else if (idx<=NUM_STICKS+NUM_POTS+2+3)
    lcd_putsiAtt(x, y, STR_VSRCRAW, idx-1, att);
  else if (idx<=NUM_STICKS+NUM_POTS+2+3+NUM_PPM)
    putsStrIdx(x, y, STR_PPM, idx - (NUM_STICKS+NUM_POTS+2+3), att);
  else if (idx<=NUM_STICKS+NUM_POTS+2+3+NUM_PPM+NUM_CHNOUT)
    putsStrIdx(x, y, STR_CH, idx - (NUM_STICKS+NUM_POTS+2+3+NUM_PPM), att);
  else if (idx<=NUM_STICKS+NUM_POTS+2+3+NUM_PPM+NUM_CHNOUT+MAX_TIMERS)
    putsStrIdx(x, y, STR_TMR, idx - (NUM_STICKS+NUM_POTS+2+3+NUM_PPM+NUM_CHNOUT), att);
#ifdef FRSKY
  else
    lcd_putsiAtt(x, y, STR_VTELEMCHNS, idx-(NUM_STICKS+NUM_POTS+2+3+NUM_PPM+NUM_CHNOUT+MAX_TIMERS), att);
#endif
}

void putsChn(uint8_t x, uint8_t y, uint8_t idx, uint8_t att)
{
  if (idx > 0 && idx <= NUM_CHNOUT)
    putsChnRaw(x, y, idx+20, att);
}

void putsMixerSource(uint8_t x, uint8_t y, uint8_t idx, uint8_t att)
{
  if (idx<=NUM_STICKS+NUM_POTS+2)
    putsChnRaw(x, y, idx, att);
  else if (idx<=NUM_STICKS+NUM_POTS+2+MAX_SWITCH)
    putsSwitches(x, y, idx-NUM_STICKS-NUM_POTS-2, att);
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

#define delay_1us() _delay_us(1)
void delay_1_5us(int ms)
{
  for(int i=0; i<ms; i++) delay_1us();
}

void lcd_init()
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

void refreshDisplay()
{
#ifdef SIMU
  memcpy(lcd_buf, displayBuf, sizeof(displayBuf));
  lcd_refresh = true;
#else
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
#endif
}
