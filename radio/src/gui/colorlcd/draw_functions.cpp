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

#include "opentx.h"

void drawStatusText(const char * text)
{
  lcdDrawText(MENUS_MARGIN_LEFT, MENU_FOOTER_TOP, text, TEXT_STATUSBAR_COLOR);
}

void drawColumnHeader(const char * const * headers, const char * const * descriptions, uint8_t index)
{
  lcdDrawText(LCD_W-MENUS_MARGIN_LEFT, MENU_TITLE_TOP + 2, headers[index], RIGHT | SMLSIZE | MENU_TITLE_COLOR);
  if (descriptions) {
    drawStatusText(descriptions[index]);
  }
}

void drawCheckBox(coord_t x, coord_t y, uint8_t value, LcdFlags attr)
{
  if (attr) {
    lcdDrawSolidFilledRect(x-1, y+3, 14, 14, TEXT_INVERTED_BGCOLOR);
    lcdDrawSolidFilledRect(x+1, y+5, 10, 10, TEXT_BGCOLOR);
    if (value) {
      lcdDrawSolidFilledRect(x+2, y+6, 8, 8, TEXT_INVERTED_BGCOLOR);
    }
  }
  else {
    if (value) {
      lcdDrawSolidFilledRect(x+2, y+6, 8, 8, SCROLLBOX_COLOR);
      lcdDrawSolidRect(x, y+4, 12, 12, 1, LINE_COLOR);
    }
    else {
      lcdDrawSolidRect(x, y+4, 12, 12, 1, LINE_COLOR);
    }
  }
}

void drawVerticalScrollbar(coord_t x, coord_t y, coord_t h, uint16_t offset, uint16_t count, uint8_t visible)
{
  if (visible < count) {
    lcdDrawSolidVerticalLine(x, y, h, LINE_COLOR);
    coord_t yofs = (h*offset + count/2) / count;
    coord_t yhgt = (h*visible + count/2) / count;
    if (yhgt + yofs > h)
      yhgt = h - yofs;
    lcdDrawSolidFilledRect(x-1, y + yofs, 3, yhgt, SCROLLBOX_COLOR);
  }
}

void drawTrimSquare(coord_t x, coord_t y)
{
  lcdDrawSolidFilledRect(x-2, y, 15, 15, TRIM_BGCOLOR);
  lcdDrawBitmapPattern(x-2, y, LBM_TRIM_SHADOW, TRIM_SHADOW_COLOR);
}

void drawHorizontalTrimPosition(coord_t x, coord_t y, int16_t dir)
{
  drawTrimSquare(x, y);
  if (dir >= 0) {
    lcdDrawSolidVerticalLine(x+8, y+3, 9, TEXT_INVERTED_COLOR);
  }
  if (dir <= 0) {
    lcdDrawSolidVerticalLine(x+2, y+3, 9, TEXT_INVERTED_COLOR);
  }
  // if (exttrim) {
  //  lcdDrawSolidVerticalLine(xm, ym, 9, TEXT_INVERTED_COLOR);
  // }
}

void drawVerticalTrimPosition(coord_t x, coord_t y, int16_t dir)
{
  drawTrimSquare(x, y);
  if (dir >= 0) {
    lcdDrawSolidHorizontalLine(x+1, y+4, 9, TEXT_INVERTED_COLOR);
  }
  if (dir <= 0) {
    lcdDrawSolidHorizontalLine(x+1, y+10, 9, TEXT_INVERTED_COLOR);
  }
  // if (exttrim) {
  //   lcdDrawSolidHorizontalLine(xm-1, ym,  3, TEXT_INVERTED_COLOR);
  // }
}

