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
    TextWidget(const WidgetFactory * factory, const Zone & zone, Widget::PersistentData * persistentData):
      Widget(factory, zone, persistentData)
    {
    }

    virtual void refresh();

    static const ZoneOption options[];
};

const ZoneOption TextWidget::options[] = {
  { "Text", ZoneOption::String, OPTION_VALUE_STRING(TEXT_WIDGET_DEFAULT_LABEL) },
  { "Color", ZoneOption::Color, OPTION_VALUE_UNSIGNED(RED) },
  { "Size", ZoneOption::TextSize, OPTION_VALUE_UNSIGNED(0) },
  { "Shadow", ZoneOption::Bool, OPTION_VALUE_BOOL(false)  },
  { NULL, ZoneOption::Bool }
};

void TextWidget::refresh()
{
  lcdSetColor(persistentData->options[1].unsignedValue);
  LcdFlags fontsize = FONTSIZE(persistentData->options[2].unsignedValue << 8);
  if(persistentData->options[3].boolValue) {
    lcdDrawSizedText(zone.x+1, zone.y+1, persistentData->options[0].stringValue, sizeof(persistentData->options[0].stringValue), ZCHAR|fontsize|BLACK);
  }
  lcdDrawSizedText(zone.x, zone.y, persistentData->options[0].stringValue, sizeof(persistentData->options[0].stringValue), ZCHAR|fontsize|CUSTOM_COLOR);
}

BaseWidgetFactory<TextWidget> textWidget("Text", TextWidget::options);
