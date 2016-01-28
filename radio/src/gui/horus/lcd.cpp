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

#include <math.h>
#include <stdio.h>
#include "../../opentx.h"

#if defined(SIMU)
display_t displayBuf[DISPLAY_BUFFER_SIZE];
#endif

uint16_t lcdColorTable[LCD_COLOR_COUNT];

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
  lcdColorTable[MENU_TITLE_DISABLE_COLOR_INDEX] = RGB(130, 1, 5);
  lcdColorTable[HEADER_COLOR_INDEX] = DARKGREY;
  lcdColorTable[ALARM_COLOR_INDEX] = RED;
  lcdColorTable[WARNING_COLOR_INDEX] = YELLOW;
  lcdColorTable[TEXT_DISABLE_COLOR_INDEX] = RGB(0x60, 0x60, 0x60);
  lcdColorTable[CURVE_AXIS_COLOR_INDEX] = RGB(180, 180, 180);
  lcdColorTable[CURVE_COLOR_INDEX] = RED;
  lcdColorTable[CURVE_CURSOR_COLOR_INDEX] = RED;
  lcdColorTable[TITLE_BGCOLOR_INDEX] = RED;
  lcdColorTable[HEADER_BGCOLOR_INDEX] = DARKRED;
}

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

int getFontPatternWidth(const uint16_t * spec, int index)
{
  return spec[index+1] - spec[index];
}

int getCharWidth(char c, const uint16_t * spec)
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
      lcdDrawAlphaPixel(p, *q, color);
      p++; q++;
    }
  }
}

#define FONT_MAX_HEIGHT 42
void lcdPutFontPattern(coord_t x, coord_t y, const uint8_t * font, const uint16_t * spec, int index, LcdFlags flags)
{
  coord_t offset = spec[index];
  coord_t width = spec[index+1] - offset;
  lcdDrawBitmapPattern(x, y, font, flags, offset, width);
  lcdNextPos = x + width;
}

void lcdDrawChar(coord_t x, coord_t y, const unsigned char c, LcdFlags flags)
{
  int fontindex = FONTSIZE(flags) >> 8;
  const pm_uchar * font = fontsTable[fontindex];
  const uint16_t * fontspecs = fontspecsTable[fontindex];
  lcdPutFontPattern(x, y, font, fontspecs, getMappedChar(c), flags);
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
  static const int heightTable[16] = { 12, 10, 11, 14, 32, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12 };
  return heightTable[FONTSIZE(flags) >> 8];
}

int getBitmapScale(const uint8_t * bmp, int dstWidth, int dstHeight)
{
  int widthScale, heightScale;

  int bmpWidth = getBitmapWidth(bmp);
  int bmpHeight = getBitmapHeight(bmp);

  if (bmpWidth == 0 || bmpHeight == 0)
    return 0;

  if (bmpWidth > dstWidth)
    widthScale = -((bmpWidth+dstWidth-1) / dstWidth);
  else
    widthScale = (dstWidth / bmpWidth);
  if (bmpHeight > dstHeight)
    heightScale = -((bmpHeight+dstHeight-1) / dstHeight);
  else
    heightScale = (dstHeight / bmpHeight);
  return min(widthScale, heightScale);
}

int getTextWidth(const pm_char * s, int len, LcdFlags flags)
{
  const uint16_t * specs = fontspecsTable[FONTSIZE(flags) >> 8];
  int result = 0;
  for (int i=0; len==0 || i<len; ++i) {
    char c;
    if (flags & ZCHAR)
      c = idx2char(*s);
    else
      c = *s;
    if (c == '\0')
      break;
    result += getCharWidth(c, specs);
    ++s;
  }
  return result;
}

