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

const MenuHandlerFunc menuTabChannels[] PROGMEM = {
  menuChannels1,
  menuChannels2,
};

uint8_t lastChannelsPage = 0;

void drawSingleOutputBar(uint16_t x, uint16_t y, uint8_t Chan)
{
  char chanString[] = "CH 32";
  uint16_t limits = (g_model.extendedLimits ? 300 : 200);
  int16_t chanVal = calcRESXto100(channelOutputs[Chan]);
  LimitData *ld = limitAddress(Chan);
  
  if (!locked_bmp) locked_bmp =  BitmapBuffer::load(getThemePath("mask_monitor_lockch.png"));

  if (safetyCh[Chan] != OVERRIDE_CHANNEL_UNDEFINED) {
    lcd->drawBitmap(x + 8,y + 5,locked_bmp);
  }
      
  strAppend(chanString, "Ch ");
  strAppendSigned(&chanString[3], Chan + 1, 2);
  lcdDrawText(x + X_OFFSET, y + 1, chanString, TINSIZE | TEXT_COLOR | LEFT);
    
  lcdDrawSizedText(x + X_OFFSET + 50, y + 1, g_model.limitData[Chan].name, sizeof(g_model.limitData[Chan].name), TINSIZE | TEXT_COLOR | LEFT | ZCHAR);
  strAppendSigned(chanString, chanVal);
  lcdDrawText(x + X_OFFSET + COLLUMN_SIZE - 2, y + 1, chanString, TINSIZE | TEXT_COLOR | RIGHT);
    
  lcdColorTable[CUSTOM_COLOR_INDEX]= RGB(222, 222, 222);
  lcdDrawSolidFilledRect(x + X_OFFSET, y + 11, COLLUMN_SIZE, BAR_HEIGHT, CUSTOM_COLOR);
  lcd->drawSolidVerticalLine(x + X_OFFSET + COLLUMN_SIZE / 2 + calcRESXto100(ld->offset), y + 11, BAR_HEIGHT , MAINVIEW_GRAPHICS_COLOR);
      
  lcd->drawSolidVerticalLine(x + X_OFFSET + divRoundClosest(COLLUMN_SIZE * calcRESXto100(ld->min), limits), y + 11, BAR_HEIGHT , MAINVIEW_GRAPHICS_COLOR);
  if (ld->min > 3) {
    lcd->drawSolidHorizontalLine(x + X_OFFSET + divRoundClosest(COLLUMN_SIZE * calcRESXto100(ld->min), limits) - 3, y + 11, 3 , MAINVIEW_GRAPHICS_COLOR);
    lcd->drawSolidHorizontalLine(x + X_OFFSET + divRoundClosest(COLLUMN_SIZE * calcRESXto100(ld->min), limits) - 3, y + 10 + BAR_HEIGHT, 3 , MAINVIEW_GRAPHICS_COLOR);
  }
        
  lcd->drawSolidVerticalLine(x + X_OFFSET + COLLUMN_SIZE + divRoundClosest(COLLUMN_SIZE * calcRESXto100(ld->max), limits), y + 11, BAR_HEIGHT , MAINVIEW_GRAPHICS_COLOR);
  if (ld->max < -3) {
    lcd->drawSolidHorizontalLine(x + X_OFFSET + COLLUMN_SIZE + divRoundClosest(COLLUMN_SIZE * calcRESXto100(ld->max), limits), y + 11, 3 , MAINVIEW_GRAPHICS_COLOR);
    lcd->drawSolidHorizontalLine(x + X_OFFSET + COLLUMN_SIZE + divRoundClosest(COLLUMN_SIZE * calcRESXto100(ld->max), limits), y + 10 + BAR_HEIGHT, 3 , MAINVIEW_GRAPHICS_COLOR);
  }
      
  lcdColorTable[CUSTOM_COLOR_INDEX]= RGB(26, 148, 49);
    
  if (chanVal > 0) {
    lcdDrawSolidFilledRect(x + X_OFFSET + COLLUMN_SIZE / 2,  y + 11, divRoundClosest(COLLUMN_SIZE * ((chanVal > limits/2) ? limits/2 : chanVal), limits), BAR_HEIGHT, CUSTOM_COLOR);
  }
  else if (chanVal < 0) {
    uint16_t endpoint = x + X_OFFSET + COLLUMN_SIZE / 2;
    uint16_t size = divRoundClosest(- COLLUMN_SIZE * ((chanVal < - limits/2) ? -limits/2 : chanVal), limits);
    lcdDrawSolidFilledRect(endpoint - size,  y + 11, size, BAR_HEIGHT, CUSTOM_COLOR);
  }
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
  for(Chan=0, x=1, y=Y_OFFSET;Chan < 8;Chan++, y+=ROW_HEIGHT){
    drawSingleOutputBar(x,y, Chan);
  }
  for(Chan=8, x=1 + LCD_W/2, y=Y_OFFSET;Chan < 16;Chan++, y+=ROW_HEIGHT){
    drawSingleOutputBar(x,y, Chan);
  }
  return menuChannelsMonitor(event, 0);
}

bool menuChannels2(evt_t event)
{
  MENU("Channels monitor 17-32", MONITOR_ICONS, menuTabChannels, e_Channels2, 0, { 0 });
  lastChannelsPage = e_Channels2;

  int8_t Chan;
  uint16_t x,y;
  for(Chan=16, x=1, y=Y_OFFSET;Chan < 24;Chan++, y+=ROW_HEIGHT){
    drawSingleOutputBar(x,y, Chan);
  }
  for(Chan=24, x=1 + LCD_W/2, y=Y_OFFSET;Chan < 32;Chan++, y+=ROW_HEIGHT){
    drawSingleOutputBar(x,y, Chan);
  }  
  
  return menuChannelsMonitor(event, 1);
}
