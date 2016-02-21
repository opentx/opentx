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

class ModelBitmapWidget: public Widget
{
  public:
    ModelBitmapWidget(const WidgetFactory * factory, const Zone & zone, Widget::PersistentData * persistentData):
      Widget(factory, zone, persistentData),
      bitmap(NULL)
    {
      memset(bitmapFilename, 0, sizeof(bitmapFilename));
    }

    void loadBitmap()
    {
      char filename[] = BITMAPS_PATH "/xxxxxxxxxx.bmp";
      strncpy(filename+sizeof(BITMAPS_PATH), g_model.header.bitmap, sizeof(g_model.header.bitmap));
      strcat(filename+sizeof(BITMAPS_PATH), BITMAPS_EXT);
      bitmap = bmpLoad(filename);
      memcpy(bitmapFilename, g_model.header.bitmap, sizeof(g_model.header.bitmap));
      // TODO rescale the bitmap here instead of every refresh!
    }

    virtual ~ModelBitmapWidget()
    {
      free(bitmap);
    }

    virtual void refresh();

  protected:
    char bitmapFilename[sizeof(g_model.header.bitmap)];
    uint8_t * bitmap;
};

void ModelBitmapWidget::refresh()
{
  if (memcmp(bitmapFilename, g_model.header.bitmap, sizeof(g_model.header.bitmap)) != 0) {
    loadBitmap();
  }

  if (zone.h >= 96) {
    lcdDrawFilledRect(zone.x, zone.y, zone.w, zone.h, SOLID, MAINVIEW_PANES_COLOR | OPACITY(5));
    lcdDrawBitmapPattern(zone.x + 6, zone.y + 4, LBM_MODEL_ICON, MAINVIEW_GRAPHICS_COLOR);
    lcdDrawSizedText(zone.x + 45, zone.y + 10, g_model.header.name, LEN_MODEL_NAME, ZCHAR | SMLSIZE);
    lcdDrawSolidFilledRect(zone.x + 39, zone.y + 27, zone.w - 48, 2, MAINVIEW_GRAPHICS_COLOR);
    if (bitmap) {
      float scale = getBitmapScale(bitmap, zone.w, zone.h - 25);
      int width = getBitmapScaledSize(getBitmapWidth(bitmap), scale);
      int height = getBitmapScaledSize(getBitmapHeight(bitmap), scale);
      lcdDrawBitmap(zone.x + (zone.w - width) / 2, zone.y + zone.h - height / 2 - height / 2, bitmap, 0, 0, scale);
    }
  }
  else if (bitmap) {
    float scale = getBitmapScale(bitmap, 1000, zone.h);
    int width = getBitmapScaledSize(getBitmapWidth(bitmap), scale);
    int height = getBitmapScaledSize(getBitmapHeight(bitmap), scale);
    lcdDrawBitmap(zone.x + (zone.w - width) / 2, zone.y + (zone.h - height) / 2, bitmap, 0, 0, scale);
  }
}

BaseWidgetFactory<ModelBitmapWidget> modelBitmapWidget("ModelBmp", NULL);