void lcdDrawSizedText(coord_t x, coord_t y, const pm_char * s, uint8_t len, LcdFlags flags)
{
  int width = getTextWidth(s, len, flags);
  int height = getFontHeight(flags);
  int fontindex = FONTSIZE(flags) >> 8;
  const pm_uchar * font = fontsTable[fontindex];
  const uint16_t * fontspecs = fontspecsTable[fontindex];

  if ((flags&INVERS) && ((~flags & BLINK) || BLINK_ON_PHASE)) {
    if (COLOR_IDX(flags) == TEXT_COLOR_INDEX) {
      flags += TEXT_INVERTED_COLOR - TEXT_COLOR;
    }
    if (FONTSIZE(flags) == TINSIZE)
      lcdDrawSolidFilledRect(x-INVERT_HORZ_MARGIN+2, y-INVERT_VERT_MARGIN+2, width+2*INVERT_HORZ_MARGIN-5, INVERT_LINE_HEIGHT-7, TEXT_INVERTED_BGCOLOR);
    else if (FONTSIZE(flags) == SMLSIZE)
      lcdDrawSolidFilledRect(x-INVERT_HORZ_MARGIN+1, y-INVERT_VERT_MARGIN, width+2*INVERT_HORZ_MARGIN-2, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
    else
      lcdDrawSolidFilledRect(x-INVERT_HORZ_MARGIN, y/*-INVERT_VERT_MARGIN*/, width+2*INVERT_HORZ_MARGIN, INVERT_LINE_HEIGHT, TEXT_INVERTED_BGCOLOR);
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
      lcdPutFontPattern(x, y, font, fontspecs, getMappedChar(c), flags);
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
  lcdNextPos = x;
}

void lcdDrawText(coord_t x, coord_t y, const pm_char * s, LcdFlags flags)
{
  lcdDrawSizedText(x, y, s, 255, flags);
}

void lcd_putsCenter(coord_t y, const pm_char * s, LcdFlags attr)
{
  int x = (LCD_W - getTextWidth(s)) / 2;
  lcdDrawText(x, y, s, attr);
}

void lcdDrawTextAtIndex(coord_t x, coord_t y, const pm_char * s,uint8_t idx, LcdFlags flags)
{
  uint8_t length;
  length = pgm_read_byte(s++);
  lcdDrawSizedText(x, y, s+length*idx, length, flags & ~ZCHAR);
}

void lcdDrawHexNumber(coord_t x, coord_t y, uint32_t val, LcdFlags flags)
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
  lcdDrawText(x, y, s, flags);
}

void lcdDrawNumber(coord_t x, coord_t y, int32_t val, LcdFlags flags, uint8_t len, const char * prefix, const char * suffix)
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
  lcdDrawText(x, y, s, flags);
}

#if !defined(BOOT)
void lcdDrawLine(coord_t x1, coord_t y1, coord_t x2, coord_t y2, uint8_t pat, LcdFlags att)
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

void lcdDrawRect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t pat, LcdFlags att)
{
  if (att & ROUND) {
    lcdDrawVerticalLine(x, y+1, h-2, pat, att);
    lcdDrawVerticalLine(x+w-1, y+1, h-2, pat, att);
    lcdDrawHorizontalLine(x+1, y+h-1, w-2, pat, att);
    lcdDrawHorizontalLine(x+1, y, w-2, pat, att);
  }
  else {
    lcdDrawVerticalLine(x, y, h, pat, att);
    lcdDrawVerticalLine(x+w-1, y, h, pat, att);
    lcdDrawHorizontalLine(x+1, y+h-1, w-2, pat, att);
    lcdDrawHorizontalLine(x+1, y, w-2, pat, att);
  }
}

#if defined(SIMU)
void lcdDrawSolidFilledRect(coord_t x, scoord_t y, coord_t w, coord_t h, LcdFlags att)
{
  for (scoord_t i=y; i<y+h; i++) {
    lcdDrawHorizontalLine(x, i, w, SOLID, att);
  }
}
#endif

#if !defined(BOOT)
void lcdDrawFilledRect(coord_t x, scoord_t y, coord_t w, coord_t h, uint8_t pat, LcdFlags att)
{
  for (scoord_t i=y; i<y+h; i++) {
    if ((att&ROUND) && (i==y || i==y+h-1))
      lcdDrawHorizontalLine(x+1, i, w-2, pat, att);
    else
      lcdDrawHorizontalLine(x, i, w, pat, att);
  }
}
#endif

