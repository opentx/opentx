/*
 * Copyright (C) OpenTX
 *
 * Based on code named
 *   th9x - http://code.google.com/p/th9x 
 *   er9x - http://code.google.com/p/er9x
 *   gruvin9x - http://code.google.com/p/gruvin9x
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "../../opentx.h"
#include "../../timers.h"

#if defined(REVPLUS) && defined(LCD_DUAL_BUFFER)
  display_t displayBuf1[DISPLAY_BUFFER_SIZE] __DMA;
  display_t displayBuf2[DISPLAY_BUFFER_SIZE] __DMA;
  display_t * displayBuf = displayBuf1;
#else
  display_t displayBuf[DISPLAY_BUFFER_SIZE] __DMA;
#endif

inline bool lcdIsPointOutside(coord_t x, coord_t y)
{
  return (x<0 || x>=LCD_W || y<0 || y>=LCD_H);
}

void lcdClear()
{
  memset(displayBuf, 0, DISPLAY_BUFFER_SIZE);
}

coord_t lcdLastPos;
coord_t lcdNextPos;

void lcdPutPattern(coord_t x, coord_t y, const uint8_t * pattern, uint8_t width, uint8_t height, LcdFlags flags)
{
  bool blink = false;
  bool inv = false;
  if (flags & BLINK) {
    if (BLINK_ON_PHASE) {
      if (flags & INVERS)
        inv = true;
      else {
        blink = true;
      }
    }
  }
  else if (flags & INVERS) {
    inv = true;
  }

  uint8_t lines = (height+7)/8;
  assert(lines <= 5);

  for (int8_t i=0; i<width+2; i++) {
    if (x<LCD_W) {
      uint8_t b[5] = { 0 };
      if (i==0) {
        if (x==0 || !inv) {
          lcdNextPos++;
          continue;
        }
        else {
          // we need to work on the previous x when INVERS
          x--;
        }
      }
      else if (i<=width) {
        uint8_t skip = true;
        for (uint8_t j=0; j<lines; j++) {
          b[j] = pgm_read_byte(pattern++); /*top byte*/
          if (b[j] != 0xff) {
            skip = false;
          }
        }
        if (skip) {
          if (flags & FIXEDWIDTH) {
            for (uint8_t j=0; j<lines; j++) {
              b[j] = 0;
            }
          }
          else {
            continue;
          }
        }
        if ((flags & CONDENSED) && i==2) {
          /*condense the letter by skipping column 3 */
          continue;
        }
      }

      for (int8_t j=-1; j<=height; j++) {
        bool plot;
        if (j < 0 || ((j == height) && !(FONTSIZE(flags) == SMLSIZE))) {
          plot = false;
          if (height >= 12) continue;
          if (j<0 && !inv) continue;
          if (y+j < 0) continue;
        }
        else {
          uint8_t line = (j / 8);
          uint8_t pixel = (j % 8);
          plot = b[line] & (1 << pixel);
        }
        if (inv) plot = !plot;
        if (!blink) {
          if (flags & VERTICAL)
            lcdDrawPoint(y+j, LCD_H-x, plot ? FORCE : ERASE);
          else
            lcdDrawPoint(x, y+j, plot ? FORCE : ERASE);
        }
      }
    }

    x++;
    lcdNextPos++;
  }
}

void lcdDrawChar(coord_t x, coord_t y, const unsigned char c, LcdFlags flags)
{
  const pm_uchar * q;

  lcdNextPos = x-1;

#if !defined(BOOT)
  uint32_t fontsize = FONTSIZE(flags);
  unsigned char c_remapped = 0;

  if (fontsize == DBLSIZE || (flags&BOLD)) {
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
    else if (c!=' ')
      flags &= ~BOLD;
  }

  if (fontsize == DBLSIZE) {
    if (c >= 0xC0) {
      q = &font_10x14_extra[((uint16_t)(c-0xC0))*20];
    }
    else {
      if (c >= 128)
        c_remapped = c - 60;
      q = &font_10x14[((uint16_t)c_remapped)*20];
    }
    lcdPutPattern(x, y, q, 10, 16, flags);
  }
  else if (fontsize == XXLSIZE) {
    q = &font_22x38_num[((uint16_t)c-'0'+5)*110];
    lcdPutPattern(x, y, q, 22, 38, flags);
  }
  else if (fontsize == MIDSIZE) {
    q = &font_8x10[((uint16_t)c-0x20)*16];
    lcdPutPattern(x, y, q, 8, 12, flags);
  }
  else if (fontsize == SMLSIZE) {
    q = (c < 0xc0 ? &font_4x6[(c-0x20)*5] : &font_4x6_extra[(c-0xc0)*5]);
    lcdPutPattern(x, y, q, 5, 6, flags);
  }
  else if (fontsize == TINSIZE) {
    q = &font_3x5[((uint16_t)c-0x20)*3];
    lcdPutPattern(x, y, q, 3, 5, flags);
  }
#if defined(BOLD_FONT)
  else if (flags & BOLD) {
    q = &font_5x7_B[c_remapped*5];
    lcdPutPattern(x, y, q, 5, 7, flags);
  }
#endif
  else
#endif
  {
#if !defined(BOOT)
    q = (c < 0xC0) ? &font_5x7[(c-0x20)*5] : &font_5x7_extra[(c-0xC0)*5];
#else
    q = &font_5x7[(c-0x20)*5];
#endif
    lcdPutPattern(x, y, q, 5, 7, flags);
  }
}

