#ifndef HARDWARE_H
#define HARDWARE_H

#include "generaledit.h"

namespace Ui {
  class Hardware;
}

class QLabel;
class AutoLineEdit;
class AutoComboBox;

class HardwarePanel : public GeneralPanel
{
    Q_OBJECT

  public:
    HardwarePanel(QWidget *parent, GeneralSettings & generalSettings, Firmware * firmware);
    virtual ~HardwarePanel();

  private slots:
    void on_txVoltageHardware_editingFinished();
    void on_PPM1_editingFinished();
    void on_PPM2_editingFinished();
    void on_PPM3_editingFinished();
    void on_PPM4_editingFinished();
    void on_PPM_MultiplierDSB_editingFinished();

    void on_txCurrentHardware_editingFinished();
    void on_bluetoothEnable_stateChanged(int);

    void on_serialPortMode_currentIndexChanged(int index);

  protected:
    void setupPotConfig(int index, QLabel *label, AutoLineEdit *name, AutoComboBox *type);
    void setupSliderConfig(int index, QLabel *label, AutoLineEdit *name, AutoComboBox *type);
    void setupSwitchConfig(int index, QLabel *label, AutoLineEdit *name, AutoComboBox *type, bool threePos);

  private:
    Ui::Hardware *ui;

    void setValues();
};

#endif // HARDWARE_H
