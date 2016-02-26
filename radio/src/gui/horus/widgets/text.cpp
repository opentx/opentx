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
  { "Text", ZoneOption::String, OPTION_DEFAULT_VALUE({ .stringValue = { '\015', '\347', '\0', '\14', '\377', '\376', '\373', '\364' } }) },
  { "Color", ZoneOption::Color, OPTION_DEFAULT_VALUE({ .unsignedValue = RED }) },
  { "Size", ZoneOption::TextSize, OPTION_DEFAULT_VALUE({ .unsignedValue = 0 }) },
  { NULL, ZoneOption::Bool }
};

void TextWidget::refresh()
{
  lcdSetColor(persistentData->options[1].unsignedValue);
  LcdFlags fontsize = FONTSIZE(persistentData->options[2].unsignedValue << 8);
  lcdDrawSizedText(zone.x, zone.y, persistentData->options[0].stringValue, sizeof(persistentData->options[0].stringValue), ZCHAR|fontsize|CUSTOM_COLOR);
}

BaseWidgetFactory<TextWidget> textWidget("Text", TextWidget::options);
