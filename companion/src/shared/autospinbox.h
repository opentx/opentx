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

#include <QSpinBox>

#include "genericpanel.h"

class AutoSpinBox : public QSpinBox
{
  Q_OBJECT

  public:
    explicit AutoSpinBox(QWidget * parent = nullptr):
      QSpinBox(parent),
      field(nullptr),
      panel(nullptr),
      lock(false)
    {
      connect(this, QOverload<int>::of(&AutoSpinBox::valueChanged), this, &AutoSpinBox::onValueChanged);
    }

    void setField(int & field, GenericPanel * panel = nullptr)
    {
      this->field = &field;
      this->panel = panel;
      updateValue();
    }

    void setField(unsigned int & field, GenericPanel * panel = nullptr)
    {
      this->field = (int *)&field;
      this->panel = panel;
      updateValue();
    }

    void updateValue()
    {
      lock = true;
      if (field) {
        setValue(*field);
      }
      lock = false;
    }

  protected slots:
    void onValueChanged(int value)
    {
      if (panel && panel->lock)
        return;
      if (field && !lock) {
        *field = value;
        if (panel) {
          emit panel->modified();
        }
      }
    }

  protected:
    int * field;
    GenericPanel * panel;
    bool lock;
};
