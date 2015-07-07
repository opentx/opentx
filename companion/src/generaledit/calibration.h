#ifndef CALIBRATION_H
#define CALIBRATION_H

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
    virtual ~CalibrationPanel();
    // virtual void update();

  private slots:
    void on_battCalibDSB_editingFinished();
    void on_PPM1_editingFinished();
    void on_PPM2_editingFinished();
    void on_PPM3_editingFinished();
    void on_PPM4_editingFinished();
    void on_PPM_MultiplierDSB_editingFinished();

    void on_CurrentCalib_SB_editingFinished();
    void on_bluetoothEnable_stateChanged(int);

    void on_ana1Neg_editingFinished();
    void on_ana2Neg_editingFinished();
    void on_ana3Neg_editingFinished();
    void on_ana4Neg_editingFinished();
    void on_ana5Neg_editingFinished();
    void on_ana6Neg_editingFinished();
    void on_ana7Neg_editingFinished();
    void on_ana8Neg_editingFinished();

    void on_ana1Mid_editingFinished();
    void on_ana2Mid_editingFinished();
    void on_ana3Mid_editingFinished();
    void on_ana4Mid_editingFinished();
    void on_ana5Mid_editingFinished();
    void on_ana6Mid_editingFinished();
    void on_ana7Mid_editingFinished();
    void on_ana8Mid_editingFinished();

    void on_ana1Pos_editingFinished();
    void on_ana2Pos_editingFinished();
    void on_ana3Pos_editingFinished();
    void on_ana4Pos_editingFinished();
    void on_ana5Pos_editingFinished();
    void on_ana6Pos_editingFinished();
    void on_ana7Pos_editingFinished();
    void on_ana8Pos_editingFinished();

    void on_serialPortMode_currentIndexChanged(int index);

  protected:
    void setupPotConfig(int index, QLabel *label, AutoLineEdit *name, AutoComboBox *type);
    void setupSliderConfig(int index, QLabel *label, AutoLineEdit *name, AutoComboBox *type);
    void setupSwitchConfig(int index, QLabel *label, AutoLineEdit *name, AutoComboBox *type);

  private:
    Ui::Calibration *ui;

    void setValues();
};

#endif // CALIBRATION_H
