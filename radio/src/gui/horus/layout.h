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

#ifndef _LAYOUT_H_
#define _LAYOUT_H_

#include "widget.h"

#define MAX_LAYOUT_ZONES               10
#define MAX_LAYOUT_OPTIONS             10

class LayoutFactory;
class Layout
{
  friend class LayoutFactory;

  public:
    struct ZonePersistentData {
      char widgetName[10];
      Widget::PersistentData widgetData;
    };

    struct PersistentData {
      ZonePersistentData zones[MAX_LAYOUT_ZONES];
      ZoneOptionValue options[MAX_LAYOUT_OPTIONS];
    };

  public:
    Layout(const LayoutFactory * factory, PersistentData * persistentData):
      factory(factory),
      persistentData(persistentData)
    {
      memset(widgets, 0, sizeof(widgets));
    }

    virtual ~Layout()
    {
      for (uint8_t i=0; i<MAX_LAYOUT_ZONES; i++) {
        delete widgets[i];
      }
      memset(widgets, 0, sizeof(widgets));
    }

    const LayoutFactory * getFactory() const
    {
      return factory;
    }

    Widget * getWidget(unsigned int index)
    {
      return widgets[index];
    }

    void setWidget(unsigned int index, Widget * widget)
    {
      widgets[index] = widget;
    }

    void createWidget(unsigned int index, const WidgetFactory * factory);

    virtual void create()
    {
      memset(persistentData, 0, sizeof(PersistentData));
    }

    virtual void load();

    ZoneOptionValue getOptionValue(unsigned int index) const
    {
      return persistentData->options[index];
    }

    void setOptionValue(unsigned int index, ZoneOptionValue value) const
    {
      persistentData->options[index] = value;
    }

    virtual unsigned int getZonesCount() const = 0;
    virtual Zone getZone(unsigned int index) const = 0;

    virtual void refresh(bool setup=false);

  protected:
    const LayoutFactory * factory;
    Widget * widgets[MAX_LAYOUT_ZONES];
    PersistentData * persistentData;
};

void registerLayout(const LayoutFactory * factory);

class LayoutFactory
{
  public:
    LayoutFactory(const char * name):
      name(name)
    {
      registerLayout(this);
    }
    const char * getName() const { return name; }
    virtual void drawThumb(uint16_t x, uint16_t y, uint32_t flags) const = 0;
    virtual const ZoneOption * getOptions() const = 0;
    virtual Layout * create(Layout::PersistentData * persistentData) const = 0;
    virtual Layout * load(Layout::PersistentData * persistentData) const = 0;

  protected:
    const char * name;
};

template<class T>
class BaseLayoutFactory: public LayoutFactory
{
  public:
    BaseLayoutFactory(const char * name, const uint8_t * bitmap, const ZoneOption * options):
      LayoutFactory(name),
      bitmap(bitmap),
      options(options)
    {
    }

    virtual void drawThumb(uint16_t x, uint16_t y, uint32_t flags) const
    {
      extern void lcdDrawBitmapPattern(int x, int y, const uint8_t * bitmap, uint32_t flags, int width=0, int offset=0);
      lcdDrawBitmapPattern(x, y, bitmap, flags);
    }

    virtual const ZoneOption * getOptions() const
    {
      return options;
    }

    virtual Layout * create(Layout::PersistentData * persistentData) const
    {
      Layout * layout = new T(this, persistentData);
      layout->create();
      return layout;
    }

    virtual Layout * load(Layout::PersistentData * persistentData) const
    {
      Layout * layout = new T(this, persistentData);
      layout->load();
      return layout;
    }

  protected:
    const uint8_t * bitmap;
    const ZoneOption * options;
};

#define MAX_REGISTERED_LAYOUTS 10

extern unsigned int countRegisteredLayouts;
extern const LayoutFactory * registeredLayouts[MAX_REGISTERED_LAYOUTS];
Layout * loadLayout(const char * name, Layout::PersistentData * persistentData);
void loadCustomScreens();

#endif // _LAYOUT_H_
