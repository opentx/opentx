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

#include "../../opentx.h"

#if defined(SIMU)
display_t displayBuf[DISPLAY_BUF_SIZE];
#endif

uint16_t lcdColorTable[LCD_COLOR_COUNT];

#define PIXEL_PTR(x, y) &displayBuf[(y)*LCD_W + (x)]

void lcdColorsInit()
{
  lcdColorTable[TEXT_COLOR_INDEX] = BLACK;
  lcdColorTable[TEXT_BGCOLOR_INDEX] = WHITE;
  lcdColorTable[TEXT_INVERTED_COLOR_INDEX] = WHITE;
  lcdColorTable[TEXT_INVERTED_BGCOLOR_INDEX] = RED;
  lcdColorTable[LINE_COLOR_INDEX] = RGB(88, 88, 90);
  lcdColorTable[SCROLLBOX_COLOR_INDEX] = RED;
  lcdColorTable[MENU_TITLE_BGCOLOR_INDEX] = DARKGREY;
  lcdColorTable[MENU_TITLE_COLOR_INDEX] = WHITE;
  lcdColorTable[HEADER_COLOR_INDEX] = DARKGREY;
  lcdColorTable[ALARM_COLOR_INDEX] = RED;
  lcdColorTable[WARNING_COLOR_INDEX] = YELLOW;
  lcdColorTable[TEXT_DISABLE_COLOR_INDEX] = RGB(0x60, 0x60, 0x60);
  lcdColorTable[CURVE_AXIS_COLOR_INDEX] = BLUE;
  lcdColorTable[TITLE_BGCOLOR_INDEX] = RED;
  lcdColorTable[HEADER_BGCOLOR_INDEX] = DARKRED;
}

coord_t lcdLastPos;
coord_t lcdNextPos;

#define FONT_CHARS_COUNT 103

char getMappedChar(unsigned char c)
{
  if (c == 0)
    return 0;
  else if (c < 0xC0)
    return c - 0x20;
  else
    return c - 0xC0 + 96;
}

int getFontPatternWidth(const uint16_t *spec, int index)
{
  return spec[index+1] - spec[index];
}

int getCharWidth(char c, const uint16_t *spec)
{
  return getFontPatternWidth(spec, getMappedChar(c));
}

void lcdDrawBitmapPattern(coord_t x, coord_t y, const uint8_t * img, LcdFlags flags, coord_t offset, coord_t width)
{
  coord_t w = *((uint16_t *)img);
  coord_t height = *(((uint16_t *)img)+1);

  if (!width || width > w) {
    width = w;
  }

  if (x+width > LCD_W) {
    width = LCD_W-x;
  }

  display_t color = lcdColorTable[COLOR_IDX(flags)];

  for (coord_t row=0; row<height; row++) {
    display_t * p = PIXEL_PTR(x, y+row);
    const uint8_t * q = img + 4 + row*w + offset;
    for (coord_t col=0; col<width; col++) {
      lcdDrawTransparentPixel(p, *q, color);
      p++; q++;
    }
  }
}

#define FONT_MAX_HEIGHT 42
void lcdPutFontPattern(coord_t x, coord_t y, const uint8_t * font, const uint16_t * spec, int index, LcdFlags flags)
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

  coord_t w = *((uint16_t *)font);
  coord_t width = getFontPatternWidth(spec, index);
  coord_t height = *(((uint16_t *)font)+1);

  assert(height <= FONT_MAX_HEIGHT);

  for (int i=0; i<width+2; i++) {
    if (x < LCD_W) {
      uint8_t b[FONT_MAX_HEIGHT] = { 0 };
      if (i == 0) {
        if (x==0 || !inv) {
          lcdNextPos++;
          continue;
        }
        else {
          // we need to work on the previous x when INVERS
          x--;
        }
      }
      else if (i <= width) {
        for (coord_t j=0; j<height; ++j) {
          uint8_t p = *(font + 4 + j*w + spec[index] + i - 1);
          b[j] = p;
        }
      }

      for (int j=-1; j<=height; ++j) {
        uint8_t plot;
        if (j < 0 || j == height) {
          plot = 0;
          if (j<0 && !inv) continue;
          if (y+j < 0) continue;
        }
        else {
          plot = b[j];
        }
        if (inv) plot = !plot;
        if (!blink) {
          if (plot) {
            lcdDrawTransparentPixel(x, y+j, plot, lcdColorTable[COLOR_IDX(flags)]);
          }
          // TODO needed?
          // else if (flags & ERASEBG) {
          //  lcdDrawPoint(x, y+j, 0x0FFF0000);
          // }
        }
      }
    }

    x++;
    lcdNextPos++;
  }

  lcdNextPos--;
}

