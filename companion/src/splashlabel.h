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
#ifndef SPLASHLABEL_H
#define SPLASHLABEL_H

#include <QtWidgets>

class splashLabel : public QLabel
{

  Q_OBJECT

public:

  explicit splashLabel(QDialog * parent = 0) :
    QLabel(parent)
  {
  }

  virtual void setId(int labelindex)
  {
    index = labelindex;
  }

  virtual int getId()
  {
    return index;
  }

protected:

  virtual void mousePressEvent(QMouseEvent * event)
  {
    if (event->type() == QEvent::MouseButtonDblClick) {
      emit buttonPressed(Qt::Key_Enter);
    }
    else {
      emit buttonPressed(Qt::Key_Select);
    }
    // QWidget::mousePressEvent(event);
  }

  virtual void mouseReleaseEvent(QMouseEvent * event)
  {
    setFocus();
    // emit buttonPressed(0);
    // QWidget::mouseReleaseEvent(event);
  }


  /*void paintEvent(QPaintEvent *)
  {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
  }
   */
signals:
  void buttonPressed(int button);

private:
  int index;
};

#endif /* SPLASHLABEL_H */

