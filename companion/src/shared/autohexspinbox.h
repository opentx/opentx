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

#include "hexspinbox.h"
#include "genericpanel.h"

class AutoHexSpinBox: public HexSpinBox
{
  Q_OBJECT

  public:
    explicit AutoHexSpinBox(QWidget * parent = nullptr):
      HexSpinBox(parent),
      field(nullptr),
      panel(nullptr),
      lock(false)
    {
      connect(this, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
    }

    void setField(unsigned int & field, GenericPanel * panel = nullptr)
    {
      this->field = &field;
      this->panel = panel;
      updateValue();
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
    unsigned int * field = nullptr;
    GenericPanel * panel = nullptr;
    bool lock = false;
};
