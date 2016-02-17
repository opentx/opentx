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

#ifndef _WIDGET_H_
#define _WIDGET_H_

#include <inttypes.h>

#define MAX_WIDGET_OPTIONS             4

struct Zone
{
  uint16_t x, y, w, h;
};

struct ZoneOption
{
  enum Type {
    Bool,
    Integer,
    String,
    TextSize,
    Timer,
    Source,
    Switch,
    Color
  };

  const char * name;
  Type type;
};

union ZoneOptionValue
{
  bool boolValue;
  uint32_t unsignedValue;
  int32_t signedValue;
  char stringValue[8];
};

class WidgetFactory;
class Widget
{
  public:
    struct PersistentData {
      ZoneOptionValue options[MAX_WIDGET_OPTIONS];
    };

    Widget(const WidgetFactory * factory, const Zone & zone, PersistentData * persistentData):
      factory(factory),
      zone(zone),
      persistentData(persistentData)
    {
    }

    virtual ~Widget()
    {
    }

    virtual void init()
    {
      memset(persistentData, 0, sizeof(PersistentData));
    }

    const WidgetFactory * getFactory() const
    {
        return factory;
    }

    virtual ZoneOptionValue getOptionValue(unsigned int index) const
    {
      return persistentData->options[index];
    }

    virtual void setOptionValue(unsigned int index, ZoneOptionValue value) const
    {
      persistentData->options[index] = value;
    }

    virtual void refresh() = 0;

  protected:
    const WidgetFactory * factory;
    Zone zone;
    PersistentData * persistentData;
};

void registerWidget(const WidgetFactory * factory);

class WidgetFactory
{
  public:
    WidgetFactory(const char * name):
      name(name)
    {
      registerWidget(this);
    }

    const char * getName() const
    {
        return name;
    }

    virtual const ZoneOption * getOptions() const = 0;

    virtual Widget * create(const Zone & zone, Widget::PersistentData * persistentData, bool init=true) const = 0;

  protected:
    const char * name;
};

template<class T>
class BaseWidgetFactory: public WidgetFactory
{
  public:
    BaseWidgetFactory(const char * name, const ZoneOption * options):
      WidgetFactory(name),
      options(options)
    {
    }

    virtual const ZoneOption * getOptions() const
    {
      return options;
    }

    virtual Widget * create(const Zone & zone, Widget::PersistentData * persistentData, bool init=true) const
    {
      Widget * widget = new T(this, zone, persistentData);
      if (init) {
        widget->init();
      }
      return widget;
    }

  protected:
    const ZoneOption * options;
};

#define MAX_REGISTERED_WIDGETS 10
extern unsigned int countRegisteredWidgets;
extern const WidgetFactory * registeredWidgets[MAX_REGISTERED_WIDGETS];
Widget * loadWidget(const char * name, const Zone & zone, Widget::PersistentData * persistentData);

#endif // _WIDGET_H_
