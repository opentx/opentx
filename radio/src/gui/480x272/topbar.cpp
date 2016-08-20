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

unsigned int Topbar::getZonesCount() const
{
  return MAX_TOPBAR_ZONES;
}

Zone Topbar::getZone(unsigned int index) const
{
  Zone zone;
  zone.x = 49 + (TOPBAR_ZONE_WIDTH + 2*TOPBAR_ZONE_MARGIN) * index;
  zone.y = TOPBAR_ZONE_MARGIN;
  zone.w = TOPBAR_ZONE_WIDTH;
  zone.h = MENU_HEADER_HEIGHT - 2*TOPBAR_ZONE_MARGIN;
  return zone;
}

const char * const STR_MONTHS[] = TR_MONTHS;

void drawTopbarDatetime()
{
  lcd->drawSolidVerticalLine(DATETIME_SEPARATOR_X, 7, 31, TEXT_INVERTED_COLOR);

  struct gtm t;
  gettime(&t);
  char str[10];
  sprintf(str, "%d %s", t.tm_mday, STR_MONTHS[t.tm_mon]);
  lcdDrawText(DATETIME_MIDDLE, DATETIME_LINE1, str, SMLSIZE|TEXT_INVERTED_COLOR|CENTERED);

  getTimerString(str, getValue(MIXSRC_TX_TIME));
  lcdDrawText(DATETIME_MIDDLE, DATETIME_LINE2, str, SMLSIZE|TEXT_INVERTED_COLOR|CENTERED);
}

void drawTopBar()
{
  theme->drawTopbarBackground(0);

  // USB icon
  if (usbPlugged()) {
    lcdDrawBitmapPattern(LCD_W-98, 8, LBM_TOPMENU_USB, MENU_TITLE_COLOR);
  }

  // RSSI
  const uint8_t rssiBarsValue[] = {30, 40, 50, 60, 80};
  const uint8_t rssiBarsHeight[] = {5, 10, 15, 21, 31};
  for (unsigned int i = 0; i < DIM(rssiBarsHeight); i++) {
    uint8_t height = rssiBarsHeight[i];
    lcdDrawSolidFilledRect(LCD_W-90 + i * 6, 38 - height, 4, height, TELEMETRY_RSSI() >= rssiBarsValue[i] ? MENU_TITLE_COLOR : MENU_TITLE_DISABLE_COLOR);
  }

  /* Audio volume */
  lcdDrawBitmapPattern(LCD_W-130, 4, LBM_TOPMENU_VOLUME_SCALE, MENU_TITLE_DISABLE_COLOR);
  if (requiredSpeakerVolume == 0 || g_eeGeneral.beepMode == e_mode_quiet)
    lcdDrawBitmapPattern(LCD_W-130, 4, LBM_TOPMENU_VOLUME_0, MENU_TITLE_COLOR);
  else if (requiredSpeakerVolume < 7)
    lcdDrawBitmapPattern(LCD_W-130, 4, LBM_TOPMENU_VOLUME_1, MENU_TITLE_COLOR);
  else if (requiredSpeakerVolume < 13)
    lcdDrawBitmapPattern(LCD_W-130, 4, LBM_TOPMENU_VOLUME_2, MENU_TITLE_COLOR);
  else if (requiredSpeakerVolume < 19)
    lcdDrawBitmapPattern(LCD_W-130, 4, LBM_TOPMENU_VOLUME_3, MENU_TITLE_COLOR);
  else
    lcdDrawBitmapPattern(LCD_W-130, 4, LBM_TOPMENU_VOLUME_4, MENU_TITLE_COLOR);

  /* Tx battery */
  uint8_t bars = limit<int8_t>(0, 6 * (g_vbat100mV - g_eeGeneral.vBatMin - 90) / (30 + g_eeGeneral.vBatMax - g_eeGeneral.vBatMin), 5);
  lcdDrawBitmapPattern(LCD_W-130, 24, LBM_TOPMENU_TXBATT, MENU_TITLE_COLOR);
  for (unsigned int i = 0; i < 5; i++) {
    lcdDrawSolidFilledRect(LCD_W-122+4*i, 30, 2, 8, i >= bars ? MENU_TITLE_DISABLE_COLOR : MENU_TITLE_COLOR);
  }

  topbar->refresh();

#if 0
  // Radio battery - TODO
  // drawValueWithUnit(370, 8, g_vbat100mV, UNIT_VOLTS, PREC1|SMLSIZE|MENU_TITLE_COLOR);
  // lcdDrawSolidRect(300, 3, 20, 50, MENU_TITLE_COLOR);
  // lcdDrawRect(batt_icon_x+FW, BAR_Y+1, 13, 7);
  // lcdDrawSolidVerticalLine(batt_icon_x+FW+13, BAR_Y+2, 5);

  // Rx battery
  if (g_model.frsky.voltsSource) { // TODO should not be in frsky struct
    TelemetryItem & item = telemetryItems[g_model.frsky.voltsSource-1];
    if (item.isAvailable()) {
      int32_t value = item.value;
      TelemetrySensor & sensor = g_model.telemetrySensors[g_model.frsky.altitudeSource-1];
      LcdFlags att = 0;
      if (sensor.prec == 2) {
        att |= PREC1;
        value /= 10;
      }
      else if (sensor.prec == 1) {
        att |= PREC1;
      }
      att |= (item.isOld() ? ALARM_COLOR : TEXT_COLOR);
      lcdDrawSolidFilledRect(ALTITUDE_X, VOLTS_Y, ALTITUDE_W, ALTITUDE_H, TEXT_BGCOLOR);
      lcdDrawText(ALTITUDE_X+PADDING, VOLTS_Y+2, "Voltage", att);
      drawValueWithUnit(ALTITUDE_X+PADDING, VOLTS_Y+12, value, UNIT_VOLTS, DBLSIZE|LEFT|att);
    }
  }

  // Model altitude
  if (g_model.frsky.altitudeSource) {
    TelemetryItem & item = telemetryItems[g_model.frsky.altitudeSource-1];
    if (item.isAvailable()) {
      int32_t value = item.value;
      TelemetrySensor & sensor = g_model.telemetrySensors[g_model.frsky.altitudeSource-1];
      if (sensor.prec) value /= sensor.prec == 2 ? 100 : 10;
      LcdFlags att = (item.isOld() ? ALARM_COLOR : TEXT_COLOR);
      lcdDrawSolidFilledRect(ALTITUDE_X, ALTITUDE_Y, ALTITUDE_W, ALTITUDE_H, TEXT_BGCOLOR);
      lcdDrawText(ALTITUDE_X+PADDING, ALTITUDE_Y+2, "Alt", att);
      drawValueWithUnit(ALTITUDE_X+PADDING, ALTITUDE_Y+12, value, UNIT_METERS, DBLSIZE|LEFT|att);
    }
  }
#endif

}
