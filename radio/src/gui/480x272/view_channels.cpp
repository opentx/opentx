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

#define RECT_OFFSET                    80
#define ROW_HEIGHT                     42
#define BAR_HEIGHT                     13
#define COLUMN_SIZE                   200
#define X_OFFSET                       25
#define Y_OFFSET                       75
#define Y_OUTBAR                       15
#define Y_MIXBAR                       28
#define LEG_COLORBOX                   15

#define VIEW_CHANNELS_LIMIT_PCT        (g_model.extendedLimits ? LIMIT_EXT_PERCENT : 100)

bool menuChannelsMonitor(event_t event, uint8_t page);
bool menuLogicalSwitches(event_t);

template<int index>
bool menuChannelsMonitor(event_t event)
{
  lastMonitorPage = e_MonChannelsFirst + index;
  MENU(STR_MONITOR_CHANNELS[index], MONITOR_ICONS, menuTabMonitors, lastMonitorPage, 0, { 0 });
  return menuChannelsMonitor(event, index);
}

const MenuHandlerFunc menuTabMonitors[] PROGMEM = {
  menuChannelsMonitor<0>,
  menuChannelsMonitor<1>,
  menuChannelsMonitor<2>,
  menuChannelsMonitor<3>,
  menuLogicalSwitches
};

uint8_t lastMonitorPage = 0;

uint16_t posOnBar(int16_t value_to100)
{
  return divRoundClosest((value_to100 + VIEW_CHANNELS_LIMIT_PCT) * COLUMN_SIZE, VIEW_CHANNELS_LIMIT_PCT * 2);
}

void drawOutputBarLimits(coord_t left, coord_t right, coord_t y)
{
  lcd->drawSolidVerticalLine(left, y, BAR_HEIGHT, TEXT_COLOR);
  lcd->drawSolidHorizontalLine(left, y, 3, TEXT_COLOR);
  lcd->drawSolidHorizontalLine(left, y + BAR_HEIGHT - 1, 3, TEXT_COLOR);

  lcd->drawSolidVerticalLine(--right, y, BAR_HEIGHT, TEXT_COLOR);
  lcd->drawSolidHorizontalLine(right - 3, y, 3, TEXT_COLOR);
  lcd->drawSolidHorizontalLine(right - 3, y + BAR_HEIGHT - 1, 3, TEXT_COLOR);
}

void drawSingleMixerBar(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t channel)
{
  int16_t chanVal = calcRESXto100(ex_chans[channel]);
  const int16_t displayVal = chanVal;

  // this could be handled nicer, but slower, by checking actual range for this mixer
  chanVal = limit<int16_t>(-VIEW_CHANNELS_LIMIT_PCT, chanVal, VIEW_CHANNELS_LIMIT_PCT);

  lcdDrawSolidFilledRect(x, y, w, h, BARGRAPH_BGCOLOR);
  if (chanVal > 0) {
    lcdDrawSolidFilledRect(x + w / 2, y, divRoundClosest(chanVal * w, VIEW_CHANNELS_LIMIT_PCT * 2), h, BARGRAPH2_COLOR);
    lcdDrawNumber(x - 10 + w / 2, y - 2, displayVal, SMLSIZE | TEXT_COLOR | RIGHT, 0, NULL, "%");
  }
  else if (chanVal < 0) {
    const uint16_t endpoint = x + w / 2;
    const uint16_t size = divRoundClosest(-chanVal * w, VIEW_CHANNELS_LIMIT_PCT * 2);
    lcdDrawSolidFilledRect(endpoint - size, y, size, h, BARGRAPH2_COLOR);
    lcdDrawNumber(x + 10 + w / 2, y - 2, displayVal, SMLSIZE | TEXT_COLOR, 0, NULL, "%");
  }

  lcd->drawSolidVerticalLine(x + w / 2, y, h, TEXT_COLOR);
}

void drawSingleOutputBar(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t channel)
{
  int16_t chanVal = calcRESXto100(channelOutputs[channel]);
  int16_t displayVal = chanVal;

  chanVal = limit<int16_t>(-VIEW_CHANNELS_LIMIT_PCT, chanVal, VIEW_CHANNELS_LIMIT_PCT);

  lcdDrawSolidFilledRect(x, y, w, h, BARGRAPH_BGCOLOR);
  if (chanVal > 0) {
    lcdDrawSolidFilledRect(x + w / 2, y, divRoundClosest(chanVal * w, VIEW_CHANNELS_LIMIT_PCT * 2), h, BARGRAPH1_COLOR);
    lcdDrawNumber(x - 10 + w / 2, y - 2, displayVal, SMLSIZE | TEXT_COLOR | RIGHT, 0, NULL, "%");
  }
  else if (chanVal < 0) {
    uint16_t endpoint = x + w / 2;
    uint16_t size = divRoundClosest(-chanVal * w, VIEW_CHANNELS_LIMIT_PCT * 2);
    lcdDrawSolidFilledRect(endpoint - size, y, size, h, BARGRAPH1_COLOR);
    lcdDrawNumber(x + 10 + w / 2, y - 2, displayVal, SMLSIZE | TEXT_COLOR, 0, NULL, "%");
  }

  lcd->drawSolidVerticalLine(x + w / 2, y, h, TEXT_COLOR);
}