void putsRtcTime(coord_t x, coord_t y, LcdFlags att)
{
  putsTimer(x, y, getValue(MIXSRC_TX_TIME), att);
}

void getTimerString(char * str, putstime_t tme, LcdFlags att)
{
  div_t qr;

  if (tme < 0) {
    // TODO lcdDrawChar(x - ((att & DBLSIZE) ? FW+2 : ((att & MIDSIZE) ? FW+0 : FWNUM)), y, '-', att);
    tme = -tme;
    *str++ = '-';
  }

  qr = div(tme, 60);

  if (att & TIMEHOUR) {
    div_t qr2 = div(qr.quot, 60);
    *str++ = '0' + (qr2.quot/10);
    *str++ = '0' + (qr2.quot%10);
    *str++ = ':';
    qr.quot = qr2.rem;
  }

  *str++ = '0' + (qr.quot/10);
  *str++ = '0' + (qr.quot%10);
  *str++ = ':';
  *str++ = '0' + (qr.rem/10);
  *str++ = '0' + (qr.rem%10);
  *str = '\0';
}

void putsTimer(coord_t x, coord_t y, putstime_t tme, LcdFlags att)
{
  char str[10]; // "-00:00:00"
  getTimerString(str, tme, att);
  att &= ~LEFT;
  lcdDrawText(x, y, str, att);
}

void drawStringWithIndex(coord_t x, coord_t y, const pm_char * str, int idx, LcdFlags att, const char * prefix)
{
  char s[64];
  strAppendNumber(strAppend(strAppend(s, prefix), str), abs(idx));
  lcdDrawText(x, y, s, att);
}

void putsStickName(coord_t x, coord_t y, uint8_t idx, LcdFlags att)
{
  uint8_t length = STR_VSRCRAW[0];
  lcdDrawSizedText(x, y, STR_VSRCRAW+2+length*(idx+1), length-1, att);
}

