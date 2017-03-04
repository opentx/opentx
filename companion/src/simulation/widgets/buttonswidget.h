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
      areas.push_back(Area(polygon, image, action));
    }

  protected:

    class Area
    {
      public:
        Area(const QPolygon & polygon, const QString &image, RadioUiAction * action = NULL):
          polygon(polygon),
          imgFile(image),
          action(action)
        {
        }

        bool contains(int x, int y)
        {
          return polygon.containsPoint(QPoint(x, y), Qt::OddEvenFill);
        }

        QPolygon polygon;
        QString imgFile;
        RadioUiAction * action;
    };

    void setBitmap(QString bitmap)
    {
      if (!bitmap.isEmpty()) {
        QWidget::setStyleSheet(QString("background:url(:/images/simulator/%1);").arg(bitmap));
      }
      else {
        QWidget::setStyleSheet(defaultStyleSheet);
      }
    }

    void onMouseButtonEvent(bool press, QMouseEvent * event)
    {
      bool trig;
      QString img = "";
      int x = event->x();
      int y = event->y();

      foreach(Area area, areas) {
        trig = false;
        if (press && event->button() == Qt::LeftButton && area.contains(x, y)) {
          img = area.imgFile;
          setFocus();
          trig = true;
        }
        if (area.action)
          area.action->trigger(trig);
      }
      setBitmap(img);
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

  protected:

    QList<Area> areas;
    QString defaultStyleSheet;
};

#endif // _BUTTONSWIDGET_H_
