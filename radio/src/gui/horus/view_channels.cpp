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

bool menuChannelsMonitor(evt_t event, uint8_t page);
bool menuLogicalSwitches(evt_t);

template<int index>
bool menuChannelsMonitor(evt_t event)
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
  return divRoundClosest((value_to100 + (g_model.extendedLimits ? 150 : 100)) * COLUMN_SIZE, (g_model.extendedLimits ? 150 : 100) * 2);
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

void drawSingleMixerBar(coord_t x, coord_t y, uint8_t chan)
{
  char chanString[] = "-500%";
  int16_t chanVal = calcRESXto100(ex_chans[chan]);
  int16_t displayVal = chanVal;

  chanVal = limit<int16_t>(-100, chanVal, 100);
  strAppend(strAppendSigned(chanString, displayVal), "%");

  lcdDrawSolidFilledRect(x, y, COLUMN_SIZE, BAR_HEIGHT, BARGRAPH_BGCOLOR);
  if (chanVal > 0) {
    lcdDrawSolidFilledRect(x + COLUMN_SIZE / 2, y, divRoundClosest(chanVal * COLUMN_SIZE, 200), BAR_HEIGHT, BARGRAPH2_COLOR);
    lcdDrawText(x - 10 + COLUMN_SIZE / 2, y, chanString, SMLSIZE | TEXT_COLOR | RIGHT);
  }
  else if (chanVal < 0) {
    uint16_t endpoint = x + COLUMN_SIZE / 2;
    uint16_t size = divRoundClosest(-chanVal * COLUMN_SIZE, 200);
    lcdDrawSolidFilledRect(endpoint - size, y, size, BAR_HEIGHT, BARGRAPH2_COLOR);
    lcdDrawText(x + 10 + COLUMN_SIZE / 2, y, chanString, SMLSIZE | TEXT_COLOR);
  }

  lcd->drawSolidVerticalLine(x + COLUMN_SIZE / 2, y, BAR_HEIGHT, TEXT_COLOR);
}

void drawSingleOutputBar(coord_t x, coord_t y, uint8_t channel)
{
  char chanString[] = "Ch32  ";
  uint16_t limits = (g_model.extendedLimits ? 300 : 200);
  int16_t chanVal = calcRESXto100(channelOutputs[channel]);
  LimitData * ld = limitAddress(channel);

  static const BitmapBuffer * locked_bmp = BitmapBuffer::load(getThemePath("mask_monitor_lockch.png"));
  static const BitmapBuffer * inver_bmp = BitmapBuffer::load(getThemePath("mask_monitor_inver.png"));

  strAppendSigned(&chanString[2], channel + 1, 2);
  lcdDrawText(x, y, chanString, SMLSIZE | TEXT_COLOR | LEFT);

  lcdDrawSizedText(x + 45, y, g_model.limitData[channel].name, sizeof(g_model.limitData[channel].name), SMLSIZE | TEXT_COLOR | LEFT | ZCHAR);
  int usValue = PPM_CH_CENTER(channel) + channelOutputs[channel] / 2;
  strAppendSigned(chanString, usValue);
  if (usValue < 1000) strAppend(&chanString[3], STR_US);
  else strAppend(&chanString[4], STR_US);
  lcdDrawText(x + COLUMN_SIZE, y, chanString, SMLSIZE | TEXT_COLOR | RIGHT);

  lcdDrawSolidFilledRect(x, y + Y_OUTBAR, COLUMN_SIZE, BAR_HEIGHT, BARGRAPH_BGCOLOR);
  lcd->drawSolidVerticalLine(x + posOnBar(calcRESXto100(ld->offset)), y + Y_OUTBAR, BAR_HEIGHT, MAINVIEW_GRAPHICS_COLOR);

  strAppend(strAppendSigned(chanString, chanVal), "%");
  if (chanVal > 0)
    lcdDrawText(x - 10 + COLUMN_SIZE / 2, y + BAR_HEIGHT, chanString, SMLSIZE | TEXT_COLOR | RIGHT);
  else
    lcdDrawText(x + 10 + COLUMN_SIZE / 2, y + BAR_HEIGHT, chanString, SMLSIZE | TEXT_COLOR);

  chanVal = limit<int16_t>(-limits / 2, chanVal, limits / 2);
  if (posOnBar(chanVal) > posOnBar(calcRESXto100(ld->offset))) {
    lcdDrawSolidFilledRect(x + posOnBar(calcRESXto100(ld->offset)), y + Y_OUTBAR, posOnBar(chanVal) - posOnBar(calcRESXto100(ld->offset)), BAR_HEIGHT, BARGRAPH1_COLOR);
  }
  else if (posOnBar(chanVal) < posOnBar(calcRESXto100(ld->offset))) {
    uint16_t endpoint = x + posOnBar(calcRESXto100(ld->offset));
    uint16_t size = posOnBar(calcRESXto100(ld->offset)) - posOnBar(chanVal);
    lcdDrawSolidFilledRect(endpoint - size, y + Y_OUTBAR, size, BAR_HEIGHT, BARGRAPH1_COLOR);
  }

  drawOutputBarLimits(x + posOnBar(-100 + ld->min / 10), x + posOnBar(100 + ld->max / 10), y + Y_OUTBAR);
  if (safetyCh[channel] != OVERRIDE_CHANNEL_UNDEFINED) lcd->drawBitmap(x - X_OFFSET + 7, y + 7, locked_bmp);
  if (ld->revert) lcd->drawBitmap(x - X_OFFSET + 7, y + 25, inver_bmp);
  lcd->drawSolidVerticalLine(x + COLUMN_SIZE / 2, y + Y_OUTBAR, BAR_HEIGHT, TEXT_COLOR);
}

coord_t drawChannelsMonitorLegend(coord_t x, const pm_char * s, int color)
{
  lcdDrawSolidFilledRect(x, MENU_FOOTER_TOP + 2, LEG_COLORBOX + 2, LEG_COLORBOX + 2, BARGRAPH_BGCOLOR);
  lcdDrawSolidFilledRect(x + 1, MENU_FOOTER_TOP + 3, LEG_COLORBOX, LEG_COLORBOX, color);
  lcdDrawText(x + 20, MENU_FOOTER_TOP, s, TEXT_STATUSBAR_COLOR);
  return x + 25 + getTextWidth(s);
}

bool menuChannelsMonitor(evt_t event, uint8_t page)
{
  uint8_t channel = 8 * page;
  coord_t x, y = Y_OFFSET;

  x = drawChannelsMonitorLegend(MENUS_MARGIN_LEFT, STR_MONITOR_OUTPUT_DESC, BARGRAPH1_COLOR);
  drawChannelsMonitorLegend(x, STR_MONITOR_MIXER_DESC, BARGRAPH2_COLOR);

  x = X_OFFSET;
  for (uint8_t i = 0; i < 4; i++, channel++, y += ROW_HEIGHT) {
    drawSingleOutputBar(x, y, channel);
    drawSingleMixerBar(x, y + Y_MIXBAR + 1, channel);
  }
  x = 1 + LCD_W / 2 + X_OFFSET;
  y = Y_OFFSET;
  for (uint8_t i = 0; i < 4; i++, channel++, y += ROW_HEIGHT) {
    drawSingleOutputBar(x, y, channel);
    drawSingleMixerBar(x, y + Y_MIXBAR + 1, channel);
  }
  return true;
}