void putsMixerSource(coord_t x, coord_t y, uint8_t idx, LcdFlags att)
{
  if (idx == MIXSRC_NONE) {
    lcdDrawTextAtIndex(x, y, STR_VSRCRAW, 0, att); // TODO macro
  }
  else if (idx <= MIXSRC_LAST_INPUT) {
    char s[32] = "\314";
    if (ZEXIST(g_model.inputNames[idx-MIXSRC_FIRST_INPUT])) {
      zchar2str(s+1, g_model.inputNames[idx-MIXSRC_FIRST_INPUT], LEN_INPUT_NAME);
      s[1+LEN_INPUT_NAME] = '\0';
    }
    else {
      strAppendNumber(s+1, idx, 2);
    }
    lcdDrawText(x, y, s, att);
  }
  else if (idx <= MIXSRC_LAST_LUA) {
#if defined(LUA_MODEL_SCRIPTS)
    div_t qr = div(idx-MIXSRC_FIRST_LUA, MAX_SCRIPT_OUTPUTS);
    if (qr.quot < MAX_SCRIPTS && qr.rem < scriptInputsOutputs[qr.quot].outputsCount) {
      char s[32] = "\322";
      s[1] = '1'+qr.quot;
      strncpy(&s[2], scriptInputsOutputs[qr.quot].outputs[qr.rem].name, 32-2);
      s[31] = '\0';
      lcdDrawText(x, y, s, att);
    }
#endif
  }
  else if (idx <= MIXSRC_LAST_POT) {
    idx = idx-MIXSRC_Rud;
    if (ZEXIST(g_eeGeneral.anaNames[idx]))
      lcdDrawSizedText(x, y, g_eeGeneral.anaNames[idx], LEN_ANA_NAME, ZCHAR|att);
    else
      lcdDrawTextAtIndex(x, y, STR_VSRCRAW, idx+1, att);
  }
  else if (idx >= MIXSRC_FIRST_SWITCH && idx < MIXSRC_FIRST_LOGICAL_SWITCH) {
    idx = idx-MIXSRC_FIRST_SWITCH;
    if (ZEXIST(g_eeGeneral.switchNames[idx]))
      lcdDrawSizedText(x, y, g_eeGeneral.switchNames[idx], LEN_SWITCH_NAME, ZCHAR|att);
    else
      lcdDrawTextAtIndex(x, y, STR_VSRCRAW, idx+MIXSRC_FIRST_SWITCH-MIXSRC_Rud+1, att);
  }
  else if (idx < MIXSRC_SW1)
    lcdDrawTextAtIndex(x, y, STR_VSRCRAW, idx-MIXSRC_Rud+1, att);
  else if (idx <= MIXSRC_LAST_LOGICAL_SWITCH)
    putsSwitches(x, y, SWSRC_SW1+idx-MIXSRC_SW1, att);
  else if (idx < MIXSRC_CH1)
    drawStringWithIndex(x, y, STR_PPM_TRAINER, idx-MIXSRC_FIRST_TRAINER+1, att);
  else if (idx <= MIXSRC_LAST_CH) {
    drawStringWithIndex(x, y, STR_CH, idx-MIXSRC_CH1+1, att);
#if 0
    if (ZEXIST(g_model.limitData[idx-MIXSRC_CH1].name) && (att & STREXPANDED)) {
      lcdDrawChar(lcdNextPos, y, ' ', att);
      lcdDrawSizedText(lcdNextPos+3, y, g_model.limitData[idx-MIXSRC_CH1].name, LEN_CHANNEL_NAME, ZCHAR|att);
    }
#endif
  }
  else if (idx <= MIXSRC_LAST_GVAR)
    drawStringWithIndex(x, y, STR_GV, idx-MIXSRC_GVAR1+1, att);
  else if (idx < MIXSRC_FIRST_TELEM) {
    lcdDrawTextAtIndex(x, y, STR_VSRCRAW, idx-MIXSRC_Rud+1-NUM_LOGICAL_SWITCH-NUM_TRAINER-NUM_CHNOUT-MAX_GVARS, att);
  }
  else {
    idx -= MIXSRC_FIRST_TELEM;
    div_t qr = div(idx, 3);
    char s[32];
    s[0] = '\321';
    int pos = 1 + zchar2str(&s[1], g_model.telemetrySensors[qr.quot].label, sizeof(g_model.telemetrySensors[qr.quot].label));
    if (qr.rem) s[pos++] = (qr.rem==2 ? '+' : '-');
    s[pos] = '\0';
    lcdDrawText(x, y, s, att);
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
    drawStringWithIndex(x, y, STR_MODEL, id+1, att|LEADING0);
  }
  else {
    lcdDrawSizedText(x, y, name, sizeof(g_model.header.name), ZCHAR|att);
  }
}

char * getStringAtIndex(char * dest, const char * s, int idx)
{
  uint8_t len = s[0];
  strncpy(dest, s+1+len*idx, len);
  dest[len] = '\0';
  return dest;
}

char * getStringWithIndex(char * dest, const char * s, int idx)
{
  strAppendNumber(strAppend(dest, s), abs(idx));
  return dest;
}

