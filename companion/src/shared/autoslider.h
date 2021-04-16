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

#include "genericpanel.h"

#include <QSlider>

class AutoSlider : public QSlider
{
  Q_OBJECT

  public:
    explicit AutoSlider(QWidget * parent = nullptr):
      QSlider(parent)
    {
      init();
    }

    explicit AutoSlider(Qt::Orientation orientation, QWidget * parent = nullptr):
      QSlider(orientation, parent)
    {
      init();
    }

    void setField(int & field, int min, int max, GenericPanel * panel = nullptr)
    {
      this->field = &field;
      setFieldInit(min, max, panel);
    }

    void setField(unsigned int & field, int min, int max, GenericPanel * panel = nullptr)
    {
      this->field = reinterpret_cast<int *>(&field);
      setFieldInit(min, max, panel);
    }

    void setTick(int interval, QSlider::TickPosition position)
    {
      setTickInterval(interval);
      setTickPosition(position);
    }

    void updateValue()
    {
      if (field) {
        lock = true;
        setValue(*field);
        lock = false;
      }
    }

  signals:
    void currentDataChanged(int value);

  protected slots:
    void init()
    {
      connect(this, &AutoSlider::valueChanged, this, &AutoSlider::onValueChanged);
    }

    void onValueChanged(int value)
    {
      if (panel && panel->lock)
        return;
      if (field && !lock) {
        *field = value;
        emit currentDataChanged(value);
        if (panel) {
          emit panel->modified();
        }
      }
    }

  protected:
    void setFieldInit(int min, int max, GenericPanel * panel)
    {
      this->panel = panel;
      setRange(min, max);
      updateValue();
    }

    int *field = nullptr;
    GenericPanel *panel = nullptr;
    bool lock = false;
};
