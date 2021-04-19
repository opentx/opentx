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

#include <QLineEdit>
#include <QRegExpValidator>
#include "genericpanel.h"

class AutoLineEdit: public QLineEdit
{
  Q_OBJECT

  public:
    explicit AutoLineEdit(QWidget * parent = nullptr, bool updateOnChange = false):
      QLineEdit(parent),
      field(NULL),
      strField(nullptr),
      panel(nullptr),
      lock(false)
    {
      if (updateOnChange)
        connect(this, &AutoLineEdit::textChanged, this, &AutoLineEdit::onEdited);
      else
        connect(this, &AutoLineEdit::editingFinished, this, &AutoLineEdit::onEdited);
    }

    void setField(char * field, int len, GenericPanel * panel = nullptr)
    {
      this->field = field;
      this->panel = panel;
      setValidator(new QRegExpValidator(QRegExp("[ A-Za-z0-9_.-,]*"), this));
      setMaxLength(len);
      updateValue();
    }

    void setField(QString & field, int len = 0, GenericPanel * panel = nullptr)
    {
      strField = &field;
      this->panel = panel;
      if (len)
        setMaxLength(len);
      updateValue();
    }

    void updateValue()
    {
      lock = true;
      if (field)
        setText(field);
      else if (strField)
        setText(*strField);
      lock = false;
    }

  signals:
    void currentDataChanged();

  protected slots:
    void onEdited()
    {
      if (panel && panel->lock)
        return;
      if (lock)
        return;

      if (field)
        strcpy(field, text().toLatin1());
      else if (strField)
        *strField = text();
      else
        return;

      emit currentDataChanged();

      if (panel)
        emit panel->modified();
    }

  protected:
    char * field;
    QString * strField = nullptr;
    GenericPanel * panel = nullptr;
    bool lock = false;
};