char * getSwitchString(char * dest, swsrc_t idx)
{
  if (idx == SWSRC_NONE) {
    return getStringAtIndex(dest, STR_VSWITCHES, 0);
  }
  else if (idx == SWSRC_OFF) {
    return getStringAtIndex(dest, STR_OFFON, 0);
  }

  char * s = dest;
  if (idx < 0) {
    *s++ = '!';
    idx = -idx;
  }

  if (idx <= SWSRC_LAST_SWITCH) {
    div_t swinfo = switchInfo(idx);
    if (ZEXIST(g_eeGeneral.switchNames[swinfo.quot])) {
      zchar2str(s, g_eeGeneral.switchNames[swinfo.quot], LEN_SWITCH_NAME);
      // TODO tous zchar2str
    }
    else {
      *s++ = 'S';
      *s++ = 'A'+swinfo.quot;
    }
    *s++ = "\300-\301"[swinfo.rem];
    *s = '\0';
  }
  else if (idx <= SWSRC_LAST_MULTIPOS_SWITCH) {
    div_t swinfo = div(idx - SWSRC_FIRST_MULTIPOS_SWITCH, XPOTS_MULTIPOS_COUNT);
    getStringWithIndex(s, "S", swinfo.quot*10+swinfo.rem+11);
  }
  else if (idx <= SWSRC_LAST_TRIM) {
    getStringAtIndex(s, STR_VSWITCHES, idx-SWSRC_FIRST_TRIM+1);
  }
  else if (idx <= SWSRC_LAST_LOGICAL_SWITCH) {
    getStringWithIndex(s, "L", idx-SWSRC_FIRST_LOGICAL_SWITCH+1);
  }
  else if (idx <= SWSRC_ONE) {
    getStringAtIndex(s, STR_VSWITCHES, idx-SWSRC_ON+1+(2*NUM_STICKS));
  }
  else if (idx <= SWSRC_LAST_FLIGHT_MODE) {
    getStringWithIndex(s, STR_FP, idx-SWSRC_FIRST_FLIGHT_MODE);
  }
  else if (idx == SWSRC_TELEMETRY_STREAMING) {
    strcpy(s, "Tele");
  }
  else {
    zchar2str(s, g_model.telemetrySensors[idx-SWSRC_FIRST_SENSOR].label, TELEM_LABEL_LEN);
  }

  return dest;
}

void putsSwitches(coord_t x, coord_t y, swsrc_t idx, LcdFlags flags)
{
  char s[8];
  getSwitchString(s, idx);
  lcdDrawText(x, y, s, flags);
}

void putsFlightMode(coord_t x, coord_t y, int8_t idx, LcdFlags att)
{
  if (idx==0) {
    lcdDrawTextAtIndex(x, y, STR_MMMINV, 0, att);
  }
  else {
    drawStringWithIndex(x, y, STR_FP, abs(idx)-1, att);
  }
}

void putsCurveRef(coord_t x, coord_t y, CurveRef &curve, LcdFlags att)
{
  if (curve.value != 0) {
    switch (curve.type) {
      case CURVE_REF_DIFF:
        lcdDrawText(x, y, "D", att);
        GVAR_MENU_ITEM(lcdNextPos, y, curve.value, -100, 100, LEFT|att, 0, 0);
        break;

      case CURVE_REF_EXPO:
        lcdDrawText(x, y, "E", att);
        GVAR_MENU_ITEM(lcdNextPos, y, curve.value, -100, 100, LEFT|att, 0, 0);
        break;

      case CURVE_REF_FUNC:
        lcdDrawTextAtIndex(x, y, STR_VCURVEFUNC, curve.value, att);
        break;

      case CURVE_REF_CUSTOM:
        drawCurveName(x, y, curve.value, att);
        break;
    }
  }
}

void drawCurveName(coord_t x, coord_t y, int8_t idx, LcdFlags flags)
{
  if (idx == 0) {
    return lcdDrawTextAtIndex(x, y, STR_MMMINV, 0, flags);
  }
  bool neg = false;
  if (idx < 0) {
    idx = -idx;
    neg = true;
  }
  if (ZEXIST(g_model.curves[idx-1].name))
    lcdDrawSizedText(x, y, g_model.curves[idx-1].name, LEN_CURVE_NAME, ZCHAR|flags);
  else
    drawStringWithIndex(x, y, STR_CV, idx, flags);
  if (neg) {
    if ((flags&INVERS) && ((~flags&BLINK) || BLINK_ON_PHASE))
      flags &= ~(INVERS|BLINK);
    lcdDrawChar(x-3, y, '!', flags);
  }
}

void putsTimerMode(coord_t x, coord_t y, int8_t mode, LcdFlags att)
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
    char s[2];
    s[0] = (mode % 2 == 0) ? ':' : '+';
    s[1] = '0'+p;
    lcdDrawSizedText(x, y, s, 2, att);
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
  if (!(att & NO_UNIT) && unit != UNIT_RAW) {
    char unitStr[8];
    strAppend(unitStr, STR_VTELEMUNIT+1+unit*STR_VTELEMUNIT[0], STR_VTELEMUNIT[0]);
    lcdDrawNumber(x, y, val, att, 0, NULL, unitStr);
  }
  else {
    lcdDrawNumber(x, y, val, att);
  }
}

