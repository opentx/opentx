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

#pragma once

#include <list>
#include "widgets_container.h"
#include "lcd.h"
#include "topbar.h"

#define MAX_LAYOUT_ZONES               10
#define MAX_LAYOUT_OPTIONS             10
constexpr coord_t TRIM_LINE_WIDTH = 8;
constexpr coord_t TRIM_SQUARE_SIZE = 17;

class LayoutFactory;

class Layout: public WidgetsContainer<MAX_LAYOUT_ZONES, MAX_LAYOUT_OPTIONS>
{
  friend class LayoutFactory;

  public:
    Layout(const LayoutFactory * factory, PersistentData * persistentData):
      WidgetsContainer<MAX_LAYOUT_ZONES, MAX_LAYOUT_OPTIONS>({0, 0, LCD_W, LCD_H}, persistentData),
      factory(factory)
    {
    }

    inline const LayoutFactory * getFactory() const
    {
      return factory;
    }

    void decorate(bool topbar, bool sliders, bool trims, bool flightMode);

  protected:
    const LayoutFactory * factory;
    TopBar * topBar = nullptr;
};

void registerLayout(const LayoutFactory * factory);

class LayoutFactory
{
  public:
    LayoutFactory(const char * id, const char * name):
      id(id),
      name(name)
    {
      registerLayout(this);
    }
    const char * getId() const { return id; }
    const char * getName() const { return name; }
    virtual void drawThumb(BitmapBuffer * dc, uint16_t x, uint16_t y, LcdFlags flags) const = 0;
    virtual const ZoneOption * getOptions() const = 0;
    virtual Layout * create(Layout::PersistentData * persistentData) const = 0;
    virtual Layout * load(Layout::PersistentData * persistentData) const = 0;

  protected:
    const char * id;
    const char * name;
};

template<class T>
class BaseLayoutFactory: public LayoutFactory
{
  public:
    BaseLayoutFactory(const char * id, const char * name, const uint8_t * bitmap, const ZoneOption * options):
      LayoutFactory(id, name),
      bitmap(bitmap),
      options(options)
    {
    }

    void drawThumb(BitmapBuffer * dc, uint16_t x, uint16_t y, uint32_t flags) const override
    {
      dc->drawBitmapPattern(x, y, bitmap, flags);
    }

    const ZoneOption * getOptions() const override
    {
      return options;
    }

    Layout * create(Layout::PersistentData * persistentData) const override
    {
      Layout * layout = new T(this, persistentData);
      if (layout) {
        layout->create();
      }
      return layout;
    }

    Layout * load(Layout::PersistentData * persistentData) const override
    {
      Layout * layout = new T(this, persistentData);
      if (layout) {
        layout->load();
      }
      return layout;
    }

  protected:
    const uint8_t * bitmap;
    const ZoneOption * options;
};

Layout * loadLayout(const char * name, Layout::PersistentData * persistentData);
void loadCustomScreens();

void drawTrimsAndSliders(Layout::PersistentData * persistentData);

std::list<const LayoutFactory *> & getRegisteredLayouts();
