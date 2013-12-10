/* 
 * File:   mySlider.h
 * Author: Romolo Manfredini
 *
 * Created on 11 gennaio 2012, 23.39
 */
#include <QSlider>
#include <QtGui>

class mySlider : public QSlider
{
  Q_OBJECT

 public:

    explicit mySlider(QFrame * parent = 0):
      QSlider(parent)
    {
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