void lcd_putcAtt(coord_t x, coord_t y, const unsigned char c, LcdFlags flags)
{
  lcdNextPos = x-1;
  int fontindex = FONTSIZE(flags) >> 8;
  const pm_uchar * font = fontsTable[fontindex];
  const uint16_t * fontspecs = fontspecsTable[fontindex];
  lcdPutFontPattern(x, y, font, fontspecs, getMappedChar(c), flags);
}

void lcd_putc(coord_t x, coord_t y, const unsigned char c)
{
  lcd_putcAtt(x, y, c, 0);
}

uint8_t getStringInfo(const char *s)
{
  uint8_t result = 0;
  for (int i=0; s[i]; i++) {
    result += s[i];
  }
  return result;
}

int getFontHeight(LcdFlags flags)
{
  static const int heightTable[16] = { 12, 10, 11, 14, 18, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12 };
  return heightTable[FONTSIZE(flags) >> 8];
}

int getTextWidth(const pm_char *s, int len, LcdFlags flags)
{
  const uint16_t *specs = fontspecsTable[FONTSIZE(flags) >> 8];
  int result = 0;
  for (int i=0; (len==0 || i<len) && (*s!='\0'); ++i) {
    char c;
    if (flags & ZCHAR)
      c = idx2char(*s);
    else
      c = pgm_read_byte(s);
    result += getCharWidth(c, specs);
    ++s;
  }
  return result;
}

