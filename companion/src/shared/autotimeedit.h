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

#include <QTimeEdit>
#include "genericpanel.h"

class AutoTimeEdit: public QTimeEdit
{
  Q_OBJECT

  public:
    explicit AutoTimeEdit(QWidget * parent = nullptr):
      QTimeEdit(parent),
      field(nullptr),
      panel(nullptr),
      lock(false)
    {
      connect(this, SIGNAL(timeChanged(QTime)), this, SLOT(onTimeChanged(QTime)));
    }

    void setField(unsigned int & field, GenericPanel * panel = nullptr)
    {
      this->field = &field;
      this->panel = panel;
      updateValue();
    }

    void setMinimumTime(const QTime time)
    {
      QTimeEdit::setMinimumTime(time);
    }

    void setMaximumTime(const QTime time)
    {
      QTimeEdit::setMaximumTime(time);
    }

    void setEnabled(bool enabled)
    {
      QTimeEdit::setEnabled(enabled);
    }

    void updateValue()
    {
      if (field) {
        lock = true;
        int hour = *field / 3600;
        int min = (*field - (hour * 3600)) / 60;
        int sec = (*field - (hour * 3600)) % 60;
        setTime(QTime(hour, min, sec));
        lock = false;
      }
    }

  signals:
    void currentDataChanged(unsigned int value);

  protected slots:
    void onTimeChanged(QTime time)
    {
      if (panel && panel->lock)
        return;
      if (!field || lock)
        return;

      unsigned int val = time.hour() * 3600 + time.minute() * 60 + time.second();
      if (*field != val) {
        *field = val;
        emit currentDataChanged(val);
        if (panel)
          emit panel->modified();
      }
    }

  protected:
    unsigned int *field = nullptr;
    GenericPanel *panel = nullptr;
    bool lock = false;
};

