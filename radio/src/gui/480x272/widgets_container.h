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
#include "widget.h"

class WidgetsContainerInterface
{
  public:
    virtual unsigned int getZonesCount() const = 0;

    virtual Zone getZone(unsigned int index) const = 0;

    inline Widget * getWidget(unsigned int index)
    {
      return widgets[index];
    }

    inline void setWidget(unsigned int index, Widget * widget)
    {
      widgets[index] = widget;
    }

    virtual void createWidget(unsigned int index, const WidgetFactory * factory) = 0;

  protected:
    Widget ** widgets;
};

template<int N, int O>
class WidgetsContainer: public WidgetsContainerInterface
{
  public:
    struct ZonePersistentData {
      char widgetName[10];
      Widget::PersistentData widgetData;
    };

    struct PersistentData {
      ZonePersistentData zones[N];
      ZoneOptionValue options[O];
    };

  public:
    WidgetsContainer(PersistentData * persistentData):
      persistentData(persistentData)
    {
      widgets = (Widget **)calloc(N, sizeof(Widget *));
    }

    virtual ~WidgetsContainer()
    {
      if (widgets) {
        for (uint8_t i=0; i<N; i++) {
          delete widgets[i];
        }
        free(widgets);
      }
    }

    virtual void createWidget(unsigned int index, const WidgetFactory * factory)
    {
      if (widgets) {
        memset(persistentData->zones[index].widgetName, 0, sizeof(persistentData->zones[index].widgetName));
        if (factory) {
          strncpy(persistentData->zones[index].widgetName, factory->getName(), sizeof(persistentData->zones[index].widgetName));
          widgets[index] = factory->create(getZone(index), &persistentData->zones[index].widgetData);
        }
        else {
          widgets[index] = NULL;
        }
      }
    }

    virtual void create()
    {
      memset(persistentData, 0, sizeof(PersistentData));
    }

    virtual void load()
    {
      if (widgets) {
        unsigned int count = getZonesCount();
        for (unsigned int i=0; i<count; i++) {
          delete widgets[i];
          if (persistentData->zones[i].widgetName[0]) {
            char name[sizeof(persistentData->zones[i].widgetName)+1];
            memset(name, 0, sizeof(name));
            strncpy(name, persistentData->zones[i].widgetName, sizeof(persistentData->zones[i].widgetName));
            widgets[i] = loadWidget(name, getZone(i), &persistentData->zones[i].widgetData);
          }
          else {
            widgets[i] = NULL;
          }
        }
      }
    }

    inline ZoneOptionValue * getOptionValue(unsigned int index) const
    {
      return &persistentData->options[index];
    }

    virtual unsigned int getZonesCount() const = 0;

    virtual Zone getZone(unsigned int index) const = 0;

    virtual void refresh()
    {
      if (widgets) {
        for (int i=0; i<N; i++) {
          if (widgets[i]) {
            widgets[i]->refresh();
          }
        }
      }
    }

    virtual void background()
    {
      if (widgets) {
        for (int i=0; i<N; i++) {
          if (widgets[i]) {
            widgets[i]->background();
          }
        }
      }
    }

  protected:
    PersistentData * persistentData;
};

#endif // _WIDGETS_CONTAINER_H_