void lcd_putsnAtt(coord_t x, coord_t y, const pm_char * s, uint8_t len, LcdFlags flags)
{
  int width = getTextWidth(s, len, flags);
  int height = getFontHeight(flags);

  if ((flags&INVERS) && ((~flags & BLINK) || BLINK_ON_PHASE)) {
    if ((COLOR_IDX(flags) == TEXT_COLOR_INDEX)) {
      flags += TEXT_INVERTED_COLOR - TEXT_COLOR;
      if (FONTSIZE(flags) == TINSIZE)
        lcdDrawFilledRect(x-INVERT_HORZ_MARGIN+2, y-INVERT_VERT_MARGIN+2, width+2*INVERT_HORZ_MARGIN-5, INVERT_LINE_HEIGHT-7, TEXT_INVERTED_BGCOLOR);
      else if (FONTSIZE(flags) == SMLSIZE)
        lcdDrawFilledRect(x-INVERT_HORZ_MARGIN+1, y-INVERT_VERT_MARGIN, width+2*INVERT_HORZ_MARGIN-2, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
      else
        lcdDrawFilledRect(x-INVERT_HORZ_MARGIN, y-INVERT_VERT_MARGIN, width+2*INVERT_HORZ_MARGIN, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
    }
    flags &= ~(INVERS|BLINK);
  }

  char str[256];
  if (flags & ZCHAR)
    strcat_zchar(str, s, len);
  else
    strAppend(str, s, len);
  
  const coord_t orig_x = x;
  bool setx = false;
  while (len--) {
    unsigned char c;
    if (flags & ZCHAR)
      c = idx2char(*s);
    else
      c = pgm_read_byte(s);
    if (setx) {
      x = c;
      setx = false;
    }
    else if (!c) {
      break;
    }
    else if (c >= 0x20) {
      lcd_putcAtt(x, y, c, flags);
      x = lcdNextPos;
    }
    else if (c == 0x1F) {  // X-coord prefix
      setx = true;
    }
    else if (c == 0x1E) {
      x = orig_x;
      y += height;
    }
    else if (c == 1) {
      x += 1;
    }
    else {
      x += 2*(c-1);
    }
    s++;
  }
  lcdLastPos = x;
  lcdNextPos = x;
}

void lcd_putsn(coord_t x, coord_t y, const pm_char * s, uint8_t len)
{
  lcd_putsnAtt(x, y, s, len, TEXT_COLOR);
}

void lcd_putsAtt(coord_t x, coord_t y, const pm_char * s, LcdFlags flags)
{
  lcd_putsnAtt(x, y, s, 255, flags);
}

void lcd_puts(coord_t x, coord_t y, const pm_char * s)
{
  lcd_putsAtt(x, y, s, 0);
}

void lcd_putsLeft(coord_t y, const pm_char * s)
{
  lcd_puts(MENU_TITLE_LEFT, y, s);
}

void lcd_putsCenter(coord_t y, const pm_char * s, LcdFlags attr)
{
  int x = (LCD_W - getTextWidth(s)) / 2;
  lcd_putsAtt(x, y, s, attr);
}

void lcd_putsiAtt(coord_t x, coord_t y, const pm_char * s,uint8_t idx, LcdFlags flags)
{
  uint8_t length;
  length = pgm_read_byte(s++);
  lcd_putsnAtt(x, y, s+length*idx, length, flags & ~ZCHAR);
}

void lcd_outhex4(coord_t x, coord_t y, uint32_t val, LcdFlags flags)
{
  char s[5];
  for (int i=0; i<4; i++) {
    char c = val & 0xf;
    s[3-i] = c>9 ? c+'A'-10 : c+'0';
    val >>= 4;
  }
  s[4] = '\0';
  if (!(flags & LEFT))
    x -= getTextWidth(s);
  else
    flags -= LEFT;
  lcd_putsAtt(x, y, s, flags);
}

void lcd_outdez8(coord_t x, coord_t y, int8_t val)
{
  lcd_outdezAtt(x, y, val);
}

void lcd_outdezAtt(coord_t x, coord_t y, lcdint_t val, LcdFlags flags, const char *suffix, const char *prefix)
{
  lcd_outdezNAtt(x, y, val, flags, 0, suffix, prefix);
}

void lcd_outdezNAtt(coord_t x, coord_t y, lcdint_t val, LcdFlags flags, int len, const char *suffix, const char *prefix)
{
  char str[48+1]; // max=16 for the prefix, 16 chars for the number, 16 chars for the suffix
  char *s = str+32;
  *s = '\0';
  int idx = 0;
  int mode = MODE(flags);
  bool neg = false;
  if (val < 0) {
    val = -val;
    neg = true;
  }
  do {
    *--s = '0' + (val % 10);
    ++idx;
    val /= 10;
    if (mode!=0 && idx==mode) {
      mode = 0;
      *--s = '.';
      if (val==0)
        *--s = '0';
    }
  } while (val!=0 || mode>0 || (mode==MODE(LEADING0) && idx<len));
  if (neg) *--s = '-';
  if (!(flags & LEFT))
    x -= getTextWidth(s, 0, flags);
  // TODO needs check on all string lengths ...
  if (prefix) {
    int len = strlen(prefix);
    if (len <= 16) {
      s -= len;
      strncpy(s, prefix, len);
    }
  }
  if (suffix) {
    strncpy(&str[32], suffix, 16);
  }
  flags &= ~(LEADING0|LEFT);
  lcd_putsAtt(x, y, s, flags);
}

void lcd_hline(coord_t x, coord_t y, coord_t w, LcdFlags att)
{
  lcd_hlineStip(x, y, w, 0xff, att);
}

#if !defined(BOOT)
void lcd_line(coord_t x1, coord_t y1, coord_t x2, coord_t y2, uint8_t pat, LcdFlags att)
{
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
      y += dyabs;
      if (y>=dxabs) {
        y -= dxabs;
        py += sdy;
      }
      if ((1<<(px%8)) & pat) {
        lcdDrawPoint(px, py, att);
      }
      px += sdx;
    }
  }
  else {
    /* the line is more vertical than horizontal */
    for (int i=0; i<=dyabs; i++) {
      x += dxabs;
      if (x >= dyabs) {
        x -= dyabs;
        px += sdx;
      }
      if ((1<<(py%8)) & pat) {
        lcdDrawPoint(px, py, att);
      }
      py += sdy;
    }
  }
}
#endif

void lcd_vline(coord_t x, scoord_t y, scoord_t h)
{
  lcd_vlineStip(x, y, h, SOLID);
}

