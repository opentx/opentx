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

#define FONT_MAX_HEIGHT 42
void lcdPutFontPattern(coord_t x, coord_t y, const uint8_t * font, const uint16_t * spec, int index, LcdFlags flags)
{
  coord_t offset = spec[index];
  coord_t width = spec[index+1] - offset;
  if (width > 0) lcdDrawBitmapPattern(x, y, font, flags, offset, width);
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

int getTextWidth(const char * s, int len, LcdFlags flags)
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

void lcdDrawText(coord_t x, coord_t y, const pm_char * s, LcdFlags flags)
{
  lcdDrawSizedText(x, y, s, 255, flags);
}

void lcdDrawTextAtIndex(coord_t x, coord_t y, const pm_char * s, uint8_t idx, LcdFlags flags)
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
  flags &= ~LEADING0;
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

void lcdDrawRect(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t thickness, uint8_t pat, LcdFlags att)
{
#if 0
  if (att & ROUND) {
    lcdDrawVerticalLine(x, y+1, h-2, pat, att);
    lcdDrawVerticalLine(x+w-1, y+1, h-2, pat, att);
    lcdDrawHorizontalLine(x+1, y+h-1, w-2, pat, att);
    lcdDrawHorizontalLine(x+1, y, w-2, pat, att);
  }
  else
#endif
  {
    for (int i=0; i<thickness; i++) {
      lcdDrawVerticalLine(x+i, y, h, pat, att);
      lcdDrawVerticalLine(x+w-1-i, y, h, pat, att);
      lcdDrawHorizontalLine(x, y+h-1-i, w, pat, att);
      lcdDrawHorizontalLine(x, y+i, w, pat, att);
    }
  }
}

#if defined(SIMU)
void lcdDrawSolidFilledRect(coord_t x, coord_t y, coord_t w, coord_t h, LcdFlags att)
{
  for (coord_t i=y; i<y+h; i++) {
    lcdDrawHorizontalLine(x, i, w, SOLID, att);
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
  strAppendUnsigned(strAppend(strAppend(s, prefix), str), abs(idx));
  lcdDrawText(x, y, s, att);
}

void putsStickName(coord_t x, coord_t y, uint8_t idx, LcdFlags att)
{
  uint8_t length = STR_VSRCRAW[0];
  lcdDrawSizedText(x, y, STR_VSRCRAW+2+length*(idx+1), length-1, att);
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
  strAppendUnsigned(strAppend(dest, s), abs(idx));
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

char * getSourceString(char * dest, mixsrc_t idx)
{
  if (idx == MIXSRC_NONE) {
    return getStringAtIndex(dest, STR_VSRCRAW, 0);
  }
  else if (idx <= MIXSRC_LAST_INPUT) {
    idx -= MIXSRC_FIRST_INPUT;
    *dest++ = '\314';
    if (ZEXIST(g_model.inputNames[idx])) {
      zchar2str(dest, g_model.inputNames[idx], LEN_INPUT_NAME);
      dest[LEN_INPUT_NAME] = '\0';
    }
    else {
      strAppendUnsigned(dest, idx, 2);
    }
  }
  else if (idx <= MIXSRC_LAST_LUA) {
#if defined(LUA_MODEL_SCRIPTS)
    div_t qr = div(idx-MIXSRC_FIRST_LUA, MAX_SCRIPT_OUTPUTS);
    if (qr.quot < MAX_SCRIPTS && qr.rem < scriptInputsOutputs[qr.quot].outputsCount) {
      *dest++ = '\322';
      // *dest++ = '1'+qr.quot;
      strcpy(dest, scriptInputsOutputs[qr.quot].outputs[qr.rem].name);
    }
#endif
  }
  else if (idx <= MIXSRC_LAST_POT) {
    idx -= MIXSRC_Rud;
    if (ZEXIST(g_eeGeneral.anaNames[idx])) {
      zchar2str(dest, g_eeGeneral.anaNames[idx], LEN_ANA_NAME);
      dest[LEN_ANA_NAME] = '\0';
    }
    else {
      getStringAtIndex(dest, STR_VSRCRAW, idx + 1);
    }
  }
  else if (idx >= MIXSRC_FIRST_SWITCH && idx <= MIXSRC_LAST_SWITCH) {
    idx -= MIXSRC_FIRST_SWITCH;
    if (ZEXIST(g_eeGeneral.switchNames[idx])) {
      zchar2str(dest, g_eeGeneral.switchNames[idx], LEN_SWITCH_NAME);
      dest[LEN_SWITCH_NAME] = '\0';
    }
    else {
      getStringAtIndex(dest, STR_VSRCRAW, idx + MIXSRC_FIRST_SWITCH - MIXSRC_Rud + 1);
    }
  }
  else if (idx <= MIXSRC_LAST_LOGICAL_SWITCH) {
    getSwitchString(dest, SWSRC_SW1 + idx - MIXSRC_SW1);
  }
  else if (idx <= MIXSRC_LAST_TRAINER) {
    getStringWithIndex(dest, STR_PPM_TRAINER, idx - MIXSRC_FIRST_TRAINER + 1);
  }
  else if (idx <= MIXSRC_LAST_CH) {
    getStringWithIndex(dest, STR_CH, idx - MIXSRC_CH1 + 1);
  }
  else if (idx <= MIXSRC_LAST_GVAR) {
    getStringWithIndex(dest, STR_GV, idx - MIXSRC_GVAR1 + 1);
  }
  else if (idx < MIXSRC_FIRST_TELEM) {
    getStringAtIndex(dest, STR_VSRCRAW, idx-MIXSRC_Rud+1-NUM_LOGICAL_SWITCH-NUM_TRAINER-NUM_CHNOUT-MAX_GVARS);
  }
  else {
    idx -= MIXSRC_FIRST_TELEM;
    div_t qr = div(idx, 3);
    dest[0] = '\321';
    int pos = 1 + zchar2str(&dest[1], g_model.telemetrySensors[qr.quot].label, sizeof(g_model.telemetrySensors[qr.quot].label));
    if (qr.rem) dest[pos++] = (qr.rem==2 ? '+' : '-');
    dest[pos] = '\0';
  }

  return dest;
}

void putsMixerSource(coord_t x, coord_t y, uint8_t idx, LcdFlags flags)
{
  char s[16];
  getSourceString(s, idx);
  lcdDrawText(x, y, s, flags);
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

void lcdDrawPoint(coord_t x, coord_t y, LcdFlags att)
{
  display_t * p = PIXEL_PTR(x, y);
  display_t color = lcdColorTable[COLOR_IDX(att)];
  if (p < DISPLAY_END) {
    *p = color;
  }
}

void lcdDrawBlackOverlay()
{
  lcdDrawFilledRect(0, 0, LCD_W, LCD_H, SOLID, OVERLAY_COLOR | OPACITY(8));
}

#if defined(SIMU)
BitmapBuffer _lcd(BMP_RGB565, LCD_W, LCD_H, displayBuf);
BitmapBuffer * lcd = &_lcd;

void DMAFillRect(uint16_t * dest, uint16_t destw, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color)
{
  for (int i=0; i<h; i++) {
    for (int j=0; j<w; j++) {
      dest[(y+i)*destw+x+j] = color;
    }
  }
}

void DMACopyBitmap(uint16_t * dest, uint16_t destw, uint16_t x, uint16_t y, const uint16_t * src, uint16_t srcw, uint16_t srcx, uint16_t srcy, uint16_t w, uint16_t h)
{
  for (int i=0; i<h; i++) {
    memcpy(dest+(y+i)*destw+x, src+(srcy+i)*srcw+srcx, 2*w);
  }
}

void DMACopyAlphaBitmap(uint16_t * dest, uint16_t destw, uint16_t x, uint16_t y, const uint16_t * src, uint16_t srcw, uint16_t srcx, uint16_t srcy, uint16_t w, uint16_t h)
{
  for (coord_t line=0; line<h; line++) {
    uint16_t * p = dest + (y+line)*destw + x;
    const uint16_t * q = src + (srcy+line)*srcw + srcx;
    for (coord_t col=0; col<w; col++) {
      uint8_t alpha = *q >> 12;
      uint8_t red = ((((*q >> 8) & 0x0f) << 1) * alpha + (*p >> 11) * (0x0f-alpha)) / 0x0f;
      uint8_t green = ((((*q >> 4) & 0x0f) << 2) * alpha + ((*p >> 5) & 0x3f) * (0x0f-alpha)) / 0x0f;
      uint8_t blue = ((((*q >> 0) & 0x0f) << 1) * alpha + ((*p >> 0) & 0x1f) * (0x0f-alpha)) / 0x0f;
      *p = (red << 11) + (green << 5) + (blue << 0);
      p++; q++;
    }
  }
}

void DMABitmapConvert(uint16_t * dest, const uint8_t * src, uint16_t w, uint16_t h, uint32_t format)
{
  if (format == DMA2D_ARGB4444) {
    for (int row = 0; row < h; ++row) {
      for (int col = 0; col < w; ++col) {
        *dest = ARGB(src[0], src[1], src[2], src[3]);
        ++dest;
        src += 4;
      }
    }
  }
  else {
    for (int row = 0; row < h; ++row) {
      for(int col = 0; col < w; ++col) {
        *dest = RGB(src[1], src[2], src[3]);
        ++dest;
        src += 4;
      }
    }
  }
}
#endif