void lcdDrawChar(coord_t x, coord_t y, const unsigned char c)
{
  lcdDrawChar(x, y, c, 0);
}

void lcdDrawSizedText(coord_t x, coord_t y, const pm_char * s, uint8_t len, LcdFlags flags)
{
  const coord_t orig_x = x;
  const uint8_t orig_len = len;
  uint32_t fontsize = FONTSIZE(flags);
  bool setx = false;
  while (len--) {
#if defined(BOOT)
    unsigned char c = *s;
#else
    unsigned char c = (flags & ZCHAR) ? idx2char(*s) : *s;
#endif

    if (setx) {
      x = c;
      setx = false;
    }
    else if (!c) {
      break;
    }
    else if (c >= 0x20) {
      lcdDrawChar(x, y, c, flags);
      x = lcdNextPos;
    }
    else if (c == 0x1F) {  //X-coord prefix
      setx = true;
    }
    else if (c == 0x1E) {  //NEWLINE
      len = orig_len;
      x = orig_x;
      y += FH;
      if (fontsize == DBLSIZE)
        y += FH;
      else if (fontsize == MIDSIZE)
        y += 4;
      else if (fontsize == SMLSIZE)
        y--;
      if (y >= LCD_H) break;
    }
    else if (c == 0x1D) {  // TAB
      x |= 0x3F;
      x += 1;
    }
    else {
      x += (c*FW/2); // EXTENDED SPACE
    }
    s++;
  }
  lcdLastPos = x;
  lcdNextPos = x;
  if (fontsize == MIDSIZE)
    lcdLastPos += 1;
}

void lcdDrawSizedText(coord_t x, coord_t y, const pm_char * s, uint8_t len)
{
  lcdDrawSizedText(x, y, s, len, 0);
}

void lcdDrawText(coord_t x, coord_t y, const pm_char * s, LcdFlags flags)
{
  lcdDrawSizedText(x, y, s, 255, flags);
}

void lcdDrawText(coord_t x, coord_t y, const pm_char * s)
{
  lcdDrawText(x, y, s, 0);
}

void lcd_putsLeft(coord_t y, const pm_char * s)
{
  lcdDrawText(0, y, s);
}

#if !defined(BOOT)
void lcdDrawTextAtIndex(coord_t x, coord_t y, const pm_char * s,uint8_t idx, LcdFlags flags)
{
  uint8_t length = pgm_read_byte(s++);
  lcdDrawSizedText(x, y, s+length*idx, length, flags & ~ZCHAR);
}

void lcdDrawHexNumber(coord_t x, coord_t y, uint32_t val, LcdFlags flags)
{
  x += FWNUM*4+1;
  for (int i=0; i<4; i++) {
    x -= FWNUM;
    char c = val & 0xf;
    c = c>9 ? c+'A'-10 : c+'0';
    lcdDrawChar(x, y, c, flags|(c>='A' ? CONDENSED : 0));
    val >>= 4;
  }
}

void lcdDraw8bitsNumber(coord_t x, coord_t y, int8_t val)
{
  lcdDrawNumber(x, y, val);
}

void lcdDrawNumber(coord_t x, coord_t y, int32_t val, LcdFlags flags)
{
  lcdDrawNumber(x, y, val, flags, 0);
}

void lcdDrawNumber(coord_t x, coord_t y, int32_t val, LcdFlags flags, uint8_t len)
{
  uint8_t fw = FWNUM;
  int8_t mode = MODE(flags);
  flags &= ~LEADING0;
  uint32_t fontsize = FONTSIZE(flags);
  bool dblsize = (fontsize == DBLSIZE);
  bool xxlsize = (fontsize == XXLSIZE);
  bool midsize = (fontsize == MIDSIZE);
  bool smlsize = (fontsize == SMLSIZE);
  bool tinsize = (fontsize == TINSIZE);

  bool neg = false;
  if (flags & UNSIGN) {
    flags -= UNSIGN;
  }
  else if (val < 0) {
    neg = true;
    val = -val;
  }

  coord_t xn = 0;
  uint8_t ln = 2;

  if (mode != MODE(LEADING0)) {
    len = 1;
    int32_t tmp = val / 10;
    while (tmp) {
      len++;
      tmp /= 10;
    }
    if (len <= mode) {
      len = mode + 1;
    }
  }

  if (dblsize) {
    fw += FWNUM;
  }
  else if (xxlsize) {
    fw += 4*FWNUM-1;
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
#if defined(BOLD_FONT)
    if (flags & BOLD) fw += 1;
#endif
  }

  if (flags & LEFT) {
    x += len * fw;
    if (neg) {
      x += ((xxlsize|dblsize|midsize) ? 7 : FWNUM);
    }
  }

  lcdLastPos = x;
  x -= fw;
  if (dblsize) x++;

  for (uint8_t i=1; i<=len; i++) {
    div_t qr = div(val, 10);
    char c = qr.rem + '0';
    LcdFlags f = flags;
    lcdDrawChar(x, y, c, f);
    if (mode == i) {
      flags &= ~PREC2; // TODO not needed but removes 20bytes, could be improved for sure, check asm
      if (dblsize) {
        xn = x - 2;
        if (c>='2' && c<='3') ln++;
        uint8_t tn = (qr.quot % 10);
        if (tn==2 || tn==4) {
          if (c=='4') {
            xn++;
          }
          else {
            xn--;
            ln++;
          }
        }
      }
      else if (xxlsize) {
        x -= 17;
        lcdDrawChar(x+2, y, '.', f);
      }
      else if (midsize) {
        x -= 3;
        xn = x;
      }
      else if (smlsize) {
        x -= 2;
        lcdDrawPoint(x+1, y+5);
        if ((flags&INVERS) && ((~flags & BLINK) || BLINK_ON_PHASE)) {
          lcdDrawSolidVerticalLine(x+1, y, 7);
        }
      }
      else if (tinsize) {
        x--;
        lcdDrawPoint(x-1, y+4);
        if ((flags&INVERS) && ((~flags & BLINK) || BLINK_ON_PHASE)) {
          lcdDrawSolidVerticalLine(x-1, y-1, 7);
        }
        x--;
      }
      else {
        x -= 2;
        lcdDrawChar(x, y, '.', f);
      }
    }
    val = qr.quot;
    x -= fw;
#if defined(BOLD_FONT) && !defined(CPUM64) || defined(EXTSTD)
    if (i==len && (flags & BOLD)) x += 1;
#endif
  }

  if (xn) {
    if (midsize) {
      if ((flags&INVERS) && ((~flags & BLINK) || BLINK_ON_PHASE)) {
        lcdDrawSolidVerticalLine(xn, y, 12);
        lcdDrawSolidVerticalLine(xn+1, y, 12);
      }
      lcdDrawSolidHorizontalLine(xn, y+9, 2);
      lcdDrawSolidHorizontalLine(xn, y+10, 2);
    }
    else {
      // TODO needed on CPUAVR? y &= ~0x07;
      lcdDrawFilledRect(xn, y+2*FH-3, ln, 2);
    }
  }
  if (neg) lcdDrawChar(x, y, '-', flags);
}
#endif