void drawVerticalSlider(coord_t x, coord_t y, int len, int val, int min, int max, uint8_t steps, uint32_t options)
{
  val = limit(min, val, max);
  if (steps) {
    int delta = len / steps;
    for (int i = 0; i <= len; i += delta) {
      if ((options & OPTION_SLIDER_BIG_TICKS) && (i == 0 || i == len / 2 || i == len))
        lcdDrawSolidHorizontalLine(x, y + i, 13, TEXT_COLOR);
      else
        lcdDrawSolidHorizontalLine(x + 2, y + i, 9, TEXT_COLOR);
    }
  }
  else {
    lcdDrawBitmapPattern(x + 1, y, LBM_VTRIM_FRAME, TEXT_COLOR);
    /* if (g_model.displayTrims != DISPLAY_TRIMS_NEVER && trim != 0) {
      if (g_model.displayTrims == DISPLAY_TRIMS_ALWAYS || (trimsDisplayTimer > 0 && (trimsDisplayMask & (1<<i)))) {
        lcdDrawNumber((stickIndex==0 ? TRIM_LH_X : TRIM_RH_X)+(trim>0 ? -20 : 50), ym+1, trim, TINSIZE);
      }
    } */
  }
  y += len - divRoundClosest(len * (val - min), max - min) - 5;
  if (options & OPTION_SLIDER_TRIM_BUTTON) {
    drawVerticalTrimPosition(x, y - 2, val);
  }
  else if (options & OPTION_SLIDER_NUMBER_BUTTON) {
    drawTrimSquare(x, y - 2);
    lcdDrawChar(x + 2, y - 1, '0' + val, SMLSIZE | TEXT_INVERTED_COLOR);
  }
  else {
    drawTrimSquare(x, y - 2);
  }
}

void drawHorizontalSlider(coord_t x, coord_t y, int len, int val, int min, int max, uint8_t steps, uint32_t options)
{
  val = limit(min, val, max);
  int w = divRoundClosest(len * (val - min), max - min);
  if (options & OPTION_SLIDER_TICKS) {
    if (steps) {
      int delta = len / steps;
      for (int i = 0; i <= len; i += delta) {
        if ((options & OPTION_SLIDER_BIG_TICKS) && (i == 0 || i == len / 2 || i == len))
          lcdDrawSolidVerticalLine(x + i, y, 13, TEXT_COLOR);
        else
          lcdDrawSolidVerticalLine(x + i, y + 2, 9, TEXT_COLOR);
      }
    }
  }
  else if (options & OPTION_SLIDER_EMPTY_BAR) {
    lcdDrawBitmapPattern(x, y + 1, LBM_HTRIM_FRAME, TEXT_COLOR);
  }
  else if (options & OPTION_SLIDER_DBL_COLOR) {
    lcdDrawBitmapPattern(x, y + 8, LBM_SLIDER_BAR_LEFT, w <= 0 ? LINE_COLOR : TEXT_INVERTED_BGCOLOR);
    if (w > 4)
      lcdDrawSolidFilledRect(x + 4, y + 8, w - 4, 4, TEXT_INVERTED_BGCOLOR);
    if (w < len - 4)
      lcdDrawSolidFilledRect(x + w, y + 8, len - w - 4, 4, LINE_COLOR);
    lcdDrawBitmapPattern(x + len - 4, y + 8, LBM_SLIDER_BAR_RIGHT, w >= len ? TEXT_INVERTED_BGCOLOR : LINE_COLOR);
  }
  else {
    lcdDrawBitmapPattern(x, y + 8, LBM_SLIDER_BAR_LEFT, LINE_COLOR);
    lcdDrawSolidFilledRect(x + 4, y + 8, len - 8, 4, LINE_COLOR);
    lcdDrawBitmapPattern(x + len - 4, y + 8, LBM_SLIDER_BAR_RIGHT, LINE_COLOR);
    //
    /* if (g_model.displayTrims != DISPLAY_TRIMS_NEVER && trim != 0) {
      if (g_model.displayTrims == DISPLAY_TRIMS_ALWAYS || (trimsDisplayTimer > 0 && (trimsDisplayMask & (1<<i)))) {
        lcdDrawNumber((stickIndex==0 ? TRIM_LH_X : TRIM_RH_X)+(trim>0 ? -20 : 50), ym+1, trim, TINSIZE);
      }
    } */
  }
  x += w - 5;
  if (options & OPTION_SLIDER_TRIM_BUTTON) {
    drawHorizontalTrimPosition(x, y - 1, val);
  }
  else if (options & OPTION_SLIDER_NUMBER_BUTTON) {
    drawTrimSquare(x+2, y - 1);
    char text[] = { (char)('0' + val), '\0' };
    lcdDrawText(x + 7, y - 1, text, SMLSIZE | CENTERED | TEXT_INVERTED_COLOR);
  }
  else if (options & OPTION_SLIDER_SQUARE_BUTTON) {
    drawTrimSquare(x, y - 1);
  }
  else {
    lcdDrawBitmapPattern(x, y + 2, LBM_SLIDER_POINT_OUT, TEXT_COLOR);
    lcdDrawBitmapPattern(x, y + 2, LBM_SLIDER_POINT_MID, TEXT_BGCOLOR);
    if ((options & INVERS) && (!(options & BLINK) || !BLINK_ON_PHASE))
      lcdDrawBitmapPattern(x, y + 2, LBM_SLIDER_POINT_IN, TEXT_INVERTED_BGCOLOR);
  }
}

