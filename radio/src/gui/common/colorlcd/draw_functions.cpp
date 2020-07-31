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
#include "opentx.h"

void drawStringWithIndex(coord_t x, coord_t y, const char * str, int idx, LcdFlags flags, const char * prefix, const char * suffix)
{
  char s[64];
  char * tmp = (prefix ? strAppend(s, prefix) : s);
  tmp = strAppend(tmp, str);
  tmp = strAppendUnsigned(tmp, abs(idx));
  if (suffix)
    strAppend(tmp, suffix);
  lcdDrawText(x, y, s, flags);
}

void drawValueWithUnit(coord_t x, coord_t y, int32_t val, uint8_t unit, LcdFlags att)
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

int editChoice(coord_t x, coord_t y, const char * values, int value, int min, int max, LcdFlags attr, event_t event, IsValueAvailable isValueAvailable)
{
  if (attr & INVERS) value = checkIncDec(event, value, min, max, (isModelMenuDisplayed()) ? EE_MODEL : EE_GENERAL, isValueAvailable);
  if (values) lcdDrawTextAtIndex(x, y, values, value-min, attr);
  return value;
}

uint8_t editCheckBox(uint8_t value, coord_t x, coord_t y, LcdFlags attr, event_t event )
{
  value = editChoice(x, y, NULL, value, 0, 1, attr, event);
  drawCheckBox(x, y, value, attr);
  return value;
}

swsrc_t editSwitch(coord_t x, coord_t y, swsrc_t value, LcdFlags attr, event_t event)
{
  if (attr & INVERS) CHECK_INCDEC_MODELSWITCH(event, value, SWSRC_FIRST_IN_MIXES, SWSRC_LAST_IN_MIXES, isSwitchAvailableInMixes);
  drawSwitch(x, y, value, attr);
  return value;
}

void drawFatalErrorScreen(const char * message)
{
  lcdClear();
  lcdDrawText(LCD_W/2, LCD_H/2-20, message, DBLSIZE|CENTERED|TEXT_BGCOLOR);
  lcdRefresh();
}

void runFatalErrorScreen(const char * message)
{
  while (true) {
    drawFatalErrorScreen(message);
    backlightEnable(100);
    uint8_t refresh = false;
    while (true) {
      uint32_t pwr_check = pwrCheck();
      if (pwr_check == e_power_off) {
        boardOff();
        return;  // only happens in SIMU, required for proper shutdown
      }
      else if (pwr_check == e_power_press) {
        refresh = true;
      }
      else if (pwr_check == e_power_on && refresh) {
        break;
      }
    }
  }
}

void drawPower(coord_t x, coord_t y, int8_t dBm, LcdFlags att)
{
  float power_W_PREC1 = pow(10.0, (dBm - 30.0) / 10.0) * 10;
  if (dBm >= 30) {
    lcdDrawNumber(x, y, power_W_PREC1, PREC1 | att);
    lcdDrawText(lcdNextPos, y, "W", att);
  }
  else if (dBm < 10) {
    uint16_t power_MW_PREC1 = round(power_W_PREC1 * 1000);
    lcdDrawNumber(x, y, power_MW_PREC1, PREC1 | att);
    lcdDrawText(lcdNextPos, y, "mW", att);
  }
  else {
    uint16_t power_MW = round(power_W_PREC1 * 100);
    if (power_MW >= 50) {
      power_MW = (power_MW / 5) * 5;
      lcdDrawNumber(x, y, power_MW, att);
      lcdDrawText(lcdNextPos, y, "mW", att);
    }
    else {
      lcdDrawNumber(x, y, power_MW, att);
      lcdDrawText(lcdNextPos, y, "mW", att);
    }
  }
}

void lcdDrawMMM(coord_t x, coord_t y, LcdFlags flags)
{
  lcdDrawTextAtIndex(x, y, STR_MMMINV, 0, flags);
}

#if defined(FLIGHT_MODES)
void drawFlightMode(coord_t x, coord_t y, int8_t idx, LcdFlags att)
{
  if (idx==0) {
    lcdDrawMMM(x, y, att);
    return;
  }
  // TODO this code was not included in Taranis! and used with abs(...) on Horus
  if (idx < 0) {
    lcdDrawChar(x-2, y, '!', att);
    idx = -idx;
  }
#if defined(CONDENSED)
  if (att & CONDENSED) {
    lcdDrawNumber(x+FW*1, y, idx-1, (att & ~CONDENSED), 1);
    return;
  }
#endif
  drawStringWithIndex(x, y, STR_FM, idx-1, att);
}
#endif

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

void drawCurveRef(coord_t x, coord_t y, CurveRef & curve, LcdFlags att)
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

void drawSensorCustomValue(coord_t x, coord_t y, uint8_t sensor, int32_t value, LcdFlags flags)
{
  if (sensor >= MAX_TELEMETRY_SENSORS) {
    // Lua luaLcdDrawChannel() can call us with a bad value
    return;
  }

  if (IS_FAI_FORBIDDEN(MIXSRC_FIRST_TELEM + 3 * sensor)) {
    lcdDrawText(x, y, "FAI mode", flags);
    return;
  }

  TelemetryItem & telemetryItem = telemetryItems[sensor];
  TelemetrySensor & telemetrySensor = g_model.telemetrySensors[sensor];

  if (telemetrySensor.unit == UNIT_DATETIME) {
    drawDate(x, y, telemetryItem, flags);
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
                lcdDrawText(x, flags & DBLSIZE ? y+1 : y, s, flags & ~DBLSIZE);
                break;
              }
            }
          }
        }
        else {
          if (value == 0) {
            lcdDrawText(x, flags & DBLSIZE ? y+1 : y, "Rx OK", flags & ~DBLSIZE);
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
              "Rx3 FS",
              "Rx3 LF",
              "Rx3 Lost",
              "Rx3 NS"
            };
            for (uint8_t i=0; i<DIM(RXS_STATUS); i++) {
              if (value & (1<<i)) {
                lcdDrawText(x, flags & DBLSIZE ? y+1 : y, RXS_STATUS[i], flags & ~DBLSIZE);
                break;
              }
            }
          }
        }
      }
    }
  }
  else if (telemetrySensor.unit == UNIT_TEXT) {
    lcdDrawSizedText(x, flags & DBLSIZE ? y+1 : y, telemetryItem.text, sizeof(telemetryItem.text), flags & ~DBLSIZE);
  }
  else {
    if (telemetrySensor.prec > 0) {
      flags |= (telemetrySensor.prec==1 ? PREC1 : PREC2);
    }
    drawValueWithUnit(x, y, value, telemetrySensor.unit == UNIT_CELLS ? UNIT_VOLTS : telemetrySensor.unit, flags);
  }
}

void drawSourceCustomValue(coord_t x, coord_t y, source_t source, int32_t value, LcdFlags flags)
{
  if (source >= MIXSRC_FIRST_TELEM) {
    source = (source-MIXSRC_FIRST_TELEM) / 3;
    drawSensorCustomValue(x, y, source, value, flags);
  }
  else if (source >= MIXSRC_FIRST_TIMER || source == MIXSRC_TX_TIME) {
    if (value < 0) flags |= BLINK|INVERS;
    drawTimer(x, y, value, flags);
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

void drawSourceValue(coord_t x, coord_t y, source_t source, LcdFlags flags)
{
  getvalue_t value = getValue(source);
  drawSourceCustomValue(x, y, source, value, flags);
}
