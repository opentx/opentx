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
#define BAR_HEIGHT                     14
#define COLUMN_SIZE                   200
#define X_OFFSET                       25
#define Y_OFFSET                       75
#define Y_OUTBAR                       15
#define Y_MIXBAR                       28

bool menuChannels1(evt_t);
bool menuChannels2(evt_t);
bool menuChannels3(evt_t);
bool menuChannels4(evt_t);
bool menuLogicalSwitches(evt_t);

const MenuHandlerFunc menuTabMonitors[] PROGMEM = {
  menuChannels1,
  menuChannels2,
  menuChannels3,
  menuChannels4,
  menuLogicalSwitches
};

uint8_t lastMonitorPage = 0;

uint16_t posOnBar(int16_t value_to100)
{
  return divRoundClosest((value_to100 + (g_model.extendedLimits ? 150 : 100)) * COLUMN_SIZE, (g_model.extendedLimits ? 150 : 100) * 2);
}


void drawSingleMixerBar(coord_t x, coord_t y, uint8_t chan)
{
  int16_t chanVal = calcRESXto100(ex_chans[chan]);

  lcdColorTable[CUSTOM_COLOR_INDEX] = RGB(222, 222, 222);
  lcdDrawSolidFilledRect(x, y, COLUMN_SIZE, BAR_HEIGHT, CUSTOM_COLOR);

  lcd->drawSolidVerticalLine(x + posOnBar(chanVal), y, BAR_HEIGHT, HEADER_BGCOLOR);

  lcdColorTable[CUSTOM_COLOR_INDEX] = RGB(25, 150, 50);
  if (chanVal > 0) {
    lcdDrawSolidFilledRect(x + COLUMN_SIZE / 2, y, divRoundClosest(chanVal * COLUMN_SIZE, 200), BAR_HEIGHT, CUSTOM_COLOR);
  }
  else if (chanVal < 0) {
    uint16_t endpoint = x + COLUMN_SIZE / 2;
    uint16_t size = divRoundClosest(-chanVal * COLUMN_SIZE, 200);
    lcdDrawSolidFilledRect(endpoint - size, y, size, BAR_HEIGHT, CUSTOM_COLOR);
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
  static const BitmapBuffer * outL_bmp = BitmapBuffer::load(getThemePath("mask_monitor_outL.png"));
  static const BitmapBuffer * outR_bmp = BitmapBuffer::load(getThemePath("mask_monitor_outR.png"));

  strAppendSigned(&chanString[2], channel + 1, 2);
  lcdDrawText(x, y, chanString, SMLSIZE | TEXT_COLOR | LEFT);

  lcdDrawSizedText(x + 45, y, g_model.limitData[channel].name, sizeof(g_model.limitData[channel].name), SMLSIZE | TEXT_COLOR | LEFT | ZCHAR);
  int usValue = PPM_CH_CENTER(channel) + channelOutputs[channel] / 2;
  strAppendSigned(chanString, usValue);
  if (usValue < 1000) strAppend(&chanString[3], STR_US);
  else strAppend(&chanString[4], STR_US);
  lcdDrawText(x + 155, y, chanString, SMLSIZE | TEXT_COLOR | RIGHT);

  lcdColorTable[CUSTOM_COLOR_INDEX] = RGB(222, 222, 222);
  lcdDrawSolidFilledRect(x, y + Y_OUTBAR, COLUMN_SIZE, BAR_HEIGHT, CUSTOM_COLOR);

  lcdDrawText(x + posOnBar(-100 + ld->min / 10) + 2, y + Y_OUTBAR, "]", TINSIZE | TEXT_COLOR | RIGHT);
  lcdDrawText(x + posOnBar(100 + ld->max / 10), y + Y_OUTBAR, "[", TINSIZE | TEXT_COLOR);

  lcd->drawSolidVerticalLine(x + posOnBar(calcRESXto100(ld->offset)), y + Y_OUTBAR, BAR_HEIGHT, MAINVIEW_GRAPHICS_COLOR);
  lcd->drawSolidVerticalLine(x + posOnBar(chanVal), y + Y_OUTBAR, BAR_HEIGHT, HEADER_BGCOLOR);

  strAppendSigned(chanString, chanVal);
  if (chanVal < -limits / 2) {
    chanVal = -limits / 2;
    lcdDrawText(x + COLUMN_SIZE - 2, y + 1, chanString, SMLSIZE | TEXT_COLOR | RIGHT | INVERS);
    lcd->drawBitmap(x + COLUMN_SIZE / 2 + 5, y + 16, outL_bmp);
  }
  else if (chanVal > limits / 2) {
    chanVal = limits / 2;
    lcdDrawText(x + COLUMN_SIZE - 2, y + 1, chanString, SMLSIZE | TEXT_COLOR | RIGHT | INVERS);
    lcd->drawBitmap(x + COLUMN_SIZE / 2 - 5, y + 16, outR_bmp);
  }
  else {
    lcdDrawText(x + COLUMN_SIZE - 2, y + 1, chanString, SMLSIZE | TEXT_COLOR | RIGHT);
  }

  lcdColorTable[CUSTOM_COLOR_INDEX] = RED;
  if (posOnBar(chanVal) > posOnBar(calcRESXto100(ld->offset))) {
    lcdDrawSolidFilledRect(x + posOnBar(calcRESXto100(ld->offset)), y + Y_OUTBAR, posOnBar(chanVal) - posOnBar(calcRESXto100(ld->offset)), BAR_HEIGHT, CUSTOM_COLOR);
  }
  else if (posOnBar(chanVal) < posOnBar(calcRESXto100(ld->offset))) {
    uint16_t endpoint = x + posOnBar(calcRESXto100(ld->offset));
    uint16_t size = posOnBar(calcRESXto100(ld->offset)) - posOnBar(chanVal);
    lcdDrawSolidFilledRect(endpoint - size, y + Y_OUTBAR, size, BAR_HEIGHT, CUSTOM_COLOR);
  }

  if (safetyCh[channel] != OVERRIDE_CHANNEL_UNDEFINED) lcd->drawBitmap(x -X_OFFSET + 7, y + 7, locked_bmp);
  if (ld->revert) lcd->drawBitmap(x -X_OFFSET + 7, y + 25, inver_bmp);
  lcd->drawSolidVerticalLine(x + COLUMN_SIZE / 2, y + Y_OUTBAR, BAR_HEIGHT, TEXT_COLOR);
}

bool menuChannelsMonitor(evt_t event, uint8_t page)
{
  uint8_t channel = 8 * page;
  uint16_t x = X_OFFSET, y = Y_OFFSET;
  
  drawStatusText(STR_MONITOR_CHANNELS_DESC);
  for (uint8_t i = 0; i < 4; i++, channel++, y += ROW_HEIGHT) {
    drawSingleOutputBar(x, y, channel);
    drawSingleMixerBar(x, y + Y_MIXBAR, channel);
  }
  x = 1 + LCD_W / 2 + X_OFFSET;
  y = Y_OFFSET;
  for (uint8_t i = 0; i < 4; i++, channel++, y += ROW_HEIGHT) {
    drawSingleOutputBar(x, y, channel);
    drawSingleMixerBar(x, y + Y_MIXBAR, channel);
  }
  return true;
}

bool menuChannels1(evt_t event)
{
  MENU(STR_MONITOR_CHANNELS1, MONITOR_ICONS, menuTabMonitors, e_MonChannels1, 0, { 0 });
  lastMonitorPage = e_MonChannels1;
  return menuChannelsMonitor(event, 0);
}

bool menuChannels2(evt_t event)
{
  MENU(STR_MONITOR_CHANNELS2, MONITOR_ICONS, menuTabMonitors, e_MonChannels2, 0, { 0 });
  lastMonitorPage = e_MonChannels2;
  return menuChannelsMonitor(event, 1);
}


bool menuChannels3(evt_t event)
{
  MENU(STR_MONITOR_CHANNELS3, MONITOR_ICONS, menuTabMonitors, e_MonChannels3, 0, { 0 });
  lastMonitorPage = e_MonChannels3;
  return menuChannelsMonitor(event, 2);
}

bool menuChannels4(evt_t event)
{
  MENU(STR_MONITOR_CHANNELS4, MONITOR_ICONS, menuTabMonitors, e_MonChannels4, 0, { 0 });
  lastMonitorPage = e_MonChannels4;
  return menuChannelsMonitor(event, 3);
}