void lcd_rect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t pat, LcdFlags att)
{
  if (att & ROUND) {
    lcd_vlineStip(x, y+1, h-2, pat, att);
    lcd_vlineStip(x+w-1, y+1, h-2, pat, att);
    lcd_hlineStip(x+1, y+h-1, w-2, pat, att);
    lcd_hlineStip(x+1, y, w-2, pat, att);
  }
  else {
    lcd_vlineStip(x, y, h, pat, att);
    lcd_vlineStip(x+w-1, y, h, pat, att);
    lcd_hlineStip(x+1, y+h-1, w-2, pat, att);
    lcd_hlineStip(x+1, y, w-2, pat, att);
  }
}

#if defined(SIMU)
void lcdDrawFilledRect(coord_t x, scoord_t y, coord_t w, coord_t h, LcdFlags att)
{
  for (scoord_t i=y; i<y+h; i++) {
    lcd_hlineStip(x, i, w, SOLID, att);
  }
}
#endif

#if !defined(BOOT)
void lcdDrawFilledRectWithAttributes(coord_t x, scoord_t y, coord_t w, coord_t h, uint8_t pat, LcdFlags att)
{
  for (scoord_t i=y; i<y+h; i++) {
    if ((att&ROUND) && (i==y || i==y+h-1))
      lcd_hlineStip(x+1, i, w-2, pat, att);
    else
      lcd_hlineStip(x, i, w, pat, att);
    pat = (pat >> 1) + ((pat & 1) << 7);
  }
}
#endif

void putsRtcTime(coord_t x, coord_t y, LcdFlags att)
{
  putsTimer(x, y, getValue(MIXSRC_TX_TIME), att);
}

void putsTimer(coord_t x, coord_t y, putstime_t tme, LcdFlags att)
{
  div_t qr;

  char str[10]; // "-00:00:00"
  char *s = str;

  att &= ~LEFT;

  if (tme < 0) {
    // TODO lcd_putcAtt(x - ((att & DBLSIZE) ? FW+2 : ((att & MIDSIZE) ? FW+0 : FWNUM)), y, '-', att);
    tme = -tme;
    *s++ = '-';
  }

  qr = div(tme, 60);

  if (att & TIMEHOUR) {
    div_t qr2 = div(qr.quot, 60);
    *s++ = '0' + (qr2.quot/10);
    *s++ = '0' + (qr2.quot%10);
    *s++ = ':';
    qr.quot = qr2.rem;
  }

  *s++ = '0' + (qr.quot/10);
  *s++ = '0' + (qr.quot%10);
  *s++ = ':';
  *s++ = '0' + (qr.rem/10);
  *s++ = '0' + (qr.rem%10);
  *s = '\0';

  lcd_putsAtt(x, y, str, att);
}

// TODO to be optimized with putsValueWithUnit
void putsVolts(coord_t x, coord_t y, uint16_t volts, LcdFlags att)
{
  lcd_outdezAtt(x, y, (int16_t)volts, (~NO_UNIT) & (att | ((att&PREC2)==PREC2 ? 0 : PREC1)), (~att & NO_UNIT) ? "v" : NULL);
}

void putsVBat(coord_t x, coord_t y, LcdFlags att)
{
  putsVolts(x, y, g_vbat100mV, att);
}

void putsStrIdx(coord_t x, coord_t y, const pm_char *str, int idx, LcdFlags att, const char *prefix)
{
  // TODO quick & dirty, dangerous
  char s[32];
  sprintf(s, "%s%s%d", prefix, str, abs(idx));
  lcd_putsAtt(x, y, s, att);
}