void lcdDrawSolidHorizontalLine(coord_t x, coord_t y, coord_t w, LcdFlags att)
{
  lcdDrawHorizontalLine(x, y, w, 0xff, att);
}

#if !defined(BOOT)
void lcdDrawLine(coord_t x1, coord_t y1, coord_t x2, coord_t y2, uint8_t pat, LcdFlags att)
{
  if (lcdIsPointOutside(x1, y1) || lcdIsPointOutside(x2, y2)) return;

  int dx = x2-x1;      /* the horizontal distance of the line */
  int dy = y2-y1;      /* the vertical distance of the line */
  int dxabs = abs(dx);
  int dyabs = abs(dy);
  int sdx = sgn(dx);
  int sdy = sgn(dy);
  int x = dyabs>>1;
  int y = dxabs>>1;
  int px = x1;
  int py = y1;

  if (dxabs >= dyabs) {
    /* the line is more horizontal than vertical */
    for (int i=0; i<=dxabs; i++) {
      if ((1<<(px%8)) & pat) {
        lcdDrawPoint(px, py, att);
      }
      y += dyabs;
      if (y>=dxabs) {
        y -= dxabs;
        py += sdy;
      }
      px += sdx;
    }
  }
  else {
    /* the line is more vertical than horizontal */
    for (int i=0; i<=dyabs; i++) {
      if ((1<<(py%8)) & pat) {
        lcdDrawPoint(px, py, att);
      }
      x += dxabs;
      if (x >= dyabs) {
        x -= dyabs;
        px += sdx;
      }
      py += sdy;
    }
  }
}

void lcdDrawSolidVerticalLine(coord_t x, scoord_t y, scoord_t h)
{
  lcdDrawVerticalLine(x, y, h, SOLID);
}
#endif

void lcdDrawRect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t pat, LcdFlags att)
{
  lcdDrawVerticalLine(x, y, h, pat, att);
  lcdDrawVerticalLine(x+w-1, y, h, pat, att);
  if (~att & ROUND) { x+=1; w-=2; }
  lcdDrawHorizontalLine(x, y+h-1, w, pat, att);
  lcdDrawHorizontalLine(x, y, w, pat, att);
}

#if !defined(BOOT)
void lcdDrawFilledRect(coord_t x, scoord_t y, coord_t w, coord_t h, uint8_t pat, LcdFlags att)
{
  for (scoord_t i=y; i<y+h; i++) {
    if ((att&ROUND) && (i==y || i==y+h-1))
      lcdDrawHorizontalLine(x+1, i, w-2, pat, att);
    else
      lcdDrawHorizontalLine(x, i, w, pat, att);
    pat = (pat >> 1) + ((pat & 1) << 7);
  }
}

void lcdDrawTelemetryTopBar()
{
  putsModelName(0, 0, g_model.header.name, g_eeGeneral.currModel, 0);
  uint8_t att = (IS_TXBATT_WARNING() ? BLINK : 0);
  putsVBat(16*FW+1,0,att);
  if (g_model.timers[0].mode) {
    att = (timersStates[0].val<0 ? BLINK : 0);
    putsTimer(22*FW+5*FWNUM+3, 0, timersStates[0].val, att, att);
    putsMixerSource(18*FW+2, 1, MIXSRC_TIMER1, SMLSIZE);
  }
  if (g_model.timers[1].mode) {
    att = (timersStates[1].val<0 ? BLINK : 0);
    putsTimer(31*FW+5*FWNUM+3, 0, timersStates[1].val, att, att);
    putsMixerSource(27*FW+2, 1, MIXSRC_TIMER2, SMLSIZE);
  }
  lcdInvertLine(0);
}

