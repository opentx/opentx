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

#include <list>
#include <string.h>
#include "zone.h"
#include "debug.h"

#define MAX_WIDGET_OPTIONS             5

// YAML_GENERATOR defs
#if !defined(USE_IDX)
#define USE_IDX
#endif

class WidgetFactory;
class Widget : public Window
{
  public:
    struct PersistentData {
      ZoneOptionValueTyped options[MAX_WIDGET_OPTIONS] USE_IDX;
    };

    Widget(const WidgetFactory * factory, Window * parent, const rect_t & rect, PersistentData * persistentData):
      Window(parent, rect),
      factory(factory),
      persistentData(persistentData)
    {
    }

    ~Widget() override = default;

    virtual void update()
    {
    }

    inline const WidgetFactory * getFactory() const
    {
        return factory;
    }

    inline const ZoneOption * getOptions() const;

    virtual const char * getErrorMessage() const
    {
      return nullptr;
    }

    inline ZoneOptionValue * getOptionValue(unsigned int index) const
    {
      return &persistentData->options[index].value;
    }

    virtual void background()
    {
    }

  protected:
    const WidgetFactory * factory;
    PersistentData * persistentData;
};

void registerWidget(const WidgetFactory * factory);

class WidgetFactory
{
  public:
    explicit WidgetFactory(const char * name, const ZoneOption * options = nullptr):
      name(name),
      options(options)
    {
      registerWidget(this);
    }

    inline const char * getName() const
    {
        return name;
    }

    inline const ZoneOption * getOptions() const
    {
      return options;
    }

    void initPersistentData(Widget::PersistentData * persistentData) const
    {
      memset(persistentData, 0, sizeof(Widget::PersistentData));
      if (options) {
        int i = 0;
        for (const ZoneOption * option = options; option->name; option++) {
          TRACE("WidgetFactory::initPersistentData() setting option '%s'", option->name);
          // TODO compiler bug? The CPU freezes ... persistentData->options[i++] = option->deflt;
          memcpy(&persistentData->options[i++].value, &option->deflt, sizeof(ZoneOptionValue));
          persistentData->options[i++].type = zoneValueEnumFromType(option->type);
        }
      }
    }

    virtual Widget * create(Window * parent, const rect_t & rect, Widget::PersistentData * persistentData, bool init = true) const = 0;

  protected:
    const char * name;
    const ZoneOption * options;
};

template<class T>
class BaseWidgetFactory: public WidgetFactory
{
  public:
    BaseWidgetFactory(const char * name, const ZoneOption * options):
      WidgetFactory(name, options)
    {
    }

    Widget * create(Window * parent, const rect_t & rect, Widget::PersistentData * persistentData, bool init = true) const override
    {
      if (init) {
        initPersistentData(persistentData);
      }

      return new T(this, parent, rect, persistentData);
    }
};

inline const ZoneOption * Widget::getOptions() const
{
  return getFactory()->getOptions();
}

Widget * loadWidget(const char * name, Window * parent, const rect_t & rect, Widget::PersistentData * persistentData);

std::list<const WidgetFactory *> & getRegisteredWidgets();

#endif // _WIDGET_H_