void putsMixerSource(coord_t x, coord_t y, uint8_t idx, LcdFlags att)
{
  if (idx == 0) {
    lcd_putsiAtt(x, y, STR_VSRCRAW, 0, att); // TODO macro
  }
  else if (idx <= MIXSRC_LAST_INPUT) {
    char s[32] = "\307";
    if (ZEXIST(g_model.inputNames[idx-MIXSRC_FIRST_INPUT])) {
      zchar2str(s+1, g_model.inputNames[idx-MIXSRC_FIRST_INPUT], LEN_INPUT_NAME);
      s[1+LEN_INPUT_NAME] = '\0';
    }
    else {
      sprintf(s+1, "%02d", idx);
    }
    lcd_putsAtt(x, y, s, att);
  }

  else if (idx <= MIXSRC_LAST_LUA) {
    div_t qr = div(idx-MIXSRC_FIRST_LUA, MAX_SCRIPT_OUTPUTS);
#if defined(LUA_MODEL_SCRIPTS)
    if (qr.quot < MAX_SCRIPTS && qr.rem < scriptInputsOutputs[qr.quot].outputsCount) {
      lcd_putcAtt(x+2, y+1, '1'+qr.quot, TINSIZE);
      lcdDrawFilledRect(x, y, 7, 7);
      lcd_putsnAtt(x+8, y, scriptInputsOutputs[qr.quot].outputs[qr.rem].name, att & STREXPANDED ? 9 : 4, att);
    }
    else
#endif
    {
      putsStrIdx(x, y, "LUA", qr.quot+1, att);
      lcd_putcAtt(x+20, y, 'a'+qr.rem, att);
    }
  }

  else if (idx < MIXSRC_LAST_POT) {
    idx = idx-MIXSRC_Rud;
    if (ZEXIST(g_eeGeneral.anaNames[idx]))
      lcd_putsnAtt(x, y, g_eeGeneral.anaNames[idx], LEN_ANA_NAME, ZCHAR|att);
    else
      lcd_putsiAtt(x, y, STR_VSRCRAW, idx+1, att);
  }
  else if (idx >= MIXSRC_FIRST_SWITCH && idx < MIXSRC_FIRST_LOGICAL_SWITCH) {
    idx = idx-MIXSRC_FIRST_SWITCH;
    if (ZEXIST(g_eeGeneral.switchNames[idx]))
      lcd_putsnAtt(x, y, g_eeGeneral.switchNames[idx], LEN_SWITCH_NAME, ZCHAR|att);
    else
      lcd_putsiAtt(x, y, STR_VSRCRAW, idx+MIXSRC_FIRST_SWITCH-MIXSRC_Rud+1, att);
  }
  else if (idx < MIXSRC_SW1)
    lcd_putsiAtt(x, y, STR_VSRCRAW, idx-MIXSRC_Rud+1, att);
  else if (idx <= MIXSRC_LAST_LOGICAL_SWITCH)
    putsSwitches(x, y, SWSRC_SW1+idx-MIXSRC_SW1, att);
  else if (idx < MIXSRC_CH1)
    putsStrIdx(x, y, STR_PPM_TRAINER, idx-MIXSRC_FIRST_TRAINER+1, att);
  else if (idx <= MIXSRC_LAST_CH) {
    putsStrIdx(x, y, STR_CH, idx-MIXSRC_CH1+1, att);
#if 0
    if (ZEXIST(g_model.limitData[idx-MIXSRC_CH1].name) && (att & STREXPANDED)) {
      lcd_putcAtt(lcdLastPos, y, ' ', att);
      lcd_putsnAtt(lcdLastPos+3, y, g_model.limitData[idx-MIXSRC_CH1].name, LEN_CHANNEL_NAME, ZCHAR|att);
    }
#endif
  }
  else if (idx <= MIXSRC_LAST_GVAR)
    putsStrIdx(x, y, STR_GV, idx-MIXSRC_GVAR1+1, att);
  else if (idx < MIXSRC_FIRST_TELEM) {
    lcd_putsiAtt(x, y, STR_VSRCRAW, idx-MIXSRC_Rud+1-NUM_LOGICAL_SWITCH-NUM_TRAINER-NUM_CHNOUT-MAX_GVARS, att);
  }
  else {
    idx -= MIXSRC_FIRST_TELEM;
    div_t qr = div(idx, 3);
    char s[sizeof(g_model.telemetrySensors[qr.quot].label)+2];
    int pos = zchar2str(s, g_model.telemetrySensors[qr.quot].label, sizeof(g_model.telemetrySensors[qr.quot].label));
    if (qr.rem) s[pos++] = (qr.rem==2 ? '+' : '-');
    s[pos] = '\0';
    lcd_putsAtt(x, y, s, att);
  }
}

void putsChnLetter(coord_t x, coord_t y, uint8_t idx, LcdFlags att)
{
  lcd_putsiAtt(x, y, STR_RETA123, idx-1, att);
}

void putsModelName(coord_t x, coord_t y, char *name, uint8_t id, LcdFlags att)
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