void putsRtcTime(coord_t x, coord_t y, LcdFlags att)
{
  putsTimer(x, y, getValue(MIXSRC_TX_TIME), att, att);
}

void putsTimer(coord_t x, coord_t y, putstime_t tme, LcdFlags att, LcdFlags att2)
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
    lcdDrawChar(x - ((att & DBLSIZE) ? FW+2 : ((att & MIDSIZE) ? FW+0 : FWNUM)), y, '-', att);
    tme = -tme;
  }

  qr = div(tme, 60);

#define separator ':'
  if (att & TIMEHOUR) {
    div_t qr2 = div(qr.quot, 60);
    lcdDrawNumber(x, y, qr2.quot, att|LEADING0|LEFT, 2);
    lcdDrawChar(lcdLastPos, y, separator, att&att2);
    qr.quot = qr2.rem;
    if (att & MIDSIZE)
      x += 17;
    else
      x += 13;
  }
  lcdDrawNumber(x, y, qr.quot, att|LEADING0|LEFT, 2);
  if (att & TIMEBLINK)
    lcdDrawChar(lcdLastPos, y, separator, BLINK);
  else
    lcdDrawChar(lcdLastPos, y, separator, att&att2);
  lcdDrawNumber(lcdNextPos, y, qr.rem, att2|LEADING0|LEFT, 2);
}

// TODO to be optimized with putsValueWithUnit
void putsVolts(coord_t x, coord_t y, uint16_t volts, LcdFlags att)
{
  lcdDrawNumber(x, y, (int16_t)volts, (~NO_UNIT) & (att | ((att&PREC2)==PREC2 ? 0 : PREC1)));
  if (~att & NO_UNIT) lcdDrawChar(lcdLastPos, y, 'V', att);
}

void putsVBat(coord_t x, coord_t y, LcdFlags att)
{
  putsVolts(x, y, g_vbat100mV, att);
}

void putsStrIdx(coord_t x, coord_t y, const pm_char *str, uint8_t idx, LcdFlags att)
{
  lcdDrawText(x, y, str, att & ~LEADING0);
  lcdDrawNumber(lcdNextPos, y, idx, att|LEFT, 2);
}

void putsStickName(coord_t x, coord_t y, uint8_t idx, LcdFlags att)
{
  uint8_t length = STR_VSRCRAW[0];
  lcdDrawSizedText(x, y, STR_VSRCRAW+2+length*(idx+1), length-1, att);
}

void putsMixerSource(coord_t x, coord_t y, uint32_t idx, LcdFlags att)
{
  if (idx == MIXSRC_NONE) {
    lcdDrawTextAtIndex(x, y, STR_VSRCRAW, 0, att); // TODO macro
  }
  else if (idx <= MIXSRC_LAST_INPUT) {
    lcdDrawChar(x+2, y+1, CHR_INPUT, TINSIZE);
    lcdDrawFilledRect(x, y, 7, 7);
    if (ZEXIST(g_model.inputNames[idx-MIXSRC_FIRST_INPUT]))
      lcdDrawSizedText(x+8, y, g_model.inputNames[idx-MIXSRC_FIRST_INPUT], LEN_INPUT_NAME, ZCHAR|att);
    else
      lcdDrawNumber(x+8, y, idx, att|LEADING0|LEFT, 2);
  }

  else if (idx <= MIXSRC_LAST_LUA) {
    div_t qr = div(idx-MIXSRC_FIRST_LUA, MAX_SCRIPT_OUTPUTS);
#if defined(LUA_MODEL_SCRIPTS)
    if (qr.quot < MAX_SCRIPTS && qr.rem < scriptInputsOutputs[qr.quot].outputsCount) {
      lcdDrawChar(x+2, y+1, '1'+qr.quot, TINSIZE);
      lcdDrawFilledRect(x, y, 7, 7);
      lcdDrawSizedText(x+8, y, scriptInputsOutputs[qr.quot].outputs[qr.rem].name, att & STREXPANDED ? 9 : 4, att);
    }
    else
#endif
    {
      putsStrIdx(x, y, "LUA", qr.quot+1, att);
      lcdDrawChar(lcdLastPos, y, 'a'+qr.rem, att);
    }
  }

  else if (idx < MIXSRC_LAST_POT) {
    idx = idx-MIXSRC_Rud;
    if (ZEXIST(g_eeGeneral.anaNames[idx])) {
      if (idx < MIXSRC_FIRST_POT-MIXSRC_Rud )
        lcdDrawChar(x, y, '\307', att); //stick symbol
      else if (idx < MIXSRC_FIRST_SLIDER-MIXSRC_Rud )
        lcdDrawChar(x, y, '\310', att); //pot symbol
      else 
        lcdDrawChar(x, y, '\311', att); //slider symbol
      lcdDrawSizedText(lcdNextPos, y, g_eeGeneral.anaNames[idx], LEN_ANA_NAME, ZCHAR|att);
    }
    else
      lcdDrawTextAtIndex(x, y, STR_VSRCRAW, idx+1, att);
  }
  else if (idx >= MIXSRC_FIRST_SWITCH && idx <= MIXSRC_LAST_SWITCH) {
    idx = idx-MIXSRC_FIRST_SWITCH;
    if (ZEXIST(g_eeGeneral.switchNames[idx])) {
      lcdDrawChar(x, y, '\312', att); //switch symbol
      lcdDrawSizedText(lcdNextPos, y, g_eeGeneral.switchNames[idx], LEN_SWITCH_NAME, ZCHAR|att);
    }
    else
      lcdDrawTextAtIndex(x, y, STR_VSRCRAW, idx+MIXSRC_FIRST_SWITCH-MIXSRC_Rud+1, att);
  }
  else if (idx < MIXSRC_SW1)
    lcdDrawTextAtIndex(x, y, STR_VSRCRAW, idx-MIXSRC_Rud+1, att);
  else if (idx <= MIXSRC_LAST_LOGICAL_SWITCH)
    putsSwitches(x, y, SWSRC_SW1+idx-MIXSRC_SW1, att);
  else if (idx < MIXSRC_CH1)
    putsStrIdx(x, y, STR_PPM_TRAINER, idx-MIXSRC_FIRST_TRAINER+1, att);
  else if (idx <= MIXSRC_LAST_CH) {
    putsStrIdx(x, y, STR_CH, idx-MIXSRC_CH1+1, att);
    if (ZEXIST(g_model.limitData[idx-MIXSRC_CH1].name) && (att & STREXPANDED)) {
      lcdDrawChar(lcdLastPos, y, ' ', att|SMLSIZE);
      lcdDrawSizedText(lcdLastPos+3, y, g_model.limitData[idx-MIXSRC_CH1].name, LEN_CHANNEL_NAME, ZCHAR|att|SMLSIZE);
    }
  }
  else if (idx <= MIXSRC_LAST_GVAR) {
    putsStrIdx(x, y, STR_GV, idx-MIXSRC_GVAR1+1, att);
  }
  else if (idx < MIXSRC_FIRST_TELEM) {
    lcdDrawTextAtIndex(x, y, STR_VSRCRAW, idx-MIXSRC_Rud+1-NUM_LOGICAL_SWITCH-NUM_TRAINER-NUM_CHNOUT-MAX_GVARS, att);
  }
  else {
    idx -= MIXSRC_FIRST_TELEM;
    div_t qr = div(idx, 3);
    lcdDrawSizedText(x, y, g_model.telemetrySensors[qr.quot].label, TELEM_LABEL_LEN, ZCHAR|att);
    if (qr.rem) lcdDrawChar(lcdLastPos, y, qr.rem==2 ? '+' : '-', att);
  }
}

