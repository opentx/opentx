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

    void addArea(int x1, int y1, int x2, int y2, int key, const char * bitmap)
    {
      QPolygon polygon;
      polygon.setPoints(4, x1, y1, x1, y2, x2, y2, x2, y1);
      addArea(polygon, key, bitmap);
    }

    void addArea(const QPolygon & polygon, int key, const char * bitmap)
    {
      areas.push_back(Area(polygon, key, bitmap));
    }

  protected:
  
    class Area {
      public:
        Area(const QPolygon & polygon, int key, const char * bitmap):
          polygon(polygon),
          key(key),
          bitmap(bitmap)
        {
        }
        bool contains(int x, int y)
        {
          return polygon.containsPoint(QPoint(x, y), Qt::OddEvenFill);
        }
      public:
        QPolygon polygon;
        int key;
        const char * bitmap;
    };
      
    void setBitmap(const char * bitmap)
    {
      if (bitmap) {
        QWidget::setStyleSheet(QString("background:url(:/images/simulator/%1);").arg(bitmap));
      }
    }
    
    virtual void mousePressEvent(QMouseEvent * event)
    {
      int x = event->x();
      int y = event->y();
      setFocus();
      if (event->button() == Qt::LeftButton) {
        foreach(Area area, areas) {
          if (area.contains(x, y)) {
            setBitmap(area.bitmap);
            emit buttonPressed(area.key);
            break;
          }
        }
      }
    }
    
    virtual void mouseReleaseEvent(QMouseEvent * event)
    {
      QWidget::setStyleSheet(defaultStyleSheet);
      setFocus();
      emit buttonPressed(0);
    }

    void paintEvent(QPaintEvent *)
    {
      QStyleOption opt;
      opt.init(this);
      QPainter p(this);
      style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }

  signals:
  
    void buttonPressed(int button);
    
  protected:

    QList<Area> areas;
    QString defaultStyleSheet;
};

#endif // _BUTTONSWIDGET_H_
