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

#ifndef _SLIDERWIDGET_H_
#define _SLIDERWIDGET_H_

#include <QtGui>
#include <QSlider>

class SliderWidget : public QSlider
{
  Q_OBJECT

  public:

    explicit SliderWidget(QWidget * parent = 0):
      QSlider(parent)
    {
      setToolTip(tr("Right-double-click to reset to center."));
    }

   protected:

     void mousePressEvent(QMouseEvent * event)
     {
       if (event->button() == Qt::RightButton && event->type() == QEvent::MouseButtonDblClick) {
         setValue(0);
         event->accept();
       }
       QSlider::mousePressEvent(event);
     }
};

#endif // _SLIDERWIDGET_H_
