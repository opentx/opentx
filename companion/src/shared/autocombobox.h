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

#include <QComboBox>
#include "genericpanel.h"

class AutoComboBox: public QComboBox
{
  Q_OBJECT

  public:
    explicit AutoComboBox(QWidget *parent = nullptr):
      QComboBox(parent)
    {
      connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
    }

    void clear()
    {
      if (!hasModel) {
        lock = true;
        QComboBox::clear();
        next = 0;
        lock = false;
      }
    }

    virtual void insertItems(int index, const QStringList & items)
    {
      if (!hasModel) {
        foreach(QString item, items) {
          addItem(item);
        }
      }
    }

    virtual void addItem(const QString & item)
    {
      if (!hasModel)
        addItem(item, next++);
    }

    virtual void addItem(const QString & item, int value)
    {
      if (!hasModel) {
        lock = true;
        QComboBox::addItem(item, value);
        lock = false;
        updateValue();
      }
    }

    void setField(unsigned int & field, GenericPanel * panel=nullptr)
    {
      this->field = (int *)&field;
      this->panel = panel;
      updateValue();
    }

    void setField(int & field, GenericPanel * panel=nullptr)
    {
      this->field = &field;
      this->panel = panel;
      updateValue();
    }

    void setModel(QAbstractItemModel * model)
    {
      lock = true;
      QComboBox::setModel(model);
      lock = false;
      hasModel = true;
      updateValue();
    }

    void setAutoIndexes()
    {
      if (!hasModel) {
        for (int i = 0; i < count(); ++i)
          setItemData(i, i);
        updateValue();
      }
    }

    void updateValue()
    {
      if (!field)
        return;
      lock = true;
      setCurrentIndex(findData(*field));
      lock = false;
    }

  signals:
    void currentDataChanged(int value);

  protected slots:
    void onCurrentIndexChanged(int index)
    {
      if (panel && panel->lock)
        return;
      if (index > -1) {
        const int val = itemData(index).toInt();
        if (field && !lock) {
          *field = val;
          if (panel)
            emit panel->modified();
        }
        emit currentDataChanged(val);
      }
    }

  protected:
    int * field = nullptr;
    GenericPanel * panel = nullptr;
    int next = 0;
    bool lock = false;
    bool hasModel = false;
};