#if defined(GVARS)
bool noZero(int val)
{
  return val != 0;
}

void drawGVarValue(BitmapBuffer * dc, coord_t x, coord_t y, uint8_t gvar, gvar_t value, LcdFlags flags)
{
  uint8_t prec = g_model.gvars[gvar].prec;
  if (prec > 0) {
    flags |= (prec == 1 ? PREC1 : PREC2);
  }
  drawValueWithUnit(x, y, value, g_model.gvars[gvar].unit ? UNIT_PERCENT : UNIT_RAW, flags);
}

int16_t editGVarFieldValue(coord_t x, coord_t y, int16_t value, int16_t min, int16_t max, LcdFlags attr, uint8_t editflags, event_t event)
{
  uint16_t delta = GV_GET_GV1_VALUE(max);
  bool invers = (attr & INVERS);

  // TRACE("editGVarFieldValue(val=%d min=%d max=%d)", value, min, max);

  if (invers && event == EVT_KEY_LONG(KEY_ENTER)) {
    s_editMode = !s_editMode;
    if (attr & PREC1)
      value = (GV_IS_GV_VALUE(value, min, max) ? GET_GVAR(value, min, max, mixerCurrentFlightMode)*10 : delta);
    else
      value = (GV_IS_GV_VALUE(value, min, max) ? GET_GVAR(value, min, max, mixerCurrentFlightMode) : delta);
    storageDirty(EE_MODEL);
  }

  if (GV_IS_GV_VALUE(value, min, max)) {
    attr &= ~PREC1;

    int8_t idx = (int16_t) GV_INDEX_CALC_DELTA(value, delta);
    if (idx >= 0) ++idx;    // transform form idx=0=GV1 to idx=1=GV1 in order to handle double keys invert
    if (invers) {
      CHECK_INCDEC_MODELVAR_CHECK(event, idx, -MAX_GVARS, MAX_GVARS, noZero);
      if (idx == 0) idx = 1;    // handle reset to zero, map to GV1
    }
    if (idx < 0) {
      value = (int16_t) GV_CALC_VALUE_IDX_NEG(idx, delta);
      idx = -idx;
      drawStringWithIndex(x, y, STR_GV, idx, attr, "-");
    }
    else {
      drawStringWithIndex(x, y, STR_GV, idx, attr);
      value = (int16_t) GV_CALC_VALUE_IDX_POS(idx-1, delta);
    }
  }
  else {
    lcdDrawNumber(x, y, value, attr, 0, NULL, "%");
    if (invers) value = checkIncDec(event, value, min, max, EE_MODEL | editflags);
  }
  return value;
}
#else
void drawValueOrGVar(BitmapBuffer * dc, coord_t x, coord_t y, gvar_t value, LcdFlags flags)
{
  drawNumber(dc, x, y, value, flags, 0, nullptr, "%");
}
#endif