void putsChnLetter(coord_t x, coord_t y, uint8_t idx, LcdFlags att)
{
  lcdDrawTextAtIndex(x, y, STR_RETA123, idx-1, att);
}

void putsModelName(coord_t x, coord_t y, char *name, uint8_t id, LcdFlags att)
{
  uint8_t len = sizeof(g_model.header.name);
  while (len>0 && !name[len-1]) --len;
  if (len==0) {
    putsStrIdx(x, y, STR_MODEL, id+1, att|LEADING0);
  }
  else {
    lcdDrawSizedText(x, y, name, sizeof(g_model.header.name), ZCHAR|att);
  }
}

void putsSwitches(coord_t x, coord_t y, int32_t idx, LcdFlags att)
{
  if (idx == SWSRC_NONE) {
    return lcdDrawTextAtIndex(x, y, STR_VSWITCHES, 0, att);
  }
  else if (idx == SWSRC_OFF) {
    return lcdDrawTextAtIndex(x, y, STR_OFFON, 0, att);
  }

  if (idx < 0) {
    lcdDrawChar(x-2, y, '!', att);
    idx = -idx;
  }

  if (idx <= SWSRC_LAST_SWITCH) {
    div_t swinfo = switchInfo(idx);
    if (ZEXIST(g_eeGeneral.switchNames[swinfo.quot])) {
      lcdDrawSizedText(x, y, g_eeGeneral.switchNames[swinfo.quot], LEN_SWITCH_NAME, ZCHAR|att);
    }
    else {
      lcdDrawChar(x, y, 'S', att);
      lcdDrawChar(lcdNextPos, y, 'A'+swinfo.quot, att);
    }
    char c = "\300-\301"[swinfo.rem];
    lcdDrawChar(lcdNextPos, y, c, att);
  }
  else if (idx <= SWSRC_LAST_MULTIPOS_SWITCH) {
    div_t swinfo = div(idx - SWSRC_FIRST_MULTIPOS_SWITCH, XPOTS_MULTIPOS_COUNT);
    putsStrIdx(x, y, "S", swinfo.quot*10+swinfo.rem+11, att);
  }
  else if (idx <= SWSRC_LAST_TRIM) {
    lcdDrawTextAtIndex(x, y, STR_VSWITCHES, idx-SWSRC_FIRST_TRIM+1, att);
  }
  else if (idx <= SWSRC_LAST_LOGICAL_SWITCH) {
    putsStrIdx(x, y, "L", idx-SWSRC_FIRST_LOGICAL_SWITCH+1, att);
  }
  else if (idx <= SWSRC_ONE) {
    lcdDrawTextAtIndex(x, y, STR_VSWITCHES, idx-SWSRC_ON+1+(2*NUM_STICKS), att);
  }
  else if (idx <= SWSRC_LAST_FLIGHT_MODE) {
    putsStrIdx(x, y, STR_FP, idx-SWSRC_FIRST_FLIGHT_MODE, att);
  }
  else if (idx == SWSRC_TELEMETRY_STREAMING) {
    lcdDrawText(x, y, "Tele", att);
  }
  else if (idx <= SWSRC_LAST_FLIGHT_MODE) {
    putsStrIdx(x, y, STR_FP, idx-SWSRC_FIRST_FLIGHT_MODE, att);
  }
  else if (idx == SWSRC_TELEMETRY_STREAMING) {
    lcdDrawText(x, y, "Tele", att);
  }
  else {
    lcdDrawSizedText(x, y, g_model.telemetrySensors[idx-SWSRC_FIRST_SENSOR].label, TELEM_LABEL_LEN, ZCHAR|att);
  }
}

