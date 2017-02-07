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

#ifndef _MIXERSLISTWIDGET_H_
#define _MIXERSLISTWIDGET_H_

#include <QtWidgets>

#define MIX_ROW_HEIGHT_INCREASE     8               //how much space is added above mixer row (for new channel), if 0 space adding is disabled
const int  GroupHeaderRole = (Qt::UserRole+2);      //defines new user role for list items. If value is > 0, then space is added before that item

class MixersListWidget : public QListWidget
{
    Q_OBJECT

  public:
    explicit MixersListWidget(QWidget *parent, bool expo);
    void keyPressEvent(QKeyEvent *event);
    void dropEvent(QDropEvent *event);

  signals:
    void mimeDropped(int index, const QMimeData *data, Qt::DropAction action);
    void keyWasPressed(QKeyEvent *event);

  public slots:
    bool dropMimeData(int index, const QMimeData *data, Qt::DropAction action);

  protected:
    virtual QStringList mimeTypes() const;

  private:
    QPoint dragStartPosition;
    bool expo;

};

/**
  @brief Provides sizing and painting of items for our mixes and inputs lists

  This class draws HTML formated strings into QListWidget class
*/
class MixersDelegate : public QStyledItemDelegate
{
    Q_OBJECT
  public:
    inline MixersDelegate(QObject *parent) : QStyledItemDelegate(parent) {};

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint ( const QStyleOptionViewItem & option, const QModelIndex & index ) const;

  private:
    void SetupDocument(QTextDocument & doc, const QStyleOptionViewItem & options) const;

};

#endif // _MIXERSLISTWIDGET_H_
