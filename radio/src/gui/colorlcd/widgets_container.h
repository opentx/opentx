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

#ifndef _WIDGETS_CONTAINER_H_
#define _WIDGETS_CONTAINER_H_

#include <stdlib.h>
#include <window.h>
#include "widget.h"

class WidgetsContainerInterface
{
  public:
    virtual unsigned int getZonesCount() const = 0;
    virtual rect_t getZone(unsigned int index) const = 0;
    virtual Widget * createWidget(unsigned int index, const WidgetFactory * factory) = 0;
    virtual Widget * getWidget(unsigned int index) = 0;
};

#define WIDGET_NAME_LEN   10

template<int N, int O>
class WidgetsContainer: public FormGroup, public WidgetsContainerInterface
{
  public:
    struct ZonePersistentData {
      char widgetName[WIDGET_NAME_LEN];
      Widget::PersistentData widgetData;
    };

    struct PersistentData {
      ZonePersistentData   zones[N];
      ZoneOptionValueTyped options[O];
    };

    WidgetsContainer(const rect_t & rect, PersistentData * persistentData):
      FormGroup(nullptr, rect, FORM_FORWARD_FOCUS),
      persistentData(persistentData)
    {
    }

    Widget * createWidget(unsigned int index, const WidgetFactory * factory) override
    {
      Widget * widget = nullptr;
      memset(persistentData->zones[index].widgetName, 0, sizeof(persistentData->zones[index].widgetName));
      if (factory) {
        strncpy(persistentData->zones[index].widgetName, factory->getName(), sizeof(persistentData->zones[index].widgetName));
        widget = factory->create(this, getZone(index), &persistentData->zones[index].widgetData);
      }
      widgets[index] = widget;

      return widget;
    }

    Widget * getWidget(unsigned int index) override
    {
      if (index < N)
        return widgets[index];

      return nullptr;
    }

    virtual void create()
    {
      memset(persistentData, 0, sizeof(PersistentData));
    }

    virtual void load()
    {
      unsigned int count = getZonesCount();
      for (unsigned int i = 0; i < count; i++) {
        delete widgets[i];
        if (persistentData->zones[i].widgetName[0]) {
          char name[WIDGET_NAME_LEN + 1];
          memset(name, 0, sizeof(name));
          strncpy(name, persistentData->zones[i].widgetName, WIDGET_NAME_LEN);
          widgets[i] = loadWidget(name, this, getZone(i), &persistentData->zones[i].widgetData);
        }
        else {
          widgets[i] = nullptr;
        }
      }
    }

    inline ZoneOptionValue * getOptionValue(unsigned int index) const
    {
      return &persistentData->options[index].value;
    }

    inline void setOptionValue(unsigned int index, const ZoneOptionValue& value)
    {
      persistentData->options[index].value = value;
    }

    unsigned int getZonesCount() const override = 0;

    rect_t getZone(unsigned int index) const override = 0;

    virtual void background()
    {
      for (int i = 0; i < N; i++) {
        if (widgets[i]) {
          widgets[i]->background();
        }
      }
    }

  protected:
    PersistentData * persistentData;
    Widget * widgets[N] = {};
};

#endif // _WIDGETS_CONTAINER_H_
