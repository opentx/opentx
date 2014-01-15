/*
 * Author - Bertrand Songis <bsongis@gmail.com>
 * 
 * Based on th9x -> http://code.google.com/p/th9x/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#ifndef MENUWIDGET_H
#define	MENUWIDGET_H

#include <QWidget>
#include <QtGui>

class menuWidget : public QWidget {

    Q_OBJECT

  public:

    explicit menuWidget(QWidget * parent = 0):
      QWidget(parent)
    {
    }

  protected:

    virtual void mousePressEvent(QMouseEvent * event)
    {
      int x=event->x();
      int y=event->y();
      setFocus();
      if (event->button()==Qt::LeftButton) {
         if (x>25 && x<71 && y>60 && y<81) {
            setStyleSheet("background:url(:/images/9xmenumenu.png);");
            emit buttonPressed(Qt::Key_Enter);
         } else if (x>25 && x<71 && y>117 && y<139) {
            setStyleSheet("background:url(:/images/9xmenuexit.png);");
            emit buttonPressed(Qt::Key_Escape);
         }
      }
      // QWidget::mousePressEvent(event);
    }
    
    virtual void mouseReleaseEvent(QMouseEvent * event)
    {
      setStyleSheet("background:url(:/images/9xmenu.png);");
      setFocus();
      emit buttonPressed(0);
      // QWidget::mouseReleaseEvent(event);
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

};
#endif	/* MENUWIDGET_H */

