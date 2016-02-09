#ifndef CALIBRATION_H
#define CALIBRATION_H

#include <QTableWidget>
#include "generaledit.h"

namespace Ui {
  class Calibration;
}

class QLabel;
class AutoLineEdit;
class AutoComboBox;

class CalibrationPanel : public GeneralPanel
{
    Q_OBJECT

  public:
    CalibrationPanel(QWidget *parent, GeneralSettings & generalSettings, Firmware * firmware);
    virtual ~CalibrationPanel() {};

  private slots:
    void onCellChanged(int value);

  private:
    QTableWidget * tableWidget;

    int getCalibrationValue(int row, int column);
    void setCalibrationValue(int row, int column, int value);
};

#endif // CALIBRATION_H
