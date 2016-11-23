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

#ifndef _AUTOLINEEDIT_H_
#define _AUTOLINEEDIT_H_

#include <QLineEdit>
#include <QRegExpValidator>
#include "genericpanel.h"

#define CHAR_FOR_NAMES_REGEX "[ A-Za-z0-9_.-,]*"

class AutoLineEdit: public QLineEdit
{
  Q_OBJECT

  public:
    explicit AutoLineEdit(QWidget *parent = 0):
      QLineEdit(parent),
      field(NULL),
      panel(NULL),
      lock(false)
    {
      QRegExp rx(CHAR_FOR_NAMES_REGEX);
      setValidator(new QRegExpValidator(rx, this));
      connect(this, SIGNAL(editingFinished()), this, SLOT(onEdited()));
    }

    void setField(char * field, int len, GenericPanel * panel=NULL)
    {
      this->field = field;
      this->panel = panel;
      setMaxLength(len);
      updateValue();
    }

    void updateValue()
    {
      lock = true;
      if (field) {
        setText(field);
      }
      lock = false;
    }

  protected slots:
    void onEdited()
    {
      if (field && !lock) {
        strcpy(field, text().toLatin1());
        if (panel) {
          emit panel->modified();
        }
      }
    }

  protected:
    char * field;
    GenericPanel * panel;
    bool lock;
};

#endif // _AUTOLINEEDIT_H_
