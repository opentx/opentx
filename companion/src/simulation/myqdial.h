#ifndef MYQDIAL_H
#define MYQDIAL_H


class myQDial : public QDial
{
public:
    explicit myQDial(QWidget *parent = 0) : QDial(parent) {}
 
protected:
    void mousePressEvent(QMouseEvent *event)
    {
        if (event->button() == Qt::RightButton && event->type() == QEvent::MouseButtonDblClick) {
            setValue(0);
            event->accept();
        } 
        QDial::mousePressEvent(event);
    }    
};

#endif