void displayDate(coord_t x, coord_t y, TelemetryItem & telemetryItem, LcdFlags att)
{
}

void displayGpsCoords(coord_t x, coord_t y, TelemetryItem & telemetryItem, LcdFlags att)
{
}

void putsTelemetryChannelValue(coord_t x, coord_t y, uint8_t channel, int32_t value, LcdFlags att)
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

void putsChannelValue(coord_t x, coord_t y, source_t channel, int32_t value, LcdFlags att)
{
  if (channel >= MIXSRC_FIRST_TELEM) {
    channel = (channel-MIXSRC_FIRST_TELEM) / 3;
    putsTelemetryChannelValue(x, y, channel, value, att);
  }
  else if (channel >= MIXSRC_FIRST_TIMER || channel == MIXSRC_TX_TIME) {
    putsTimer(x, y, value, att);
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

void lcdDrawAlphaPixel(display_t * p, uint8_t opacity, uint16_t color)
{
  ASSERT_IN_DISPLAY(p);

  if (opacity == OPACITY_MAX) {
    lcdDrawPixel(p, color);
  }
  else if (opacity != 0) {
    uint8_t bgWeight = OPACITY_MAX - opacity;
    COLOR_SPLIT(color, red, green, blue);
    COLOR_SPLIT(*p, bgRed, bgGreen, bgBlue);
    uint16_t r = (bgRed * bgWeight + red * opacity) / OPACITY_MAX;
    uint16_t g = (bgGreen * bgWeight + green * opacity) / OPACITY_MAX;
    uint16_t b = (bgBlue * bgWeight + blue * opacity) / OPACITY_MAX;
    lcdDrawPixel(p, COLOR_JOIN(r, g, b));
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

void lcdDrawHorizontalLine(coord_t x, coord_t y, coord_t w, uint8_t pat, LcdFlags att)
{
  if (y >= LCD_H) return;
  if (x+w > LCD_W) { w = LCD_W - x; }

  display_t * p = PIXEL_PTR(x, y);
  display_t color = lcdColorTable[COLOR_IDX(att)];
  uint8_t opacity = 0x0F - (att >> 24);

  if (pat == SOLID) {
    while (w--) {
      lcdDrawAlphaPixel(p, opacity, color);
      p++;
    }
  }
  else {
    while (w--) {
      if (pat & 1) {
        lcdDrawAlphaPixel(p, opacity, color);
        pat = (pat >> 1) | 0x80;
      }
      else {
        pat = pat >> 1;
      }
      p++;
    }
  }
}

void lcdDrawVerticalLine(coord_t x, coord_t y, coord_t h, uint8_t pat, LcdFlags att)
{
  if (x >= LCD_W) return;
  if (y >= LCD_H) return;
  if (h<0) { y+=h; h=-h; }
  if (y<0) { h+=y; y=0; if (h<=0) return; }
  if (y+h > LCD_H) { h = LCD_H - y; }

  display_t color = lcdColorTable[COLOR_IDX(att)];
  uint8_t opacity = 0x0F - (att >> 24);

  if (pat == SOLID) {
    while (h--) {
      lcdDrawAlphaPixel(x, y, opacity, color);
      y++;
    }
  }
  else {
    if (pat==DOTTED && !(y%2)) {
      pat = ~pat;
    }
    while (h--) {
      if (pat & 1) {
        lcdDrawAlphaPixel(x, y, opacity, color);
        pat = (pat >> 1) | 0x80;
      }
      else {
        pat = pat >> 1;
      }
      y++;
    }
  }
}

#if defined(SIMU)
inline void lcdDrawBitmapDMA(coord_t x, coord_t y, coord_t width, coord_t height, const uint8_t * img)
{
  lcdDrawBitmap(x, y, img-4, 0, 0, -1);
}
#endif

#if !defined(BOOT)
void lcdDrawAlphaBitmap(coord_t x, coord_t y, const uint8_t * bmp)
{
  int width = getBitmapWidth(bmp);
  int height = getBitmapHeight(bmp);

  if (width == 0 || height == 0) {
    return;
  }

  for (coord_t line=0; line<height; line++) {
    display_t * p = &displayBuf[(y+line)*LCD_W + x];
    const uint8_t * q = bmp + 4 + line*width*3;
    for (coord_t col=0; col<width; col++) {
      lcdDrawAlphaPixel(p, q[2]>>4, *((uint16_t *)q));
      p++; q+=3;
    }
  }
}

void lcdDrawBitmap(coord_t x, coord_t y, const uint8_t * bmp, coord_t offset, coord_t height, int scale)
{
  int width = getBitmapWidth(bmp);
  int h = getBitmapHeight(bmp);

  if (!height || height > h) {
    height = h;
  }

  if (x+height > LCD_W) {
    height = LCD_W-x;
  }

  if (width == 0 || height == 0) {
    return;
  }

  if (scale == 0) {
    lcdDrawBitmapDMA(x, y, width, height, bmp + 4 + offset * width * 2);
  }
  else if (scale < 0) {
    for (coord_t i=0, row=0; row<height; i+=1, row-=scale) {
      display_t * p = &displayBuf[(y+i)*LCD_W + x];
      const uint8_t * q = bmp + 4 + (offset+row)*width*2;
      for (coord_t col=0; col<width; col-=scale) {
        lcdDrawPixel(p, *((uint16_t *)q));
        p++; q-=2*scale;
      }
    }
  }
  else {
    for (coord_t row=0; row<height; row++) {
      for (int i=0; i<scale; i++) {
        display_t * p = &displayBuf[(y+scale*row+i)*LCD_W + x];
        const uint8_t * q = bmp + 4 + (offset+row)*width*2;
        for (coord_t col=0; col<width; col++) {
          for (int j=0; j<scale; j++) {
            lcdDrawPixel(p, *((uint16_t *)q));
            p++;
          }
          q+=2;
        }
      }
    }
  }
}
#endif

void lcdDrawBlackOverlay()
{
  lcdDrawFilledRect(0, 0, LCD_W, LCD_H, SOLID, TEXT_COLOR | (8<<24));
}

void lcdDrawCircle(int x0, int y0, int radius)
{
  int x = radius;
  int y = 0;
  int decisionOver2 = 1 - x;

  while (y <= x) {
    lcdDrawPixel(x+x0, y+y0, WHITE);
    lcdDrawPixel(y+x0, x+y0, WHITE);
    lcdDrawPixel(-x+x0, y+y0, WHITE);
    lcdDrawPixel(-y+x0, x+y0, WHITE);
    lcdDrawPixel(-x+x0, -y+y0, WHITE);
    lcdDrawPixel(-y+x0, -x+y0, WHITE);
    lcdDrawPixel(x+x0, -y+y0, WHITE);
    lcdDrawPixel(y+x0, -x+y0, WHITE);
    y++;
    if (decisionOver2 <= 0) {
      decisionOver2 += 2*y + 1;
    }
    else {
      x--;
      decisionOver2 += 2 * (y-x) + 1;
    }
  }
}

#define PI 3.14159265

bool evalSlopes(int * slopes, int startAngle, int endAngle)
{
  if (startAngle >= 360 || endAngle <= 0)
    return false;

  if (startAngle == 0) {
    slopes[1] = 100000;
    slopes[2] = -100000;
  }
  else {
    float angle1 = float(startAngle) * PI / 180;
    if (startAngle >= 180) {
      slopes[1] = -100000;
      slopes[2] = cos(angle1) * 100 / sin(angle1);
    }
    else {
      slopes[1] = cos(angle1) * 100 / sin(angle1);
      slopes[2] = -100000;
    }
  }

  if (endAngle == 360) {
    slopes[0] = -100000;
    slopes[3] = 100000;
  }
  else {
    float angle2 = float(endAngle) * PI / 180;
    if (endAngle >= 180) {
      slopes[0] = -100000;
      slopes[3] = -cos(angle2) * 100 / sin(angle2);
    }
    else {
      slopes[0] = cos(angle2) * 100 / sin(angle2);
      slopes[3] = -100000;
    }
  }

  return true;
}

void lcdDrawPie(int x0, int y0, int radius, int startAngle, int endAngle)
{
  int slopes[4];
  if (!evalSlopes(slopes, startAngle, endAngle))
    return;

  for (int y=0; y<=radius; y++) {
    for (int x=0; x<=radius; x++) {
      if (x*x+y*y <= radius*radius) {
        int slope = (x==0 ? (y<0 ? -99000 : 99000) : y*100/x);
        if (slope >= slopes[0] && slope < slopes[1]) {
          lcdDrawPixel(x0+x, y0-y, WHITE);
        }
        if (-slope >= slopes[0] && -slope < slopes[1]) {
          lcdDrawPixel(x0+x, y0+y, WHITE);
        }
        if (slope >= slopes[2] && slope < slopes[3]) {
          lcdDrawPixel(x0-x, y0-y, WHITE);
        }
        if (-slope >= slopes[2] && -slope < slopes[3]) {
          lcdDrawPixel(x0-x, y0+y, WHITE);
        }
      }
    }
  }
}

void lcdDrawBitmapPie(int x0, int y0, const uint16_t * img, int startAngle, int endAngle)
{
  const uint16_t * q = img;
  coord_t width = *q++;
  coord_t height = *q++;

  int slopes[4];
  if (!evalSlopes(slopes, startAngle, endAngle))
    return;

  int w2 = width/2;
  int h2 = height/2;

  for (int y=h2-1; y>=0; y--) {
    for (int x=w2-1; x>=0; x--) {
      int slope = (x==0 ? (y<0 ? -99000 : 99000) : y*100/x);
      if (slope >= slopes[0] && slope < slopes[1]) {
        displayBuf[(y0+h2-y)*LCD_W + x0+w2+x] = q[(h2-y)*width + w2+x];
      }
      if (-slope >= slopes[0] && -slope < slopes[1]) {
        displayBuf[(y0+h2+y)*LCD_W + x0+w2+x] = q[(h2+y)*width + w2+x];
      }
      if (slope >= slopes[2] && slope < slopes[3]) {
        displayBuf[(y0+h2-y)*LCD_W + x0+w2-x] = q[(h2-y)*width + w2-x];
      }
      if (-slope >= slopes[2] && -slope < slopes[3]) {
        displayBuf[(y0+h2+y)*LCD_W + x0+w2-x] = q[(h2+y)*width + w2-x];
      }
    }
  }
}

void lcdDrawBitmapPatternPie(coord_t x0, coord_t y0, const uint8_t * img, LcdFlags flags, int startAngle, int endAngle)
{
  coord_t width = *((uint16_t *)img);
  coord_t height = *(((uint16_t *)img)+1);
  const uint8_t * q = img+4;

  int slopes[4];
  if (!evalSlopes(slopes, startAngle, endAngle))
    return;

  display_t color = lcdColorTable[COLOR_IDX(flags)];

  int w2 = width/2;
  int h2 = height/2;

  for (int y=h2-1; y>=0; y--) {
    for (int x=w2-1; x>=0; x--) {
      int slope = (x==0 ? (y<0 ? -99000 : 99000) : y*100/x);
      if (slope >= slopes[0] && slope < slopes[1]) {
        lcdDrawAlphaPixel(x0+w2+x, y0+h2-y, q[(h2-y)*width + w2+x], color);
      }
      if (-slope >= slopes[0] && -slope < slopes[1]) {
        lcdDrawAlphaPixel(x0+w2+x, y0+h2+y, q[(h2+y)*width + w2+x], color);
      }
      if (slope >= slopes[2] && slope < slopes[3]) {
        lcdDrawAlphaPixel(x0+w2-x, y0+h2-y, q[(h2-y)*width + w2-x], color);
      }
      if (-slope >= slopes[2] && -slope < slopes[3]) {
        lcdDrawAlphaPixel(x0+w2-x, y0+h2+y, q[(h2+y)*width + w2-x], color);
      }
    }
  }
}
