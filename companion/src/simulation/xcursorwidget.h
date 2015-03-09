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
#ifndef XCURSORWIDGET_H
#define	XCURSORWIDGET_H

#include <QWidget>
#include <QtGui>

class xcursorWidget : public QWidget {

  Q_OBJECT

  public:

    explicit xcursorWidget(QWidget * parent = 0):
      QWidget(parent)
    {
    }

  protected:

    virtual void mousePressEvent(QMouseEvent * event)
    {
        float p1x[] = {20,27,45,56,50,26};
        float p1y[] = {59,50,52,59,71,72};
        float p2x[] = {23,30,46,55,47,28};
        float p2y[] = {107,99,100,106,117,117};
        float p3x[] = {24,32,46,57,46,29};
        float p3y[] = {154,144,146,156,167,166};

        int x=event->x();
        int y=event->y();
        setFocus();
        if (event->button()==Qt::LeftButton) {
          if (pnpoly(6,p1x,p1y,(float)x,(float)y)==1) {
            setStyleSheet("background:url(:/images/x9l1.png);");
            emit buttonPressed(Qt::Key_PageUp);
          }
          else if (pnpoly(6,p2x,p2y,(float)x,(float)y)==1) {
            setStyleSheet("background:url(:/images/x9l2.png);");
            emit buttonPressed(Qt::Key_PageDown);
          }
          else if (pnpoly(6,p3x,p3y,(float)x,(float)y)==1) {
            setStyleSheet("background:url(:/images/x9l3.png);");
            emit buttonPressed(Qt::Key_Escape);
          } else if ((x>90 && x<118) && (y>177 && y<197)) {
            setStyleSheet("background:url(:/images/x9l4.png);");
            emit buttonPressed(Qt::Key_Print);
          }
        }
        // QWidget::mousePressEvent(event);
    }
    
    virtual void mouseReleaseEvent(QMouseEvent * event)
    {
      setStyleSheet("background:url(:/images/x9l0.png);");
      emit buttonPressed(0);
      setFocus();
      //QWidget::mouseReleaseEvent(event);
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


#endif	/* CURSORWIDGET_H */

