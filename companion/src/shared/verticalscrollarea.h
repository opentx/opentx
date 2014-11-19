#ifndef VERTICALSCROLLAREA_H_
#define VERTICALSCROLLAREA_H_

#include <QScrollArea>

class GenericPanel;

class VerticalScrollArea : public QScrollArea
{
  public:
    VerticalScrollArea(QWidget * parent, GenericPanel * panel);

  protected:
    virtual bool eventFilter(QObject *o, QEvent *e);

  private:
    GenericPanel * panel;
    QWidget * parent;
};

#endif

