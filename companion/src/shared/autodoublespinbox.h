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

#ifndef _AUTODOUBLESPINBOX_H_
#define _AUTODOUBLESPINBOX_H_

#include <QDoubleSpinBox>
#include "modeledit/modeledit.h"
#if __GNUC__
  #include <math.h>
#endif

class AutoDoubleSpinBox: public QDoubleSpinBox
{
  Q_OBJECT

  public:
    explicit AutoDoubleSpinBox(QWidget *parent = 0):
      QDoubleSpinBox(parent),
      field(NULL),
      panel(NULL),
      lock(false)
    {
      connect(this, SIGNAL(valueChanged(double)), this, SLOT(onValueChanged(double)));
    }

    void setField(int & field, ModelPanel * panel=NULL)
    {
      this->field = &field;
      this->panel = panel;
      updateValue();
    }

    void setField(unsigned int & field, ModelPanel * panel=NULL)
    {
      this->field = (int *)&field;
      this->panel = panel;
      updateValue();
    }

    void updateValue()
    {
      if (field) {
        setValue(float(*field)/multiplier());
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

  protected slots:
    void onValueChanged(double value)
    {
      if (field && !lock) {
        *field = round(value * multiplier());
        if (panel) {
          emit panel->modified();
        }
      }
    }

  protected:
    int * field;
    ModelPanel * panel;
    bool lock;
};

#endif // _AUTODOUBLESPINBOX_H_