void putsSwitches(coord_t x, coord_t y, int8_t idx, LcdFlags att)
{
  if (idx == SWSRC_OFF)
    return lcd_putsiAtt(x, y, STR_OFFON, 0, att);

  char s[8];
  int pos = 0;
  if (idx < 0) {
    s[pos++] = '!';
  }

  int absidx = abs(idx);

#if defined(FLIGHT_MODES)
  if (absidx >= SWSRC_FIRST_FLIGHT_MODE) {
    return putsStrIdx(x, y, STR_FP, absidx-SWSRC_FIRST_FLIGHT_MODE, att, idx < 0 ? "!" : "");
  }
  else
#endif
  if (absidx >= SWSRC_FIRST_SWITCH && absidx <= SWSRC_LAST_SWITCH) {
    div_t swinfo = switchInfo(absidx);
    if (ZEXIST(g_eeGeneral.switchNames[swinfo.quot])) {
      pos = zchar2str(&s[pos], g_eeGeneral.switchNames[swinfo.quot], LEN_SWITCH_NAME);
      s[pos++] = "\300-\301"[swinfo.rem];
      s[pos] = '\0';
      lcd_putsAtt(x, y, s, att);
      return;
    }
  }

  strAppend(&s[pos], STR_VSWITCHES+1+absidx*STR_VSWITCHES[0], STR_VSWITCHES[0]);
  lcd_putsAtt(x, y, s, att);
}

#if defined(FLIGHT_MODES)
void putsFlightMode(coord_t x, coord_t y, int8_t idx, LcdFlags att)
{
  if (idx==0) {
    lcd_putsiAtt(x, y, STR_MMMINV, 0, att);
  }
  else {
    putsStrIdx(x, y, STR_FP, abs(idx)-1, att);
  }
}
#endif

