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
#ifndef CURSORWIDGET_H
#define	CURSORWIDGET_H

#include <QWidget>
#include <QtGui>

class cursorWidget : public QWidget {

  Q_OBJECT

  public:

    explicit cursorWidget(QWidget * parent = 0):
      QWidget(parent)
    {
    }

  protected:

    virtual void mousePressEvent(QMouseEvent * event)
    {
        float upx[] = {68,28,51,83,105,68};
        float upy[] = {83,45,32,32,45,83};
        float minx[] = {74,114,127,127,114,74};
        float miny[] = {90,51,80,106,130,90};
        float dox[] = {68,28,51,83,105,68};
        float doy[] = {98,137,151,151,137,98};
        float plusx[] = { 80,20,7,7,20,80};
        float plusy[] = {90,51,80,106,130,90};

        int x=event->x();
        int y=event->y();
        setFocus();
        if (event->button()==Qt::LeftButton) {
          if (pnpoly(6,upx,upy,(float)x,(float)y)==1) {
            setStyleSheet("background:url(:/images/9xcursup.png);");
            emit buttonPressed(Qt::Key_Up);
          }
          else if (pnpoly(6,minx,miny,(float)x,(float)y)==1) {
            setStyleSheet("background:url(:/images/9xcursmin.png);");
            emit buttonPressed(Qt::Key_Right);
          }
          else if (pnpoly(6,dox,doy,(float)x,(float)y)==1) {
            setStyleSheet("background:url(:/images/9xcursdown.png);");
            emit buttonPressed(Qt::Key_Down);
          }
          else if (pnpoly(6,plusx,plusy,(float)x,(float)y)==1) {
            setStyleSheet("background:url(:/images/9xcursplus.png);");
            emit buttonPressed(Qt::Key_Left);
          } else if (sqrt(((float)x-22)*((float)x-22)+((float)y-165)*((float)y-165))<17) {
            setStyleSheet("background:url(:/images/9xcursphoto.png);");
            emit buttonPressed(Qt::Key_Print);
          }
        }
        // QWidget::mousePressEvent(event);
    }
    
    virtual void mouseReleaseEvent(QMouseEvent * event)
    {
      setStyleSheet("background:url(:/images/9xcurs.png);");
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