void drawSleepBitmap()
{
  lcd->setOffset(0, 0);
  lcd->clearClippingRect();
  lcd->clear();

  const BitmapBuffer * bitmap = BitmapBuffer::load(getThemePath("sleep.bmp"));
  if (bitmap) {
    lcd->drawBitmap((LCD_W-bitmap->getWidth())/2, (LCD_H-bitmap->getHeight())/2, bitmap);
    delete bitmap;
  }

  lcdRefresh();
}

#define SHUTDOWN_CIRCLE_DIAMETER       150
void drawShutdownAnimation(uint32_t duration, uint32_t totalDuration, const char * message)
{
  if (totalDuration == 0)
    return;

  static uint32_t lastDuration = 0xffffffff;
  static const BitmapBuffer * shutdown = BitmapBuffer::load(getThemePath("shutdown.bmp"));

  if (shutdown) {
    if (duration < lastDuration) {
      theme->drawBackground(lcd);
      lcd->drawBitmap((LCD_W-shutdown->getWidth())/2, (LCD_H-shutdown->getHeight())/2, shutdown);
      lcdStoreBackupBuffer();
    }
    else {
      lcdRestoreBackupBuffer();
      int quarter = duration / (totalDuration / 5);
      if (quarter >= 1) lcdDrawBitmapPattern(LCD_W/2,                            (LCD_H-SHUTDOWN_CIRCLE_DIAMETER)/2, LBM_SHUTDOWN_CIRCLE, TEXT_COLOR, 0, SHUTDOWN_CIRCLE_DIAMETER/2);
      if (quarter >= 2) lcdDrawBitmapPattern(LCD_W/2,                            LCD_H/2,                            LBM_SHUTDOWN_CIRCLE, TEXT_COLOR, SHUTDOWN_CIRCLE_DIAMETER/2, SHUTDOWN_CIRCLE_DIAMETER/2);
      if (quarter >= 3) lcdDrawBitmapPattern((LCD_W-SHUTDOWN_CIRCLE_DIAMETER)/2, LCD_H/2,                            LBM_SHUTDOWN_CIRCLE, TEXT_COLOR, SHUTDOWN_CIRCLE_DIAMETER, SHUTDOWN_CIRCLE_DIAMETER/2);
      if (quarter >= 4) lcdDrawBitmapPattern((LCD_W-SHUTDOWN_CIRCLE_DIAMETER)/2, (LCD_H-SHUTDOWN_CIRCLE_DIAMETER)/2, LBM_SHUTDOWN_CIRCLE, TEXT_COLOR, SHUTDOWN_CIRCLE_DIAMETER*3/2, SHUTDOWN_CIRCLE_DIAMETER/2);
    }
  }
  else {
    lcd->clear();
    int quarter = duration / (totalDuration / 5);
    for (int i=1; i<=4; i++) {
      if (quarter >= i) {
        lcd->drawSolidFilledRect(LCD_W / 2 - 70 + 24 * i, LCD_H / 2 - 10, 20, 20, TEXT_BGCOLOR);
      }
    }
  }

  lcdRefresh();
  lastDuration = duration;
}

void drawCurveRef(BitmapBuffer * dc, coord_t x, coord_t y, const CurveRef & curve, LcdFlags flags)
{
  if (curve.value != 0) {
    switch (curve.type) {
      case CURVE_REF_DIFF:
        dc->drawText(x, y, "D", flags);
        drawValueOrGVar(dc, lcdNextPos + 1, y + 2, curve.value, LEFT|SMLSIZE|flags);
        break;

      case CURVE_REF_EXPO:
        dc->drawText(x, y, "E", flags);
        drawValueOrGVar(dc, lcdNextPos + 1, y + 2, curve.value, LEFT|SMLSIZE|flags);
        break;

      case CURVE_REF_FUNC:
        drawTextAtIndex(dc, x, y, STR_VCURVEFUNC, curve.value, flags);
        break;

      case CURVE_REF_CUSTOM:
        dc->drawText(x, y, getCurveString(curve.value), flags);
        break;
    }
  }
}

