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

#ifndef _AUTOCOMBOBOX_H_
#define _AUTOCOMBOBOX_H_

#include <QComboBox>
#include "genericpanel.h"

class AutoComboBox: public QComboBox
{
  Q_OBJECT

  public:
    explicit AutoComboBox(QWidget *parent = 0):
      QComboBox(parent),
      field(NULL),
      panel(NULL),
      next(0),
      lock(false)
    {
      connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
    }

    void clear()
    {
      lock = true;
      QComboBox::clear();
      next = 0;
      lock = false;
    }

    virtual void insertItems(int index, const QStringList & items)
    {
      foreach(QString item, items) {
        addItem(item);
      }
    }

    virtual void addItem(const QString & item)
    {
      addItem(item, next++);
    }

    virtual void addItem(const QString & item, int value)
    {
      lock = true;
      QComboBox::addItem(item, value);
      if (field && *field==value) {
        setCurrentIndex(count()-1);
      }
      lock = false;
    }

    void setField(unsigned int & field, GenericPanel * panel=NULL)
    {
      this->field = (int *)&field;
      this->panel = panel;
      for (int i=0; i<count(); ++i) {
        if ((int)field == itemData(i))
          setCurrentIndex(i);
      }
    }

    void setField(int & field, GenericPanel * panel=NULL)
    {
      this->field = &field;
      this->panel = panel;
      for (int i=0; i<count(); ++i) {
        if ((int)field == itemData(i))
          setCurrentIndex(i);
      }
    }

    void setAutoIndexes()
    {
      for (int i=0; i<count(); ++i) {
        setItemData(i, i);
        if (this->field && *this->field == i)
          setCurrentIndex(i);
      }
    }

  protected slots:
    void onCurrentIndexChanged(int index)
    {
      if (field && !lock) {
        *field = itemData(index).toInt();
        if (panel) {
          emit panel->modified();
        }
      }
    }

  protected:
    int * field;
    GenericPanel * panel;
    int next;
    bool lock;
};

#endif // _AUTOCOMBOBOX_H_
