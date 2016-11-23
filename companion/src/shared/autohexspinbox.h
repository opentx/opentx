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

#ifndef _AUTOHEXSPINBOX_H_
#define _AUTOHEXSPINBOX_H_

#include "hexspinbox.h"
#include "modeledit/modeledit.h"

class AutoHexSpinBox: public HexSpinBox
{
  Q_OBJECT

  public:
    explicit AutoHexSpinBox(QWidget *parent = 0):
      HexSpinBox(parent),
      field(NULL),
      panel(NULL),
      lock(false)
    {
      connect(this, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
    }

    void setField(unsigned int & field, ModelPanel * panel=NULL)
    {
      this->field = &field;
      this->panel = panel;
      updateValue();
    }

    void updateValue()
    {
      if (field) {
        setValue(*field);
      }
    }

  protected slots:
    void onValueChanged(int value)
    {
      if (field && !lock) {
        *field = value;
        if (panel) {
          emit panel->modified();
        }
      }
    }

  protected:
    unsigned int * field;
    ModelPanel * panel;
    bool lock;
};

#endif // _AUTOHEXSPINBOX_H_
