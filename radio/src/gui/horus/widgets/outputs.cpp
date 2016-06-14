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

class OutputsWidget: public Widget
{
  public:
    OutputsWidget(const WidgetFactory * factory, const Zone & zone, Widget::PersistentData * persistentData):
      Widget(factory, zone, persistentData)
    {
    }

    virtual void refresh();

    void drawSingleOutputBar(uint16_t x, uint16_t y, uint8_t Chan)
    {
      char chanString[] = "CH 32";
      int16_t chanVal = calcRESXto100(channelOutputs[Chan]);
      LimitData *ld = limitAddress(Chan);
      
      strAppend(chanString, "Ch ");
      strAppendSigned(&chanString[3], Chan + 1, 2);
      lcdDrawText(x, y + 1, chanString, TINSIZE | TEXT_COLOR | LEFT);
      
      lcdDrawSizedText(x+50, y + 1, g_model.limitData[Chan].name, sizeof(g_model.limitData[Chan].name), TINSIZE | TEXT_COLOR | LEFT | ZCHAR);
      strAppendSigned(chanString, chanVal);
      lcdDrawText(x + COLLUMN_SIZE - 2, y + 1, chanString, TINSIZE | TEXT_COLOR | RIGHT);
      
      lcdColorTable[CUSTOM_COLOR_INDEX]= RGB(222, 222, 222);
      lcdDrawSolidFilledRect(x, y + 11, COLLUMN_SIZE, BAR_HEIGHT, CUSTOM_COLOR);
      lcd->drawSolidVerticalLine(x + COLLUMN_SIZE / 2 + calcRESXto100(ld->offset), y + 11, BAR_HEIGHT , MAINVIEW_GRAPHICS_COLOR);
      
      lcd->drawSolidVerticalLine(x + divRoundClosest(COLLUMN_SIZE * calcRESXto100(ld->min), 200), y + 11, BAR_HEIGHT , MAINVIEW_GRAPHICS_COLOR);
      if (ld->min > 3) {
        lcd->drawSolidHorizontalLine(x + divRoundClosest(COLLUMN_SIZE * calcRESXto100(ld->min), 200) - 3, y + 11, 3 , MAINVIEW_GRAPHICS_COLOR);
        lcd->drawSolidHorizontalLine(x + divRoundClosest(COLLUMN_SIZE * calcRESXto100(ld->min), 200) - 3, y + 10 + BAR_HEIGHT, 3 , MAINVIEW_GRAPHICS_COLOR);
      }
        
      lcd->drawSolidVerticalLine(x + COLLUMN_SIZE + divRoundClosest(COLLUMN_SIZE * calcRESXto100(ld->max), 200), y + 11, BAR_HEIGHT , MAINVIEW_GRAPHICS_COLOR);
      if (ld->max < -3) {
        lcd->drawSolidHorizontalLine(x + COLLUMN_SIZE + divRoundClosest(COLLUMN_SIZE * calcRESXto100(ld->max), 200), y + 11, 3 , MAINVIEW_GRAPHICS_COLOR);
        lcd->drawSolidHorizontalLine(x + COLLUMN_SIZE + divRoundClosest(COLLUMN_SIZE * calcRESXto100(ld->max), 200), y + 10 + BAR_HEIGHT, 3 , MAINVIEW_GRAPHICS_COLOR);
      }
      
      lcdColorTable[CUSTOM_COLOR_INDEX]= RGB(26, 148, 49);
      
      if (chanVal > 0) {
        lcdDrawSolidFilledRect(x + COLLUMN_SIZE / 2,  y + 11, divRoundClosest(COLLUMN_SIZE * chanVal, 200), BAR_HEIGHT, CUSTOM_COLOR);
      }
      else if (chanVal < 0) {
        uint16_t endpoint = x + COLLUMN_SIZE / 2;
        uint16_t size = divRoundClosest(- COLLUMN_SIZE * chanVal, 200);
        lcdDrawSolidFilledRect(endpoint - size,  y + 11, size, BAR_HEIGHT, CUSTOM_COLOR);
      }
    }


    void twoColumns()
    {
      lcdColorTable[CUSTOM_COLOR_INDEX]= WHITE;
      lcdDrawSolidFilledRect(zone.x, zone.y, zone.w, zone.h, CUSTOM_COLOR);
      
      int8_t Chan;
      uint16_t x,y;
      for(Chan=0, x=zone.x, y=zone.y;Chan < 8;Chan++, y+=ROW_HEIGHT){
        drawSingleOutputBar(x,y, Chan);
      }
      for(Chan=8, x=zone.x + zone.w/2, y=zone.y;Chan < 16;Chan++, y+=ROW_HEIGHT){
        drawSingleOutputBar(x,y, Chan);
      }
    }

    void oneColumn()
    {
    }

    static const ZoneOption options[];
};

const ZoneOption OutputsWidget::options[] = {
  { "First channel", ZoneOption::Integer, OPTION_VALUE_UNSIGNED(1), OPTION_VALUE_UNSIGNED(1), OPTION_VALUE_UNSIGNED(32) },
  { NULL, ZoneOption::Bool }
};


void OutputsWidget::refresh()
{
  if (zone.w > 300 && zone.h > 20)
    twoColumns();
  else if (zone.w > 150 && zone.h > 20)
    oneColumn();
};

BaseWidgetFactory<OutputsWidget> outputsWidget("Outputs", OutputsWidget::options);
