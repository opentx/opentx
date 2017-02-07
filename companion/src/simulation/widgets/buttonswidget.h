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

#ifndef _BUTTONSWIDGET_H_
#define _BUTTONSWIDGET_H_

#include "radiouiaction.h"

#include <QWidget>
#include <QtGui>
#include <QStyleOption>

class Area : public QObject
{
  Q_OBJECT

  public:
    explicit Area(const QPolygon & polygon, const QString &image, RadioUiAction * action = NULL, QObject * parent = NULL):
      QObject(parent),
      polygon(polygon),
      imgFile(image),
      action(action)
    {
      if (action)
        connect(action, &RadioUiAction::triggered, this, &Area::onActionTriggered);
    }

    bool contains(int x, int y)
    {
      return polygon.containsPoint(QPoint(x, y), Qt::OddEvenFill);
    }

    void onActionTriggered(int, bool active)
    {
      if (active)
        emit imageChanged(imgFile);
      else
        emit imageChanged("");
    }

    QPolygon polygon;
    QString imgFile;
    RadioUiAction * action;

  signals:
    void imageChanged(const QString image);
};

class ButtonsWidget : public QWidget
{
  Q_OBJECT

  public:

    explicit ButtonsWidget(QWidget * parent):
      QWidget(parent)
    {
    }

    virtual void setStyleSheet(const QString & sheet)
    {
      defaultStyleSheet = sheet;
      QWidget::setStyleSheet(sheet);
    }

    void addArea(int x1, int y1, int x2, int y2, const char * image, RadioUiAction * action = NULL)
    {
      QPolygon polygon;
      polygon.setPoints(4, x1, y1, x1, y2, x2, y2, x2, y1);
      addArea(polygon, image, action);
    }

    void addArea(const QPolygon & polygon, const char * image, RadioUiAction * action = NULL)
    {
      Area * area = new Area(polygon, image, action, this);
      areas.push_back(area);
      connect(area, &Area::imageChanged, this, &ButtonsWidget::setBitmap);
    }

  protected:

    void setBitmap(QString bitmap)
    {
      QString css = defaultStyleSheet;
      if (!bitmap.isEmpty())
        css = QString("background:url(:/images/simulator/%1);").arg(bitmap);

      QWidget::setStyleSheet(css);
      setFocus();
    }

    void onMouseButtonEvent(bool press, QMouseEvent * event)
    {
      bool anyTriggered = false;
      int x = event->x();
      int y = event->y();

      foreach(Area * area, areas) {
        if (event->button() == Qt::LeftButton && area->contains(x, y)) {
          if (area->action)
            area->action->trigger(press);
          anyTriggered = true;
          break;
        }
      }
      if (!anyTriggered)
        setBitmap("");
    }

    virtual void mousePressEvent(QMouseEvent * event)
    {
      onMouseButtonEvent(true, event);
    }
    virtual void mouseReleaseEvent(QMouseEvent * event)
    {
      onMouseButtonEvent(false, event);
    }
    virtual void mouseLeaveEvent(QMouseEvent * event)
    {
      onMouseButtonEvent(false, event);
    }

    void paintEvent(QPaintEvent *)
    {
      QStyleOption opt;
      opt.init(this);
      QPainter p(this);
      style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }

    QList<Area *> areas;
    QString defaultStyleSheet;
};

#endif // _BUTTONSWIDGET_H_