#if defined(FLIGHT_MODES)
void putsFlightMode(coord_t x, coord_t y, int8_t idx, LcdFlags att)
{
  if (idx==0) { lcdDrawTextAtIndex(x, y, STR_MMMINV, 0, att); return; }
  if (idx < 0) { lcdDrawChar(x-2, y, '!', att); idx = -idx; }
  if (att & CONDENSED)
    lcdDrawNumber(x+FW*1, y, idx-1, (att & ~CONDENSED), 1);
  else
    putsStrIdx(x, y, STR_FP, idx-1, att);
}
#endif

void putsCurveRef(coord_t x, coord_t y, CurveRef &curve, LcdFlags att)
{
  if (curve.value != 0) {
    switch (curve.type) {
      case CURVE_REF_DIFF:
        lcdDrawChar(x, y, 'D', att);
        GVAR_MENU_ITEM(x+FW, y, curve.value, -100, 100, LEFT|att, 0, 0);
        break;

      case CURVE_REF_EXPO:
        lcdDrawChar(x, y, 'E', att);
        GVAR_MENU_ITEM(x+FW, y, curve.value, -100, 100, LEFT|att, 0, 0);
        break;

      case CURVE_REF_FUNC:
        lcdDrawTextAtIndex(x, y, STR_VCURVEFUNC, curve.value, att);
        break;

      case CURVE_REF_CUSTOM:
        putsCurve(x, y, curve.value, att);
        break;
    }
  }
}

void putsCurve(coord_t x, coord_t y, int8_t idx, LcdFlags att)
{
  if (idx == 0) {
    return lcdDrawTextAtIndex(x, y, STR_MMMINV, 0, att);
  }

  if (idx < 0) {
    lcdDrawChar(x-3, y, '!', att);
    idx = -idx;
  }

  putsStrIdx(x, y, STR_CV, idx, att);
}

void putsTimerMode(coord_t x, coord_t y, int32_t mode, LcdFlags att)
{
  if (mode >= 0) {
    if (mode < TMRMODE_COUNT)
      return lcdDrawTextAtIndex(x, y, STR_VTMRMODES, mode, att);
    else
      mode -= (TMRMODE_COUNT-1);
  }
  putsSwitches(x, y, mode, att);
}