void putsStickName(coord_t x, coord_t y, uint8_t idx, LcdFlags att)
{
  uint8_t length = STR_VSRCRAW[0];
  lcdDrawSizedText(x, y, STR_VSRCRAW+2+length*(idx+1), length-1, att);
}

void putsModelName(coord_t x, coord_t y, char * name, uint8_t id, LcdFlags att)
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

void drawCurveName(coord_t x, coord_t y, int8_t idx, LcdFlags flags)
{
  char s[8];
  getCurveString(s, idx);
  lcdDrawText(x, y, s, flags);
}

void drawSource(BitmapBuffer * dc, coord_t x, coord_t y, mixsrc_t idx, LcdFlags flags)
{
  char s[16];
  getSourceString(s, idx);
  dc->drawText(x, y, s, flags);
}

void drawSwitch(BitmapBuffer * dc, coord_t x, coord_t y, int32_t idx, LcdFlags flags)
{
  dc->drawText(x, y, getSwitchPositionName(idx), flags);
}

void drawTrimMode(BitmapBuffer * dc, coord_t x, coord_t y, uint8_t phase, uint8_t idx, LcdFlags att)
{
  trim_t v = getRawTrimValue(phase, idx);
  unsigned int mode = v.mode;
  unsigned int p = mode >> 1;

  if (mode == TRIM_MODE_NONE) {
    dc->drawText(x, y, "--", att);
  }
  else {
    char s[2];
    s[0] = (mode % 2 == 0) ? ':' : '+';
    s[1] = '0'+p;
    dc->drawSizedText(x, y, s, 2, att);
  }
}

void drawDate(BitmapBuffer * dc, coord_t x, coord_t y, TelemetryItem & telemetryItem, LcdFlags att)
{
  // TODO
  if (att & DBLSIZE) {
    x -= 42;
    att &= ~FONTSIZE_MASK;
    lcdDrawNumber(x, y, telemetryItem.datetime.day, att|LEADING0|LEFT, 2);
    lcdDrawChar(lcdNextPos-1, y, '-', att);
    lcdDrawNumber(lcdNextPos-1, y, telemetryItem.datetime.month, att|LEFT, 2);
    lcdDrawChar(lcdNextPos-1, y, '-', att);
    lcdDrawNumber(lcdNextPos-1, y, telemetryItem.datetime.year-2000, att|LEFT);
    y += FH;
    lcdDrawNumber(x, y, telemetryItem.datetime.hour, att|LEADING0|LEFT, 2);
    lcdDrawChar(lcdNextPos, y, ':', att);
    lcdDrawNumber(lcdNextPos, y, telemetryItem.datetime.min, att|LEADING0|LEFT, 2);
    lcdDrawChar(lcdNextPos, y, ':', att);
    lcdDrawNumber(lcdNextPos, y, telemetryItem.datetime.sec, att|LEADING0|LEFT, 2);
  }
  else {
    lcdDrawNumber(x, y, telemetryItem.datetime.day, att|LEADING0|LEFT, 2);
    lcdDrawChar(lcdNextPos-1, y, '-', att);
    lcdDrawNumber(lcdNextPos, y, telemetryItem.datetime.month, att|LEFT, 2);
    lcdDrawChar(lcdNextPos-1, y, '-', att);
    lcdDrawNumber(lcdNextPos, y, telemetryItem.datetime.year-2000, att|LEFT);
    lcdDrawNumber(lcdNextPos+11, y, telemetryItem.datetime.hour, att|LEADING0|LEFT, 2);
    lcdDrawChar(lcdNextPos, y, ':', att);
    lcdDrawNumber(lcdNextPos, y, telemetryItem.datetime.min, att|LEADING0|LEFT, 2);
    lcdDrawChar(lcdNextPos, y, ':', att);
    lcdDrawNumber(lcdNextPos, y, telemetryItem.datetime.sec, att|LEADING0|LEFT, 2);
  }
}

