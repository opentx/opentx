#include "verticalscrollarea.h"
#include "genericpanel.h"
#include <QEvent>
#include <QScrollBar>

VerticalScrollArea::VerticalScrollArea(QWidget * parent, GenericPanel * panel):
  QScrollArea(parent),
  panel(panel),
  parent(parent)
{
  setWidgetResizable(true);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setWidget(panel);
  panel->installEventFilter(this);
}

bool VerticalScrollArea::eventFilter(QObject *o, QEvent *e)
{
  if (o == panel && e->type() == QEvent::Resize) {
    setMinimumWidth(panel->minimumSizeHint().width() + verticalScrollBar()->width());
  }
  return false;
}
