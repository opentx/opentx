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

#ifndef _AUTOCHECKBOX_H_
#define _AUTOCHECKBOX_H_

#include <QCheckBox>
#include "modeledit/modeledit.h"

class AutoCheckBox: public QCheckBox
{
  Q_OBJECT

  public:
    explicit AutoCheckBox(QWidget *parent = 0):
      QCheckBox(parent),
      field(NULL),
      panel(NULL),
      lock(false)
    {
      connect(this, SIGNAL(toggled(bool)), this, SLOT(onToggled(bool)));
    }

    void setField(bool & field, ModelPanel * panel=NULL)
    {
      this->field = &field;
      this->panel = panel;
      updateValue();
    }

    void updateValue()
    {
      lock = true;
      if (field) {
        setChecked(*field);
      }
      lock = false;
    }

  protected slots:
    void onToggled(bool checked)
    {
      if (field && !lock) {
        *field = checked;
        if (panel) {
          emit panel->modified();
        }
      }
    }

  protected:
    bool * field;
    ModelPanel * panel;
    bool lock;
};

#endif // _AUTOCHECKBOX_H_
