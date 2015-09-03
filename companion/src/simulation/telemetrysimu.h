#ifndef telemetrysimu_h
#define telemetrysimu_h


#include <QCloseEvent>
#include <QDialog>
#include <QTimer>
#include "simulatorinterface.h"

#define INSTANCE (ui->Instance->text().toInt(&ok,0) -1)

namespace Ui {
  class TelemetrySimulator;
}


class TelemetrySimulator : public QDialog
{
  Q_OBJECT

  public:
    explicit TelemetrySimulator(QWidget * parent, SimulatorInterface * simulator);
    virtual ~TelemetrySimulator();


  protected:
    virtual void closeEvent(QCloseEvent *event);

  private:
    Ui::TelemetrySimulator * ui;
    QTimer * timer;
    SimulatorInterface *simulator;

    void generateTelemetryFrame();

  private slots:
    void onTimerEvent();

};

#endif

