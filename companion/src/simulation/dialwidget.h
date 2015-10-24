#ifndef _DIALWIDGET_H_
#define _DIALWIDGET_H_

class DialWidget : public QDial
{
  public:

    explicit DialWidget(QWidget * parent = 0):
      QDial(parent)
    {
    }
 
  protected:

    void mousePressEvent(QMouseEvent * event)
    {
        if (event->button() == Qt::RightButton && event->type() == QEvent::MouseButtonDblClick) {
          setValue(0);
          event->accept();
        } 
        QDial::mousePressEvent(event);
    }    
};

#endif // _DIALWIDGET_H_
