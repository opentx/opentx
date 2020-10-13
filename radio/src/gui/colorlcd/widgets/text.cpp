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

#define TEXT_WIDGET_DEFAULT_LABEL  '\15', '\347', '\0', '\14', '\377', '\376', '\373', '\364'  // "My Label"

class TextWidget: public Widget
{
  public:
    TextWidget(const WidgetFactory * factory, Window * parent, const rect_t & rect, Widget::PersistentData * persistentData):
      Widget(factory, parent, rect, persistentData)
    {
    }

    void paint(BitmapBuffer * dc) override
    {
      // TODO DELETE THOSE TEST INIT VALUE !!
      strcpy(persistentData->options[0].value.stringValue, "This is a test string");
      persistentData->options[3].value.boolValue = true;
      // TODO END DELETE AREA

      dc->clear(DEFAULT_BGCOLOR);
      lcdSetColor(persistentData->options[1].value.unsignedValue);
      LcdFlags fontsize = FONT_INDEX(persistentData->options[2].value.unsignedValue << 8u);

      if(persistentData->options[3].value.boolValue) {
        dc->drawText(1, 1, persistentData->options[0].value.stringValue, fontsize | BLACK);
      }
      dc->drawText(0, 0, persistentData->options[0].value.stringValue, fontsize | CUSTOM_COLOR);
    }

    static const ZoneOption options[];
};

const ZoneOption TextWidget::options[] = {
  { STR_TEXT, ZoneOption::String, OPTION_VALUE_STRING(TEXT_WIDGET_DEFAULT_LABEL) },
  { STR_COLOR, ZoneOption::Color, OPTION_VALUE_UNSIGNED(RED) },
  { STR_SIZE, ZoneOption::TextSize, OPTION_VALUE_UNSIGNED(0) },
  { STR_SHADOW, ZoneOption::Bool, OPTION_VALUE_BOOL(false)  },
  { nullptr, ZoneOption::Bool }
};

//
//void TextWidget::refresh()
//{
//  lcdSetColor(persistentData->options[1].value.unsignedValue);
//  LcdFlags fontsize = FONTSIZE(persistentData->options[2].value.unsignedValue << 8);
//  if(persistentData->options[3].value.boolValue) {
//    lcdDrawSizedText(zone.x+1, zone.y+1,
//                     persistentData->options[0].value.stringValue,
//                     sizeof(persistentData->options[0].value.stringValue),
//                     ZCHAR|fontsize|BLACK);
//  }
//  lcdDrawSizedText(zone.x, zone.y,
//                   persistentData->options[0].value.stringValue,
//                   sizeof(persistentData->options[0].value.stringValue),
//                   ZCHAR|fontsize|CUSTOM_COLOR);
//}

BaseWidgetFactory<TextWidget> textWidget("Text", TextWidget::options);