void drawComboOutputBar(coord_t x, coord_t y, coord_t w, coord_t h, uint8_t channel)
{
  char chanString[] = "Ch32 ";
  int16_t chanVal = calcRESXto100(channelOutputs[channel]);
  LimitData * ld = limitAddress(channel);
  int usValue = PPM_CH_CENTER(channel) + channelOutputs[channel] / 2;
  const uint16_t limPos = ld ? posOnBar(calcRESXto100((ld && ld->revert) ? -ld->offset : ld->offset)) : 0;
  uint16_t valPos;

  strAppendSigned(&chanString[2], channel + 1, 2);
  lcdDrawText(x, y, chanString, SMLSIZE | TEXT_COLOR | LEFT);

  lcdDrawSizedText(x + 45, y, g_model.limitData[channel].name, sizeof(g_model.limitData[channel].name), SMLSIZE | TEXT_COLOR | LEFT | ZCHAR);
  lcdDrawNumber(x + w, y, usValue, SMLSIZE | TEXT_COLOR | RIGHT, 0, NULL, STR_US);

  lcdDrawSolidFilledRect(x, y + Y_OUTBAR, w, h, BARGRAPH_BGCOLOR);
  lcd->drawSolidVerticalLine(x + limPos, y + Y_OUTBAR, h, MAINVIEW_GRAPHICS_COLOR);

  chanVal = limit<int16_t>(-VIEW_CHANNELS_LIMIT_PCT, chanVal, VIEW_CHANNELS_LIMIT_PCT);
  valPos = posOnBar(chanVal);

  if (valPos > limPos) {
    lcdDrawSolidFilledRect(x + limPos, y + Y_OUTBAR, valPos - limPos, h, BARGRAPH1_COLOR);
  }
  else if (valPos < limPos) {
    uint16_t endpoint = x + limPos;
    uint16_t size = limPos - valPos;
    lcdDrawSolidFilledRect(endpoint - size, y + Y_OUTBAR, size, h, BARGRAPH1_COLOR);
  }

  if (ld && ld->revert) {
    drawOutputBarLimits(x + posOnBar(-100 - ld->max / 10), x + posOnBar(100 - ld->min / 10), y + Y_OUTBAR);
    lcd->drawBitmap(x - X_OFFSET + 7, y + 25, chanMonInvertedBitmap);
  }
  else if (ld) {
    drawOutputBarLimits(x + posOnBar(-100 + ld->min / 10), x + posOnBar(100 + ld->max / 10), y + Y_OUTBAR);
  }
#if defined(OVERRIDE_CHANNEL_FUNCTION)
  if (safetyCh[channel] != OVERRIDE_CHANNEL_UNDEFINED)
    lcd->drawBitmap(x - X_OFFSET + 7, y + 7, chanMonLockedBitmap);
#endif
  lcd->drawSolidVerticalLine(x + w / 2, y + Y_OUTBAR, h, TEXT_COLOR);
  if (chanVal > calcRESXto100((ld && ld->revert) ? -ld->offset : ld->offset))
    lcdDrawNumber(x + limPos, y + h, chanVal, SMLSIZE | TEXT_COLOR | RIGHT, 0, NULL, "%");
  else
    lcdDrawNumber(x + limPos, y + h, chanVal, SMLSIZE | TEXT_COLOR, 0, NULL, "%");
}

coord_t drawChannelsMonitorLegend(coord_t x, const pm_char * s, int color)
{
  lcdDrawSolidFilledRect(x, MENU_FOOTER_TOP + 2, LEG_COLORBOX + 2, LEG_COLORBOX + 2, BARGRAPH_BGCOLOR);
  lcdDrawSolidFilledRect(x + 1, MENU_FOOTER_TOP + 3, LEG_COLORBOX, LEG_COLORBOX, color);
  lcdDrawText(x + 20, MENU_FOOTER_TOP, s, TEXT_STATUSBAR_COLOR);
  return x + 25 + getTextWidth(s);
}

bool menuChannelsMonitor(event_t event, uint8_t page)
{
  uint8_t channel = 8 * page;
  coord_t x, y = Y_OFFSET;

  x = drawChannelsMonitorLegend(MENUS_MARGIN_LEFT, STR_MONITOR_OUTPUT_DESC, BARGRAPH1_COLOR);
  drawChannelsMonitorLegend(x, STR_MONITOR_MIXER_DESC, BARGRAPH2_COLOR);

  x = X_OFFSET;
  for (uint8_t i = 0; i < 8; i++, channel++, y += ROW_HEIGHT) {
    if (i == 4) {
      x = 1 + LCD_W / 2 + X_OFFSET;
      y = Y_OFFSET;
    }
    drawComboOutputBar(x, y, COLUMN_SIZE, BAR_HEIGHT, channel);
    drawSingleMixerBar(x, y + Y_MIXBAR + 1, COLUMN_SIZE, BAR_HEIGHT, channel);
  }
  return true;
}
