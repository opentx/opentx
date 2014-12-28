#ifndef telemetrysimu_h
#define telemetrysimu_h


#include <QDialog>
#include <QTimer>
#include "simulatorinterface.h"

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

  private:
    Ui::TelemetrySimulator * ui;
    QTimer * timer;
    SimulatorInterface *simulator;

    void generateTelemetryFrame();

  private slots:
    // void onButtonPressed(int value);
    // void on_FixRightY_clicked(bool checked);
    // void on_FixRightX_clicked(bool checked);
    // void on_FixLeftY_clicked(bool checked);
    // void on_FixLeftX_clicked(bool checked);
    // void on_holdRightY_clicked(bool checked);
    // void on_holdRightX_clicked(bool checked);
    // void on_holdLeftY_clicked(bool checked);
    // void on_holdLeftX_clicked(bool checked);
    // void on_trimHLeft_valueChanged(int);
    // void on_trimVLeft_valueChanged(int);
    // void on_trimHRight_valueChanged(int);
    // void on_trimVRight_valueChanged(int);
    void onTimerEvent();

};

#endif

