#ifndef debugoutput_h
#define debugoutput_h

#include <QtWidgets>
#include "simulatorinterface.h"

namespace Ui {
  class DebugOutput;
}


class DebugOutput : public QDialog
{
  Q_OBJECT

  public:
    explicit DebugOutput(QWidget * parent);
    virtual ~DebugOutput();
    void traceCallback(const QString & text);

  protected:

  private:
    Ui::DebugOutput * ui;

  private slots:

};

#endif

