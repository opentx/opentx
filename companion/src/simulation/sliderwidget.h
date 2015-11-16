#ifndef _SLIDERWIDGET_H_
#define _SLIDERWIDGET_H_

#include <QSlider>
#include <QtGui>

class SliderWidget : public QSlider
{
  Q_OBJECT

  public:

    explicit SliderWidget(QFrame * parent = 0):
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

#endif // _SLIDERWIDGET_H_
