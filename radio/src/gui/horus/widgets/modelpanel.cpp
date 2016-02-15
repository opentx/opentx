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

class ModelPanelWidget: public Widget
{
  public:
    ModelPanelWidget(const WidgetFactory * factory, const Zone & zone, Widget::PersistentData * persistentData):
      Widget(factory, zone, persistentData)
    {
    }

    void refresh();
};

void ModelPanelWidget::refresh()
{
  if (zone.h >= MODEL_BITMAP_HEIGHT) {
    lcdDrawFilledRect(zone.x, zone.y, zone.w, zone.h, SOLID, MAINVIEW_PANES_COLOR | OPACITY(5));
    lcdDrawBitmapPattern(zone.x + 6, zone.y + 4, LBM_MODEL_ICON, MAINVIEW_GRAPHICS_COLOR);
    lcdDrawSizedText(zone.x + 45, zone.y + 10, g_model.header.name, LEN_MODEL_NAME, ZCHAR | SMLSIZE);
    lcdDrawSolidFilledRect(zone.x + 39, zone.y + 27, zone.w - 48, 2, MAINVIEW_GRAPHICS_COLOR);
    int scale = getBitmapScale(modelBitmap, zone.w, zone.h - 25);
    int width = getBitmapScaledSize(getBitmapWidth(modelBitmap), scale);
    int height = getBitmapScaledSize(getBitmapHeight(modelBitmap), scale);
    lcdDrawBitmap(zone.x + (zone.w - width) / 2, zone.y + zone.h - height / 2 - height / 2, modelBitmap, 0, 0, scale);
  }
  else {
    int scale = getBitmapScale(modelBitmap, zone.w, zone.h);
    int width = getBitmapScaledSize(getBitmapWidth(modelBitmap), scale);
    int height = getBitmapScaledSize(getBitmapHeight(modelBitmap), scale);
    lcdDrawFilledRect(zone.x, zone.y, zone.w, height, SOLID, MAINVIEW_PANES_COLOR | OPACITY(5));
    lcdDrawSizedText(zone.x + 5, zone.y + 10, g_model.header.name, LEN_MODEL_NAME, ZCHAR | SMLSIZE);
    lcdDrawBitmap(zone.x + zone.w - width, zone.y, modelBitmap, 0, 0, scale);
  }
}

BaseWidgetFactory<ModelPanelWidget> modelPanelWidget("ModelPanel", "", NULL);