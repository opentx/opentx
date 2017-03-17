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
      buffer(NULL),
      deps_hash(0)
    {
    }

    virtual ~ModelBitmapWidget()
    {
      delete buffer;
    }

    void refreshBuffer()
    {
      delete buffer;
      buffer = new BitmapBuffer(BMP_RGB565, zone.w, zone.h);

      if (buffer) {
        buffer->drawBitmap(0, 0, lcd, zone.x, zone.y, zone.w, zone.h);
        GET_FILENAME(filename, BITMAPS_PATH, g_model.header.bitmap, "");
        BitmapBuffer * bitmap = BitmapBuffer::load(filename);
        if (zone.h >= 96 && zone.w >= 120) {
          buffer->drawFilledRect(0, 0, zone.w, zone.h, SOLID, MAINVIEW_PANES_COLOR | OPACITY(5));
          static BitmapBuffer * icon = BitmapBuffer::loadMask(getThemePath("mask_menu_model.png"));
          buffer->drawMask(6, 4, icon, MAINVIEW_GRAPHICS_COLOR);
          buffer->drawSizedText(45, 10, g_model.header.name, LEN_MODEL_NAME, ZCHAR | SMLSIZE);
          buffer->drawSolidFilledRect(39, 27, zone.w - 48, 2, MAINVIEW_GRAPHICS_COLOR);
          if (bitmap) {
            buffer->drawScaledBitmap(bitmap, 0, 38, zone.w, zone.h - 38);
          }
        }
        else {
          if (bitmap) {
            buffer->drawScaledBitmap(bitmap, 0, 0, zone.w, zone.h);
          }
        }
        delete bitmap;
      }
    }

    virtual void refresh()
    {
      uint32_t new_hash = hash(g_model.header.bitmap, sizeof(g_model.header.bitmap));
      new_hash ^= hash(g_model.header.name, sizeof(g_model.header.name));
      new_hash ^= hash(g_eeGeneral.themeName, sizeof(g_eeGeneral.themeName));
      if (new_hash != deps_hash) {
        deps_hash = new_hash;
        refreshBuffer();
      }

      if (buffer) {
        lcd->drawBitmap(zone.x, zone.y, buffer);
      }
    }

  protected:
    BitmapBuffer * buffer;
    uint32_t deps_hash;
};

BaseWidgetFactory<ModelBitmapWidget> modelBitmapWidget("ModelBmp", NULL);
const WidgetFactory * defaultWidget = &modelBitmapWidget;