void putsTrimMode(coord_t x, coord_t y, uint8_t phase, uint8_t idx, LcdFlags att)
{
  trim_t v = getRawTrimValue(phase, idx);
  unsigned int mode = v.mode;
  unsigned int p = mode >> 1;

  if (mode == TRIM_MODE_NONE) {
    lcdDrawText(x, y, "--", att);
  }
  else {
    if (mode % 2 == 0)
      lcdDrawChar(x, y, ':', att|FIXEDWIDTH);
    else
      lcdDrawChar(x, y, '+', att|FIXEDWIDTH);
    lcdDrawChar(lcdNextPos, y, '0'+p, att);
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

void putsValueWithUnit(coord_t x, coord_t y, int32_t val, uint8_t unit, LcdFlags att)
{
  // convertUnit(val, unit);
  lcdDrawNumber(x, y, val, att & (~NO_UNIT));
  if (!(att & NO_UNIT) && unit != UNIT_RAW) {
    lcdDrawTextAtIndex(lcdLastPos/*+1*/, y, STR_VTELEMUNIT, unit, 0);
  }
}

void displayGpsCoord(coord_t x, coord_t y, char direction, int16_t bp, int16_t ap, LcdFlags att, bool seconds=true)
{
    if (!direction) direction = '-';
    lcdDrawNumber(x, y, bp / 100, att); // ddd before '.'
    lcdDrawChar(lcdLastPos, y, '@', att);
    uint8_t mn = bp % 100; // TODO div_t
    if (g_eeGeneral.gpsFormat == 0) {
      lcdDrawNumber(lcdNextPos, y, mn, att|LEFT|LEADING0, 2); // mm before '.'
      lcdDrawSolidVerticalLine(lcdLastPos, y, 2);
      if (seconds) {
        uint16_t ss = ap * 6 / 10;
        lcdDrawNumber(lcdLastPos+3, y, ss / 100, att|LEFT|LEADING0, 2); // ''
        lcdDrawPoint(lcdLastPos, y+FH-2, 0); // small decimal point
        lcdDrawNumber(lcdLastPos+2, y, ss % 100, att|LEFT|LEADING0, 2); // ''
        lcdDrawSolidVerticalLine(lcdLastPos, y, 2);
        lcdDrawSolidVerticalLine(lcdLastPos+2, y, 2);
      }
      lcdDrawChar(lcdLastPos+2, y, direction);
    }
    else {
      lcdDrawNumber(lcdLastPos+FW, y, mn, att|LEFT|LEADING0, 2); // mm before '.'
      lcdDrawPoint(lcdLastPos, y+FH-2, 0); // small decimal point
      lcdDrawNumber(lcdLastPos+2, y, ap, att|LEFT|UNSIGN|LEADING0, 4); // after '.'
      lcdDrawChar(lcdLastPos+1, y, direction);
    }
}

void displayDate(coord_t x, coord_t y, TelemetryItem & telemetryItem, LcdFlags att)
{
  if (att & DBLSIZE) {
    x -= 42;
    att &= ~0x0F00; // TODO constant
    lcdDrawNumber(x, y, telemetryItem.datetime.day, att|LEADING0|LEFT, 2);
    lcdDrawChar(lcdLastPos-1, y, '-', att);
    lcdDrawNumber(lcdNextPos-1, y, telemetryItem.datetime.month, att|LEFT, 2);
    lcdDrawChar(lcdLastPos-1, y, '-', att);
    lcdDrawNumber(lcdNextPos-1, y, telemetryItem.datetime.year, att|LEFT);
    y += FH;
    lcdDrawNumber(x, y, telemetryItem.datetime.hour, att|LEADING0|LEFT, 2);
    lcdDrawChar(lcdLastPos, y, ':', att);
    lcdDrawNumber(lcdNextPos, y, telemetryItem.datetime.min, att|LEADING0|LEFT, 2);
    lcdDrawChar(lcdLastPos, y, ':', att);
    lcdDrawNumber(lcdNextPos, y, telemetryItem.datetime.sec, att|LEADING0|LEFT, 2);
  }
  else {
    lcdDrawNumber(x, y, telemetryItem.datetime.day, att|LEADING0|LEFT, 2);
    lcdDrawChar(lcdLastPos-1, y, '-', att);
    lcdDrawNumber(lcdNextPos, y, telemetryItem.datetime.month, att|LEFT, 2);
    lcdDrawChar(lcdLastPos-1, y, '-', att);
    lcdDrawNumber(lcdNextPos, y, telemetryItem.datetime.year, att|LEFT);
    lcdDrawNumber(lcdNextPos+FW+1, y, telemetryItem.datetime.hour, att|LEADING0|LEFT, 2);
    lcdDrawChar(lcdLastPos, y, ':', att);
    lcdDrawNumber(lcdNextPos, y, telemetryItem.datetime.min, att|LEADING0|LEFT, 2);
    lcdDrawChar(lcdLastPos, y, ':', att);
    lcdDrawNumber(lcdNextPos, y, telemetryItem.datetime.sec, att|LEADING0|LEFT, 2);
  }
}

void displayGpsCoords(coord_t x, coord_t y, TelemetryItem & telemetryItem, LcdFlags att)
{
  if (att & DBLSIZE) {
    x -= (g_eeGeneral.gpsFormat == 0 ? 54 : 51);
    att &= ~0x0F00; // TODO constant
    displayGpsCoord(x, y, telemetryItem.gps.longitudeEW, telemetryItem.gps.longitude_bp, telemetryItem.gps.longitude_ap, att);
    displayGpsCoord(x, y+FH, telemetryItem.gps.latitudeNS, telemetryItem.gps.latitude_bp, telemetryItem.gps.latitude_ap, att);
  }
  else {
    displayGpsCoord(x, y, telemetryItem.gps.longitudeEW, telemetryItem.gps.longitude_bp, telemetryItem.gps.longitude_ap, att, false);
    displayGpsCoord(lcdNextPos+FWNUM, y, telemetryItem.gps.latitudeNS, telemetryItem.gps.latitude_bp, telemetryItem.gps.latitude_ap, att, false);
  }
}

void putsTelemetryChannelValue(coord_t x, coord_t y, uint8_t channel, int32_t value, LcdFlags att)
{
  if (channel >= MAX_SENSORS) return;     //Lua luaLcdDrawChannel() can call us with a bad value 
  TelemetryItem & telemetryItem = telemetryItems[channel];
  TelemetrySensor & telemetrySensor = g_model.telemetrySensors[channel];
  if (telemetrySensor.unit == UNIT_DATETIME) {
    displayDate(x, y, telemetryItem, att);
  }
  else if (telemetrySensor.unit == UNIT_GPS) {
    displayGpsCoords(x, y, telemetryItem, att);
  }
  else {
    LcdFlags flags = att;
    if (telemetrySensor.prec==2)
      flags |= PREC2;
    else if (telemetrySensor.prec==1)
      flags |= PREC1;
    putsValueWithUnit(x, y, value, telemetrySensor.unit == UNIT_CELLS ? UNIT_VOLTS : telemetrySensor.unit, flags);
  }
}

void putsChannelValue(coord_t x, coord_t y, source_t channel, int32_t value, LcdFlags att)
{
  if (channel >= MIXSRC_FIRST_TELEM) {
    channel = (channel-MIXSRC_FIRST_TELEM) / 3;
    putsTelemetryChannelValue(x, y, channel, value, att);
  }
  else if (channel >= MIXSRC_FIRST_TIMER || channel == MIXSRC_TX_TIME) {
    putsTimer(x, y, value, att, att);
  }
  else if (channel == MIXSRC_TX_VOLTAGE) {
    lcdDrawNumber(x, y, value, att|PREC1);
  }
  else if (channel < MIXSRC_FIRST_CH) {
    lcdDrawNumber(x, y, calcRESXto100(value), att);
  }
  else if (channel <= MIXSRC_LAST_CH) {
#if defined(PPM_UNIT_PERCENT_PREC1)
    lcdDrawNumber(x, y, calcRESXto1000(value), att|PREC1);
#else
    lcdDrawNumber(x, y, calcRESXto100(value), att);
#endif
  }
  else {
    lcdDrawNumber(x, y, value, att);
  }
}

void putsChannel(coord_t x, coord_t y, source_t channel, LcdFlags att)
{
  getvalue_t value = getValue(channel);
  putsChannelValue(x, y, channel, value, att);
}

void lcdSetContrast()
{
  lcdSetRefVolt(g_eeGeneral.contrast);
}
#endif // BOOT

void lcdMaskPoint(uint8_t *p, uint8_t mask, LcdFlags att)
{
  if ((p) >= DISPLAY_END) {
    return;
  }

  if (att&FILL_WHITE) {
    // TODO I could remove this, it's used for the top bar
    if (*p & 0x0F) mask &= 0xF0;
    if (*p & 0xF0) mask &= 0x0F;
  }

  if (att & FORCE) {
    *p |= mask;
  }
  else if (att & ERASE) {
    *p &= ~mask;
  }
  else {
    *p ^= mask;
  }
}

#define PIXEL_GREY_MASK(y, att) (((y) & 1) ? (0xF0 - (COLOUR_MASK(att) >> 12)) : (0x0F - (COLOUR_MASK(att) >> 16)))

void lcdDrawPoint(coord_t x, coord_t y, LcdFlags att)
{
  if (lcdIsPointOutside(x, y)) return;
  uint8_t *p = &displayBuf[ y / 2 * LCD_W + x ];
  uint8_t mask = PIXEL_GREY_MASK(y, att);
  lcdMaskPoint(p, mask, att);
}

void lcdDrawHorizontalLine(coord_t x, coord_t y, coord_t w, uint8_t pat, LcdFlags att)
{
  if (y < 0 || y >= LCD_H) return;
  if (x+w > LCD_W) {
    if (x >= LCD_W ) return;
    w = LCD_W - x;
  }

  uint8_t *p  = &displayBuf[ y / 2 * LCD_W + x ];
  uint8_t mask = PIXEL_GREY_MASK(y, att);
  while (w--) {
    if (pat&1) {
      lcdMaskPoint(p, mask, att);
      pat = (pat >> 1) | 0x80;
    }
    else {
      pat = pat >> 1;
    }
    p++;
  }
}

void lcdDrawVerticalLine(coord_t x, scoord_t y, scoord_t h, uint8_t pat, LcdFlags att)
{
  if (x >= LCD_W) return;
  if (y >= LCD_H) return;
  if (h<0) { y+=h; h=-h; }
  if (y<0) { h+=y; y=0; if (h<=0) return; }
  if (y+h > LCD_H) { h = LCD_H - y; }

  if (pat==DOTTED && !(y%2)) {
    pat = ~pat;
  }

  while (h--) {
    if (pat & 1) {
      lcdDrawPoint(x, y, att);
      pat = (pat >> 1) | 0x80;
    }
    else {
      pat = pat >> 1;
    }
    y++;
  }
}

void lcdInvertLine(int8_t line)
{
  uint8_t *p  = &displayBuf[line * 4 * LCD_W];
  for (coord_t x=0; x<LCD_W*4; x++) {
    ASSERT_IN_DISPLAY(p);
    *p++ ^= 0xff;
  }
}

#if !defined(BOOT)
void lcd_img(coord_t x, coord_t y, const pm_uchar * img, uint8_t idx, LcdFlags att)
{
  const pm_uchar *q = img;
  uint8_t w    = pgm_read_byte(q++);
  uint8_t hb   = (pgm_read_byte(q++)+7) / 8;
  bool    inv  = (att & INVERS) ? true : (att & BLINK ? BLINK_ON_PHASE : false);
  q += idx*w*hb;
  for (uint8_t yb = 0; yb < hb; yb++) {
    for (coord_t i=0; i<w; i++) {
      uint8_t b = pgm_read_byte(q++);
      uint8_t val = inv ? ~b : b;
      for (int k=0; k<8; k++) {
        if (val & (1<<k)) {
          lcdDrawPoint(x+i, y+yb*8+k, 0);
        }
      }
    }
  }
}

void lcd_bmp(coord_t x, coord_t y, const uint8_t * img, coord_t offset, coord_t width)
{
  const uint8_t *q = img;
  uint8_t w = *q++;
  if (!width || width > w) {
    width = w;
  }
  if (x+width > LCD_W) {
    if (x >= LCD_W ) return;
    width = LCD_W-x;
  }
  uint8_t rows = (*q++ + 1) / 2;

  for (uint8_t row=0; row<rows; row++) {
    q = img + 2 + row*w + offset;
    uint8_t *p = &displayBuf[(row + (y/2)) * LCD_W + x];
    for (coord_t i=0; i<width; i++) {
      if (p >= DISPLAY_END) return;
      uint8_t b = *q++;
      if (y & 1) {
        *p = (*p & 0x0f) + ((b & 0x0f) << 4);
        if ((p+LCD_W) < DISPLAY_END) {
          *(p+LCD_W) = (*(p+LCD_W) & 0xf0) + ((b & 0xf0) >> 4);
        }
      }
      else {
        *p = b;
      }
      p++;
    }
  }
}
#endif