void drawGPSCoord(coord_t x, coord_t y, int32_t value, const char * direction, LcdFlags flags, bool seconds=true)
{
  char s[32];
  uint32_t absvalue = abs(value);
  char * tmp = strAppendUnsigned(s, absvalue / 1000000);
  *tmp++ = '@';
  absvalue = absvalue % 1000000;
  absvalue *= 60;
  if (g_eeGeneral.gpsFormat == 0 || !seconds) {
    tmp = strAppendUnsigned(tmp, absvalue / 1000000, 2);
    *tmp++ = '\'';
    if (seconds) {
      absvalue %= 1000000;
      absvalue *= 60;
      absvalue /= 100000;
      tmp = strAppendUnsigned(tmp, absvalue / 10);
      *tmp++ = '.';
      tmp = strAppendUnsigned(tmp, absvalue % 10);
      *tmp++ = '"';
    }
  }
  else {
    tmp = strAppendUnsigned(tmp, absvalue / 1000000, 2);
    *tmp++ = '.';
    absvalue /= 1000;
    tmp = strAppendUnsigned(tmp, absvalue, 3);
  }
  *tmp++ = direction[value>=0 ? 0 : 1];
  *tmp = '\0';
  lcdDrawText(x, y, s, flags);
}

void drawGPSPosition(coord_t x, coord_t y, int32_t longitude, int32_t latitude, LcdFlags flags)
{
  if (flags & EXPANDED) {
    drawGPSCoord(x, y, latitude, "NS", flags, true);
    drawGPSCoord(x, y + FH, longitude, "EW", flags, true);
  }
  else {
    drawGPSCoord(x, y, latitude, "NS", flags, false);
    drawGPSCoord(lcdNextPos+5, y, longitude, "EW", flags, false);
  }
}

void drawGPSSensorValue(coord_t x, coord_t y, TelemetryItem & telemetryItem, LcdFlags flags)
{
  drawGPSPosition(x, y, telemetryItem.gps.longitude, telemetryItem.gps.latitude, flags);
}


void drawSensorCustomValue(BitmapBuffer * dc, coord_t x, coord_t y, uint8_t sensor, int32_t value, LcdFlags flags)
{
  if (sensor >= MAX_TELEMETRY_SENSORS) {
    // Lua luaLcdDrawChannel() can call us with a bad value
    return;
  }

  TelemetryItem & telemetryItem = telemetryItems[sensor];
  TelemetrySensor & telemetrySensor = g_model.telemetrySensors[sensor];

  if (telemetrySensor.unit == UNIT_DATETIME) {
    drawDate(dc, x, y, telemetryItem, flags);
  }
  else if (telemetrySensor.unit == UNIT_GPS) {
    drawGPSSensorValue(x, y, telemetryItem, flags);
  }
  else if (telemetrySensor.unit == UNIT_BITFIELD) {
    if (IS_FRSKY_SPORT_PROTOCOL()) {
      if (telemetrySensor.id >= RBOX_STATE_FIRST_ID && telemetrySensor.id <= RBOX_STATE_LAST_ID) {
        if (telemetrySensor.subId == 0) {
          if (value == 0) {
            lcdDrawText(x, y, "OK", flags);
          }
          else {
            for (uint8_t i=0; i<16; i++) {
              if (value & (1 << i)) {
                char s[] = "CH__ KO";
                strAppendUnsigned(&s[2], i+1, 2);
                dc->drawText(x, flags & DBLSIZE ? y+1 : y, s, flags & ~DBLSIZE);
                break;
              }
            }
          }
        }
        else {
          if (value == 0) {
            dc->drawText(x, flags & DBLSIZE ? y+1 : y, "Rx OK", flags & ~DBLSIZE);
          }
          else {
            static const char * const RXS_STATUS[] = {
              "Rx1 Ovl",
              "Rx2 Ovl",
              "SBUS Ovl",
              "Rx1 FS",
              "Rx1 LF",
              "Rx2 FS",
              "Rx2 LF",
              "Rx1 Lost",
              "Rx2 Lost",
              "Rx1 NS",
              "Rx2 NS",
            };
            for (uint8_t i=0; i<DIM(RXS_STATUS); i++) {
              if (value & (1<<i)) {
                dc->drawText(x, flags & DBLSIZE ? y+1 : y, RXS_STATUS[i], flags & ~DBLSIZE);
                break;
              }
            }
          }
        }
      }
    }
  }
  else if (telemetrySensor.unit == UNIT_TEXT) {
    dc->drawSizedText(x, flags & DBLSIZE ? y+1 : y, telemetryItem.text, sizeof(telemetryItem.text), flags & ~DBLSIZE);
  }
  else {
    if (telemetrySensor.prec > 0) {
      flags |= (telemetrySensor.prec==1 ? PREC1 : PREC2);
    }
    drawValueWithUnit(dc, x, y, value, telemetrySensor.unit == UNIT_CELLS ? UNIT_VOLTS : telemetrySensor.unit, flags);
  }
}

