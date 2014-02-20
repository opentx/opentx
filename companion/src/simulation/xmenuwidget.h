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
#ifndef XMENUWIDGET_H
#define	XMENUWIDGET_H

#include <QWidget>
#include <QtGui>

class xmenuWidget : public QWidget {

    Q_OBJECT

  public:

    explicit xmenuWidget(QWidget * parent = 0):
      QWidget(parent)
    {
    }

  protected:

    virtual void mousePressEvent(QMouseEvent * event)
    {
      float p1x[] = {64,71,90,100,90,72};
      float p1y[] = {60,50,50,60,73,73};
      float p2x[] = {63,73,88,98,88,72};
      float p2y[] = {109,100,100,109,119,119};
      float p3x[] = {63,72,90,98,88,72};
      float p3y[] = {155,146,146,155,166,166};

      int x=event->x();
      int y=event->y();
      setFocus();
      if (event->button()==Qt::LeftButton) {
        if (pnpoly(6,p1x,p1y,(float)x,(float)y)==1) {
          setStyleSheet("background:url(:/images/x9r1.png);");
          emit buttonPressed(Qt::Key_Plus);
        }
        else if (pnpoly(6,p2x,p2y,(float)x,(float)y)==1) {
          setStyleSheet("background:url(:/images/x9r2.png);");
          emit buttonPressed(Qt::Key_Minus);
        }
        else if (pnpoly(6,p3x,p3y,(float)x,(float)y)==1) {
          setStyleSheet("background:url(:/images/x9r3.png);");
          emit buttonPressed(Qt::Key_Enter);
        } 
      }
      // QWidget::mousePressEvent(event);
    }
    
    virtual void mouseReleaseEvent(QMouseEvent * event)
    {
      setStyleSheet("background:url(:/images/x9r0.png);");
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

    inline int pnpoly(int nvert, float *vertx, float *verty, float testx, float testy)
    {
        int i, j, c = 0;
        for (i = 0, j = nvert-1; i < nvert; j = i++) {
            if ( ((verty[i]>testy) != (verty[j]>testy)) &&  (testx < (vertx[j]-vertx[i]) * (testy-verty[i]) / (verty[j]-verty[i]) + vertx[i]) )
               c = !c;
        }
        return c;
    }

  signals:
    void buttonPressed(int button);

};
#endif	/* XMENUWIDGET_H */