void putsCurveRef(coord_t x, coord_t y, CurveRef &curve, LcdFlags att)
{
  if (curve.value != 0) {
    switch (curve.type) {
      case CURVE_REF_DIFF:
        lcd_putsAtt(x, y, "D", att);
        GVAR_MENU_ITEM(x+8, y, curve.value, -100, 100, LEFT|att, 0, 0);
        break;

      case CURVE_REF_EXPO:
        lcd_putsAtt(x, y, "E", att);
        GVAR_MENU_ITEM(x+8, y, curve.value, -100, 100, LEFT|att, 0, 0);
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

void putsCurve(coord_t x, coord_t y, int8_t idx, LcdFlags att)
{
  if (idx == 0) {
    return lcd_putsiAtt(x, y, STR_MMMINV, 0, att);
  }
  bool neg = false;
  if (idx < 0) {
    idx = -idx;
    neg = true;
  }
  putsStrIdx(x, y, STR_CV, idx, att);
  if (neg) {
    if ((att&INVERS) && ((~att&BLINK) || BLINK_ON_PHASE))
      att &= ~(INVERS|BLINK);
    lcd_putcAtt(x-3, y, '!', att);
  }
}

void putsTimerMode(coord_t x, coord_t y, int8_t mode, LcdFlags att)
{
  if (mode >= 0) {
    if (mode < TMRMODE_COUNT)
      return lcd_putsiAtt(x, y, STR_VTMRMODES, mode, att);
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
    lcd_putsAtt(x, y, "--", att);
  }
  else {
    char s[2];
    s[0] = (mode % 2 == 0) ? ':' : '+';
    s[1] = '0'+p;
    lcd_putsnAtt(x, y, s, 2, att);
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

void putsValueWithUnit(coord_t x, coord_t y, lcdint_t val, uint8_t unit, LcdFlags att)
{
  // convertUnit(val, unit);
  if (!(att & NO_UNIT) && unit != UNIT_RAW) {
    char unitStr[8];
    strAppend(unitStr, STR_VTELEMUNIT+1+unit*STR_VTELEMUNIT[0], STR_VTELEMUNIT[0]);
    lcd_outdezAtt(x, y, val, att, unitStr);
  }
  else {
    lcd_outdezAtt(x, y, val, att);
  }
}

void displayDate(coord_t x, coord_t y, TelemetryItem & telemetryItem, LcdFlags att)
{
}

void displayGpsCoords(coord_t x, coord_t y, TelemetryItem & telemetryItem, LcdFlags att)
{
}

void putsTelemetryChannelValue(coord_t x, coord_t y, uint8_t channel, lcdint_t value, LcdFlags att)
{
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

void putsChannelValue(coord_t x, coord_t y, source_t channel, lcdint_t value, LcdFlags att)
{
  if (channel >= MIXSRC_FIRST_TELEM) {
    channel = (channel-MIXSRC_FIRST_TELEM) / 3;
    putsTelemetryChannelValue(x, y, channel, value, att);
  }
  else if (channel >= MIXSRC_FIRST_TIMER || channel == MIXSRC_TX_TIME) {
    putsTimer(x, y, value, att);
  }
  else if (channel == MIXSRC_TX_VOLTAGE) {
    lcd_outdezAtt(x, y, value, att|PREC1);
  }
  else if (channel < MIXSRC_FIRST_CH) {
    lcd_outdezAtt(x, y, calcRESXto100(value), att);
  }
  else if (channel <= MIXSRC_LAST_CH) {
#if defined(PPM_UNIT_PERCENT_PREC1)
    lcd_outdezAtt(x, y, calcRESXto1000(value), att|PREC1);
#else
    lcd_outdezAtt(x, y, calcRESXto100(value), att);
#endif
  }
  else {
    lcd_outdezAtt(x, y, value, att);
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

void lcdDrawPixel(display_t * p, display_t value)
{
  *p = value;
}

void lcdDrawTransparentPixel(coord_t x, coord_t y, uint8_t opacity, uint16_t color)
{
  display_t * p = PIXEL_PTR(x, y);
  lcdDrawTransparentPixel(p, opacity, color);
}

void lcdDrawTransparentPixel(display_t * p, uint8_t opacity, uint16_t color)
{
  ASSERT_IN_DISPLAY(p);

  if (opacity != OPACITY_MAX) {
    uint8_t bgWeight = OPACITY_MAX - opacity;
    COLOR_SPLIT(color, red, green, blue);
    COLOR_SPLIT(*p, bgRed, bgGreen, bgBlue);
    uint16_t r = (bgRed * bgWeight + red * opacity) / OPACITY_MAX;
    uint16_t g = (bgGreen * bgWeight + green * opacity) / OPACITY_MAX;
    uint16_t b = (bgBlue * bgWeight + blue * opacity) / OPACITY_MAX;
    lcdDrawPixel(p, COLOR_JOIN(r, g, b));
  }
  else if (opacity != 0) {
    lcdDrawPixel(p, color);
  }
}

void lcdDrawPoint(coord_t x, coord_t y, LcdFlags att)
{
  display_t * p = PIXEL_PTR(x, y);
  display_t color = lcdColorTable[COLOR_IDX(att)];
  if (p < DISPLAY_END) {
    *p = color;
  }
}

void lcd_hlineStip(coord_t x, coord_t y, coord_t w, uint8_t pat, LcdFlags att)
{
  if (y >= LCD_H) return;
  if (x+w > LCD_W) { w = LCD_W - x; }

  display_t * p = PIXEL_PTR(x, y);
  display_t color = lcdColorTable[COLOR_IDX(att)];

  while (w--) {
    if (pat&1) {
      *p = color;
      // lcd_mask(p, mask, 0);
      pat = (pat >> 1) | 0x80;
    }
    else {
      pat = pat >> 1;
    }
    p++;
  }
}

void lcd_vlineStip(coord_t x, coord_t y, coord_t h, uint8_t pat, LcdFlags att)
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

#if !defined(BOOT)
void lcdDrawBitmap(coord_t x, coord_t y, const uint16_t * img, coord_t offset, coord_t width)
{
  const uint16_t * q = img;
  coord_t w = *q++;
  coord_t height = *q++;

  if (!width || width > w) {
    width = w;
  }

  if (x+width > LCD_W) {
    width = LCD_W-x;
  }

  for (coord_t row=0; row<height; row++) {
    display_t * p = &displayBuf[(row+y)*LCD_W + x];
    q = img + 2 + row*w + offset;
    for (coord_t col=0; col<width; col++) {
      lcdDrawPixel(p, *q);
      p++; q++;
    }
  }
}
#endif

void drawBlackOverlay()
{
  // TODO drawFilledRect(0, 0, LCD_W, LCD_H, SOLID, OPAQUE(10, GREY(3)));
}
