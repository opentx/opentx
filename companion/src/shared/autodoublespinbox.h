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

#include <QDoubleSpinBox>
#include "genericpanel.h"
#if __GNUC__
  #include <math.h>
#endif

class AutoDoubleSpinBox: public QDoubleSpinBox
{
  Q_OBJECT

  public:
    explicit AutoDoubleSpinBox(QWidget * parent = nullptr):
      QDoubleSpinBox(parent),
      field(nullptr),
      panel(nullptr),
      lock(false)
    {
      connect(this, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
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
      if (field) {
        lock = true;
        setValue(float(*field) / multiplier());
        lock = false;
      }
    }

    void setDecimals(int prec)
    {
      QDoubleSpinBox::setDecimals(prec);
      updateValue();
    }

  protected:
    int multiplier()
    {
      switch (decimals()) {
        case 1:
          return 10;
        case 2:
          return 100;
        default:
          return 1;
       }
     }

  signals:
    void currentDataChanged(double value);

  protected slots:
    void onValueChanged(double value)
    {
      if (panel && panel->lock)
        return;
      if (field && !lock) {
        *field = round(value * multiplier());
        emit currentDataChanged(value);
        if (panel) {
          emit panel->modified();
        }
      }
    }

  protected:
    int * field = nullptr;
    GenericPanel * panel = nullptr;
    bool lock = false;
};