void drawSourceCustomValue(BitmapBuffer * dc, coord_t x, coord_t y, source_t source, int32_t value, LcdFlags flags)
{
  if (source >= MIXSRC_FIRST_TELEM) {
    source = (source-MIXSRC_FIRST_TELEM) / 3;
    drawSensorCustomValue(dc, y, source, value, flags);
  }
  else if (source >= MIXSRC_FIRST_TIMER || source == MIXSRC_TX_TIME) {
    if (value < 0) flags |= BLINK|INVERS;
    // TODO drawTimer(dc, x, y, value, flags);
  }
  else if (source == MIXSRC_TX_VOLTAGE) {
    lcdDrawNumber(x, y, value, flags|PREC1);
  }
#if defined(INTERNAL_GPS)
    else if (source == MIXSRC_TX_GPS) {
    if (gpsData.fix) {
      drawGPSPosition(x, y, gpsData.longitude, gpsData.latitude, flags);
    }
    else {
      lcdDrawText(x, y, "sats: ", flags);
      lcdDrawNumber(lcdNextPos, y, gpsData.numSat, flags);
    }
  }
#endif
#if defined(GVARS)
    else if (source >= MIXSRC_FIRST_GVAR && source <= MIXSRC_LAST_GVAR) {
    drawGVarValue(x, y, source - MIXSRC_FIRST_GVAR, value, flags);
  }
#endif
  else if (source < MIXSRC_FIRST_CH) {
    lcdDrawNumber(x, y, calcRESXto100(value), flags);
  }
  else if (source <= MIXSRC_LAST_CH) {
#if defined(PPM_UNIT_PERCENT_PREC1)
    lcdDrawNumber(x, y, calcRESXto1000(value), flags|PREC1);
#else
    lcdDrawNumber(x, y, calcRESXto100(value), flags);
#endif
  }
  else {
    lcdDrawNumber(x, y, value, flags);
  }
}

void drawValueWithUnit(BitmapBuffer * dc, coord_t x, coord_t y, int val, uint8_t unit, LcdFlags flags)
{
  // convertUnit(val, unit);
  drawNumber(dc, x, y, val, flags & (~NO_UNIT));
  if (!(flags & NO_UNIT) && unit != UNIT_RAW) {
    drawTextAtIndex(dc, lcdNextPos/*+1*/, y, STR_VTELEMUNIT, unit, 0);
  }
}

void drawHexNumber(BitmapBuffer * dc, coord_t x, coord_t y, uint32_t val, LcdFlags flags)
{
  for (int i=12; i>=0; i-=4) {
    char c = (val >> i) & 0xf;
    c = c>9 ? c+'A'-10 : c+'0';
    dc->drawSizedText(x, y, &c, 1, flags);
    x = lcdNextPos;
  }
}