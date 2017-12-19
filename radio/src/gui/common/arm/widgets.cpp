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
