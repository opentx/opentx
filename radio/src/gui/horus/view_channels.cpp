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
#define ROW_HEIGHT                     21
#define BAR_HEIGHT                     10
#define COLLUMN_SIZE                   180
#define X_OFFSET                       30
#define Y_OFFSET                       75

bool menuChannels1(evt_t event);
bool menuChannels2(evt_t event);
const BitmapBuffer *locked_bmp = NULL;
const BitmapBuffer *inver_bmp = NULL;
const BitmapBuffer *outL_bmp = NULL;
const BitmapBuffer *outR_bmp = NULL;

const MenuHandlerFunc menuTabChannels[] PROGMEM = {
  menuChannels1,
  menuChannels2,
};

uint8_t lastChannelsPage = 0;

uint16_t posOnBar(int16_t value_to100){
  return divRoundClosest((value_to100 + (g_model.extendedLimits ? 150 : 100)) * COLLUMN_SIZE, (g_model.extendedLimits ? 150 : 100)*2);
}

void drawSingleOutputBar(uint16_t x, uint16_t y, uint8_t Chan)
{
  char chanString[] = "CH 32";
  uint16_t limits = (g_model.extendedLimits ? 300 : 200);
  int16_t chanVal = calcRESXto100(channelOutputs[Chan]);
  LimitData *ld = limitAddress(Chan);
  
  if (!locked_bmp) locked_bmp =  BitmapBuffer::load(getThemePath("mask_monitor_lockch.png"));
  if (!inver_bmp) inver_bmp =  BitmapBuffer::load(getThemePath("mask_monitor_inver.png"));
  if (!outL_bmp) outL_bmp =  BitmapBuffer::load(getThemePath("mask_monitor_outL.png"));
  if (!outR_bmp) outR_bmp =  BitmapBuffer::load(getThemePath("mask_monitor_outR.png"));
      
  strAppend(chanString, "Ch ");
  strAppendSigned(&chanString[3], Chan + 1, 2);
  lcdDrawText(x + X_OFFSET, y + 1, chanString, TINSIZE | TEXT_COLOR | LEFT);
    
  lcdDrawSizedText(x + X_OFFSET + 50, y + 1, g_model.limitData[Chan].name, sizeof(g_model.limitData[Chan].name), TINSIZE | TEXT_COLOR | LEFT | ZCHAR);

  lcdColorTable[CUSTOM_COLOR_INDEX]= RGB(222, 222, 222);
  lcdDrawSolidFilledRect(x + X_OFFSET, y + 11, COLLUMN_SIZE, BAR_HEIGHT, CUSTOM_COLOR);
  
  lcdColorTable[CUSTOM_COLOR_INDEX]= RGB(180, 180, 180);
  lcdDrawText(x + X_OFFSET + posOnBar(-100 + ld->min/10)+2, y + 10, "]",TINSIZE| TEXT_COLOR | RIGHT);
  lcdDrawText(x + X_OFFSET + posOnBar(100 + ld->max/10), y + 10, "[",TINSIZE| TEXT_COLOR);

  lcd->drawSolidVerticalLine(x + X_OFFSET + posOnBar(calcRESXto100(ld->offset)), y + 11, BAR_HEIGHT , MAINVIEW_GRAPHICS_COLOR);
  lcd->drawSolidVerticalLine(x + X_OFFSET + posOnBar(chanVal), y + 11, BAR_HEIGHT , HEADER_BGCOLOR);

  strAppendSigned(chanString, chanVal);
  if (chanVal < -limits / 2) {
    lcd->drawBitmap(x + X_OFFSET + COLLUMN_SIZE / 2 - 55, y + 8, outL_bmp);
    chanVal = - limits / 2;
    lcdDrawText(x + X_OFFSET + COLLUMN_SIZE - 2, y + 1, chanString, TINSIZE | TEXT_COLOR | RIGHT | INVERS);
  } else if (chanVal > limits / 2){
    lcd->drawBitmap(x + X_OFFSET + COLLUMN_SIZE / 2 + 55, y + 8, outR_bmp);
    chanVal = limits / 2;
    lcdDrawText(x + X_OFFSET + COLLUMN_SIZE - 2, y + 1, chanString, TINSIZE | TEXT_COLOR | RIGHT | INVERS);
  } else {
    lcdDrawText(x + X_OFFSET + COLLUMN_SIZE - 2, y + 1, chanString, TINSIZE | TEXT_COLOR | RIGHT);
  }
    
  if(posOnBar(chanVal) > posOnBar(calcRESXto100(ld->offset))){
    lcdDrawSolidFilledRect(x + X_OFFSET + posOnBar(calcRESXto100(ld->offset)),  y + 11, posOnBar(chanVal) - posOnBar(calcRESXto100(ld->offset)), BAR_HEIGHT, MAINVIEW_GRAPHICS_COLOR);
  }
  else if (posOnBar(chanVal) < posOnBar(calcRESXto100(ld->offset))) {
    uint16_t endpoint = x + X_OFFSET + posOnBar(calcRESXto100(ld->offset));
    uint16_t size = posOnBar(calcRESXto100(ld->offset)) - posOnBar(chanVal);
    lcdDrawSolidFilledRect(endpoint - size,  y + 11, size, BAR_HEIGHT, MAINVIEW_GRAPHICS_COLOR);
  }
  
  if (safetyCh[Chan] != OVERRIDE_CHANNEL_UNDEFINED) lcd->drawBitmap(x + 3, y + 5, locked_bmp);
  if (ld->revert) lcd->drawBitmap(x + 16, y + 5, inver_bmp);
  lcd->drawSolidVerticalLine(x + X_OFFSET + COLLUMN_SIZE / 2, y + 11, BAR_HEIGHT , TEXT_COLOR);
}

bool menuChannelsMonitor(evt_t event, uint8_t page)
{
  return true;
}

bool menuChannels1(evt_t event)
{
  MENU("Channels monitor 1-16", MONITOR_ICONS, menuTabChannels, e_Channels1, 0, { 0 });
  lastChannelsPage = e_Channels1;
  
  int8_t Chan;
  uint16_t x=0,y=0;
  for(Chan=0, x=1, y=Y_OFFSET;Chan < 8; Chan++, y+=ROW_HEIGHT){
    drawSingleOutputBar(x, y, Chan);
  }
  for(Chan=8, x=1 + LCD_W/2, y=Y_OFFSET; Chan < 16;Chan++, y+=ROW_HEIGHT){
    drawSingleOutputBar(x, y, Chan);
  }
  return menuChannelsMonitor(event, 0);
}

bool menuChannels2(evt_t event)
{
  MENU("Channels monitor 17-32", MONITOR_ICONS, menuTabChannels, e_Channels2, 0, { 0 });
  lastChannelsPage = e_Channels2;

  int8_t Chan;
  uint16_t x,y;
  for(Chan=16, x=1, y=Y_OFFSET; Chan < 24; Chan++, y+=ROW_HEIGHT){
    drawSingleOutputBar(x, y, Chan);
  }
  for(Chan=24, x=1 + LCD_W/2, y=Y_OFFSET; Chan < 32; Chan++, y+=ROW_HEIGHT){
    drawSingleOutputBar(x, y, Chan);
  }  
  
  return menuChannelsMonitor(event, 1);
}
